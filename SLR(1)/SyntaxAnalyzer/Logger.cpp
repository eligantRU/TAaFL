#include "Logger.h"

Logger::Logger(const std::string fileName)
{
	m_outputFile = std::ofstream(fileName);
}

void Logger::Log(std::string str)
{
	m_logger.push_back(str);
}

void Logger::Print()
{
	for (auto str : m_logger)
	{
		m_outputFile << str << std::endl;
	}
}