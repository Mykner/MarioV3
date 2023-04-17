#pragma once

#include "Common.h"

class PipeInfo
{
public:
	PipeInfo();
	virtual ~PipeInfo();

	NaRect m_rcEnterance;
	NaString m_strStageName;
	int m_nType;
	int m_nWarpType;
	int m_nExitIndex;
};

class PipeExitInfo
{
public:
	PipeExitInfo();
	virtual ~PipeExitInfo();

	NaRect m_rcExit;
	int m_nType;
	int m_nIndex;
};

