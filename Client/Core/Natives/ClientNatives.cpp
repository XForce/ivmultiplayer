//============== IV: Multiplayer - http://code.iv-multiplayer.com ==============
//
// File: ClientNatives.cpp
// Project: Client.Core
// Author(s): jenksta
//            Einstein
//            Sebihunter
// License: See LICENSE in root directory
//
//==============================================================================

#include "../Natives.h"
#include "Squirrel/sqstate.h"
#include "Squirrel/sqvm.h"
#include "Squirrel/sqstring.h"
#include "../CClient.h"
#include "../Scripting.h"
#include "../IV/CIVWeather.h"

extern CClient * g_pClient;

// Client functions

void RegisterClientNatives(CScriptingManager * pScriptingManager)
{
	pScriptingManager->RegisterFunction("addChatMessage", sq_addChatMessage, -1, NULL);
	pScriptingManager->RegisterFunction("toggleChatWindow", sq_toggleChatWindow, 1, "b");
	pScriptingManager->RegisterFunction("isChatWindowVisible", sq_isChatWindowVisible, 0, NULL);
	pScriptingManager->RegisterFunction("getWeather", sq_getWeather, 0, NULL);
	pScriptingManager->RegisterFunction("setWeather", sq_setWeather, 1, "i");
	pScriptingManager->RegisterFunction("getGameScrollBarText", sq_getGameScrollBarText, 1, "i");
	pScriptingManager->RegisterFunction("setGameScrollBarText", sq_setGameScrollBarText, 2, "is");
	pScriptingManager->RegisterFunction("resetGameScrollBarText", sq_resetGameScrollBarText, 1, "i");
	pScriptingManager->RegisterFunction("getGameScrollBarColor", sq_getGameScrollBarColor, 0, NULL);
	pScriptingManager->RegisterFunction("setGameScrollBarColor", sq_setGameScrollBarColor, 1, "i");
	pScriptingManager->RegisterFunction("getMoney", sq_getMoney, 0, NULL);
	pScriptingManager->RegisterFunction("getCurrentAreaName", sq_getCurrentAreaName, 0, NULL);
	pScriptingManager->RegisterFunction("getCurrentStreetName", sq_getCurrentStreetName, 0, NULL);
	pScriptingManager->RegisterFunction("getVehicleName", sq_getVehicleName, 1, "i");
	pScriptingManager->RegisterFunction("getFPS", sq_getFPS, 0, NULL);
	pScriptingManager->RegisterFunction("isGameFocused", sq_isGameFocused, 0, NULL);
	pScriptingManager->RegisterFunction("setRadarZoom", sq_setRadarZoom, 1, "f");
	pScriptingManager->RegisterFunction("drawLightWithRange", sq_drawLightWithRange, 8, "fffiiiff");
	pScriptingManager->RegisterFunction("getScreenPositionFromWorldPosition", sq_getScreenPositionFromWorldPosition, 3, "fff");
	pScriptingManager->RegisterFunction("getActorPosition", sq_getActorCoordinates, 1, "i");
	pScriptingManager->RegisterFunction("tuneRadio", sq_tuneRadio, 2, "bi");
	pScriptingManager->RegisterFunction("triggerServerEvent", sq_triggerServerEvent, -1, NULL);
	pScriptingManager->RegisterFunction("getGameLanguage", sq_getGameLanguage, -1, NULL);
	pScriptingManager->RegisterFunction("importAndLoadGameFile", sq_importAndLoadGameFile, 1, "s");
	pScriptingManager->RegisterFunction("getGroundZ", sq_getGroundZ, 3, "fff");
}

// addChatMessage(string)
int sq_addChatMessage(SQVM * pVM)
{
	CHECK_PARAMS_MIN_MAX("addChatMessage", 1, 3);
	CHECK_TYPE("timer", 1, 2, OT_STRING);

	const char* text = NULL;
	sq_getstring(pVM, 2, &text);

	int color = 0xFFFFFFAA;
	SQBool bAllowFormatting = false;
	int iTop = sq_gettop(pVM);

	if(iTop >= 3)
	{
		if(sq_gettype(pVM, 3) != OT_INTEGER)
		{
			sq_pushbool(pVM, false);
			return 1;
		}

		sq_getinteger(pVM, 3, &color);

		if(iTop >= 4)
		{
			if(sq_gettype(pVM, 4) != OT_BOOL)
			{
				sq_pushbool(pVM, false);
				return 1;
			}

			sq_getbool(pVM, 4, &bAllowFormatting);
		}
	}

	g_pClient->GetChatWindow()->AddMessage(color, (bAllowFormatting != 0), text);
	sq_pushbool(pVM, true);
	return 1;
}

