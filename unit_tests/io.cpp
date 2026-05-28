/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtimofee <dtimofee@student.42berlin.de>    #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-05-15 10:58:02 by dtimofee          #+#    #+#             */
/*   Updated: 2026-05-15 10:58:02 by dtimofee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "io.hpp"

#include <fstream>

#include "sys/stat.h"
#include "unistd.h"

// Helpers
void createFile(const std::string& path, const std::string& content) {
    std::ofstream f(path.c_str());
    f << content;
}

void createDir(const std::string& path) { mkdir(path.c_str(), 0755); }

void removeFile(const std::string& path) { ::unlink(path.c_str()); }

void removeDir(const std::string& path) { ::rmdir(path.c_str()); }
