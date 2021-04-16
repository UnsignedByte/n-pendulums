/*
* @Author: UnsignedByte
* @Date:   2021-04-15 13:34:57
* @Last Modified by:   UnsignedByte
* @Last Modified time: 2021-04-15 22:49:32
*/
#include <iostream>
#include "pendulum.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include "utils.hpp"

void Pendulum::update(sf::Vector2f fT, int idx)
{
	// std::cout << "updating " << *this << std::endl;
	sf::Vector2f disp = _parent->pos()-_pos;
	// std::cout << "updating " << disp << std::endl;
	_theta = std::atan2(disp.x, disp.y);

	float fTdisptheta = _theta-std::atan2(fT.x, fT.y);
	_acc = (-G*m*std::sin(_theta)+sign(fT.y)*abs(fT)*std::sin(fTdisptheta))/(l*m); // angular acceleration
	_vel += _acc*DT;
	// _vel = std::max(std::min(_vel, mv), -mv);
	// _vel *= 1-DT*0.01;
	fT = sf::Vector2f(std::sin(_theta), std::cos(_theta))*(std::cos(_theta)*G*m+std::cos(fTdisptheta)*abs(fT)); //update with new tension force
	_theta -= _vel*DT;
	// std::cout << "became " << *this << std::endl;
	_pos = _parent->pos()-sf::Vector2f(std::sin(_theta)*l, std::cos(_theta)*l);
	_parent->update(fT, ++idx);
}

void Pendulum::render(sf::VertexArray& arr, int idx)
{
	// std::cout << "rendering " << idx << std::endl;
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
	// std::cout << "updating ROOT " << idx << std::endl;
}

void Root::render(sf::VertexArray& arr, int idx)
{
	// std::cout << "rendering ROOT " << idx << std::endl;
	arr[idx].position = _pos;
	arr[idx].color = sf::Color::White;
}