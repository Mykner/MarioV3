#include "Player.h"

#include "Game.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "Camera.h"
#include "Stage.h"
#include "UserProfile.h"
#include "StagePack.h"
#include "VehicleObjectBase.h"

#include "RenderManager.h"
#include "InputManager.h"
#include "FModManager.h"
#include "SoundManager.h"

#include "DoorKey.h"

Player::Player(Game *pGame)
	: ControllerBase(pGame)
{
	m_pControlObj = nullptr;

	m_nScore = 0;
	m_nLife = 3;
	m_nCoin = 0;
	m_nTime = -1;
	m_nDoorKeyCoin = 0;
	m_nPSwitchTime = 0;

	m_pProfile = nullptr;
}

Player::~Player()
{
	if (m_pProfile != nullptr)
	{
		delete m_pProfile;
		m_pProfile = nullptr;
	}

	ClearDoorKey();
}

bool Player::IsInputHold(int nInputType, bool bApplyInputLock)
{
	bool bRet = false;
	if (!(m_bInputLock && bApplyInputLock))
		bRet = KS(m_nKeys[nInputType]);
	bRet |= m_nForceInput[nInputType];

	switch (nInputType)
	{
	case INPUT_F:
		// Moved to ControllableObject::OnWalk
		break;
	case INPUT_L:
		if (m_pGame->IsActiveAbility(ABILITY_AUTORUN))
			bRet = false;
		break;
	case INPUT_R:
		if (m_pGame->IsActiveAbility(ABILITY_AUTORUN))
		{
			switch (m_pGame->m_pGameState->m_nType)
			{
			case GAMESTATE_FLAGDOWN:
			case GAMESTATE_GOAL:
			case GAMESTATE_SINGLESTAGECLEAR:
				break;
			default:
				bRet = true;
				break;
			}
		}
		break;
	}

	return bRet;
}

void Player::OnBeforeProcess()
{
	if (m_pControlObj)
	{
		m_pControlObj->OnBeforeProcess();

		if (m_pControlObj->m_pVehicle)
		{
			GameObjectBase *pV = m_pControlObj->m_pVehicle;
			pV->OnBeforeProcess();
		}

		if (m_pControlObj->m_pCarry)
			m_pControlObj->m_pCarry->OnBeforeProcess();

		BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecDoorKey) {
			pObj->OnBeforeProcess();
		} END_VECTOR_LOOP();
	}
}

void Player::Process()
{
	if (m_pControlObj)
	{
		m_pControlObj->Process();

		if (m_pControlObj->m_pVehicle)
		{
			GameObjectBase *pV = m_pControlObj->m_pVehicle;
			pV->Process();
		}

		if (m_pControlObj->m_pCarry)
			m_pControlObj->m_pCarry->Process();

		BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecDoorKey) {
			pObj->Process();
		} END_VECTOR_LOOP();
	}
}

void Player::OnAfterProcess()
{
	if (m_pControlObj)
	{
		m_pControlObj->OnAfterProcess();

		if (m_pControlObj->m_pVehicle)
		{
			GameObjectBase *pV = m_pControlObj->m_pVehicle;
			pV->OnAfterProcess();
		}

		if (m_pControlObj->m_pCarry)
			m_pControlObj->m_pCarry->OnAfterProcess();

		BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecDoorKey) {
			pObj->OnAfterProcess();
		} END_VECTOR_LOOP();
	}
}

void Player::ProcessTime()
{
	if (m_nTime == 0)
	{
		m_pControlObj->ChangeState(STATE_DIE);
		m_pGame->ChangeState(GAMESTATE_DIE);
	}

	ProcessPSwitchTime();

	if (m_pGame->m_nGameFrame % 24 == 0)
	{
		m_nTime--;

		if (m_nTime == 100)
		{
			SOUND_MANAGER->PlayHurryUp();
		}

		if (m_nTime == 92)
		{
			SOUND_MANAGER->PlayStageBGM();
		}
	}
}

void Player::ProcessPSwitchTime()
{
	if (m_nPSwitchTime > 0)
	{
		m_nPSwitchTime--;

		if (m_nPSwitchTime == 0)
		{
			m_pGame->SwapCoinAndBrick(false);
			SOUND_MANAGER->PlayStageBGM();
		}
	}
}

void Player::RenderControlObj()
{
	if (m_pControlObj)
	{
		m_pControlObj->Render();

		if (m_pControlObj->m_pVehicle)
			m_pControlObj->m_pVehicle->Render();
		
		if (m_pControlObj->m_pCarry)
			m_pControlObj->m_pCarry->Render();

		BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecDoorKey) {
			pObj->Render();
		} END_VECTOR_LOOP();
	}
}

