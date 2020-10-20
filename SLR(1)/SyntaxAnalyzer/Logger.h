#pragma once
#include <fstream>
#include <string>
#include <vector>

class Logger
{
public:
	Logger(const std::string fileName);
	void Log(std::string str);
	void Print();

private:
	std::ofstream m_outputFile;
	std::vector<std::string> m_logger;
};