// toggleChatWindow(toggle)
int sq_toggleChatWindow(SQVM * pVM)
{
	SQBool bToggle;
	sq_getbool(pVM, 2, &bToggle);
	g_pClient->GetChatWindow()->SetEnabled(bToggle != 0);
	sq_pushbool(pVM, true);
	return 1;
}

// isChatWindowVisible()
int sq_isChatWindowVisible(SQVM * pVM)
{
	sq_pushbool(pVM, g_pClient->GetChatWindow()->IsEnabled());
	return 1;
}

// GUI functions

void RegisterGUINatives(CScriptingManager * pScriptingManager)
{
	pScriptingManager->RegisterFunction("guiShowMessageBox", sq_guiShowMessageBox, 2, "ss");
	pScriptingManager->RegisterFunction("guiToggleCursor", sq_guiToggleCursor, 1, "b");
	pScriptingManager->RegisterFunction("isCursorVisible", sq_guiIsCursorVisible, 0, NULL);
	pScriptingManager->RegisterFunction("guiSetCursorPosition", sq_guiSetCursorPosition, 2, "ff");
	pScriptingManager->RegisterFunction("guiGetCursorPosition", sq_guiGetCursorPosition, 0, NULL);
	pScriptingManager->RegisterFunction("guiGetScreenSize", sq_guiGetScreenSize, 0, NULL);
	pScriptingManager->RegisterFunction("guiDrawRectangle", sq_guiDrawRectangle,  6, "ffffib");
}

// guiShowMessageBox(string text, string title)
int sq_guiShowMessageBox(SQVM * pVM)
{
	const char* text;
	const char* title;
	sq_getstring(pVM, -2, &text);
	sq_getstring(pVM, -1, &title);
	sq_pushbool(pVM, true);

	g_pClient->GetGUI()->ShowMessageBox(text, title);
	CGame::SetInputState(false);
	return 1;
}	

// guiToggleCursor(bool toggle)
int sq_guiToggleCursor(SQVM * pVM)
{	
	SQBool sbBool;
	sq_getbool(pVM, -1, &sbBool);
	bool bBool = (sbBool != 0);

	// Get our GUI instance
	CGUI * pGUI = g_pClient->GetGUI();

	pGUI->SetScriptedCursorVisible(bBool);
	pGUI->SetCursorVisible(bBool);

	sq_pushbool(pVM, true);
	return 1;
}

// isCursorVisible()
int sq_guiIsCursorVisible(SQVM * pVM)
{
	sq_pushbool(pVM, g_pClient->GetGUI()->IsCursorVisible(true));
	return 1;
}

// setCursorPosition(float posX, float posY)
int sq_guiSetCursorPosition(SQVM * pVM)
{
	float posX;
	float posY;
	sq_getfloat(pVM, -2, &posX);
	sq_getfloat(pVM, -1, &posY);

	g_pClient->GetGUI()->SetCursorPosition(posX, posY);
	return 1;
}

// guiGetCursorPosition()
int sq_guiGetCursorPosition(SQVM * pVM)
{	
	// Returning mouse cursor position to script vm:
	// Cursor position is array [X, Y]
	RECT curPos = g_pClient->GetGUI()->GetCursorPosition();
	CSquirrelArguments args;
	args.push(curPos.left);
	args.push(curPos.top);
	sq_pusharg(pVM, CSquirrelArgument(args, true));
	return 1;
}

// guiGetScreenSize()
int sq_guiGetScreenSize(SQVM * pVM)
{
	// Get our GUI instance
	CGUI * pGUI = g_pClient->GetGUI();

	sq_newarray(pVM, 0);
	sq_pushfloat(pVM, (float)pGUI->GetDisplayWidth());
	sq_arrayappend(pVM, -2);
	sq_pushfloat(pVM, (float)pGUI->GetDisplayHeight());
	sq_arrayappend(pVM, -2);
	return 1;
}

