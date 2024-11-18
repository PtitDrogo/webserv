/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilbendib <ilbendib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/11 12:48:17 by ilbendib          #+#    #+#             */
/*   Updated: 2024/11/14 14:46:17 by ilbendib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "location.hpp"

location::location() {}

location::~location() {}

location::location(const location &copy)
{
	*this = copy;
}

location &location::operator=(const location &copy)
{
	if (this != &copy)
	{
		this->_index = copy._index;
		this->path = copy.path;
	}
	return *this;
}

std::string location::getIndex() const
{
	return (this->_index);
}

std::string location::getPath() const
{
	return (this->path);
}

void location::setIndex(std::string index)
{
	this->_index = index;
}

void location::setPath(std::string path)
{
	this->path = path;
}