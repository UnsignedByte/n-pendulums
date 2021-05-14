/*
* @Author: UnsignedByte
* @Date:   2021-04-15 13:21:00
* @Last Modified by:   UnsignedByte
* @Last Modified time: 2021-05-13 18:16:11
*/

#include <SFML/Graphics.hpp>
// #include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <list>
extern "C"
{
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libavutil/avutil.h>
	#include <libavformat/avio.h>

	void dgesv_(int *n, int *nrhs, double *a, int *lda, int *ipiv, double *b, int *ldb, int *info);
}
#include "utils.hpp"

#define EPSILON 1e-9L
#undef av_err2str
#define av_err2str(errnum) \
av_make_error_string((char*)__builtin_alloca(AV_ERROR_MAX_STRING_SIZE), AV_ERROR_MAX_STRING_SIZE, errnum)

int N = 2;
int WIDTH = 800;
int HEIGHT = 800;
float speed = 0.05; // time speed
int fps = 30; // fps
int duration = 60; // duration in seconds
float trail_length = 5; //# of seconds of trail to save (will round)
int _r_trail_length;
std::vector<int> trail_ids; // pendulum index for each trail
std::list<sf::Vector2f> trails; // pendulum index for each trail

double L;
const double ANGLE = 3*M_PI/4;

const double DT = 0.00001L; // 1 step of time
const double G = 9.8L; //acceleration due to Gravity
const double M = 5.L; //total mass of rope

std::vector<double> theta;
std::vector<double> thetad;

static void rgbyuv(sf::Color rgb, uint8_t* Y, uint8_t* U, uint8_t* V)
{
	*Y = static_cast<uint8_t>(rgb.r *  .299000 + rgb.g *  .587000 + rgb.b *  .114000);
	*U = static_cast<uint8_t>(rgb.r * -.168736 + rgb.g * -.331264 + rgb.b *  .500000 + 128);
	*V = static_cast<uint8_t>(rgb.r *  .500000 + rgb.g * -.418688 + rgb.b * -.081312 + 128);
}

std::vector<double> system_eqs; // system_eqs of equations
std::vector<double> system_rhs; // right hand side constants
std::vector<int> ipiv;
int one = 1;

static void update() {
	// Generate system_eqs of equations using terms from paper
	for (int i = 0; i < N; i++){
		system_rhs[i] = -M*G*L*(N-i)*std::sin(theta[i]); // initial constant term
		for(int j = 0; j < N; j++){
			int mj = (N-std::max(i, j));
			system_rhs[i] -= mj*thetad[j]*thetad[j]*std::sin(theta[i]-theta[j]); // add constant term
			system_eqs[i*N+j] = mj*std::cos(theta[i]-theta[j]);
		}
	}

	//Gaussian elimination on system_eqs of equations

	// std::cout << system_eqs << '\n';

	int res;

	// std::cout << "EQS:" << system_eqs << std::endl;
	// std::cout << "RHS:" << system_rhs << std::endl;
	// std::cout << "PIVOT:" << ipiv << std::endl;

	dgesv_(&N, &one, system_eqs.data(), &N, ipiv.data(), system_rhs.data(), &N, &res);

	if (res != 0){
		printf("Argument %d had failed with error %s.\n", res, (res < 0) ? "illegal value":"zero value");
	}

	// std::cout << "RHS:" << system_rhs << std::endl;
	//Backpropogation portion to get accelerations

	// std::cout << system_eqs << '\n';

	// std::cout << "accel: "<< thetadd << '\n';

	// Update angular velocities and positions using dt, and put these into vertexarray

	for(int i = 0; i < N; i++) {
		thetad[i]+=system_rhs[i]*DT;
		theta[i]+=thetad[i]*DT;
	}
}

static int write_frame(AVFormatContext* av_format_context, const AVRational& time_base, AVStream *av_stream, AVPacket *packet)
{
	/* rescale output packet timestamp values from codec to stream timebase */
	av_packet_rescale_ts(packet, time_base, av_stream->time_base);
	packet->stream_index = av_stream->index;
	// printf("Stream Index: %d, PTS: %" PRId64 ", DTS: %" PRId64 ", pos: %" PRId64 ", duration: %" PRId64 "\n", packet->stream_index, packet->pts, packet->dts, packet->pos, packet->duration);
	return av_interleaved_write_frame(av_format_context, packet);
}