// guiDrawRectangle(x1, y1, x2, y2, color, relative)
int sq_guiDrawRectangle(SQVM * pVM)
{
	float x,y,x1,y1;
	int color;
	SQBool sbRelative;
	sq_getfloat(pVM, -6, &x);
	sq_getfloat(pVM, -5, &y);
	sq_getfloat(pVM, -4, &x1);
	sq_getfloat(pVM, -3, &y1);
	sq_getinteger(pVM, -2, &color);
	sq_getbool(pVM, -1, &sbRelative);

	bool bRelative = (sbRelative != 0);

	if(bRelative)
	{
		// Get our GUI instance
		CGUI * pGUI = g_pClient->GetGUI();

		x *= pGUI->GetDisplayHeight();
		y *= pGUI->GetDisplayHeight();
		x1 *= pGUI->GetDisplayWidth();
		y1 *= pGUI->GetDisplayHeight();
	}

	g_pClient->GetGraphics()->DrawRect(x, y, x1, y1, ( color >> 8 ) + ( ( color & 0xFF ) << 24 ));
	sq_pushbool(pVM, true);
	return 1;
}	

int sq_triggerServerEvent(SQVM * pVM)
{
	CSquirrelArguments arguments;
	for(int i = 2; i <= sq_gettop( pVM ); ++ i )
	{
		if(!arguments.pushFromStack(pVM, i))
			return 1;
	}

	CBitStream bsSend;
	arguments.serialize(&bsSend);
	g_pClient->GetNetworkManager()->RPC(RPC_ScriptingEventCall, &bsSend, PRIORITY_HIGH, RELIABILITY_RELIABLE_ORDERED, PACKET_CHANNEL_SCRIPT);
	return 1;
}

// Scripting functions

void RegisterScriptNatives(CScriptingManager * pScriptingManager)
{
	pScriptingManager->RegisterFunction("getClientScripts", sq_getScripts, 0, NULL);
	pScriptingManager->RegisterFunction("getScriptName", sq_getScriptName, 0, NULL);
}

// getClientScripts()
int sq_getScripts(SQVM * pVM)
{
	sq_newarray(pVM, 0);
	std::list<CSquirrel*>* scripts = g_pClient->GetScriptingManager()->GetScriptList();
	for(std::list<CSquirrel*>::iterator iter = scripts->begin(); iter != scripts->end(); ++ iter)
	{
		sq_pushstring(pVM, (*iter)->GetName().C_String(), (*iter)->GetName().GetLength());
		sq_arrayappend(pVM, -2);
	}
	return 1;
}

// getScriptName()
int sq_getScriptName(SQVM * pVM)
{	
	String strScriptName = g_pClient->GetScriptingManager()->Get(pVM)->GetName();
	sq_pushstring(pVM, strScriptName.Get(), strScriptName.GetLength());
	return 1;
}

// getWeather()
int sq_getWeather(SQVM * pVM)
{
	sq_pushinteger(pVM, (CGame::GetWeather()->GetWeather() + 1));
	return 1;
}

// setWeather()
int sq_setWeather(SQVM * pVM)
{
	int iWeather;
	sq_getinteger(pVM, 2, &iWeather);
	if(iWeather >= 1 && iWeather <= 10)
	{
		CGame::GetWeather()->SetWeather((eWeather)(iWeather - 1));
		sq_pushbool(pVM, true);
		return 1;
	}

	sq_pushbool(pVM, false);
	return 1;
}

// getGameScrollBarText(id)
int sq_getGameScrollBarText(SQVM * pVM)
{
	int iID;
	sq_getinteger(pVM, 2, &iID);

	char szText[1300] = {0};
	CGame::GetScrollBarText(iID, szText);

	sq_pushstring(pVM, szText, strlen(szText));
	return 1;
}

// setGameScrollBarText(id, text)
int sq_setGameScrollBarText(SQVM * pVM)
{
	int iID;
	const char* szText;
	sq_getinteger(pVM, 2, &iID);
	sq_getstring(pVM, 3, &szText);

	if(iID >= 0 && iID != 2 && iID <= 7)
		sq_pushbool(pVM, CGame::SetScrollBarText(iID, szText));
	else
		sq_pushbool(pVM, false);
	return 1;
}

