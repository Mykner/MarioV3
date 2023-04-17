#include "BowserFireShooter.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "FModManager.h"
#include "Stage.h"
#include "Camera.h"
#include "SoundManager.h"

BowserFireShooter::BowserFireShooter(Game *pGame, Stage *pStage) :
	EventObjectBase(pGame, pStage)
{
	m_nType = EVENTOBJECT_BOWSERFIRESHOOTER;
	m_nState = STATE_IDLE;
	RequestSprites(pStage);

	m_bFlip = true;
}

BowserFireShooter::~BowserFireShooter()
{
}

void BowserFireShooter::RequestSprites(Stage * pStage)
{
	if (pStage == nullptr)
		pStage = CUR_STAGE;

	m_pSprite = SPR_MANAGER->Request(L"Bullet");
}

void BowserFireShooter::OnBeforeProcess()
{
	//CheckCollision();
	//CheckCollisionPlayer();
}

void BowserFireShooter::OnAfterProcess()
{
	m_fX += m_fXS;
	m_fY += m_fYS;

    ProcessState();
}

void BowserFireShooter::RenderDesign(int nColor, int nZOrder)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	int _x = x - pt.x;
	int _y = y - pt.y;

	SIZE s = GetSize();
	_x -= (s.cx / 2);
	_x += 8;
	
	m_pSprite->RenderToQueue(_x, _y, SPRIDX_BULLET_BOWSERFIRE1,
			true, false, nColor, 1.0f, 1.0f, Z_COMMON_OBJ);
}

void BowserFireShooter::OnIdle()
{
	if (m_nStateFrame % 120 == 0 && rand() % 4 == 0)
	{
		if (CAMERA->IsOutOfCamera(this))
			return;

		// Find Empty Row
		NaRect rcTileViewport = CAMERA->GetTileViewport();
		std::vector<int> vecEmptyRow;
		
		for (int y = 0; y < GameDefaults::nPageTileHeight; y++)
		{
			int nTileData = m_pStage->GetTileData(rcTileViewport.right, y);
			if (m_pStage->GetDataType(nTileData) == TILETYPE_EMPTY)
				vecEmptyRow.push_back(y);
		}

		if (vecEmptyRow.size() == 0)
			return;

		NaRect rcViewport = CAMERA->GetViewport();
		int nRandomY = (rand() % vecEmptyRow.size()) * TILE_YS;

		CreateParameter param;
		param.pFromPlayer = nullptr;
		param.pFromObject = this;
		param.nType = BULLET_BOWSERFIRE;
		CUR_STAGE->CreateBullet(rcViewport.right + 30, nRandomY, BULLET_BOWSERFIRE, &param);

		SOUND_MANAGER->PlaySoundEffect(L"BowserFire");
	}

	BEGIN_VECTOR_LOOP(GameObjectBase*, m_pStage->m_vecEnemy) {
		if (pObj->m_nType == ENEMY_BOWSER)
		{
			if (!CAMERA->IsOutOfCamera(pObj))
			{
				ChangeState(STATE_DEAD);
			}
		}
	} END_VECTOR_LOOP();
}

SIZE BowserFireShooter::GetSize()
{
	SIZE s = { 24, 8 };
	return s;
}
