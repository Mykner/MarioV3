#include "ControllableObject.h"

#include "Game.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "Camera.h"
#include "Stage.h"
#include "PipeInfo.h"
#include "Fireball.h"
#include "SoundManager.h"

#include "RenderManager.h"
#include "InputManager.h"
#include "Player.h"
#include "FModManager.h"
#include "EventObjectBase.h"
#include "VehicleObjectBase.h"
#include "Door.h"

#include "UIBase.h"

BEGIN_IMPL_PROPERTYMAP(PhysicalProperties)
	/*
	PROP_FLOAT("JumpSpeed", VT_R4, false, nullptr, "PhysicalProperties"),
	PROP_INT("JumpPower", VT_I4, false, nullptr, "PhysicalProperties"),
	*/
	PROP_FLOAT("AccelWalk", VT_R4, 0.05f, nullptr, "PhysicalProperties"),
	PROP_FLOAT("AccelRun", VT_R4, 1.0f, nullptr, "PhysicalProperties"),
	PROP_FLOAT("MaxWalkSpeed", VT_R4, 1.5f, nullptr, "PhysicalProperties"),
	PROP_FLOAT("MaxRunSpeedClassic", VT_R4, 3.0f, nullptr, "PhysicalProperties"),
	PROP_FLOAT("MaxRunSpeedNonClassic", VT_R4, 3.25f, nullptr, "PhysicalProperties"),
	PROP_FLOAT("MaxRunSpeedInvinsible", VT_R4, 4.0f, nullptr, "PhysicalProperties"),
	PROP_FLOAT("DecelWalk", VT_R4, 0.08f, nullptr, "PhysicalProperties"),
	PROP_FLOAT("AccelSkid", VT_R4, 0.15f, nullptr, "PhysicalProperties"),
END_IMPL_PROPERTYMAP()

PhysicalProperties::PhysicalProperties()
{
	fAccelWalk = 0.05f;
	fAccelRun = 0.1f;
	fMaxWalkSpeed = 1.5f;
	fMaxRunSpeedClassic = 3.0f;
	fMaxRunSpeedNonClassic = 3.25f; // 3.5f;
	fMaxRunSpeedInvinsible = 4.0f;
	fJumpSpeed = -4.0f;
	nJumpPower = 27;

	fDecelWalk = 0.08f;
	fAccelSkid = 0.15f;
}
