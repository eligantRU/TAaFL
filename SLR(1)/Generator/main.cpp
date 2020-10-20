#include "GeneratorSLR.h"
#include "GuideSets.h"
#include <ctime>
#include <iostream>

namespace
{

void Bla(std::ostream& fileOutput, const std::vector<OutputDataGuideSets>& outputDatas)
{
	for (const auto& outputData : outputDatas)
	{
		fileOutput << outputData.nonterminal << SPACE << DELIMITER << SPACE;
		PrintInfoVector(fileOutput, outputData.terminals, SPACE);
		fileOutput << std::endl;
	}
}

}

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		std::cerr << "The number of arguments does not match the task condition\n"
					 "Input should look: GeneratorLR1.exe <input file> <output file>\n";
		return 1;
	}

	std::ifstream fileInput(argv[1]);
	std::ofstream fileOutput(argv[2]);

	if (!fileInput.is_open())
	{
		std::cerr << "This file does not exist" << std::endl;
		return 1;
	}

	std::srand(unsigned(std::time(nullptr)));

	const auto bla = GetFormingGuideSets(fileInput);

	std::ofstream grammarStrm("grammar.txt");
	Bla(grammarStrm, bla);

	GeneratorSLR generator(bla);
	generator.Print(fileOutput);
}
