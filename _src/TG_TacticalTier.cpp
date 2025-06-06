#include "TG_TacticalTier.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <scene_tree.hpp>					// for get_tree()
#include <window.hpp>						// for get_root()


void TG_TacticalTier::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("_on_SurfaceButton_pressed", "pSender"), &TG_TacticalTier::_on_SurfaceButton_pressed);
	ClassDB::bind_method(D_METHOD("_on_PeriscButton_pressed", "pSender"), &TG_TacticalTier::_on_PeriscButton_pressed);
	ClassDB::bind_method(D_METHOD("_on_CrashDive_pressed", "pSender"), &TG_TacticalTier::_on_CrashDive_pressed);
	ClassDB::bind_method(D_METHOD("_on_EMBlowing_pressed", "pSender"), &TG_TacticalTier::_on_EMBlowing_pressed);
	ClassDB::bind_method(D_METHOD("_on_DepthPlusMinus_pressed", "pSender"), &TG_TacticalTier::_on_DepthPlusMinus_pressed);
}

//											Przygotowanie komponentu TacticalTier (15.03.2025)
/*=============================================================================================================================
(updated 31.05.2025)
	- dodane przypisanie wskaŸnika na OceanEnvironment, który jest wêz³em nadrzêdnym
=============================================================================================================================*/
void TG_TacticalTier::_ready()
{
// Przypisanie wskaŸnika na OceanEnvironment, który jest wêz³em nadrzêdnym
	auto pRoot2 = get_tree()->get_root();
	auto pOE = pRoot2->find_child("OceanEnvironment", true, false);
	if (pOE)
	{
		pOceanEnvironment = pOE;
	}
	else 
	{
		UtilityFunctions::printerr("TacticalTier: not found OceanEnvironment");
	}
// Przypisanie wskaŸnika na TG_PlayerUboot, który jest dzieckiem tego wêz³a
	if (has_node("TG_Wolfs/TG_PlayerUboot"))
		pPlayerUboot = get_node<TG_PlayerUboot>("TG_Wolfs/TG_PlayerUboot");
	else
		UtilityFunctions::printerr("TacticalTier: not found TG_PlayerUboot");

	UtilityFunctions::print("TacticalTier ready InEditor(2/3)");
	
	if (Engine::get_singleton()->is_editor_hint()) return;
	
// Przypisanie wskaŸnika na HUD, który jest dzieckiem g³ównego wêz³a
	auto pRoot = get_tree()->get_root();				// get_tree() zwaraca typ SceneTree, get_root() zwraca typ Window
	auto pNode = pRoot->get_child(0);					// get_child(0) zwraca typ Node g³ównego wêz³a
	//auto pHUD_node = pNode->get_node_or_null("TG_HUD");
	auto pHUD_node = get_node_or_null("TG_HUD");
	if (pHUD_node)
	{
		pHUD = Object::cast_to<TG_HUD>(pHUD_node);
	}
	else
	{
		UtilityFunctions::printerr("TacticalTier: not child node with HUD");
	}

	UtilityFunctions::print("TacticalTier ready");
}

void TG_TacticalTier::_process(double delta)
{
	if (Engine::get_singleton()->is_editor_hint()) return; // Early return if we are in editor

}

//											Obs³uga przycisku Surface (15.03.2025)
/*=============================================================================================================================

=============================================================================================================================*/
void TG_TacticalTier::_on_SurfaceButton_pressed(Variant pSender)
{
	if (!pPlayerUboot) return;
	if (pPlayerUboot->IsSurface()) return;
	
	Button* pButton = Object::cast_to<Button>(pSender);
	pPlayerUboot->SetDesiredDepthOff();
	pHUD->SetDepthRudder(1);
	UtilityFunctions::print("Heading to surface, Sir");
}

//											Obs³uga przycisku Perisc (15.03.2025)
/*=============================================================================================================================

=============================================================================================================================*/
void TG_TacticalTier::_on_PeriscButton_pressed(Variant pSender)
{
	if (!pPlayerUboot) return;
	if (pPlayerUboot->IsSurface()) return;

	Button* pButton = Object::cast_to<Button>(pSender);
	pPlayerUboot->SetDesiredDepth(12.5f);
	if (pPlayerUboot->GetActualDepth() < 12.5f)
	{
		pHUD->SetDepthRudder(-1);
	}
	else
	{
		pHUD->SetDepthRudder(1);
	}
	UtilityFunctions::print("Heading to periscop depth, Sir");
}

