/*
* @Author: UnsignedByte
* @Date:   2021-04-15 13:21:00
* @Last Modified by:   UnsignedByte
* @Last Modified time: 2021-05-12 11:36:58
*/

#include <SFML/Graphics.hpp>
// #include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <cstdio>
extern "C"
{
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libavutil/avutil.h>
	#include <libavutil/avio.h>
}
#include "utils.hpp"

#define EPSILON 1e-9L
#undef av_err2str
#define av_err2str(errnum) \
av_make_error_string((char*)__builtin_alloca(AV_ERROR_MAX_STRING_SIZE), AV_ERROR_MAX_STRING_SIZE, errnum)

const int N = 2;
const int WIDTH = 800;
const int HEIGHT = 800;
const float speed = 0.05; // time speed
const int fps = 30; // fps
const int duration = 60; // duration in seconds
// const sf::Time frameTime = sf::seconds(1/60.f);
// const sf::Time tickTime = sf::seconds(1/400.f);
const long double L = HEIGHT/static_cast<long double>(N)/2;
const long double ANGLE = 3*M_PI/4;

const long double DT = 0.00001L; // 1 step of time
const long double G = 9.8L; //acceleration due to Gravity
const long double M = 5.L; //total mass of rope

std::vector<long double> theta(N,ANGLE); // angular position
std::vector<long double> thetad(N,0); // angular velocity

sf::VertexArray pendulums(sf::LineStrip, N+1);
sf::RenderTexture frame_render;

static void rgbyuv(sf::Color rgb, uint8_t* Y, uint8_t* U, uint8_t* V)
{
	*Y = static_cast<uint8_t>(rgb.r *  .299000 + rgb.g *  .587000 + rgb.b *  .114000);
	*U = static_cast<uint8_t>(rgb.r * -.168736 + rgb.g * -.331264 + rgb.b *  .500000 + 128);
	*V = static_cast<uint8_t>(rgb.r *  .500000 + rgb.g * -.418688 + rgb.b * -.081312 + 128);
}


static void update() {
	std::vector<std::vector<long double>> system(N,std::vector<long double>(N+1,0)); // system of equations

	// Generate system of equations using terms from paper
	for (int i = 0; i < N; i++){
		system[i][N] = -M*G*L*(N-i)*std::sin(theta[i]); // initial constant term
		for(int j = 0; j < N; j++){
			int mj = (N-std::max(i, j));
			system[i][N] -= mj*thetad[j]*thetad[j]*std::sin(theta[i]-theta[j]); // add constant term
			system[i][j] = mj*std::cos(theta[i]-theta[j]);
		}
	}

	//Gaussian elimination on system of equations

	// std::cout << system << '\n';

	for(int i = 0; i < N; i++){
		for(int j = i+1; j < N; j++){
			long double factor = system[j][i]/system[i][i];
			for(int k = 0; k < N+1; k++){
				system[j][k]-=factor*system[i][k];
			}
		}
	}

	//Backpropogation portion to get accelerations

	// std::cout << system << '\n';

	std::vector<long double> thetadd(N);
	for(int i = N-1; i >= 0; i--){
		for(int j = N-1; j > i; j--){
			system[i][N]-=thetadd[j]*system[i][j];
		}
		thetadd[i] = system[i][N]/system[i][i];
	}
	// std::cout << "accel: "<< thetadd << '\n';

	// Update angular velocities and positions using dt, and put these into vertexarray

	for(int i = 0; i < N; i++) {
		thetad[i]+=thetadd[i]*DT;
		theta[i]+=thetad[i]*DT;
	}
	// std::cout << "theta: "<< theta << '\n';
}

