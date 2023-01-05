#include "Logger.h"
#include <iomanip>

std::vector<LogEntry> Logger::messages;

std::string Logger::CurrentDateTimeToString()
{
	std::time_t t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::stringstream returnValue;
	returnValue << std::put_time(&tm, "%Y/%m/%d %H:%M:%S");
	return returnValue.str();
}

void Logger::Log(const std::string &message)
{
	LogEntry logEntry;
	logEntry.type = LOG_INFO;

	std::stringstream fullMessage;
	fullMessage << "\033[32mLOG: [" << CurrentDateTimeToString() << "] " << message;

	logEntry.message = fullMessage.str();

	messages.push_back(logEntry);

	std::cout << "\033[32m" << fullMessage.str() << std::endl;
}

void Logger::Err(const std::string &message)
{
	LogEntry logEntry;
	logEntry.type = LOG_INFO;

	std::stringstream fullMessage;
	fullMessage << "[" << CurrentDateTimeToString() << "] " << message;

	logEntry.message = fullMessage.str();
	messages.push_back(logEntry);

	std::cout << "\033[31mERR: " << fullMessage.str()
			  << std::endl;
}