//============== IV: Multiplayer - http://code.iv-multiplayer.com ==============
//
// File: CActorManager.cpp
// Project: Client.Core
// Author(s): jenksta
//            Sebihunter
// License: See LICENSE in root directory
//
//==============================================================================

#include "CActorManager.h"
#include "CChatWindow.h"
#include "CModelmanager.h"
#include <CLogfile.h>

extern CModelManager * g_pModelManager;

CActorManager::CActorManager()
{
	for(EntityId x = 0; x < MAX_ACTORS; x++)
		m_bActive[x] = false;
}

CActorManager::~CActorManager()
{
	for(EntityId x = 0; x < MAX_ACTORS; x++)
	{
		if(m_bActive[x])
			Delete(x);
	}
}

void CActorManager::Create(EntityId actorId, int iModelId, CVector3 vecPosition, float fHeading, String name, bool togglename, int color, bool frozen, bool helmet)
{
	if(m_bActive[actorId])
		Delete(actorId);

	// Get the model hash
	DWORD dwModelHash = SkinIdToModelHash(iModelId);

	// Get the model index
	int iModelIndex = CGame::GetStreaming()->GetModelIndexFromHash(dwModelHash);

	// Get the model info
	CIVModelInfo * pModelInfo = CGame::GetModelInfo(iModelIndex);

	// Do we have a valid model info?
	if(pModelInfo)
	{
		// Add our model info reference
		pModelInfo->AddReference(true);
	}

	// Set the spawn location
	memcpy(&m_Actors[actorId].vecPosition,&vecPosition,sizeof(vecPosition));

	// Set the model info
	m_Actors[actorId].pModelInfo = pModelInfo;

	CreateChar(1, (Scripting::eModel)dwModelHash, vecPosition.fX, vecPosition.fY, vecPosition.fZ, &m_Actors[actorId].uiActorIndex, true);
	Scripting::SetBlockingOfNonTemporaryEvents(m_Actors[actorId].uiActorIndex, true);
	Scripting::SetCharInvincible(m_Actors[actorId].uiActorIndex, true);
	Scripting::SetCharHealth(m_Actors[actorId].uiActorIndex, 200);
	Scripting::AddArmourToChar(m_Actors[actorId].uiActorIndex, 200);
	Scripting::SetCharDefaultComponentVariation(m_Actors[actorId].uiActorIndex);
	Scripting::AddArmourToChar(m_Actors[actorId].uiActorIndex, 200);
	Scripting::SetCharHeading(m_Actors[actorId].uiActorIndex, fHeading);
	Scripting::AllowReactionAnims(m_Actors[actorId].uiActorIndex,false);

	if(frozen)
		Scripting::FreezeCharPosition(m_Actors[actorId].uiActorIndex,true);
	if(helmet)
		Scripting::GivePedHelmet(m_Actors[actorId].uiActorIndex);

	if(!CGame::GetNameTags())
	{
		Scripting::AddBlipForChar(m_Actors[actorId].uiActorIndex, &m_Actors[actorId].uiBlipId);
		Scripting::ChangeBlipSprite(m_Actors[actorId].uiBlipId, Scripting::BLIP_OBJECTIVE);
		Scripting::ChangeBlipScale(m_Actors[actorId].uiBlipId, 0.5);
		Scripting::ChangeBlipNameFromAscii(m_Actors[actorId].uiBlipId, m_Actors[actorId].name);
		Scripting::RemoveFakeNetworkNameFromPed(m_Actors[actorId].uiActorIndex);
		Scripting::GivePedFakeNetworkName(m_Actors[actorId].uiActorIndex,name.Get(),255,255,255,255);
	}
	m_bActive[actorId] = true;

	// Check position for freeze char
	bool m_bSpawned = false;
	CVector3 spawnControl;

	while(!m_bSpawned)
	{
		Scripting::GetCharCoordinates(m_Actors[actorId].uiActorIndex,&spawnControl.fX,&spawnControl.fY,&spawnControl.fZ);
		if((m_Actors[actorId].vecPosition.fZ - spawnControl.fZ) < 0.1f)
		{
			Scripting::FreezeCharPosition(m_Actors[actorId].uiActorIndex,true);
			Scripting::SetCharCoordinates(m_Actors[actorId].uiActorIndex,vecPosition.fX,vecPosition.fY,(vecPosition.fZ - 1.0f));
			m_bSpawned = true;
		}
	}

}

bool CActorManager::Delete(EntityId actorId)
{
	if(!m_bActive[actorId])
	{
		CLogFile::Printf("Tried to delete inexistent Actor %d", actorId);
		return false;
	}

	Scripting::DeleteChar(&m_Actors[actorId].uiActorIndex);

	// Do we have a valid model info?
	if(m_Actors[actorId].pModelInfo)
	{
		// Remove our model info reference
		m_Actors[actorId].pModelInfo->RemoveReference();
	}

	m_bActive[actorId] = false;
	return true;
}

