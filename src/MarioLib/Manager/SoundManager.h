#pragma once

#include "NaString.h"

// #TODO Wrap FMod to Something other
#include "fmod.hpp"
#include "fmod_errors.h"

enum BGMModes
{
	BGM_MODE_NORMAL = 0,
	BGM_MODE_FAST,
	BGM_MODE_INVINSIBLE,
	BGM_MODE_GIANT,
	BGM_MODE_ETC,
};

class Game;
class GameObjectBase;
class SoundManager
{
public:
	SoundManager(Game *pGame);
	virtual ~SoundManager();

	void PlaySoundEffect(NaString str, GameObjectBase *pObj = nullptr);
	void PlayHurryUp();
	void PlayStageBGM();
	void PlayInvinsibleBGM();
	void PlayGiantBGM();
	void PlayPSwitchBGM();

	FMOD::Sound* PlayBGM(NaString str, bool bLoop = true);
	void PauseBGM(bool bPause = true);
	void StopBGM();
	void StopSoundEffect();

	bool CanPlayStageBGM();

	NaString m_strCurrentGameTheme;
	NaString m_strCurrentBGM;
	Game *m_pGame;
};