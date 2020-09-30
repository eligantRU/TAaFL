#include <iostream>
#include <fstream>
#include <ctime>

#include "GuideSets.h"
#include "Generator.h"

int main(int argc, char* argv[])
{
	try
	{
		std::srand(unsigned(std::time(nullptr)));
		if (argc != 3)
		{
			std::cerr << "Invalid input, should be: <exe> <input> <output>" << std::endl;
			return 1;
		}

		std::ifstream input(argv[1]);
		std::ofstream output(argv[2]);

		if (!input.is_open())
		{
			std::cerr << "<input> is not opened" << std::endl;
			return 1;
		}

		std::vector<OutputDataGuideSets> outputDatasSets = GetFormingGuideSets(input);
	
		std::ofstream guideSetsOut("output_guideSets.txt");
		PrintResultGuideSets(guideSetsOut, outputDatasSets);
		ValidateGuideCharacters(outputDatasSets);

		std::vector<OutputData> outputDatas = GetGenerateData(outputDatasSets);
		PrintResult(output, outputDatas);
	}
	catch (const std::exception & ex)
	{
		std::cerr << ex.what() << std::endl;
	}
}
