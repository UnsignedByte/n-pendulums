/*
* @Author: UnsignedByte
* @Date:   2021-04-15 13:34:57
* @Last Modified by:   UnsignedByte
* @Last Modified time: 2021-04-15 18:42:29
*/
#include <iostream>
#include "pendulum.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include "utils.hpp"

void Pendulum::update(sf::Vector2f fT, int idx)
{
	std::cout << "updating " << *this << std::endl;
	sf::Vector2f disp = _parent->pos()-_pos;
	// (-50, 0)
	_theta = std::atan2(disp.x, disp.y);
	_acc = G*std::sin(_theta)/l; // angular acceleration
	_vel += _acc*DT;
	_theta -= _vel*DT;
	std::cout << "became " << *this << std::endl;
	_pos = sf::Vector2f(std::sin(_theta)*l, std::cos(_theta)*l) + _parent->pos();
	_parent->update(sf::Vector2f(0,0), ++idx);
}

void Pendulum::render(sf::VertexArray& arr, int idx) const
{
	std::cout << "rendering " << idx << std::endl;
	arr[idx].position = _pos;
	arr[idx].position.y *= -1;
	arr[idx].color = sf::Color::White;
	_parent->render(arr, idx+1);
}

sf::Vector2f Pendulum::pos()
{
	return _pos;
}

std::ostream& operator<<(std::ostream& os, const Pendulum& p)
{
	os << "Pendulum\nPosition: " << p._pos << "\nVelocity: " << p._vel << "\nAcceleration: " << p._acc << "\nAngle: " << p._theta;
	return os;
}

void Root::update(sf::Vector2f fT, int idx)
{
	std::cout << "updating ROOT " << idx << std::endl;
}

void Root::render(sf::VertexArray& arr, int idx) const
{
	std::cout << "rendering ROOT " << idx << std::endl;
	arr[idx].position = _pos;
	arr[idx].color = sf::Color::White;
}