// resetGameScrollBarText(id)
int sq_resetGameScrollBarText(SQVM * pVM)
{
	int iID;
	sq_getinteger(pVM, 2, &iID);

	if(iID >= -1 && iID <= 7)
	{
		CGame::ResetScrollBars(iID);
		sq_pushbool(pVM, true);
	}
	else
		sq_pushbool(pVM, false);
	return 1;
}

// setGameScrollBarColor(color)
int sq_setGameScrollBarColor(SQVM * pVM)
{
	int iColor;
	sq_getinteger(pVM, 2, &iColor);

	CGame::SetScrollBarColor(iColor);

	sq_pushbool(pVM, true);
	return 1;
}

// getGameScrollBarColor()
int sq_getGameScrollBarColor(SQVM * pVM)
{
	sq_pushinteger(pVM, CGame::GetScrollBarColor());
	return 1;
}

// getMoney()
int sq_getMoney(SQVM * pVM)
{
	sq_pushinteger(pVM, g_pClient->GetLocalPlayer()->GetMoney());
	return 1;
}

// getCurrentAreaName()
int sq_getCurrentAreaName(SQVM * pVM)
{
	sq_pushstring(pVM, CGame::GetCurrentAreaName().Get(), -1);
	return 1;
}

// getCurrentStreetName()
int sq_getCurrentStreetName(SQVM * pVM)
{
	sq_pushstring(pVM, CGame::GetCurrentStreetName().Get(), -1);
	return 1;
}

char szVehicleNames[124][32] = 
{
	"Admiral", "Airtug", "Ambulance", "Banshee", "Benson", "Biff", "Blista Compact", "Bobcat", "Boxville",
	"Buccaneer", "Burrito", "Burrito", "Bus", "Cabby", "Cavalcade", "Chavos", "Cognoscenti", "Comet",
	"Coquette", "DF8", "Dilettante", "Dukes", "E109", "Emperor", "Emperor", "Esperanto", "Faction", "FBI",
	"Feltzer", "Feroci", "Feroci", "Firetruck", "Flatbed", "Fortune", "Forklift", "Futo", "FXT", "Habanero",
	"Hakumai", "Huntley", "Infernus", "Ingot", "Intruder", "Landstalker", "Lokus", "Manana", "Marbella",
	"Merit", "Minivan", "Moonbeam", "Mr Tasty", "Mule", "Noose", "Noose Stockade", "Oracle", "Packer", "Patriot",
	"Perennial", "Perennial", "Peyote", "Phantom", "Pinnacle", "PMP600", "Police", "Police", "Police Patriot",
	"Pony", "Premier", "Pres", "Primo", "Police Stockade", "Rancher", "Rebla", "Ripley", "Romero", "Rom",
	"Ruiner", "Sabre", "Sabre", "Sabre GT", "Schafter", "Sentinel", "Solair", "Speedo", "Stalion", "Steed",
	"Stockade", "Stratum", "Stretch", "Sultan", "Sultan RS", "Super GT", "Taxi", "Taxi", "Trash", "Turismo",
	"Uranus", "Vigero", "Vigero", "Vincent", "Virgo", "Voodoo", "Washington", "Willard", "Yankee", "Freeway",
	"Faggio", "Hellfury", "NRG 900", "PCJ600", "Sanchez", "Zombie", "Annihilator", "Maverick", "Police Maverick",
	"Tour Maverick", "Dinghy", "Jetmax", "Marquis", "Predator", "Reefer", "Squalo", "Tug", "Tropic",
};

// getVehicleName(modelid)
int sq_getVehicleName(SQVM * pVM)
{
	int iModelId;
	sq_getinteger(pVM, -1, &iModelId);

	if(iModelId < 0 || iModelId > 123)
	{
		sq_pushbool(pVM, false);
		return 1;
	}

	sq_pushstring(pVM, szVehicleNames[iModelId], -1);
	return 1;
}

// getFPS()
int sq_getFPS(SQVM * pVM)
{
	sq_pushinteger(pVM, g_pClient->GetFPSCounter()->Get());
	return 1;
}

