/*
* @Author: UnsignedByte
* @Date:   2021-04-15 13:34:46
* @Last Modified by:   UnsignedByte
* @Last Modified time: 2021-04-20 13:41:05
*/

#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>


struct Pendulum
{
public:
	Pendulum() = default;

	Pendulum(sf::Vector2<long double> pos): _pos(pos)
	{
	}

	Pendulum(Pendulum* parent, long double angle, long double length, long double mass): _parent(parent), _theta(angle), l(length), m(mass)
	{
		_pos = sf::Vector2<long double>(std::cos(angle)*l, std::sin(angle)*l)+_parent->pos();
	}

	friend std::ostream& operator<<(std::ostream&, const Pendulum&);
 
	sf::Vector2<long double> pos();

	virtual void render(sf::VertexArray&, int);

	/**
	 * @param fT force of tension exerted
	 * @param idx Number of pendulums below this
	 */
	virtual void update(sf::Vector2<long double> fT);

	long double m = 100.L; //mass
	long double _vel = 0.L;
	long double l; //length
	sf::Vector2<long double> cartVel = sf::Vector2<long double>(10, 10);
protected:
	Pendulum* _parent;
	sf::Vector2<long double> _pos;
	sf::Vector2<long double> _fT = sf::Vector2<long double>(0, 0);
	long double _theta; //angle of pendulum
};

struct Root : Pendulum
{
	Root(sf::Vector2<long double> pos): Pendulum(pos)
	{
	}

	void render(sf::VertexArray&, int);
	void update(sf::Vector2<long double>);
};