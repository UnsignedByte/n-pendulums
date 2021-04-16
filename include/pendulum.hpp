/*
* @Author: UnsignedByte
* @Date:   2021-04-15 13:34:46
* @Last Modified by:   UnsignedByte
* @Last Modified time: 2021-04-15 18:34:26
*/

#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>


struct Pendulum
{
public:
	Pendulum() = default;

	Pendulum(sf::Vector2f pos): _pos(pos)
	{
	}

	Pendulum(Pendulum* parent, float angle, float length): _parent(parent), _theta(angle), l(length)
	{
		_pos = sf::Vector2f(std::sin(angle)*l, std::cos(angle)*l)+_parent->pos();
	}

	friend std::ostream& operator<<(std::ostream&, const Pendulum&);
 
	sf::Vector2f pos();

	virtual void render(sf::VertexArray&, int) const;

	/**
	 * @param fT force of tension exerted
	 * @param idx Number of pendulums below this
	 */
	virtual void update(sf::Vector2f fT, int);
protected:
	float l; //length
	float m = 1.f; //mass
	Pendulum* _parent;
	sf::Vector2f _pos;
	float _vel = 0;
	float _acc = 0;
	float _theta; //angle of pendulum
};

struct Root : Pendulum
{
	Root(sf::Vector2f pos): Pendulum(pos)
	{
	}

	void render(sf::VertexArray&, int) const;
	void update(sf::Vector2f, int);
};