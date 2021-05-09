/*
* @Author: UnsignedByte
* @Date:   2021-04-15 13:21:00
* @Last Modified by:   UnsignedByte
* @Last Modified time: 2021-05-08 17:50:23
*/

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>
#include "utils.hpp"

const int N = 50;
const int WIDTH = 800;
const int HEIGHT = 800;
const sf::Time frameTime = sf::seconds(1/60.f);
const sf::Time tickTime = sf::seconds(1/400.f);
const long double L = HEIGHT/static_cast<long double>(N)/2;
const long double ANGLE = M_PI/2;

const long double DT = 0.0001L;
const long double G = 9.8L; //acceleration due to Gravity
const long double M = 5.L; //total mass of rope

std::vector<long double> theta(N,ANGLE); // angular position
std::vector<long double> thetad(N,0); // angular velocity

sf::VertexArray pendulums(sf::LineStrip, N+1);

void update() {
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
		sf::Vector2<long double>(std::sin(theta[i]),std::cos(theta[i]));
		pendulums[i+1].position = pendulums[i].position+static_cast<sf::Vector2f>(L*sf::Vector2<long double>(std::sin(theta[i]),std::cos(theta[i])));
	}
	// std::cout << "theta: "<< theta << '\n';
}

int main()
{
	pendulums[0] = sf::Vector2f(WIDTH/2,HEIGHT/2);
	// pendulums[N-1] = Pendulum(&pendulums[N-2], M_PI-ANGLE, LEN, N*5);

	// Create the main window
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Pendulous Pendulum");
	window.setVerticalSyncEnabled(0);
	sf::Clock renderClock;
	sf::Clock tickClock;

	// window.setFramerateLimit(60);
	while (window.isOpen())
	{
		update();
		if(renderClock.getElapsedTime() > frameTime){
			// Process events
			sf::Event event;
			while (window.pollEvent(event))
			{
				// Close window: exit
				if (event.type == sf::Event::Closed)
					window.close();
			}

			window.clear();

			window.draw(pendulums);
			window.display();
		
			renderClock.restart();
		}
		if (tickClock.getElapsedTime() < tickTime)
		{
			// std::cout << tickClock.getElapsedTime().asSeconds() << std::endl;
			sf::sleep(tickTime-tickClock.getElapsedTime());
		}
		tickClock.restart();
	}

	return 0;
}