void CActorManager::SetPosition(EntityId actorId, CVector3 vecPosition)
{
	if(m_bActive[actorId])
		Scripting::SetCharCoordinates(m_Actors[actorId].uiActorIndex, vecPosition.fX, vecPosition.fY, vecPosition.fZ);
}

void CActorManager::SetHeading(EntityId actorId, float fHeading)
{
	if(m_bActive[actorId])
		Scripting::SetCharHeading(m_Actors[actorId].uiActorIndex, fHeading);
}

CVector3 CActorManager::GetPosition(EntityId actorId)
{	
	float vecPosition1;
	float vecPosition2;
	float vecPosition3;
	
	Scripting::GetCharCoordinates(m_Actors[actorId].uiActorIndex,&vecPosition1,&vecPosition2,&vecPosition3);

	if(m_bActive[actorId])
		m_Actors[actorId].vecPosition = CVector3(vecPosition1,vecPosition2,vecPosition3); 
		return CVector3(vecPosition1,vecPosition2,vecPosition3);

	return CVector3(0.0f, 0.0f, 0.0f);
}

void CActorManager::SetName(EntityId actorId, String name)
{
	if(m_bActive[actorId])
	{
		m_Actors[actorId].name = name;
		if(!CGame::GetNameTags()) {
		Scripting::ChangeBlipNameFromAscii(m_Actors[actorId].uiBlipId, m_Actors[actorId].name);
		Scripting::RemoveFakeNetworkNameFromPed(m_Actors[actorId].uiActorIndex);
		Scripting::GivePedFakeNetworkName(m_Actors[actorId].uiActorIndex,name.Get(),255,255,255,255);
		}
	}
}

bool CActorManager::ToggleNametag(EntityId actorId, bool show)
{
	if(m_bActive[actorId])
	{
		m_Actors[actorId].nametag = show;
		return 1;
	}
	return false;
}

void CActorManager::SetColor(EntityId actorId, int color)
{
	if(m_bActive[actorId])
		m_Actors[actorId].nametagColor = color;
}

bool CActorManager::ToggleFrozen(EntityId actorId, bool frozen)
{
	if(m_bActive[actorId])
	{
		m_Actors[actorId].frozen = frozen;
		Scripting::FreezeCharPosition(m_Actors[actorId].uiActorIndex,frozen);
		return 1;
	}
	return false;
}

bool CActorManager::ToggleHelmet(EntityId actorId, bool helmet)
{
	if(m_bActive[actorId])
	{
		Scripting::EnablePedHelmet(m_Actors[actorId].uiActorIndex, helmet);
		m_Actors[actorId].helmet = helmet;
		return 1;
	}
	return false;
}

void CActorManager::WarpIntoVehicle(EntityId actorId, int vehicleid, int seatid)
{
	if(m_bActive[actorId])
	{
		if(seatid > 0 && seatid <= 3)
		{
			Scripting::WarpCharIntoCarAsPassenger(m_Actors[actorId].uiActorIndex, vehicleid, (seatid - 1));
			m_Actors[actorId].vehicleid = vehicleid;
			m_Actors[actorId].seatid = seatid;
			m_Actors[actorId].stateincar = true;
		}
	}
}

void CActorManager::RemoveFromVehicle(EntityId actorId)
{
	if(m_bActive[actorId])
	{
		if(m_Actors[actorId].stateincar)
		{
			Scripting::TaskLeaveCar(m_Actors[actorId].uiActorIndex, m_Actors[actorId].vehicleid);
			m_Actors[actorId].vehicleid = -1;
			m_Actors[actorId].seatid = -1;
			m_Actors[actorId].stateincar = false;
		}
	}
}

bool CActorManager::IsCharOnScreen(EntityId actorId)
{
	if(m_bActive[actorId])
	{
		if(Scripting::IsCharOnScreen(m_Actors[actorId].uiActorIndex))
			return true;

		return false;
	}
	return false;
}

unsigned int CActorManager::GetScriptingHandle(EntityId actorId)
{
	if(m_bActive[actorId])
		return m_Actors[actorId].uiActorIndex;

 	return -1;
}

void CActorManager::ForceAnimation(EntityId actorId, const char * szGroup, const char * szAnim)
{
	if(m_bActive[actorId])
		Scripting::TaskPlayAnim(m_Actors[actorId].uiActorIndex,szAnim,szGroup,float(8),0,0,0,0,-1);
}

float CActorManager::GetHealth(EntityId actorId)
{
	if(m_bActive[actorId])
	{
		unsigned int health;
		Scripting::GetCharHealth(m_Actors[actorId].uiActorIndex,&health);
		return (float)health;
	}

	return 0.0f;
}

float CActorManager::GetArmour(EntityId actorId)
{
	if(m_bActive[actorId])
	{
		unsigned int armour;
		Scripting::GetCharArmour(m_Actors[actorId].uiActorIndex,&armour);
		return (float)armour;
	}

	return 0.0f;
}
