#pragma once

#include <ostream>
#include <vector>

#include "Structs.h"

std::vector<OutputData> GetGenerateData(const std::vector<OutputDataGuideSets>& inputDatas);
void PrintResult(std::ostream& fileOutput, const std::vector<OutputData>& outputDatas);
