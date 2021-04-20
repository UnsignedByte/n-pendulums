/*
* @Author: UnsignedByte
* @Date:   2021-04-15 16:49:16
* @Last Modified by:   UnsignedByte
* @Last Modified time: 2021-04-19 18:51:43
*/

#include <SFML/System.hpp>
#include <cmath>


long double abs(sf::Vector2<long double> v)
{
	return std::sqrt(v.x*v.x+v.y*v.y);
}

std::ostream& operator<<(std::ostream& os, const sf::Vector2<long double> v)
{
	os << "("<<v.x<<", "<<v.y<<")";
	return os;
}