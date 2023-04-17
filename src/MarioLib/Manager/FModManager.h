#pragma once

#include "Common.h"

#include "fmod.hpp"
#include "fmod_errors.h"

#include <map>

typedef std::map<NaString, FMOD::Sound*> NamedSoundMap;
typedef std::pair<NaString, FMOD::Sound*> NamedSoundPair;

typedef std::map<NaString, NaString> SoundQueueMap;
typedef std::pair<NaString, NaString> SoundQueuePair;

enum FModChannelTypes
{
	CHANNEL_BGM = 0,
	CHANNEL_SUBBGM,
	CHANNEL_SOUND,
};

enum FModGroupTypes
{
	GROUP_BGM = 0,
	GROUP_SOUND,
};

#define CHANNEL_COUNT	(3)
#define GROUP_COUNT		(2)

class Game;
class FModManager
{
public:
	FModManager(Game *pGame);
	virtual ~FModManager();

	void Init();
	void Release();

	void Process();

	void LoadBGM(const wchar_t* name, const wchar_t *theme = nullptr);
	void LoadSound(const wchar_t* name, const wchar_t* ext = L"mp3", const wchar_t *theme = nullptr);
	FMOD::Sound* PlayBGM(const wchar_t* name, bool bLoop = true);
	FMOD::Sound* PlaySound(const wchar_t* name);
	FMOD::Sound* RequestPlayBGM(const wchar_t* name, const wchar_t *theme, bool bLoop = true);
	FMOD::Sound* RequestPlaySound(const wchar_t* name, const wchar_t *theme);
	FMOD::Sound* RequestPlaySoundQueue(const wchar_t* name, const wchar_t *theme);
	void PauseBGM(bool bPause = true);
	void StopBGM();
	void StopSound();
	bool IsPlayingBGM();

	FMOD::System *m_pSystem;
	unsigned int m_nVersion;
	FMOD::Channel *m_pChannel[CHANNEL_COUNT];
	FMOD::ChannelGroup *m_pGroup[GROUP_COUNT], *m_pMasterGroup;

	NamedSoundMap m_mapBGM;
	NamedSoundMap m_mapSound;
	Game *m_pGame;
	bool m_bInitialized;

	SoundQueueMap m_mapReqPlaySound;
};