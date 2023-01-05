#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <string>

enum LogType
{
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR
};

struct LogEntry
{
	LogType type;
	std::string message;
};

class Logger
{
private:
	static std::vector<LogEntry> messages;

public:
	static std::string CurrentDateTimeToString();
	static void Log(const std::string &message);
	static void Err(const std::string &message);
};