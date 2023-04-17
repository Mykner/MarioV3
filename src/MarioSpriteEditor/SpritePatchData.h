#pragma once

#include <vector>

class SpritePatchData
{
public:
	POINT ptFrom;
	POINT ptTo;
	int nWidth;
	int nHeight;
};

typedef std::vector<SpritePatchData> SpritePatchDataVector;