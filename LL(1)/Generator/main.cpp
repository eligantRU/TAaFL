#include <iostream>
#include <fstream>
#include <ctime>

#include "GuideSets.h"
#include "Generator.h"

int main(int argc, char* argv[])
{
	std::srand(unsigned(std::time(nullptr)));
	if (argc != 3)
	{
		std::cerr << "Invalid input, should be: <exe> <input> <output>" << std::endl;
		return 1;
	}

	std::ifstream input(argv[1]);
	std::ofstream output(argv[2]);

	std::ofstream guideSetsOut("output_guideSets.txt");

	if (!input.is_open())
	{
		std::cerr << "<input> is not opened" << std::endl;
		return 1;
	}

	std::vector<OutputDataGuideSets> outputDatasSets = GetFormingGuideSets(input);
	PrintResultGuideSets(guideSetsOut, outputDatasSets);

	std::vector<OutputData> outputDatas = GetGenerateData(outputDatasSets);
	PrintResult(output, outputDatas);
}
