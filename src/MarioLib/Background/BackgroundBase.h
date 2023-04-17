#pragma once

#include "Common.h"

class Stage;
class BackgroundBase
{
public:
	BackgroundBase(Stage *pStage);
	virtual ~BackgroundBase();

	virtual void BuildBackground();

	// From SMB1
	void CreateCloud(int x, int y, int size);
	void CreateRedCloud(int x, int y, int size);
	void CreateMountain(int x, int y, int size);
	void CreateBush(int x, int y, int size);
	void CreateFence(int x, int y, int size);
	void CreateTree(int x, int y, int size);
	void CreateSnowTree(int x, int y, int size);

    // UnderWater
    void CreateShortCoral(int x, int y, int size);
    void CreateLongCoral(int x, int y, int size);
    void CreateWaterSky();
	
	// From MarioMaker
	void CreateDeadBush(int x, int y, int size);
	void CreateDeadBone(int x, int y, int size);
	void CreateBowserJrStatue(int x, int y, int size);
	void CreateBowserStatue(int x, int y, int size);
	void CreateCastleFence(int x, int y, int size);
    void CreateLavaLayer();
	
	void CreateGhostDeadMushroom(int x, int y, int size);
	void CreateLamp(int x, int y, int size);
	void CreateClock(int x, int y, int size);
    void CreateGhostHouseFence(int x, int y, int size);
	
	void CreateMiniBolt(int x, int y, int size);
	void CreateMiniPipe(int x, int y, int size);
	void CreateMiniFlag(int x, int y, int size);
    void CreateMiniFence(int x, int y, int size);

    int GetGroundY();

	bool IsContainHole(int x, int width);

	static BackgroundBase* CreateInstance(int nType, Stage* pStage);

	Stage* m_pStage;
};
