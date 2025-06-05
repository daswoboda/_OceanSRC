#include "T_SpeedTelegraph.h"



T_SpeedTelegraph::T_SpeedTelegraph()
{
	
	pInput = Input::get_singleton();
	UtilityFunctions::print("T_SpeedTelegraph constructed");
}

//										Ustawienie nowej nastawy telegrafu (11.01.2024)
/*=============================================================================================================================
	
=============================================================================================================================*/
void T_SpeedTelegraph::SetOperatingTelegraph(enTelegraph iProperTelegraph)
{
	if (operatingTelegraph == iProperTelegraph) return;

	telegraphState = tsDelayCountdown;
	operatingTelegraph = iProperTelegraph;
	delayCounter = defaultTelegraphDelay;
	UtilityFunctions::print("Telegraph set to: ", (int)iProperTelegraph);
	Variant format_string = "We're waiting for {iProperTelegraph}";

}

//										Aktualizacja telegrafu (11.01.2024)
/*=============================================================================================================================
	- wywo³ywana przez T_PlayerUboot ProcessSpeedTelegraph() w ka¿dej pêtli
(updated 11.03.2025)
	- rezygnacja z iIsSurface na rzecz workingEngine
=============================================================================================================================*/
void T_SpeedTelegraph::Update(float iDeltaTime, bool iIsSurface)
{
// Aktualizacja odliczania opóŸnienia
	if (telegraphState == tsDelayCountdown)
	{
		delayCounter -= iDeltaTime;
		if (delayCounter <= 0.0f)
		{
			telegraphState = tsReadyToChange;
			UtilityFunctions::print("Delay countdown finished");
		}
	}

// Pobranie aktualnie wciœniêtego przycisku prêdkoœci i ustawienie odpowiedniego telegrafu
	if (pHUD->GetCurrentSpeedButton() != pCurrentSpeedButton || telegraphState == tsReadyToChange)
	{
		pCurrentSpeedButton = pHUD->GetCurrentSpeedButton();
		
		if (pCurrentSpeedButton->get_name() == StringName("STOP_button"))
		{
			if (workingEngine == weDiesel)
			{
				SetOperatingTelegraph(diesel_Stop);
			}
			else
			{
				SetOperatingTelegraph(electric_Stop);
			}
		}
		if (pCurrentSpeedButton->get_name() == StringName("vD_button"))
		{
			if (workingEngine == weDiesel)
			{
				SetOperatingTelegraph(dieselV_DeadSlow);
			}
			else
			{
				SetOperatingTelegraph(electricV_DeadSlow);
			}
		}
		if (pCurrentSpeedButton->get_name() == StringName("vS_button"))
		{
			if (workingEngine == weDiesel)
			{
				SetOperatingTelegraph(dieselV_Slow);
			}
			else
			{
				SetOperatingTelegraph(electricV_Slow);
			}
		}
		if (pCurrentSpeedButton->get_name() == StringName("vST_button"))
		{
			if (workingEngine == weDiesel)
			{
				SetOperatingTelegraph(dieselV_Standard);
			}
			else
			{
				SetOperatingTelegraph(electricV_Standard);
			}
		}
		if (pCurrentSpeedButton->get_name() == StringName("vF_button"))
		{
			if (workingEngine == weDiesel)
			{
				SetOperatingTelegraph(dieselV_Full);
			}
			else
			{
				SetOperatingTelegraph(electricV_Full);
			}
		}
		if (pCurrentSpeedButton->get_name() == StringName("vFL_button"))
		{
			if (workingEngine == weDiesel)
			{
				SetOperatingTelegraph(dieselV_Flank);
			}
			else
			{
				SetOperatingTelegraph(electricV_Flank);
			}
		}
		if (pCurrentSpeedButton->get_name() == StringName("zD_button"))
		{
			if (workingEngine == weDiesel)
			{
				SetOperatingTelegraph(dieselZ_DeadSlow);
			}
			else
			{
				SetOperatingTelegraph(electricZ_DeadSlow);
			}
		}
		if (pCurrentSpeedButton->get_name() == StringName("zS_button"))
		{
			if (workingEngine == weDiesel)
			{
				SetOperatingTelegraph(dieselZ_Slow);
			}
			else
			{
				SetOperatingTelegraph(electricZ_Slow);
			}
		}
		if (pCurrentSpeedButton->get_name() == StringName("zST_button"))
		{
			if (workingEngine == weDiesel)
			{
				SetOperatingTelegraph(dieselZ_Standard);
			}
			else
			{
				SetOperatingTelegraph(electricZ_Standard);
			}
		}
		if (pCurrentSpeedButton->get_name() == StringName("zF_button"))
		{
			if (workingEngine == weDiesel)
			{
				SetOperatingTelegraph(dieselZ_Flank);
			}
			else
			{
				SetOperatingTelegraph(electricZ_Flank);
			}
		}
		
	}
	
}