static void encode(AVCodecContext* av_codec_context, AVFrame* frame, AVPacket* packet, FILE* file)
{
	int res;

	res = avcodec_send_frame(av_codec_context, frame);
	if (res < 0) {
		fprintf(stderr, "Unable to send frame for encoding.\n");
		exit(1);
	}

	while(res >= 0) {
		res = avcodec_receive_packet(av_codec_context, packet);

		// all packets sent
		if (res == AVERROR(EAGAIN) || res == AVERROR_EOF)
		{
			return;
		} else if (res < 0) {
			fprintf(stderr, "Error during encoding.\n");
			exit(1);
		}

		// printf("Encoded frame %3" PRId64 " (size=%5d)\n", packet->pts, packet->size);
		fwrite(packet->data, 1, packet->size, file);
		av_packet_unref(packet);
	}
}

int main(int argc, char **argv)
{
	frame_render.create(WIDTH, HEIGHT);
	pendulums[0] = sf::Vector2f(WIDTH/2,HEIGHT/2);
	char* filename;

	asprintf(&filename, "output/%dPendulum%.2Lfkg%.3Lftheta_%dx%d_%dsecs%dfps_%.3fspeed%Lfdt.mp4", N, M, ANGLE, WIDTH, HEIGHT, duration, fps, speed, DT);

	AVOutputFormat* av_output_format = av_guess_format(NULL, filename, NULL);
	AVFormatContext* av_format_context;
	AVCodecContext* av_codec_context;
	AVCodec* av_codec;
	FILE* file;
  uint8_t endcode[] = { 0, 0, 1, 0xb7 };
	int res; //stores error codes (if returned);

	printf("Writing file with format %s (MIME type %s).\n", av_output_format->long_name, av_output_format->mime_type);

	av_codec = avcodec_find_encoder(av_output_format->video_codec);
	if (!av_codec) {
		fprintf(stderr, "Could not create video encoder.\n");
		exit(1);
	}

	av_codec_context = avcodec_alloc_context3(av_codec);
	if (!av_codec_context) {
		fprintf(stderr, "Could not create codec context.\n");
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

	av_codec_context->bit_rate = 2000000;
	av_codec_context->width = WIDTH;
	av_codec_context->height = HEIGHT;
	av_codec_context->time_base = (AVRational){1,fps};
	av_codec_context->framerate = (AVRational){fps,1};

	av_codec_context->gop_size = fps*0.5; //intra frame rate
	av_codec_context->max_b_frames = 1; //max number of B-frames between normal frames
	av_codec_context->pix_fmt = AV_PIX_FMT_YUV444P; //color format

	//open codec
	if (avcodec_open2(av_codec_context, av_codec, NULL) < 0) {
		fprintf(stderr, "Could not open codec.\n");
		exit(1);
	}

	file = fopen(filename, "wb");
	if (!file) {
		fprintf(stderr, "Could not open file: %s\n", filename);
		exit(1);
	}

	frame->format = av_codec_context->pix_fmt;
	frame->width = av_codec_context->width;
	frame->height = av_codec_context->height;

	res = av_frame_get_buffer(frame, 32);

	long double lastFrame = 0;

	for (int i = 0; i < duration*speed/DT; i++) { //encode 60 seconds of video
		update();
		// printf("Calculated time %Lf elapsed %Lf\n", i*DT, (i*DT-lastFrame)*fps-speed);

		if ((i-lastFrame)*DT*fps-speed>=0){
			lastFrame = i;
			if (static_cast<int>(i*DT*fps/speed)%fps == 0)
				printf("Writing frame %d\n", static_cast<int>(i*DT*fps/speed));

			fflush(stdout);
			
			for(int i = 0; i < N; i++){
				sf::Vector2<long double>(std::sin(theta[i]),std::cos(theta[i]));
				pendulums[i+1].position = pendulums[i].position+static_cast<sf::Vector2f>(L*sf::Vector2<long double>(std::sin(theta[i]),-std::cos(theta[i])));
			}

			frame_render.clear();
			frame_render.draw(pendulums);

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

			frame->pts = i;

			//encode image
			encode(av_codec_context, frame, packet, file);
		}
	}

	//flush encoder
	encode(av_codec_context, NULL, packet, file);

	//write file end code
	fwrite(endcode, 1, sizeof(endcode), file);
	fclose(file);

	avcodec_free_context(&av_codec_context);
	av_frame_free(&frame);
	av_packet_free(&packet);

	return 0;
}