static void encode(AVFormatContext* av_format_context, AVCodecContext* av_codec_context, AVStream* av_stream, AVFrame* frame, AVPacket* packet)
{
	int res;

	res = avcodec_send_frame(av_codec_context, frame);
	if (res < 0) {
		fprintf(stderr, "Unable to send frame for encoding.\n");
		exit(1);
	}

	while(1) {
		res = avcodec_receive_packet(av_codec_context, packet);

		// all packets sent
		if (res == AVERROR(EAGAIN) || res == AVERROR_EOF)
		{
			// av_packet_free(&packet);
			break;
		} else if (res < 0) {
			fprintf(stderr, "Error during encoding.\n");
			exit(1);
		}

		// packet->pts = pts;
		packet->duration = 1;
		// printf("%ld\n", packet->duration);

		res = write_frame(av_format_context, av_codec_context->time_base, av_stream, packet);
		if (res < 0) {
			fprintf(stderr, "Error muxing packet\n");
			break;
		}
		av_packet_unref(packet);
	}
}

// reads argc and argv and writes to names, values
static int parse_cli(int argc, char** argv, std::vector<char*>& names, std::vector<char*>& values)
{
	int argi = -1;
	for(int i = 1; i < argc; i++)
	{
		char* word = argv[i];
		if (word[0] == '-') {
			word++; // remove "-" from the string
			argi++;
			// printf("Parsing argument name %s, index %d\n", word, argi);
			names.push_back(word);
			values.push_back(argv[++i]);
		} else {
			asprintf(&values[argi], "%s %s", values[argi], word);
			// printf("Updating argument value to %s\n", word);
		}
	}
	return argi+1;
}

// https://stackoverflow.com/questions/40629345/fill-array-dynamicly-with-gradient-color-c
//input: ratio is between 0 to 1
//output: rgb color
sf::Color rainbowrgb(double ratio)
{
    //we want to normalize ratio so that it fits in to 6 regions
    //where each region is 256 units long
    int normalized = int(ratio * 256 * 6);

    //find the distance to the start of the closest region
    int x = normalized % 256;

    int red = 0, grn = 0, blu = 0;
    switch(normalized / 256)
    {
    case 0: red = 255;      grn = x;        blu = 0;       break;//red
    case 1: red = 255 - x;  grn = 255;      blu = 0;       break;//yellow
    case 2: red = 0;        grn = 255;      blu = x;       break;//green
    case 3: red = 0;        grn = 255 - x;  blu = 255;     break;//cyan
    case 4: red = x;        grn = 0;        blu = 255;     break;//blue
    case 5: red = 255;      grn = 0;        blu = 255 - x; break;//magenta
    }

    return sf::Color(red, grn, blu);
}

// http://www.cse.yorku.ca/~oz/hash.html
unsigned constexpr static hash(const char* str)
{
	return *str ?
    static_cast<unsigned int>(*str) + 33 * hash(str + 1) :
    5381;
}

// struct Node {
// 	Node(): x(-1,-1)
// 	{
// 	}
// 	Node(sf::Vector2f v): x(v)
// 	{
// 	}
// 	sf::Vector2f x;
// 	Node* next;
// };

// Node* head;
// Node* tail;

// void node_push(Node* n) {
// 	printf("Pushing node\n");
// 	tail->next = n;
// 	tail = n;
// 	printf("Pushed node\n");
// }

