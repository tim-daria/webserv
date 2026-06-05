/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tsemenov <tsemenov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/14 20:33:02 by dtimofee          #+#    #+#             */
/*   Updated: 2026/05/29 12:33:11 by tsemenov         ###   ########.fr       */
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

std::string toString(int value);

// Helper struct: calling a non-const member on a temporary is valid C++98,
// unlike binding a temporary to std::ostream& (which is not).
struct LogStrHelper {
    std::ostringstream ss;
    template<typename T>
    LogStrHelper& operator<<(const T& v) { ss << v; return *this; }
    std::string str() const { return ss.str(); }
};

// Helper: build a string from a stream expression, e.g. LOG_DEBUG("fd " << fd)
#define LOG_STR(expr) (LogStrHelper() << expr).str()

#define LOG_DEBUG(msg)   Logger::log(DEBUG,   LOG_STR(msg), __FILE__, __LINE__)
#define LOG_INFO(msg)    Logger::log(INFO,     LOG_STR(msg), __FILE__, __LINE__)
#define LOG_WARNING(msg) Logger::log(WARNING,  LOG_STR(msg), __FILE__, __LINE__)
#define LOG_ERROR(msg)   Logger::log(ERROR,    LOG_STR(msg), __FILE__, __LINE__)
