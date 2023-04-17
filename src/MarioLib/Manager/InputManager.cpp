#include "InputManager.h"

#include "Game.h"
#include "NaPlatformAPI.h"

#ifdef WIN32
#else
extern bool g_bKeyMap[256];
#endif

InputManager::InputManager(Game *pGame)
{
	m_pGame = pGame;

	m_bMouseClick = false;
    m_ptLButtonDown = { -1, -1 };
    m_ptLButtonUp = { -1, -1 };
    m_ptMouse = { -1, -1 };
	m_nWheelMoved = 0;
}

InputManager::~InputManager()
{
}

void InputManager::Init()
{
	
}

void InputManager::Release()
{
	
}

void InputManager::ClearStatus()
{
	for (int i = 0; i < 256; i++)
	{
		m_bOldInput[i] = false;
		m_bCurInput[i] = false;
	}
}

bool InputManager::IsKeyPressed(int k)
{
	if (!m_bOldInput[k] && m_bCurInput[k])
	{ 
		return true;
	}

	return false;
}

bool InputManager::IsKeyHold(int k)
{
	if (m_bCurInput[k]) 
	{
		return true;
	}

	return false;
}

bool InputManager::IsMouseClicked(NaRect rc)
{
	if (m_bMouseClick)
	{
		if (rc.ptInRect(m_ptLButtonDown.x, m_ptLButtonDown.y) &&
			rc.ptInRect(m_ptLButtonUp.x, m_ptLButtonUp.y))
			return true;
	}

	return false;
}

void InputManager::OnClick()
{
	m_bMouseClick = true;
}

void InputManager::Process()
{
	NaPlatformWindowHandle hTopWindow = NaPlatform::GetTopWindow(m_pGame->m_hWindow);

	bool bActive = NaPlatform::IsActiveWindow(hTopWindow);
	for (int i = 0; i<256; i++) 
	{
		m_bOldInput[i] = m_bCurInput[i];
		if (bActive)
		{
#if defined(WIN32)
			m_bCurInput[i] = GetAsyncKeyState(i) ? 1 : 0;
#else
			m_bCurInput[i] = g_bKeyMap[i];
#endif
		}
		else
		{
			m_bCurInput[i] = 0;
		}

		if (m_bOldInput[i] && m_bCurInput[i])
			m_nHoldInput[i]++;
		else
			m_nHoldInput[i] = 0;
	}
}

void InputManager::OnAfterProcess()
{
	m_bMouseClick = false;
	m_nWheelMoved = 0;
    
#ifdef WIN32
#else
    for (int i = 0; i<256; i++)
    {
        //m_bCurInput[i] = 0;
    }
#endif            
}
