#pragma once

#include "Utils.h"

std::vector<OutputDataGuideSets> GetFormingGuideSets(std::istream& fileInput);
void PrintResultGuideSets(std::ostream& fileOutput, const std::vector<OutputDataGuideSets>& outputDatas);
