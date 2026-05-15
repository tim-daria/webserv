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

#include <catch.hpp>
#include <fstream>

#include "sys/stat.h"
#include "unistd.h"

// Helpers
static void createFile(const std::string& path, const std::string& content) {
    std::ofstream f(path.c_str());
    f << content;
}

static void createDir(const std::string& path) { mkdir(path.c_str(), 0755); }

static void removeFile(const std::string& path) { ::unlink(path.c_str()); }

static void removeDir(const std::string& path) { ::rmdir(path.c_str()); }
