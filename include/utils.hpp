/*
* @Author: UnsignedByte
* @Date:   2021-04-15 16:49:10
* @Last Modified by:   UnsignedByte
* @Last Modified time: 2021-04-19 23:29:11
*/

#pragma once
#include <SFML/System.hpp>
#include <iostream>

const long double DT = 0.02;
const long double G = 9.8L; //acceleration due to Gravity
// const float mv = static_cast<float>(2*M_PI); // maximum velocity

long double abs(sf::Vector2<long double> v);

template <typename T> int sign(T x)
{
	return (0 < x) - (x < 0);
}

std::ostream& operator<<(std::ostream&, const sf::Vector2<long double>);