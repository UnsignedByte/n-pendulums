/*
* @Author: UnsignedByte
* @Date:   2021-04-15 13:34:57
* @Last Modified by:   UnsignedByte
* @Last Modified time: 2021-04-20 13:46:18
*/
#include <iostream>
#include "pendulum.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include "utils.hpp"

void Pendulum::update(sf::Vector2<long double> fT)
{
	// std::cout << "updating " << *this << std::endl;
	sf::Vector2<long double> disp = _pos-_parent->pos();
	// std::cout << "updating " << disp << std::endl;
	_theta = std::atan2(disp.y, disp.x);

	fT+=sf::Vector2<long double>(0, G*m);

	long double fTdisptheta = std::atan2(fT.y, fT.x)-_theta;
	_vel += (abs(fT)*std::sin(fTdisptheta))/(m*l)*DT;
	// _vel = std::max(std::min(_vel, mv), -mv);
	// _vel *= 1-DT*0.01;
	_fT = sf::Vector2<long double>(std::cos(_theta), std::sin(_theta))*(std::cos(fTdisptheta)*abs(fT)+m*_vel*_vel/l); //update with new tension force
	_theta += _vel*DT;
	// std::cout << "became " << *this << std::endl;
	_pos = _parent->pos()+sf::Vector2<long double>(std::cos(_theta)*l, std::sin(_theta)*l);
	_parent->update(_fT);
}

void Pendulum::render(sf::VertexArray& arr, int idx)
{
	// std::cout << "rendering " << idx << std::endl;
	arr[idx].position = sf::Vector2f(_pos);
	arr[idx].color = sf::Color::White;
	_parent->render(arr, idx+1);
}

sf::Vector2<long double> Pendulum::pos()
{
	return _pos;
}

std::ostream& operator<<(std::ostream& os, const Pendulum& p)
{
	os << "Pendulum\nPosition: " << p._pos << "\nVelocity: " << p._vel << "\nAcceleration: " << p._fT << "\nAngle: " << p._theta;
	return os;
}

void Root::update(sf::Vector2<long double> fT)
{
	// std::cout << "updating ROOT " << idx << std::endl;
}

void Root::render(sf::VertexArray& arr, int idx)
{
	// std::cout << "rendering ROOT " << idx << std::endl;
	arr[idx].position = sf::Vector2f(_pos);
	arr[idx].color = sf::Color::White;
}