void Player::RenderShapeChange()
{
	if (m_pControlObj)
	{
		m_pControlObj->RenderShapeChange();

		if (m_pControlObj->m_pVehicle)
			m_pControlObj->m_pVehicle->Render();

		if (m_pControlObj->m_pCarry)
			m_pControlObj->m_pCarry->Render();

		BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecDoorKey) {
			pObj->Render();
		} END_VECTOR_LOOP();
	}
}

void Player::RenderStatus()
{
	NaString str;

	if (m_pGame->m_bIsDesignGame)
	{
		RenderDesignStatus();
	}
	else if (m_pGame->m_nStagePackType == STAGEPACK_CLASSIC)
	{
		RenderClassicStatus();		
	}
	else
	{
		RenderCustomStatus();		
	}
}

void Player::RenderClassicStatus()
{
	const int nWidth = m_pGame->Width();
	NaString str;

	NaString strName = L"MARIO";
	RENDER_MANAGER->Text(22, 14, strName, 0xffffffff, 1.0f, Z_STATUS);

	str.Format(L"%06d", m_nScore);
	RENDER_MANAGER->Text(22, 24, str, 0xffffffff, 1.0f, Z_STATUS);

	str.Format(L"$*%02d", m_nCoin);
	RENDER_MANAGER->Text(88, 24, str, 0xffffffff, 1.0f, Z_STATUS);

	RENDER_MANAGER->Text(nWidth - 112, 14, L"WORLD", 0xffffffff, 1.0f, Z_STATUS);

	Stage *pTopStage = m_pGame->m_pCurStage;
	while (pTopStage)
	{
		if (pTopStage->m_pParentStage != nullptr)
			pTopStage = pTopStage->m_pParentStage;
		else
			break;
	}

	if (pTopStage->m_strName.GetLength() >= 5)
		str.Format(L"%ls\n%ls", pTopStage->m_strName.Left(5).wstr(), pTopStage->m_strName.Mid(5).wstr());
	else if (pTopStage->m_strName.GetLength() == 4)
		str.Format(L"&%ls", pTopStage->m_strName.wstr());
	else if (pTopStage->m_strName.GetLength() <= 3)
		str.Format(L" %ls", pTopStage->m_strName.wstr());

	RENDER_MANAGER->Text(nWidth - 112, 24, str, 0xffffffff, 1.0f, Z_STATUS);

	RENDER_MANAGER->Text(nWidth - 56, 14, L"TIME", 0xffffffff, 1.0f, Z_STATUS);
	if (m_nTime >= 0)
	{
		str.Format(L"%3d", m_nTime);
		RENDER_MANAGER->Text(nWidth - 48, 24, str, 0xffffffff, 1.0f, Z_STATUS);
	}
}

void Player::RenderCustomStatus()
{
	const int nWidth = m_pGame->Width();
	NaString str;

	// Custom StagePack -> Simple Status
	str.Format(L"$*%02d", CUR_PLAYER->m_nCoin);
	RENDER_MANAGER->Text(8, 14, str, 0xffffffff, 1.0f, Z_STATUS, true);

	str.Format(L"%08d", CUR_PLAYER->m_nScore);
	RENDER_MANAGER->Text(m_pGame->Width() - 120, 14, str, 0xffffffff, 1.0f, Z_STATUS, true);

	RENDER_MANAGER->Text(m_pGame->Width() - 40, 14, L"\x0f", 0xffffffff, 1.0f, Z_STATUS, true);
	int nTime = 0;
	if (m_nTime >= 0)
		nTime = m_nTime;

	if (m_nTime <= -1)
		str.Format(L"---");
	else
		str.Format(L"%03d", nTime);
	RENDER_MANAGER->Text(m_pGame->Width() - 32, 14, str, 0xffffffff, 1.0f, Z_STATUS, true);

	Stage *pTopStage = m_pGame->m_pCurStage;
	while (pTopStage)
	{
		if (pTopStage->m_pParentStage != nullptr)
			pTopStage = pTopStage->m_pParentStage;
		else
			break;
	}

	// Render DoorKey Status
	int nMax = pTopStage->m_nMaxDoorKey;
	if (nMax > 0)
	{
		int nKeyStatusX = m_pGame->Width() - 12;
		int nKeyStatusY = 26;
		m_pGame->m_pRenderManager->m_pFontSprite->RenderToQueue(
			nKeyStatusX, nKeyStatusY, 6,
			false, false, 0xffffffff, 1.0f, 1.0f, Z_STATUS
		);
		nKeyStatusX -= 8;

		int nCur = m_nDoorKeyCoin;

		for (int i = nMax - 1; i >= 0; i--)
		{
			int nFrame = 7;
			if (i < nCur)
				nFrame = 5;
			m_pGame->m_pRenderManager->m_pFontSprite->RenderToQueue(
				nKeyStatusX, nKeyStatusY, nFrame,
				false, false, 0xffffffff, 1.0f, 1.0f, Z_STATUS
			);
			nKeyStatusX -= 8;
		}

		m_pGame->m_pRenderManager->m_pFontSprite->RenderToQueue(
			nKeyStatusX, nKeyStatusY, 4,
			false, false, 0xffffffff, 1.0f, 1.0f, Z_STATUS
		);
	}
}

