/*
* @Author: UnsignedByte
* @Date:   2021-04-15 16:49:16
* @Last Modified by:   UnsignedByte
* @Last Modified time: 2021-05-13 18:07:58
*/

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <vector>

// long double abs(sf::Vector2<long double> v)
// {
// 	return std::sqrt(v.x*v.x+v.y*v.y);
// }

// std::ostream& operator<<(std::ostream& os, const sf::Vector2<long double> v)
// {
// 	os << "("<<v.x<<", "<<v.y<<")";
// 	return os;
// }

std::ostream& operator<<(std::ostream& os, std::vector<std::vector<long double> > matrix)
{
	for(int i = 0; i < matrix.size(); i++){
		for(int j = 0; j < matrix[i].size(); j++){
			os << std::to_string(matrix[i][j]).substr(0,8) << " ";
		}
		os << "\b\n";
	}
	return os << '\b';
}

std::ostream& operator<<(std::ostream& os, std::vector<long double> matrix)
{
	for(int i = 0; i < matrix.size(); i++){
		os << std::to_string(matrix[i]).substr(0,8) << " ";
	}
	return os << '\b';
}

std::ostream& operator<<(std::ostream& os, std::vector<std::vector<double> > matrix)
{
	for(int i = 0; i < matrix.size(); i++){
		for(int j = 0; j < matrix[i].size(); j++){
			os << std::to_string(matrix[i][j]).substr(0,8) << " ";
		}
		os << "\b\n";
	}
	return os << '\b';
}

std::ostream& operator<<(std::ostream& os, std::vector<double> matrix)
{
	for(int i = 0; i < matrix.size(); i++){
		os << std::to_string(matrix[i]).substr(0,8) << " ";
	}
	return os << '\b';
}

std::ostream& operator<<(std::ostream& os, std::vector<int> matrix)
{
	for(int i = 0; i < matrix.size(); i++){
		os << matrix[i] << " ";
	}
	return os << '\b';
}

sf::Color operator*(sf::Color&c, double n)
{
	return sf::Color((int)n*c.r, (int)n*c.g, (int)n*c.b);
}

sf::Color& operator*=(sf::Color&c, double n)
{
	// n = 0.1;
	c.r*=n;
	c.g*=n;
	c.b*=n;
	return c;
}