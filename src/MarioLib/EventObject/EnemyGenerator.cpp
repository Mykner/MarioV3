#include "EnemyGenerator.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "FModManager.h"
#include "Stage.h"
#include "Camera.h"
#include "SoundManager.h"

#include "TypeNames.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(EnemyGenerator)
	PROP_INT("EnemyType",		VT_I4,	-1,		g_szEnemyTypes,					"EnemyGenerator"),
	PROP_INT("GenerateType",	VT_I4,	-1,		g_szGeneratableTypes,			"EnemyGenerator"),
	PROP_INT("Interval",		VT_I4,	1,		0,								"EnemyGenerator"),
	PROP_INT("Chance",			VT_I4,	100,	0,								"EnemyGenerator"),
	PROP_INT("MaxCount",		VT_I4,	3,		0,								"EnemyGenerator"),
	PROP_INT("Range",			VT_I4,	0,		g_szEnemyGeneratorRangeTypes,	"EnemyGenerator"),
	PROP_INT("GeneratePosition", VT_I4, 0,		g_szEnemyGeneratorPositions,	"EnemyGenerator"),
END_IMPL_PROPERTYMAP()

EnemyGenerator::EnemyGenerator(Game *pGame, Stage *pStage) :
	EventObjectBase(pGame, pStage)
{
	m_nType = EVENTOBJECT_ENEMYGENERATOR;
	m_nState = STATE_IDLE;

	m_nEnemyType = -1;
	m_nInterval = 1;
	m_nChance = 100;
	m_nMaxCount = 3;
	m_nRange = 0;
	m_nGeneratePosition = 0;

	m_nGenerateTypeIndex = -1;
	m_nGenerateType = -1;
}

EnemyGenerator::~EnemyGenerator()
{
}

void EnemyGenerator::OnBeforeProcess()
{
	CheckCollision();
	//CheckCollisionPlayer();
}

void EnemyGenerator::OnAfterProcess()
{
	m_fX += m_fXS;
	m_fY += m_fYS;

	m_nStateFrame++;
	if (m_nNextState != -1)
	{
		m_nState = m_nNextState;
		m_nNextState = -1;
		m_nStateFrame = 0;

		OnChangeState(m_nState);
	}
}

void EnemyGenerator::Render(int nColor, int nZOrder)
{
	/*
	NaPointT<float> pt = CAMERA->GetPosition();
	int _x = x - pt.x;
	int _y = y - pt.y;

	NaString str;
	str.Format(L"EnemyGenerator");
	DRAWTEXT(_x, _y, str);

	// For Test
	Sprite *pSprite = SPR_MANAGER->Request(L"Design");
	pSprite->RenderToQueue(_x, _y, SPRIDX_DESIGN_ENEMYGENERATOR,
		false, false, nColor, 1.0f, 1.0f, Z_COMMON_OBJ);
	*/
}

void EnemyGenerator::RenderDesign(int nColor, int nZOrder)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	int _x = x - pt.x;
	int _y = y - pt.y;
	
	Sprite *pSprite = SPR_MANAGER->Find(L"Design");
	pSprite->RenderToQueue(_x, _y, SPRIDX_DESIGN_ENEMYGENERATOR,
			false, false, nColor, 1.0f, 1.0f, Z_COMMON_OBJ);
}

void EnemyGenerator::OnIdle()
{
	if (IsOutOfCamera())
		return;

	// #TODO Degradation. How to solve this?
	/*
	NaRect rcTile = GetTileRect();
	if (CUR_STAGE->IsHardTile(rcTile.left, rcTile.top) ||
		CUR_STAGE->IsHardTile(rcTile.left, rcTile.bottom) ||
		CUR_STAGE->IsHardTile(rcTile.right, rcTile.top) ||
		CUR_STAGE->IsHardTile(rcTile.right, rcTile.bottom))
		return;
		*/

	if (m_nGenerateType == -1 && m_nEnemyType != -1)
		m_nGenerateType = m_nEnemyType + ENEMY_BEGIN;
	else
	{
		if (m_nGenerateTypeIndex != -1)
		{
			m_nGenerateType = g_nGeneratableTypes[m_nGenerateTypeIndex];
		}
	}
	
	if (m_nStateFrame % (m_nInterval * 24) == 0 && 
		rand() % 100 <= m_nChance)
	{
		if (CAMERA->IsOutOfCamera(this))
			return;

		if (m_nGenerateType != 0)
		{
			int nCount = CUR_STAGE->GetObjectCount(m_nGenerateType, true);
			SIZE s = GetSize();
			if (nCount < m_nMaxCount)
			{
				NaRect rcView = CAMERA->GetViewport();
				int nGenX;
				int nGenY;
				switch (m_nGeneratePosition)
				{
				case 0:
				case 3:
					// Static
					nGenX = m_fX + (s.cx / 2);
					nGenY = m_fY + s.cy;
					break;
				case 1:
					{
						// Bottom Of Screen
						//nGenX = m_fX + (s.cx / 2);
						nGenX = rcView.left + (rand() % rcView.Width());
						nGenY = GameDefaults::nPageHeight;
					}
					break;
				case 2:
					{
						// Right Of Screen
						rcView.top += 32; // Info Text Area
						rcView.top += 16; // Obj's height

						nGenX = rcView.right + 32;
						nGenY = rcView.top + (rand() % rcView.Height());
					}
					break;
				}

				GameObjectBase *pObj = CUR_STAGE->ReserveCreateGameObject(nGenX, nGenY, m_nGenerateType);
				switch (m_nGenerateType)
				{
				case ENEMY_BULLETBILL:
					SOUND_MANAGER->PlaySoundEffect(L"Cannon");
					break;
				}

				if (pObj && m_nGeneratePosition == 3)
				{
					pObj->SetState(STATE_PIPEUP);
					pObj->ChangeState(STATE_PIPEUP);
				}
			}
		}

		// Generate Position
		// 0: Just Page
		// 1: Whole Stage
		if (m_nRange == 1)
		{
			// Follow Camera
			m_fX = CAMERA->m_ptPos.x + (GameDefaults::nPageWidth / 2);
			if (m_fX >= CUR_STAGE->m_ptEnemyStop.x)
			{
				m_fX = CUR_STAGE->m_ptEnemyStop.x;
				ChangeState(STATE_DEAD);
			}
		}
	}
}

NaRect EnemyGenerator::GetRect()
{
	SIZE s = GetSize();
	NaRect rc;
	rc.left = m_fX;
	rc.top = m_fY;
	rc.right = m_fX + s.cx - 1;
	rc.bottom = m_fY + s.cy - 1;

	return rc;
}

NaRectT<float> EnemyGenerator::GetRectFloat()
{
	SIZE s = GetSize();
	NaRectT<float> rc;
	rc.left = m_fX;
	rc.top = m_fY;
	rc.right = m_fX + s.cx - 1;
	rc.bottom = m_fY + s.cy - 1;

	return rc;
}

SIZE EnemyGenerator::GetSize()
{
	SIZE s = { 16, 16 };
	return s;
}