// getScreenPositionFromWorldPosition(x, y, z)
int sq_getScreenPositionFromWorldPosition(SQVM * pVM)
{
	CVector3 vecWorldPos;
	Vector2 vecScreenPos;
	sq_getvector3(pVM, -3, &vecWorldPos);
	CGame::GetScreenPositionFromWorldPosition(vecWorldPos, vecScreenPos);
	sq_newarray(pVM, 0);
	sq_pushfloat(pVM, vecScreenPos.fX);
	sq_arrayappend(pVM, -2);
	sq_pushfloat(pVM, vecScreenPos.fY);
	sq_arrayappend(pVM, -2);
	return 1;
}

// isGameFocused()
int sq_isGameFocused(SQVM * pVM)
{
	sq_pushbool(pVM, CGame::IsFocused());
	return 1;
}

// setRadarZoom(fZoom)
int sq_setRadarZoom(SQVM * pVM)
{
	float fZoom;
	sq_getfloat(pVM,-1,&fZoom);

	Scripting::SetRadarZoom(fZoom);
	sq_pushbool(pVM,true);
	return 1;
}

int sq_drawLightWithRange(SQVM * pVM)
{
	CVector3 vecPos;
	int R,G,B;
	float fDensity, fRange;
	sq_getfloat(pVM, -8, &vecPos.fX);
	sq_getfloat(pVM, -7, &vecPos.fY);
	sq_getfloat(pVM, -6, &vecPos.fZ);
	sq_getinteger(pVM, -5, &R);
	sq_getinteger(pVM, -4, &G);
	sq_getinteger(pVM, -3, &B);
	sq_getfloat(pVM, -2, &fDensity);
	sq_getfloat(pVM, -1, &fRange);

	Scripting::DrawLightWithRange(vecPos.fX, vecPos.fY, vecPos.fZ, R, G, B, fDensity, fRange);
	sq_pushbool(pVM,true);
	return 1;
}

// getActorCoordinates(actorid) // pls take a look at the wiki ;)
int sq_getActorCoordinates(SQVM * pVM)
{
	EntityId actorId;
	sq_getentity(pVM,-1,&actorId);

	// Get our actor manager
	CActorManager * pActorManager = g_pClient->GetActorManager();

	if(pActorManager->DoesExist(actorId))
	{
		CVector3 vecPosition;
		Scripting::GetCharCoordinates(pActorManager->GetScriptingHandle(actorId), &vecPosition.fX, &vecPosition.fY, &vecPosition.fZ);
		CSquirrelArguments args;
		args.push(vecPosition.fX);
		args.push(vecPosition.fY);
		args.push(vecPosition.fZ);
		sq_pusharg(pVM, CSquirrelArgument(args, true));
		return 1;
	}

	sq_pushbool(pVM, false);
	return 1;
}

int sq_tuneRadio(SQVM * pVM)
{
	int iStationIndex;
	sq_getinteger(pVM, -1, &iStationIndex);

	SQBool bToggle;
	sq_getbool(pVM, -2, &bToggle);

	bool bSwitch = (bToggle != 0);

	if(bSwitch)
	{
		Scripting::SetMobileRadioEnabledDuringGameplay(1);
		Scripting::SetMobilePhoneRadioState(1);
		Scripting::RetuneRadioToStationIndex((Scripting::eRadioStation)iStationIndex);
	}
	else
	{
		Scripting::SetMobileRadioEnabledDuringGameplay(0);
		Scripting::SetMobilePhoneRadioState(0);
	}
	sq_pushbool(pVM,true);
	return 1;
}

int sq_getGameLanguage(SQVM * pVM)
{
	int iLanguage = Scripting::GetCurrentLanguage();
	String strLanguage;

	switch(iLanguage)
	{
	case 0:
		strLanguage = "AMERICAN";
	case 1:
		strLanguage = "FRENCH";
	case 2:
		strLanguage = "GERMAN";
	case 3:
		strLanguage = "ITALIAN";
	case 4:
		strLanguage = "SPANISH";
	case 5:
		strLanguage = "RUSSIAN";
	default:
		strLanguage = "UNKOWN";
	}

	sq_pushstring(pVM,strLanguage.Get(),-1);
	return 1;
}

