#pragma once

#include "Common.h"

class Game;
class GameObjectBase;

enum CameraPin
{
	CAMERA_PIN_NORMAL = 0,
	CAMERA_PIN_BACKGROUND,
	CAMERA_PIN_FARBACKGROUND,
};

class Camera
{
public:
	Camera(Game *pGame);
	virtual ~Camera();

	void LookAt(GameObjectBase *pTarget);
	void Process();
	void ProcessPlayerBound();
	void ProcessLimit();

	NaPointT<float> GetPosition(int nPin = CAMERA_PIN_NORMAL);
	void SetPosition(float fX, float fY);

	NaRect GetViewport();
	NaRect GetTileViewport();
	void CheckViewport();
	
	bool IsOutOfCamera(GameObjectBase *pTarget);
	void MakeEarthquake(int nPower = 2);
	void StopEarthquake();

    NaPointT<float> GetScrollSpeed();

	/*
	union {
		POINT m_pt;
		POINT m_ptPos;
	};
	*/

	NaPointT<float> m_ptPos;
	float m_fXS;
	float m_fYS;

	NaRect m_rcViewport;

	int m_nEarthQuakePower;
	int m_nEarthQuakeCooltime;
	POINT m_ptEarthQuakeOffset;
	POINT m_ptAirShipOffset;

	GameObjectBase *m_pTarget;
	Game *m_pGame;
};

