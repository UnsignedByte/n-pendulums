/*
* @Author: UnsignedByte
* @Date:   2021-04-15 16:49:10
* @Last Modified by:   UnsignedByte
* @Last Modified time: 2021-04-15 22:55:36
*/

#pragma once
#include <SFML/System.hpp>
#include <iostream>

const float DT = 0.02;
const float G = -9.8f; //acceleration due to Gravity
// const float mv = static_cast<float>(2*M_PI); // maximum velocity

float abs(sf::Vector2f v);

template <typename T> int sign(T x)
{
	return (0 < x) - (x < 0);
}

std::ostream& operator<<(std::ostream&, const sf::Vector2f);