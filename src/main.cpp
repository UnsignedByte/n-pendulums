/*
* @Author: UnsignedByte
* @Date:   2021-04-15 13:21:00
* @Last Modified by:   UnsignedByte
* @Last Modified time: 2021-04-20 13:14:15
*/

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <cstring>
#include "pendulum.hpp"

const int WIDTH = 800;
const int HEIGHT = 800;
const sf::Time frameTime = sf::seconds(1/60.f);
const sf::Time tickTime = sf::seconds(1/1000.f);
const int COUNT = 1;
const long double LEN = HEIGHT/static_cast<long double>(COUNT)/2;
const long double ANGLE = 3*M_PI/4;

int main()
{
	int x = 0;

	Pendulum* pendulums = new Pendulum[COUNT];
	Root root(sf::Vector2<long double>(WIDTH/2, HEIGHT/2));

	pendulums[0] = Pendulum(&root, ANGLE, LEN, 1);
	for(int i = 1; i < COUNT; i++){
		pendulums[i] = Pendulum(&pendulums[i-1], ANGLE, LEN, 1);
	}
	// pendulums[COUNT-1] = Pendulum(&pendulums[COUNT-2], M_PI-ANGLE, LEN, COUNT*5);

	// Create the main window
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Pendulous Pendulum");
	window.setVerticalSyncEnabled(0);
	sf::Clock renderClock;
	sf::Time elapsed = sf::seconds(0); 

	// window.setFramerateLimit(60);
	while (window.isOpen())
	{
		pendulums[COUNT-1].update(sf::Vector2<long double>(0,0));
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

			sf::VertexArray arr(sf::LineStrip, COUNT+1);
			pendulums[COUNT-1].render(arr, 0);
			window.draw(arr);
			window.display();
		
			renderClock.restart();
		}
		sf::sleep(tickTime);
	}

	return 0;
}