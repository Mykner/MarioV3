#include "ControllerBase.h"

#include "Game.h"
#include "InputManager.h"

ControllerBase::ControllerBase(Game *pGame)
{
	m_pGame = pGame;

	m_bInputLock = false;
    for (int i=0; i<6; i++)
    {
        m_nForceInput[i] = false;
    }
}

ControllerBase::~ControllerBase()
{
}

void ControllerBase::BindInputs(int l, int r, int u, int d, int j, int f)
{
	m_nKeys[INPUT_L] = l;
	m_nKeys[INPUT_R] = r;
	m_nKeys[INPUT_U] = u;
	m_nKeys[INPUT_D] = d;
	m_nKeys[INPUT_J] = j;
	m_nKeys[INPUT_F] = f;
}

void ControllerBase::BindInputs(int ar[6])
{
	m_nKeys[INPUT_L] = ar[0];
	m_nKeys[INPUT_R] = ar[1];
	m_nKeys[INPUT_U] = ar[2];
	m_nKeys[INPUT_D] = ar[3];
	m_nKeys[INPUT_J] = ar[4];
	m_nKeys[INPUT_F] = ar[5];
}

bool ControllerBase::IsInputHold(int nInputType, bool bApplyInputLock)
{
	bool bRet = false;
	if (!(m_bInputLock && bApplyInputLock))
		bRet = KS(m_nKeys[nInputType]);
	bRet |= m_nForceInput[nInputType];

	return bRet;
}

bool ControllerBase::IsInputPressed(int nInputType, bool bApplyInputLock)
{
	if (bApplyInputLock && m_bInputLock)
		return false;
	return KE(m_nKeys[nInputType]);
}

void ControllerBase::LockInput()
{
	m_bInputLock = true;
}

void ControllerBase::UnlockInput()
{
	m_bInputLock = false;
}

void ControllerBase::HoldInput(int nInputType)
{
	m_nForceInput[nInputType] = true;
}

void ControllerBase::UnholdInput(int nInputType)
{
	m_nForceInput[nInputType] = false;
}

void ControllerBase::UnholdInputAll()
{
	for (int i = 0; i < 6; i++)
		m_nForceInput[i] = false;
}