//										Prze³¹czenie na silnik elektryczny (11.03.2025)
/*=============================================================================================================================
	- wywo³ywana przez T_PlayerUboot _process
=============================================================================================================================*/
void T_SpeedTelegraph::SwitchToElectricEngine()
{
	if (workingEngine == weElectric) return;
	
	workingEngine = weElectric;
	if (properTelegraph == diesel_Stop)
	{
		properTelegraph = electric_Stop;
	}
	if (properTelegraph == dieselV_DeadSlow)
	{
		properTelegraph = electricV_DeadSlow;
	}
	if (properTelegraph == dieselV_Slow)
	{
		properTelegraph = electricV_Slow;
	}
	if (properTelegraph == dieselV_Standard)
	{
		properTelegraph = electricV_Standard;
	}
	if (properTelegraph == dieselV_Full)
	{
		properTelegraph = electricV_Full;
	}
	if (properTelegraph == dieselV_Flank)
	{
		properTelegraph = electricV_Flank;
	}
	if (properTelegraph == dieselZ_DeadSlow)
	{
		properTelegraph = electricZ_DeadSlow;
	}
	if (properTelegraph == dieselZ_Slow)
	{
		properTelegraph = electricZ_Slow;
	}
	if (properTelegraph == dieselZ_Standard)
	{
		properTelegraph = electricZ_Standard;
	}
	if (properTelegraph == dieselZ_Flank)
	{
		properTelegraph = electricZ_Flank;
	}
	telegraphState = tsDelayCountdown;
	delayCounter = 0.0f;
}

//										Prze³¹czenie na silnik Diesla (14.03.2025)
/*=============================================================================================================================

=============================================================================================================================*/
void T_SpeedTelegraph::SwitchToDieselEngine()
{
	if (workingEngine == weDiesel) return;

	workingEngine = weDiesel;
	
	if (properTelegraph == electric_Stop)
	{
		properTelegraph = diesel_Stop;
	}
	if (properTelegraph == electricV_DeadSlow)
	{
		properTelegraph = dieselV_DeadSlow;
	}
	if (properTelegraph == electricV_Slow)
	{
		properTelegraph = dieselV_Slow;
	}
	if (properTelegraph == electricV_Standard)
	{
		properTelegraph = dieselV_Standard;
	}
	if (properTelegraph == electricV_Full)
	{
		properTelegraph = dieselV_Full;
	}
	if (properTelegraph == electricV_Flank)
	{
		properTelegraph = dieselV_Flank;
	}
	if (properTelegraph == electricZ_DeadSlow)
	{
		properTelegraph = dieselZ_DeadSlow;
	}
	if (properTelegraph == electricZ_Slow)
	{
		properTelegraph = dieselZ_Slow;
	}
	if (properTelegraph == electricZ_Standard)
	{
		properTelegraph = dieselZ_Standard;
	}
	if (properTelegraph == electricZ_Flank)
	{
		properTelegraph = dieselZ_Flank;
	}
	
	telegraphState = tsDelayCountdown;
	delayCounter = 10.0f;
}
