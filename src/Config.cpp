/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nefimov <nefimov@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/03 14:48:03 by nefimov           #+#    #+#             */
/*   Updated: 2026/02/03 15:34:53 by nefimov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include <iostream>

Config::Config() {
	std::cout << "Config default constructor called" << std::endl;
	path = default_path;
}

Config::Config(const char* fpath) {
	std::cout << "Config cstring constructor called" << std::endl;
	std::string tmp(fpath);
	this->path = tmp;
}

Config::Config(const std::string &path) {
	std::cout << "Config string constructor called" << std::endl;
	this->path = path;
}

Config::Config(const Config &other) {
	std::cout << "Config copy constructor called" << std::endl;
	*this = other;
}

Config Config::operator=(const Config &other) {
	std::cout << "Config assignment operator called" << std::endl;
	if (this != &other)
		this->path = other.path;

	return (*this);
}

// Getters and Setters
void Config::setPath(const std::string &path) {
	this->path = path;
}

std::string Config::getPath() const {
	return (path);	
}

// Read configuration file
int Config::read(const char* fpath) {
	std::cout << "Read method called: " << fpath << std::endl;
}

int Config::read(void) {
	read(path.c_str());
}