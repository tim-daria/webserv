#pragma once

#include <string>

void createFile(const std::string& path, const std::string& content);
void createDir(const std::string& path);
void removeFile(const std::string& path);
void removeDir(const std::string& path);