void Player::RenderDesignStatus()
{
	NaString str;

	if (CUR_PLAYER_OBJ)
	{
		// Design Play
		if (m_pGame->m_nWidth > 300)
			str = L"DOUBLE CLICK / SPACE KEY TO STOP";
		else
			str = L"DOUBLE CLICK TO STOP";
		RENDER_MANAGER->Text(22, 14, str, 0xffffffff, 1.0f, Z_TEXT, true);

		str.Format(L"$*%02d", CUR_PLAYER->m_nCoin);
		RENDER_MANAGER->Text(22, 24, str, 0xffffffff, 1.0f, Z_TEXT, true);

		str.Format(L"%06d", CUR_PLAYER->m_nScore);
		RENDER_MANAGER->Text(
			m_pGame->m_nWidth - 22 - (6 * 8), 
			24, 
			str, 0xffffffff, 1.0f, Z_TEXT, true);
	}
	else
	{
		// Design
		if (m_pGame->m_nWidth > 300)
			str = L"DOUBLE CLICK / SPACE KEY TO PLAY";
		else
			str = L"DOUBLE CLICK TO PLAY";
		RENDER_MANAGER->Text(22, 14, str, 0xffffffff, 1.0f, Z_TEXT, true);

		Stage *pTopStage = CUR_STAGE->GetTopStage();
		int nSubSize = pTopStage->m_vecSubStage.size();
		if (nSubSize > 0)
		{
			int nIdx = pTopStage->GetSubStageIndex(CUR_STAGE);

			str = L"";
			for (int i = -1; i < nSubSize; i++)
			{
				bool bMatch = false;
				if (i == nIdx)
					bMatch = true;
				
				NaString strTemp;
				if (i == -1)
					strTemp.Format(L"%lsMainStage%ls", bMatch ? L"[" : L" ", bMatch ? L"]" : L" ");
				else
					strTemp.Format(L"%lsSub%d%ls", bMatch ? L"[" : L" ", (i + 1), bMatch ? L"]" : L" ");

				str += strTemp;
			}

			RENDER_MANAGER->Text(22, 24, str, 0xffffffff, 1.0f, Z_TEXT, true);
		}	
	}	
}

void Player::SetControl(GameObjectBase * pObj)
{
	m_pControlObj = pObj;
	
	if (pObj)
	{
		m_pControlObj->m_pController = this;

		// #TODO check
		pObj->m_nDamageGroup = DAMAGEGROUP_PLAYER;
	}
}

GameObjectBase * Player::GetControl()
{
	return m_pControlObj;
}

GameObjectBase * Player::GetLastControlObject()
{
	if (m_pControlObj)
	{
		if (m_pControlObj->m_pVehicle)
			return m_pControlObj->m_pVehicle;

		return m_pControlObj;
	}

	return nullptr;
}

void Player::AddScore(int nScore)
{
	m_nScore += nScore;
}

void Player::AddScoreWithEffect(int nScore, GameObjectBase* pFromObject)
{
	CreateParameter param;
	param.nScore = nScore;
	param.nType = EFFECT_SCORE;
	param.pFromObject = pFromObject;
	param.pFromPlayer = this;
	param.fYS = -1.0f;

	SIZE s = pFromObject->GetSize();
	CUR_STAGE->CreateEffect(
		pFromObject->m_fX, 
		pFromObject->m_fY - s.cy, EFFECT_SCORE, &param);
}

void Player::AddCoin()
{
	m_nCoin++;

	if (m_nCoin == 100)
	{
		m_nCoin = 0;

		AddLife();

		// #TODO move to AddLife ?
		if (m_pGame->m_bModernStyle)
		{
			CreateParameter param;
			param.nScore = 10000;
			param.fYS = -1.0f;
			CUR_STAGE->CreateEffect(
				m_pControlObj->m_fX, 
				m_pControlObj->m_fY - 16,
				EFFECT_SCORE, &param);
		}
	}
}

void Player::AddLife()
{
	m_nLife += 1;

	SOUND_MANAGER->PlaySoundEffect(L"1UP");
}

void Player::OnLostLife()
{
	m_nLife--;
	m_nDoorKeyCoin = 0;

	ClearDoorKey();
}

void Player::AddDoorKeyCoin()
{
	m_nDoorKeyCoin++;
}

