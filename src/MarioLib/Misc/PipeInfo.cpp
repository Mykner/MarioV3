#include "PipeInfo.h"

PipeInfo::PipeInfo()
{
	m_nType = PIPEENTRANCE_UPTODOWN;
	m_nExitIndex = -1;
	m_nWarpType = PIPE_NONE;
	m_strStageName = L"";
}

PipeInfo::~PipeInfo()
{
}

PipeExitInfo::PipeExitInfo()
{
	m_nType = PIPEENTRANCE_UPTODOWN;
	m_nIndex = -1;
}

PipeExitInfo::~PipeExitInfo()
{
}
