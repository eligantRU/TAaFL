#include "GeneratorLR.h"

GeneratorLR::GeneratorLR(const std::vector<OutputDataGuideSets>& inputDatas)
	:m_inputDatas(inputDatas)
{
	Generate();
}

void GeneratorLR::Generate()
{
}

void GeneratorLR::Print(std::ostream& fileOutput)
{
}