int main(int argc, char **argv)
{
	//handle cli arguments;
	std::vector<char*> names, values;
	int argL = parse_cli(argc, argv, names, values);
	for(int i = 0; i < argL; i++){
		switch (hash(names[i])) {
			case hash("n"):
			case hash("N"):
			case hash("count"):
				N = atoi(values[i]);
				printf("Set pendulum count to %d\n", N);
				break;
			case hash("width"):
			case hash("w"):
				WIDTH = atoi(values[i]);
				break;
			case hash("height"):
			case hash("h"):
				HEIGHT = atoi(values[i]);
				break;
			case hash("fps"):
				fps = atoi(values[i]);
				printf("Set fps to %d\n", fps);
				break;
			case hash("duration"):
			case hash("d"):
				duration = atoi(values[i]);
				printf("Set duration to %d\n", duration);
				break;
			case hash("speed"):
			case hash("s"):
				speed = atof(values[i]);
				printf("Set speed to %f\n", speed);
				break;
			case hash("trail"):
			case hash("t"):
				switch(hash(values[i])){
					case hash("all"):
					case hash("a"):
						for(int i = 0; i < N; i++){
							trail_ids.push_back(i);
						}
						break;
					case hash("last"):
					case hash("l"):
						trail_ids.push_back(N-1);
						break;
					default:
						printf("Setting trails failed with value %s\n", values[i]);
				}
				printf("Set trails to type %s\n", values[i]);
				break;
			case hash("trail_duration"):
			case hash("td"):
				trail_length = atof(values[i]);
				printf("Set trail length to %f seconds\n", trail_length);
				break;
			default:
				printf("Unknown parameter %s had value %s\n", names[i], values[i]);
		}
	}

	int trail_count = trail_ids.size();
	_r_trail_length = trail_count*trail_length/DT*speed;

	L = std::min(WIDTH, HEIGHT)/static_cast<double>(N)/2;

	theta = std::vector<double>(N,ANGLE); // angular position
	thetad = std::vector<double>(N,0); // angular velocity

	system_eqs = std::vector<double>(N*N,0);
	system_rhs = std::vector<double>(N,0);
	ipiv = std::vector<int>(N);

	sf::VertexArray pendulums(sf::LineStrip, N+1);
	sf::RenderTexture frame_render;

	frame_render.create(WIDTH, HEIGHT);
	pendulums[0] = sf::Vector2f(WIDTH/2,HEIGHT/2);
	char* filename;

	asprintf(&filename, "output/%dPendulum%.2lfkg%.3lftheta_%dx%d_%dsecs%dfps_%.3fspeed%lfdt.mp4", N, M, ANGLE, WIDTH, HEIGHT, duration, fps, speed, DT);

	AVFormatContext* av_format_context;
	AVCodecContext* av_codec_context;
	AVCodec* av_codec;
	AVStream* av_stream;
	int res; //stores error codes (if returned);

	av_format_context = avformat_alloc_context();
	if (!av_format_context) {
		fprintf(stderr, "Could not allocate format context.\n");
		exit(1);
	}

	res = avformat_alloc_output_context2(&av_format_context, av_guess_format(NULL, filename, NULL), NULL, filename);
	if (res < 0){
		printf("Setting output format failed with code %s\n", av_err2str(res));
		exit(1);
	}

	printf("Writing file with format %s (MIME type %s).\n", av_format_context->oformat->long_name, av_format_context->oformat->mime_type);

	av_codec = avcodec_find_encoder(av_format_context->oformat->video_codec);
	if (!av_codec) {
		fprintf(stderr, "Could not create video encoder.\n");
		exit(1);
	}

	av_codec_context = avcodec_alloc_context3(av_codec);
	if (!av_codec_context) {
		fprintf(stderr, "Could not create codec context.\n");
		exit(1);
	}

	av_stream = avformat_new_stream(av_format_context, av_codec);
	if (!av_stream) {
		fprintf(stderr, "Could not create AVStream.\n");
		exit(1);
	}

	AVFrame* frame = av_frame_alloc();
	if (!frame) {
		fprintf(stderr, "Frame failed allocation.\n");
		exit(1);
	}

	AVPacket* packet = av_packet_alloc();
	if (!packet) {
		fprintf(stderr, "Packet failed allocation.\n");
		exit(1);
	}

	av_codec_context->width = WIDTH;
	av_codec_context->height = HEIGHT;
	av_codec_context->bit_rate = av_codec_context->width*av_codec_context->height*4;
	av_codec_context->time_base = (AVRational){1,fps};
	av_codec_context->framerate = (AVRational){fps,1};

	av_codec_context->gop_size = fps; // intra frames
	av_codec_context->max_b_frames = 1; //max number of B-frames between normal frames
	av_codec_context->pix_fmt = AV_PIX_FMT_YUV444P; //acolor format

	frame->format = av_codec_context->pix_fmt;
	frame->width = av_codec_context->width;
	frame->height = av_codec_context->height;

	av_stream->r_frame_rate = av_codec_context->framerate;
	av_stream->avg_frame_rate = av_codec_context->framerate;
	av_stream->time_base = av_codec_context->time_base;

	//open codec
	if (avcodec_open2(av_codec_context, av_codec, NULL) < 0) {
		fprintf(stderr, "Could not open codec.\n");
		exit(1);
	}

	// av_stream->codec = av_codec_context;

	res = avcodec_parameters_from_context(av_stream->codecpar, av_codec_context);
	if (res < 0) {
		fprintf(stderr, "Failed to get avcodec params; Error code '%s'", av_err2str(res));
    exit(1);
	}

	res = avio_open(&av_format_context->pb, filename, AVIO_FLAG_WRITE); // open output file
	if (res < 0) {
    fprintf(stderr, "Could not open output file '%s'", filename);
    exit(1);
	}

	AVDictionary* av_opts = NULL;
	av_dict_set(&av_opts, "movflags", "frag_keyframe+empty_moov+default_base_moof", 0);

	res = avformat_write_header(av_format_context, &av_opts);
	if (res < 0) {
		fprintf(stderr, "Writing video header fails with code %s.\n", av_err2str(res));
		exit(1);
	}

	res = av_frame_get_buffer(frame, 32);

	double lastFrame = 0;
	sf::VertexArray trails_vertex(sf::Points, 0);

	for (int i = 0; i < duration*speed/DT; i++) { //encode 60 seconds of video
		// printf("Time %d calculated.\n", i);
		// printf("Calculated time %Lf elapsed %Lf\n", i*DT, (i*DT-lastFrame)*fps-speed);

		if ((i-lastFrame)*DT*fps-speed>=0){
			lastFrame = i;
			int frid = static_cast<int>(i*DT*fps/speed);
			if (frid%fps == 0)
				printf("Writing frame %d\n", frid);

			fflush(stdout);

			// printf("%ld\n", trails.size());
			trails_vertex.resize(trails.size());

			// Node* curr = head;
			// int ind = 0;
			// while(curr->next)
			// {
			// 	trails_vertex[ind] = curr->x;
			// 	ind++;
			// 	curr = curr->next;
			// }

			std::list<sf::Vector2f>::iterator it;
			int ind = _r_trail_length;
			for (it = trails.end(); it != trails.begin(); it--)
			{
				trails_vertex[_r_trail_length-ind].position = *it;
				trails_vertex[_r_trail_length-ind].color = rainbowrgb((ind%trail_count)/(double)trail_count);
				double colRatio = 1-(ind/trail_count)/(double)(_r_trail_length/trail_count);

				colRatio*=colRatio;
				// colRatio*=colRatio; //fourth power
				trails_vertex[_r_trail_length-ind].color *= 1-colRatio;
				ind--;
			}

			frame_render.clear();
			frame_render.draw(pendulums);
			frame_render.draw(trails_vertex);

			sf::Image frame_img = frame_render.getTexture().copyToImage();

			res = av_frame_make_writable(frame); // make frame writable
			if (res < 0)
				exit(1);

			//create dummy frame
			for(int y = 0; y < frame->height; y++) {
				for(int x = 0; x < frame->width; x++) {
					rgbyuv(frame_img.getPixel(x,y), 
						&frame->data[0][y * frame->linesize[0] + x], 
						&frame->data[1][y * frame->linesize[1] + x], 
						&frame->data[2][y * frame->linesize[2] + x]);
				}
			}

			// frame->pts = av_rescale_q(i, av_codec_context->time_base, av_stream->time_base);
			frame->pts = frid;

			//encode image
			encode(av_format_context, av_codec_context, av_stream, frame, packet);
		}
		update();
			
		for(int i = 0; i < N; i++){
			sf::Vector2<double>(std::sin(theta[i]),std::cos(theta[i]));
			pendulums[i+1].position = pendulums[i].position+static_cast<sf::Vector2f>(L*sf::Vector2<double>(std::sin(theta[i]),-std::cos(theta[i])));
		}

		for(int i = 0; i < trail_ids.size(); i++)
		{
			if (i >= _r_trail_length)
			{
				// Node* oldhead = &*head;
				// head = head->next;
				// delete oldhead;
				trails.pop_front();
			}
			// Node nnode(pendulums[trail_ids[i]+1].position);
			// node_push(&nnode);
			trails.push_back(pendulums[trail_ids[i]+1].position);
		}
	}

	//flush encoder
	encode(av_format_context, av_codec_context, av_stream, NULL, packet);

	//write file end code
	res = av_write_trailer(av_format_context);
	if (res < 0) {
		fprintf(stderr, "Writing video trailer fails with code %s.\n", av_err2str(res));
		exit(1);
	}

	// printf(
	// 	"\nCodec Parameters:\n"
	// 	"Video size: %dx%d\n" 
	// 	"Time base: %d/%d, Framerate: %d/%d\n"
	// 	"\nStream Parameters:\n"
	// 	"Time base: %d/%d\n"
	// 	"Real Framerate: %d/%d\n"
	// 	"Average Framerate: %d/%d\n"
	// 	"Duration: %" PRId64 "\n"
	// 	"Num frames: %" PRId64 "\n",
	// 	av_codec_context->width, av_codec_context->height,
	// 	av_codec_context->time_base.num, av_codec_context->time_base.den,
	// 	av_codec_context->framerate.num, av_codec_context->framerate.den,
	// 	av_stream->time_base.num, av_stream->time_base.den,
	// 	av_stream->r_frame_rate.num, av_stream->r_frame_rate.den,
	// 	av_stream->avg_frame_rate.num, av_stream->avg_frame_rate.den,
	// 	av_stream->duration, av_stream->nb_frames
	// 	);

	avcodec_free_context(&av_codec_context);
	av_frame_free(&frame);
	av_packet_free(&packet);

	return 0;
}