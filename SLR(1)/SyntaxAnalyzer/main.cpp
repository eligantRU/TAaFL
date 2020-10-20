#include "LRReader.h"
#include "SyntacticalAnalyzer.h"

int main(int argc, char* argv[])
{
	if (argc != 4)
	{
		std::cout << "The number of arguments does not match the task condition\n"
					 "Input should look: SyntacticalAnalyzer.exe <grammar> <table> <sentence>\n";
		return 1;
	}

	try
	{
		std::ifstream fileGuideSetsInput(argv[1]);
		std::ifstream fileTableInput(argv[2]);
		std::ifstream fileSentenceInput(argv[3]);

		if (!fileTableInput.is_open() || !fileSentenceInput.is_open())
		{
			std::cerr << "This file does not exist" << std::endl;
			return 1;
		}

		LRReader lrReader;
		lrReader.ReadGuideSets(fileGuideSetsInput);
		lrReader.ReadSentence(fileSentenceInput);
		lrReader.ReadTable(fileTableInput);

		auto guideSets = lrReader.GetGuideSets();
		auto headerSymbols = lrReader.GetHeaderSymbols();
		auto lrData = lrReader.GetLRData();
		auto sentence = lrReader.GetSentence();

		SyntacticalAnalyzer analyzer(guideSets, headerSymbols, lrData, sentence);
		analyzer.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}
