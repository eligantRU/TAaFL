#pragma once

#include "Structs.h"

std::vector<OutputDataGuideSets> GetFormingGuideSets(std::istream& fileInput);
void PrintResultGuideSets(std::ostream& fileOutput, const std::vector<OutputDataGuideSets>& outputDatas);
void ValidateGuideCharacters(const std::vector<OutputDataGuideSets>& outputDatas);
