/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtimofee <dtimofee@student.42berlin.de>    #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026-05-14 20:33:08 by dtimofee          #+#    #+#             */
/*   Updated: 2026-05-14 20:33:08 by dtimofee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"

LogLevel Logger::_minLevel = DEBUG;

void Logger::setLevel(LogLevel level) { _minLevel = level; }

void Logger::log(LogLevel level, const std::string& message, const std::string& file, int line) {
    if (level < _minLevel) return;

    std::ostringstream ss;
    ss << getColor(level) << "[" << getLevelText(level) << "] " << message << " (" << file << ":"
       << line << ")"
       << "\033[0m";

    std::cout << ss.str() << std::endl;
}

std::string Logger::getLevelText(LogLevel level) {
    switch (level) {
        case DEBUG:
            return "DEBUG";
        case INFO:
            return "INFO";
        case WARNING:
            return "WARNING";
        case ERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
    }
}

std::string Logger::getColor(LogLevel level) {
    switch (level) {
        case DEBUG:
            return "\033[37m";  // white
        case INFO:
            return "\033[32m";  // green
        case WARNING:
            return "\033[33m";  // yellow
        case ERROR:
            return "\033[31m";  // red
        default:
            return "\033[0m";
    }
}

std::string toString(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}
