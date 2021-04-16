/*
* @Author: UnsignedByte
* @Date:   2021-04-15 13:21:00
* @Last Modified by:   UnsignedByte
* @Last Modified time: 2021-04-15 18:32:42
*/

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <cstring>
#include "pendulum.hpp"

const int WIDTH = 800;
const int HEIGHT = 600;
const sf::Time frameTime = sf::seconds(1.f);


int main()
{
	int x = 0;
	Root root(sf::Vector2f(WIDTH/2, 0));
	Pendulum p1(&root, M_PI/2, 50.f);

	// Create the main window
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Pendulous Pendulum");
	window.setVerticalSyncEnabled(0);
	sf::Clock renderClock;
	sf::Time elapsed = sf::seconds(0); 

	// window.setFramerateLimit(60);
	while (window.isOpen())
	{
		// Process events
		sf::Event event;
		while (window.pollEvent(event))
		{
			// Close window: exit
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();

		p1.update(sf::Vector2f(0,0), 0);

		sf::VertexArray arr(sf::LineStrip, 2);
		p1.render(arr, 0);
		window.draw(arr);

		window.display();

		std::cout << "FRAME #: " << ++x << std::endl;
		std::cout << renderClock.getElapsedTime().asMilliseconds() << std::endl;
		sf::sleep(frameTime-renderClock.getElapsedTime());
		renderClock.restart();
	}

	return 0;
}