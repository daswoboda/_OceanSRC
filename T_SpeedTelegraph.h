#pragma once
#ifndef T_SPEEDTELEGRAPH_H
#define T_SPEEDTELEGRAPH_H

#include "TG_HUD.h"
#include <input.hpp>
#include <button.hpp>                           // for button

class TG_HUD;

using namespace godot;


enum enTelegraph
{
	dieselZ_Flank = 0,
	dieselZ_Standard = 1,
	dieselZ_Slow = 2,
	dieselZ_DeadSlow = 3,
	diesel_Stop = 4,
	dieselV_DeadSlow = 5,
	dieselV_Slow = 6,
	dieselV_Standard = 7,
	dieselV_Full = 8,
	dieselV_Flank = 9,

	electricZ_Flank = 10,
	electricZ_Standard = 11,
	electricZ_Slow = 12,
	electricZ_DeadSlow = 13,
	electric_Stop = 14,
	electricV_DeadSlow = 15,
	electricV_Slow = 16,
	electricV_Standard = 17,
	electricV_Full = 18,
	electricV_Flank = 19,

	switchingToElectric = 20,
	switchingToDiesel = 21,
	enginesOff = 22,
};

enum enWorkingEngine
{
	weNone = 0,
	weDiesel = 1,
	weElectric = 2
};

enum enTelegraphState
{
	tsNone = 0,
	tsDelayCountdown = 1,
	tsReadyToChange = 2
};

//										Klasa Telegrafu prêdkoœci (11.01.2024)
/*=============================================================================================================================
	- noGodot
	- Kontener dla kontraktów vContracts znajduje siê w shipbuildingIndustryClass
=============================================================================================================================*/
class T_SpeedTelegraph
{
public:
	// Konstruktor i destruktor
	T_SpeedTelegraph();
	~T_SpeedTelegraph() {};
	// Gettery i Settery
	enTelegraph GetProperTelegraph()						const { return properTelegraph; }
	enTelegraph GetOperatingTelegraph()						const { return operatingTelegraph; }
	enTelegraphState GetTelegraphState()					const { return telegraphState; }
	float GetDelayCounter()									const { return delayCounter; }

	void SetOperatingTelegraph(enTelegraph iProperTelegraph);
	void SyncProperTelegraph()								{ properTelegraph = operatingTelegraph; }
	void ResetTelegraphState()								{ telegraphState = tsNone; }
	void SetHUD(TG_HUD* iHUD)								{ pHUD = iHUD; }
	void SetCurrentSpeedButton(Button* iButton)				{ pCurrentSpeedButton = iButton; }
	// Metody
	void Update(float iDeltaTime, bool iIsSurface);
	void SwitchToElectricEngine();
	void SwitchToDieselEngine();

private:
	// Pola
	TG_HUD* pHUD{ nullptr };
	Input* pInput{ nullptr };
	enTelegraph properTelegraph{ diesel_Stop };				// Na polu w³aœciwym dzia³anie przenoszone z telegrafu operac.  jest z opóŸnieniem symuluj¹cym pracê za³ogi
	enTelegraph operatingTelegraph{ diesel_Stop };			// Na polu operacyjnym wykonywane jest natychmiast zadane dzia³anie (np. zmiana telegramu z stop na na full)
	enTelegraphState telegraphState{ tsNone };				// Stan telegrafu
	enWorkingEngine workingEngine{ weDiesel };				// Aktualnie pracuj¹cy silnik
	float defaultTelegraphDelay{ 2.5f };					// Standardowe opóŸnienie w sekundach symuluj¹ce pracê za³ogi
	float delayCounter{ 0.0f };								// Licznik opóŸnienia do zmiany telegrafu
	Button* pCurrentSpeedButton{ nullptr };					// WskaŸnik na aktualnie wciœniêty przycisk prêdkoœci
};

#endif