//											Obs³uga przycisku CrashDive (16.03.2025)
/*=============================================================================================================================

=============================================================================================================================*/
void TG_TacticalTier::_on_CrashDive_pressed(Variant pSender)
{
	if (!pPlayerUboot) return;
	if (!pPlayerUboot->IsSurface()) return;

	Button* pButton = Object::cast_to<Button>(pSender);
	pHUD->SetButtonPressed(pButton);
	pHUD->SetDepthRudder(-1);
	pHUD->SetDiveSpeedRudderVal(3);
	pHUD->ResetDIVEButton();
	pHUD->DisableDIVEButton(true);
	pPlayerUboot->SetIsDivingFromSurface(false);
	pPlayerUboot->SetIsDivingPossible(true);
	pPlayerUboot->SetIsRequiredDepthStabilization(true);
	pPlayerUboot->SpeedTelegraph()->SwitchToElectricEngine();
	//pHUD->SetButtonPressed(pHUD->vFLButton());
	pHUD->ClickButton(pHUD->vFLButton());
	pPlayerUboot->SetDesiredDepth(100.0f);

	
	UtilityFunctions::print("ALARM! ALARM! ALARM!");
}

//											Obs³uga przycisku EMBlowing (16.03.2025)
/*=============================================================================================================================

=============================================================================================================================*/
void TG_TacticalTier::_on_EMBlowing_pressed(Variant pSender)
{
	if (!pPlayerUboot) return;
	if (pPlayerUboot->IsSurface()) return;
	
	Button* pButton = Object::cast_to<Button>(pSender);
	pHUD->SetButtonPressed(pButton);
	pPlayerUboot->SetIsDivingFromSurface(false);
	pPlayerUboot->SetIsDivingPossible(true);
	pPlayerUboot->SetIsRequiredDepthStabilization(true);
	pHUD->SetDepthRudder(1);
	pHUD->SetDiveSpeedRudderVal(3);
	pHUD->ResetDIVEButton();
	pHUD->DisableDIVEButton(true);
	pHUD->ClickButton(pHUD->vFLButton());
	pPlayerUboot->SetDesiredDepth(2.0f);
	UtilityFunctions::print("EMERGENCY BLOWING, Sir");
}

//											Obs³uga przycisków Depth Plus/Minus (16.03.2025)
/*=============================================================================================================================

=============================================================================================================================*/
void TG_TacticalTier::_on_DepthPlusMinus_pressed(Variant pSender)
{
	if (!pPlayerUboot) return;
	if (pPlayerUboot->IsSurface()) return;
	
	Button* pButton = Object::cast_to<Button>(pSender);
	
// Set desired depth
	if (pButton == pHUD->PlusTen_button())
	{
		pPlayerUboot->SetDesiredDepth(pPlayerUboot->GetActualDepth() + 10.0f);
	}
	else if (pButton == pHUD->PlusTwenty_button())
	{
		pPlayerUboot->SetDesiredDepth(pPlayerUboot->GetActualDepth() + 20.0f);
	}
	else if (pButton == pHUD->PlusThirty_button())
	{
		pPlayerUboot->SetDesiredDepth(pPlayerUboot->GetActualDepth() + 30.0f);
	}
	else if (pButton == pHUD->PlusFifty_button())
	{
		pPlayerUboot->SetDesiredDepth(pPlayerUboot->GetActualDepth() + 50.0f);
	}
	else if (pButton == pHUD->MinusTen_button())
	{
		pPlayerUboot->SetDesiredDepth(pPlayerUboot->GetActualDepth() - 10.0f);
	}
	else if (pButton == pHUD->MinusTwenty_button())
	{
		pPlayerUboot->SetDesiredDepth(pPlayerUboot->GetActualDepth() - 20.0f);
	}
	else if (pButton == pHUD->MinusThirty_button())
	{
		pPlayerUboot->SetDesiredDepth(pPlayerUboot->GetActualDepth() - 30.0f);
	}
	else if (pButton == pHUD->MinusFifty_button())
	{
		pPlayerUboot->SetDesiredDepth(pPlayerUboot->GetActualDepth() - 50.0f);
	}

// Depth control
	if (pPlayerUboot->GetDesiredDepth() < 3.0f)
		pPlayerUboot->SetDesiredDepth(4.0f);

// Set rudder
	if (pPlayerUboot->GetDesiredDepth() > pPlayerUboot->GetActualDepth())
	{
		pHUD->SetDepthRudder(-1);
	}
	else
	{
		pHUD->SetDepthRudder(1);
	}

	UtilityFunctions::print("Depth set to " + String::num_int64(pPlayerUboot->GetDesiredDepth()) + " meters");
	pHUD->SetButtonPressed(pButton, false);
}

//										Obliczenie œrodków 4 boków Prostok¹ta (16.05.2025)
/*=============================================================================================================================
- powered by Gemini
(updated 18.05.2025)
	- poprawki obliczeñ do wspó³rzêdnych osi Godota
=============================================================================================================================*/
std::vector<Vector2> Rectangle::GetSideMidpoints() const
{
	Vector2 center = GetCenter();
	return
	{
		{center.x, topLeft.y},				// Œrodek górnego boku
		{topLeft.x - width, center.y},		// Œrodek prawego boku
		{center.x, topLeft.y - height},		// Œrodek dolnego boku
		{topLeft.x, center.y}				// Œrodek lewego boku
	};
}
