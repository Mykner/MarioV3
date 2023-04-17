#pragma once

#include "Common.h"

#ifdef WIN32
enum {
	VK_0 = 0x30,
	VK_1,
	VK_2,
	VK_3,
	VK_4,
	VK_5,
	VK_6,
	VK_7, 
	VK_8,
	VK_9,

	VK_A = 0x41,
	VK_B,
	VK_C,
	VK_D,
	VK_E,
	VK_F,
	VK_G,
	VK_H,
	VK_I,
	VK_J,
	VK_K,
	VK_L,
	VK_M,
	VK_N,
	VK_O,
	VK_P,
	VK_Q,
	VK_R,
	VK_S,
	VK_T,
	VK_U,
	VK_V,
	VK_W,
	VK_X,
	VK_Y,
	VK_Z,
};
#endif

class Game;
class InputManager
{
public:
	InputManager(Game *pGame);
	virtual ~InputManager();

	void Init();
	void Release();

	void ClearStatus();
	bool IsKeyPressed(int k);
	bool IsKeyHold(int k);

	bool IsMouseClicked(NaRect rc);
	void OnClick();

	void Process();
	void OnAfterProcess();

	bool m_bOldInput[256];
	bool m_bCurInput[256];
	int m_nHoldInput[256];

	bool m_bMouseClick;
	POINT m_ptMouse;
	POINT m_ptLButtonDown;
	POINT m_ptLButtonUp;
	int m_nWheelMoved;

	Game *m_pGame;
};

