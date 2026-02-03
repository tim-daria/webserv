/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nefimov <nefimov@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 22:30:06 by dtimofee          #+#    #+#             */
/*   Updated: 2026/02/03 15:27:06 by nefimov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "../include/Config.hpp"

int main(int argc, char *argv[]) {
	if (argc > 3)
		return (1);
	
	Config config;
	if (argc == 2)
		config.read(argv[1]);
	else
		config.read(config.default_path);
	
	std::cout << "Hello world!" << std::endl;
}
