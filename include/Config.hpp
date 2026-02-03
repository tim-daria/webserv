/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nefimov <nefimov@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/03 14:48:11 by nefimov           #+#    #+#             */
/*   Updated: 2026/02/03 15:33:35 by nefimov          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <cstring>

class Config {
public:
	Config();
	Config(const char* path);
	Config(const std::string &path);
	Config(const Config &other);
	~Config();

	Config operator=(const Config &other);

	void setPath(const std::string &path);
	std::string getPath() const;	
		
	const char* default_path = "webserv.conf";

	int read(const char* fpath);
	int read(void);
	
private:
	std::string path;
};

#endif  // CONFIG_HPP