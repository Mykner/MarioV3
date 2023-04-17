#pragma once

#include <stdlib.h>

#include <vector>
#include <math.h>

#pragma warning(disable:4244)

const double PI = acos(-1);
const float fToRadian = (1 / 180.0f) * (float)PI;

#define CAMERA						(m_pGame->m_pCamera)
#define CUR_STAGE					(m_pGame->m_pCurStage)
#define CUR_PLAYER					(m_pGame->m_pCurPlayer)
#define CUR_PLAYER_OBJ				(m_pGame->m_pCurPlayer->m_pControlObj)
#define SPR_MANAGER					(m_pGame->m_pSpriteManager)
#define RENDER_MANAGER				(m_pGame->m_pRenderManager)
#define FMOD_MANAGER				(m_pGame->m_pFMod)
#define UI_MANAGER					(m_pGame->m_pUIManager)
#define SOUND_MANAGER				(m_pGame->m_pSoundManager)

#define IS_MODERN					(m_pGame->m_bModernStyle == true)
#define IS_CLASSIC					(m_pGame->m_nStagePackType == STAGEPACK_CLASSIC)
#define IS_ONLINESTAGE				(m_pGame->m_strStagePack.CompareNoCase(L"Download") == 0)
#define IS_SPINNING					(IS_MODERN && (m_nState == STATE_BURNT || m_nState == STATE_POWED))

#define IS_MAPOBJECT(pObj)			(pObj->m_nType >= MAPOBJECT_BEGIN && pObj->m_nType <= MAPOBJECT_END)
#define IS_ITEM(pObj)				(pObj->m_nType >= ITEM_BEGIN && pObj->m_nType <= ITEM_END)
#define IS_ENEMY(pObj)				(pObj->m_nType >= ENEMY_BEGIN && pObj->m_nType <= ENEMY_END)
#define IS_BULLET(pObj)				(pObj->m_nType >= BULLET_BEGIN && pObj->m_nType <= BULLET_END)
#define IS_EFFECT(pObj)				(pObj->m_nType >= EFFECT_BEGIN && pObj->m_nType <= EFFECT_END)
#define IS_EVENTOBJECT(pObj)		(pObj->m_nType >= EVENTOBJECT_BEGIN && pObj->m_nType <= EVENTOBJECT_END)
#define IS_PLAYEROBJECT(pObj)		(pObj->m_nType >= PLAYEROBJECT_BEGIN && pObj->m_nType <= PLAYEROBJECT_END)
#define IS_VEHICLE(pObj)			(pObj->m_nType >= VEHICLE_BEGIN && pObj->m_nType <= VEHICLE_END)

#define GET_CONTROLLER_IF_PLAYER(pWho) \
	((pWho->m_nDamageGroup == DAMAGEGROUP_PLAYER) ? (pWho->m_pController) : (nullptr))

#define FIND_THEME_SPRITE(str)		m_pGame->m_pThemeManager->FindSprite(str, m_pStage->m_nTheme)

#define WSZ_COIN1					L"\x01"
#define WSZ_COIN2					L"\x02"
#define WSZ_COIN3					L"\x03"
#define WSZ_PINKCOIN_BRACE_L		L"\x04"
#define WSZ_PINKCOIN				L"\x05"
#define WSZ_PINKCOIN_BRACE_R		L"\x06"
#define WSZ_PINKCOIN_EMPTY			L"\x07"
#define WSZ_MUSHROOM				L"\x08"
#define WSZ_COPYRIGHT				L"\x09"
#define WSZ_CLOCK					L"\x0f"

#define WSZ_PLAYED					L"\x10"
#define WSZ_CLEAR					L"\x11"
#define WSZ_LIKE					L"\x12"
#define WSZ_DOWNLOAD				L"\x13"
#define WSZ_NEW						L"\x14"
#define WSZ_GOOD					L"\x15"
#define WSZ_NEUTRAL					L"\x16"
#define WSZ_BAD						L"\x17"
#define WSZ_UPARROW					L"\x1e"
#define WSZ_DOWNARROW				L"\x1f"

#define KE(k)						(m_pGame->m_pInputManager->IsKeyPressed(k))
#define KS(k)						(m_pGame->m_pInputManager->IsKeyHold(k))

#define TEXTOUT(x, y, str) \
	RENDER_MANAGER->m_pFont->DrawText(RENDER_MANAGER->m_pSprite, str.wstr(), str.GetLength(), NaRect(x, y, 9999, 9999), DT_LEFT, 0xffffffff); 

#define DRAWTEXT(x, y, str)			RENDER_MANAGER->Text(x, y, str); 
#define DRAWTEXTC(x, y, str, c)		RENDER_MANAGER->Text(x, y, str, c); 

#define STOPBGM()					FMOD_MANAGER->StopBGM();
#define PLAYBGM(str)				FMOD_MANAGER->PlayBGM(str);
#define PLAYBGMONCE(str)			FMOD_MANAGER->PlayBGM(str, false);
#define PLAYSOUND(str)				FMOD_MANAGER->PlaySound(str);

#define TILE_XS						(GameDefaults::nTileWidth)
#define TILE_YS						(GameDefaults::nTileHeight)

#define TO_PAGE_X(x)				((x) / GameDefaults::nPageWidth)
#define TO_PAGECOORD_X(x)			((x) % GameDefaults::nPageWidth)

#define CENTER_OF_PAGE				(GameDefaults::nPageWidth / 2)
#define MIDDLE_OF_PAGE				(GameDefaults::nPageHeight / 2)

#define TO_REALCOORD_X(page, x)		(page * GameDefaults::nPageWidth + x)

#define SPRITE_INDEX_CYCLE(from, cnt, delay) \
	(from + (m_nStateFrame % (cnt * delay)) / delay)

#define SPRITE_INDEX_NOCYCLE(from, cnt, delay) \
	(m_nStateFrame >= (cnt*delay)) ? (from + cnt - 1) : (from + (m_nStateFrame % (cnt * delay)) / delay)

#define BEGIN_VECTOR_LOOP(t, v)	\
	{ \
		std::vector<t> &vecCurrent = v; \
		std::vector<t>::iterator it = v.begin(); \
		for (; it != v.end(); ++it) \
		{ \
			t pObj = *it;

#define END_VECTOR_LOOP() \
		} \
	}

#define BEGIN_VECTOR_LOOP_NOINC(t, v)	\
	{ \
		std::vector<t> &vecCurrent = v; \
		std::vector<t>::iterator it = v.begin(); \
		for (; it != v.end(); ) \
		{ \
			t pObj = *it;

#define PASTE_VECTOR(target, v) target.insert(target.end(), v.begin(), v.end())

#define SAFE_RELEASE(v) { if (v) v->Release(); }
#define SAFE_DELETE(v) { if (v) delete v; v = nullptr;}

#define SAFE_DELETE_OBJPTR_VECTOR(t, v) \
	{ \
		std::vector<t>::iterator it = v.begin(); \
		for ( ; it != v.end(); ) \
		{ \
			t pObj = *it; \
			delete pObj; \
			it = v.erase(it); \
		} \
	}