void Player::AddDoorKey(GameObjectBase * pKey)
{
	GameObjectBase *pObj = GameObjectBase::CreateInstance(m_pGame, CUR_STAGE, pKey->m_nType);
	if (pObj)
	{
		pObj->m_fX = pKey->m_fX;
		pObj->m_fY = pKey->m_fY;
		pObj->m_nState = STATE_FOLLOW;

		if (m_vecDoorKey.size() == 0)
		{
			((DoorKey*)pObj)->m_pFollow = m_pControlObj;
		}
		else
		{
			((DoorKey*)pObj)->m_pFollow = m_vecDoorKey[m_vecDoorKey.size() - 1];
		}
		m_vecDoorKey.push_back(pObj);
	}
}

bool Player::HasDoorKey()
{
	return m_vecDoorKey.size() > 0;
}

void Player::UseDoorKey()
{
	if (m_vecDoorKey.size() == 0)
	{
		NaDebugOut(L"Error: Invalid call UseDoorKey!\n");
		return;
	}

	int i = 0;
	BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecDoorKey) {
		i++;

		// Remove last one
		if (i == m_vecDoorKey.size())
		{
			pObj->ChangeState(STATE_DEAD);
			CUR_STAGE->m_vecReservedGameObject.push_back(pObj);
			
			it = m_vecDoorKey.erase(it);

			return;
		}
	} END_VECTOR_LOOP();
}

void Player::ClearDoorKey()
{
	SAFE_DELETE_OBJPTR_VECTOR(GameObjectBase*, m_vecDoorKey);
}

void Player::ApplyUserProfile(UserProfile * pProfile)
{
	if (m_pProfile)
	{
		delete m_pProfile;
		m_pProfile = nullptr;
	}

	Reset();

	if (pProfile == nullptr)
		return;

	LoadUserProfile(pProfile->m_strFileName);	
}

void Player::ApplyUserProfile(NaString strName)
{
	if (m_pProfile)
	{
		delete m_pProfile;
		m_pProfile = nullptr;
	}

	Reset();

	LoadUserProfile(strName);
}

void Player::LoadUserProfile(NaString strName)
{
	m_pProfile = new UserProfile(m_pGame);
	m_pProfile->BuildPropertyMap();
	m_pProfile->m_strFileName = strName;
	m_pProfile->Load();

	if (m_pGame->m_strStagePack.Compare(L".") != 0)
	{
		StagePackRecord *pStatus = m_pProfile->GetStagePackRecord(m_pGame->m_strStagePack);

		m_nScore = pStatus->m_nScore;
		m_nLife = pStatus->m_nLife;
		m_nCoin = pStatus->m_nCoin;

		if (m_pControlObj)
			m_pControlObj->OnChangeShape(pStatus->m_nShape);
	}

	// Parse SpecialCode
	m_pGame->m_bModernStyle = false;
	m_pGame->m_nSpecialEvents = 0;

	NaStrArray ar = m_pProfile->m_strSpecialCodes.Split(L",");
	for (int i = 0; i < ar.GetCount(); i++)
	{
		if (ar[i].Compare(L"ModernStyle") == 0)
		{
			m_pGame->m_bModernStyle = true;
		}
		else if (ar[i].Compare(L"SpecialEventGiantMushroom") == 0)
		{
			m_pGame->m_nSpecialEvents |= SPECIALEVENT_GIANTMUSHROOM;
		}
		else if (ar[i].Compare(L"SpecialEventGiantPreventHole") == 0)
		{
			m_pGame->m_nSpecialEvents |= SPECIALEVENT_GIANTPREVENTHOLE;
		}
	}
}

void Player::SaveUserProfile()
{
	if (m_pProfile)
	{
		m_pProfile->Update(this);
		m_pProfile->Save();
	}
}

void Player::Reset()
{
	m_nScore = 0;
	m_nLife = 3;
	m_nCoin = 0;

	if (m_pControlObj)
	{
		m_pControlObj->m_nInvinsibleTime = 0;
		m_pControlObj->m_nGiantTime = 0;
		m_pControlObj->m_nDamageCooltime = 0;
		m_pControlObj->m_nImmuneTime = 0;

		m_pControlObj->OnChangeShape(SHAPE_MINI);

		if (m_pControlObj->m_pVehicle)
		{
			delete m_pControlObj->m_pVehicle;
			m_pControlObj->m_pVehicle = nullptr;
		}

		if (m_pControlObj->m_pCarry)
		{
			delete m_pControlObj->m_pCarry;
			m_pControlObj->m_pCarry = nullptr;
		}
	}

	ClearDoorKey();
}

void Player::ApplyTheme()
{
	if (m_pControlObj)
	{
		((ControllableObject*)m_pControlObj)->RequestSprites();

		if (m_pControlObj->m_pVehicle)
			m_pControlObj->m_pVehicle->RequestSprites();

		if (m_pControlObj->m_pCarry)
			m_pControlObj->m_pCarry->RequestSprites();
	}
}
