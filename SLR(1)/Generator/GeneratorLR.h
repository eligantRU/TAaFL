#pragma once

#include "Utils.h"

class GeneratorLR
{
public:
	GeneratorLR(const std::vector<OutputDataGuideSets>& inputDatas);

	void Generate();
	void Print(std::ostream& fileOutput);

private:
	std::vector<OutputDataGuideSets> m_inputDatas;
};
