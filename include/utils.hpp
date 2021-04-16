/*
* @Author: UnsignedByte
* @Date:   2021-04-15 16:49:10
* @Last Modified by:   UnsignedByte
* @Last Modified time: 2021-04-15 18:37:38
*/

#pragma once
#include <SFML/System.hpp>
#include <iostream>

const float DT = 1;
const float G = -5.f; //acceleration due to Gravity

float abs(sf::Vector2f v);

template <typename T> int sign(T x)
{
	return (0 < x) - (x < 0);
}

std::ostream& operator<<(std::ostream&, const sf::Vector2f);