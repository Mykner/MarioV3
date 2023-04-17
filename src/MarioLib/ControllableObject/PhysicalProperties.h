#pragma once

#include "Common.h"
#include "GameObjectBase.h"

class PhysicalProperties : public PropertyObjectBase
{
public:
	PhysicalProperties();

	float fAccelWalk;
	float fAccelRun;
	float fMaxWalkSpeed;
	float fMaxRunSpeedClassic;
	float fMaxRunSpeedNonClassic;
	float fMaxRunSpeedInvinsible;
	float fDecelWalk;
	float fAccelSkid;
	float fJumpSpeed;
	int nJumpPower;

	BEGIN_DECLARE_PROPERTYMAP()
		/*
		DECL_PROP("JumpSpeed", &fJumpSpeed)
		DECL_PROP("JumpPower", &nJumpPower)
		*/
		DECL_PROP("AccelWalk", &fAccelWalk)
		DECL_PROP("AccelRun", &fAccelRun)
		DECL_PROP("MaxWalkSpeed", &fMaxWalkSpeed)
		DECL_PROP("MaxRunSpeedClassic", &fMaxRunSpeedClassic)
		DECL_PROP("MaxRunSpeedNonClassic", &fMaxRunSpeedNonClassic)
		DECL_PROP("MaxRunSpeedInvinsible", &fMaxRunSpeedInvinsible)
		DECL_PROP("DecelWalk", &fDecelWalk)
		DECL_PROP("AccelSkid", &fAccelSkid)
	END_DECLARE_PROPERTYMAP()
};
