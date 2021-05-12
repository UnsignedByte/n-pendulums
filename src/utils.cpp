/*
* @Author: UnsignedByte
* @Date:   2021-04-15 16:49:16
* @Last Modified by:   UnsignedByte
* @Last Modified time: 2021-05-08 17:56:18
*/

// #include <SFML/System.hpp>
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