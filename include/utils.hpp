/*
* @Author: UnsignedByte
* @Date:   2021-04-15 16:49:10
* @Last Modified by:   UnsignedByte
* @Last Modified time: 2021-05-13 17:56:09
*/

#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
// const float mv = static_cast<float>(2*M_PI); // maximum velocity

// long double abs(sf::Vector2<long double> v);

// template <typename T> int sign(T x)
// {
// 	return (0 < x) - (x < 0);
// }

// std::ostream& operator<<(std::ostream&, const sf::Vector2<long double>);
std::ostream& operator<<(std::ostream& os, std::vector<std::vector<long double> > matrix);
std::ostream& operator<<(std::ostream& os, std::vector<long double> matrix);
std::ostream& operator<<(std::ostream& os, std::vector<std::vector<double> > matrix);
std::ostream& operator<<(std::ostream& os, std::vector<double> matrix);
std::ostream& operator<<(std::ostream& os, std::vector<int> matrix);

sf::Color operator*(sf::Color&c, double n);
sf::Color& operator*=(sf::Color&c, double n);