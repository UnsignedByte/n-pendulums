/*
* @Author: UnsignedByte
* @Date:   2021-04-15 16:49:16
* @Last Modified by:   UnsignedByte
* @Last Modified time: 2021-04-15 18:14:48
*/

#include <SFML/System.hpp>
#include <cmath>


float abs(sf::Vector2f v)
{
	return std::sqrt(v.x*v.x+v.y*v.y);
}

std::ostream& operator<<(std::ostream& os, const sf::Vector2f v)
{
	os << "("<<v.x<<", "<<v.y<<")";
	return os;
}