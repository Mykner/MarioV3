#pragma once

#include "Common.h"
#include "BackgroundBase.h"

class Stage;
class BackgroundCastle : public BackgroundBase
{
public:
	BackgroundCastle(Stage *pStage);
	virtual ~BackgroundCastle();

	virtual void BuildBackground();
};