int sq_importAndLoadGameFile(SQVM * pVM)
{
	const char * szFile;
	sq_getstring(pVM, -1, &szFile);
	String strFile = szFile;

	String strAbsolutePath;
	strAbsolutePath = SharedUtility::GetAbsolutePath("clientfiles");
	if(!strcmp(szFile,"hud.dat") || !strcmp(szFile,"RadioLogo.dat")) {
		std::ifstream szFileExist(String("%s\\resources\\%s", strAbsolutePath.Get(), strFile.Get()).Get());
		if(!szFileExist) {
			CLogFile::Printf("[IMPORT] Failed to import file %s into GTA [File does not exist]",strFile.Get());
			sq_pushbool(pVM,false);
			return 1;
		}

		char szInstallDirectory[MAX_PATH];
		if(!SharedUtility::ReadRegistryString(HKEY_LOCAL_MACHINE, "Software\\Rockstar Games\\Grand Theft Auto IV", 
			"InstallFolder", NULL, szInstallDirectory, sizeof(szInstallDirectory)) || !SharedUtility::Exists(szInstallDirectory))
		{
			if(!SharedUtility::ReadRegistryString(HKEY_CURRENT_USER, "Software\\IVMP", "gtaivdir", NULL, 
				szInstallDirectory, sizeof(szInstallDirectory)) || 
				!SharedUtility::Exists(szInstallDirectory))
			{
				CLogFile::Printf("[IMPORT] Failed to import file %s into GTA [Failed to find gta iv directory]",strFile.Get());
				sq_pushbool(pVM,false);
				return 1;
			}
		}
		
		String strCopyPath = String("%s\\common\\data",szInstallDirectory);
		if(!SharedUtility::Exists(strCopyPath.Get()))
			SharedUtility::CreateDirectoryA(strCopyPath.Get());

		String strFolderName = SharedUtility::GetAbsolutePath("clientfiles");
		strFolderName.AppendF("\\resources\\%s",strFile.Get());
		
		if(!strcmp(szFile,"hud.dat"))
			strCopyPath.Append("\\hud_ivmp.dat");
		else if(!strcmp(szFile,"RadioLogo.dat"))
			strCopyPath.Append("\\RadioLogo_ivmp.dat");

		if(g_pClient->GetChatWindow())
			g_pClient->GetChatWindow()->AddInfoMessage("IMPORT from: %s | to: %s",strFolderName.Get(), strCopyPath.Get());

		if(CopyFileA(strFolderName.Get(),strCopyPath.Get(),false))
		{
			if(!strcmp(szFile,"hud.dat"))
			{
				// Load custom HUD.dat
				CGame::LoadHUD("common:/DATA/HUD_IVMP.DAT");
				sq_pushbool(pVM, true);
				return 1;
			}
			else if(!strcmp(szFile,"RadioLogo.dat"))
			{
				// Load custom RadioLogo.dat
				CGame::LoadRadioLogo("common:/DATA/RadioLogo_ivmp.DAT");
				sq_pushbool(pVM, true);
				return 1;
			}
			else
			{
				CLogFile::Printf("[IMPORT] Failed to import file %s into GTA [Not supported file]",strFile.Get());
				sq_pushbool(pVM, false);
				return 1;
			}
		}
		else
		{
			CLogFile::Printf("[IMPORT] Failed to import file %s into GTA [Copy failed]",strFile.Get());
			sq_pushbool(pVM, false);
			return 1;
		}
	}
	else
	{	
		CLogFile::Printf("[IMPORT] Failed to import file %s into GTA [Not supported file]",strFile.Get());
		sq_pushbool(pVM, false);
		return 1;
	}

	sq_pushbool(pVM, false);
	return 1;
}

// getGroundZ(float x, float y, float z) : float
SQInteger sq_getGroundZ(SQVM * pVM)
{
	// Read call arguments
	float x, y, z;
	sq_getfloat(pVM, -3, &x);
	sq_getfloat(pVM, -2, &y);
	sq_getfloat(pVM, -1, &z);

	// Get ground z-coord
	float fGroundZ;
	Scripting::GetGroundZFor3DCoord(x, y, z, &fGroundZ);

	sq_pushfloat(pVM, fGroundZ);
	return 1;
}