/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtimofee <dtimofee@student.42berlin.de>    #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-05-14 20:33:02 by dtimofee          #+#    #+#             */
/*   Updated: 2026-05-14 20:33:02 by dtimofee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <sstream>
#include <string>

enum LogLevel { DEBUG, INFO, WARNING, ERROR };

class Logger {
   private:
    static LogLevel _minLevel;

    static std::string getLevelText(LogLevel level);
    static std::string getColor(LogLevel level);

   public:
    static void log(LogLevel level, const std::string& message, const std::string& file, int line);
    static void setLevel(LogLevel level);
};

#define LOG_DEBUG(msg) Logger::log(DEBUG, msg, __FILE__, __LINE__)
#define LOG_INFO(msg) Logger::log(INFO, msg, __FILE__, __LINE__)
#define LOG_WARNING(msg) Logger::log(WARNING, msg, __FILE__, __LINE__)
#define LOG_ERROR(msg) Logger::log(ERROR, msg, __FILE__, __LINE__)
