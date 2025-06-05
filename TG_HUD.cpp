#include "TG_HUD.h"


#include <scene_tree.hpp>					// for get_tree()
#include <window.hpp>						// for get_root()


void TG_HUD::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("_on_engineButton_pressed", "pSender"), &TG_HUD::_on_engineButton_pressed);
	ClassDB::bind_method(D_METHOD("_on_rudderSlider_value_changed", "pSender"), &TG_HUD::_on_rudderSlider_value_changed);
	ClassDB::bind_method(D_METHOD("_on_DIVEButton_pressed", "pSender"), &TG_HUD::_on_DIVEButton_pressed);

	//ADD_SIGNAL(MethodInfo("speedTelegraphButtonPressed_signal", PropertyInfo(Variant::OBJECT, "button")));
	//ADD_SIGNAL(MethodInfo("_on_button_pressed", PropertyInfo(Variant::OBJECT, "button")));
}

TG_HUD::TG_HUD()
{
}

TG_HUD::~TG_HUD()
{
}

//											Przygotowanie HUDu do pracy (21.01.2025)
/*=============================================================================================================================
(updated 16.03.2025)
	- dodany sygna³ presssed do CrashDive_button, EMBlowing_button
	- dodane przypisanie wskaŸników na przyciski Plus i Minus
(updated 14.03.2025)
	- dodane przypisanie wskaŸników na kontrolki tymczasowe
	- dodane przypisanie wskaŸników na kontrolki Dive Station
(updated 09.02.2025)
	- dodane przypisanie wskaŸnika na pDepth_val
(updated 26.01.2025)
	- dodane przypisanie wskaŸnika na T_TacticalTier
	- dodane przypisanie wskaŸników na przyciski prêdkoœci
=============================================================================================================================*/
void TG_HUD::_ready()
{

	if (Engine::get_singleton()->is_editor_hint()) return;
// Przypisanie wskaŸnika na obiekt Input
	pInput = Input::get_singleton();
// Przypisanie wskaŸnika na T_TacticalTier, który jest dzieckiem g³ównego wêz³a
	// w Edytorze pobierany jest wêze³ Edytora a nie g³ównego wêzz³a
	auto pRoot = get_tree()->get_root();			// get_tree() zwraca typ SceneTree, get_root() zwraca typ Window
	auto pNode = pRoot->get_child(0);				// get_child(0) zwraca typ Node g³ównego wêz³a
	//auto pTT_node = pNode->get_node_or_null("TG_TacticalTier");
	auto pTT_node = get_node_or_null("../");
	if (pTT_node)
	{
		pTacticalTier = Object::cast_to<TG_TacticalTier>(pTT_node);
	}
	else
	{
		UtilityFunctions::printerr("TG-HUD: not T_TacticalTier");
	}

// Przypisanie wskaŸników na kontrolki
	if (has_node("BottomScreenPanel/HBoxContainer/Speed_val"))
		pSpeed_val = get_node<Label>("BottomScreenPanel/HBoxContainer/Speed_val");
	else
		UtilityFunctions::printerr("Node Speed_val does not exist");
	if (has_node("BottomScreenPanel/HBoxContainer/Course_val"))
		pCourse_val = get_node<Label>("BottomScreenPanel/HBoxContainer/Course_val");
	else
		UtilityFunctions::printerr("Node Course_val does not exist");
	if (has_node("BottomScreenPanel/HBoxContainer/Depth_val"))
		pDepth_val = get_node<Label>("BottomScreenPanel/HBoxContainer/Depth_val");
	else
		UtilityFunctions::printerr("Node Depth_val does not exist");

// Przypisanie wskaŸników na przyciski prêdkoœci 
	pzF_button = get_node<Button>(pTacticalTier->PathsToRepo.zF_button);
	if (!pzF_button)
		UtilityFunctions::printerr("Button zF does not exist");
	else
		pzF_button->connect("pressed", Callable(this, "_on_engineButton_pressed").bind(pzF_button));
	
	pzST_button = get_node<Button>(pTacticalTier->PathsToRepo.zST_button);
	if (!pzST_button)
		UtilityFunctions::printerr("Button zST does not exist");
	else
		pzST_button->connect("pressed", Callable(this, "_on_engineButton_pressed").bind(pzST_button));

	pzS_button = get_node<Button>(pTacticalTier->PathsToRepo.zS_button);
	if (!pzS_button)
		UtilityFunctions::printerr("Button zS does not exist");
	else
		pzS_button->connect("pressed", Callable(this, "_on_engineButton_pressed").bind(pzS_button));

	pzD_button = get_node<Button>(pTacticalTier->PathsToRepo.zD_button);
	if (!pzD_button)
		UtilityFunctions::printerr("Button zD does not exist");
	else
		pzD_button->connect("pressed", Callable(this, "_on_engineButton_pressed").bind(pzD_button));

	pSTOP_button = get_node<Button>(pTacticalTier->PathsToRepo.STOP_button);
	if (!pSTOP_button)
		UtilityFunctions::printerr("Button STOP does not exist");
	else
		pSTOP_button->connect("pressed", Callable(this, "_on_engineButton_pressed").bind(pSTOP_button));

	pvD_button = get_node<Button>(pTacticalTier->PathsToRepo.vD_button);
	if (!pvD_button)
		UtilityFunctions::printerr("Button vD does not exist");
	else
		pvD_button->connect("pressed", Callable(this, "_on_engineButton_pressed").bind(pvD_button));

	pvS_button = get_node<Button>(pTacticalTier->PathsToRepo.vS_button);
	if (!pvS_button)
		UtilityFunctions::printerr("Button vS does not exist");
	else
		pvS_button->connect("pressed", Callable(this, "_on_engineButton_pressed").bind(pvS_button));

	pvST_button = get_node<Button>(pTacticalTier->PathsToRepo.vST_button);
	if (!pvST_button)
		UtilityFunctions::printerr("Button vST does not exist");
	else
		pvST_button->connect("pressed", Callable(this, "_on_engineButton_pressed").bind(pvST_button));

	pvF_button = get_node<Button>(pTacticalTier->PathsToRepo.vF_button);
	if (!pvF_button)
		UtilityFunctions::printerr("Button vF does not exist");
	else
		pvF_button->connect("pressed", Callable(this, "_on_engineButton_pressed").bind(pvF_button));
	
	pvFL_button = get_node<Button>(pTacticalTier->PathsToRepo.vFL_button);
	if (!pvFL_button)
		UtilityFunctions::printerr("Button vFL does not exist");
	else
		pvFL_button->connect("pressed", Callable(this, "_on_engineButton_pressed").bind(pvFL_button));
	
	pCurrentSpeedButton = pSTOP_button;
	pSTOP_button->set_pressed(true);

// Przypisanie wskaŸników na ster kierunku
	pRudderSetting_val = get_node<Label>(pTacticalTier->PathsToRepo.Rudder_label);
	if (!pRudderSetting_val)
		UtilityFunctions::printerr("Label Rudder_label does not exist");

	pRudderSlider = get_node<HSlider>(pTacticalTier->PathsToRepo.Rudder_slider);
	if (!pRudderSlider)
		UtilityFunctions::printerr("Slider Rudder_slider does not exist");
	else
		pRudderSlider->connect("value_changed", Callable(this, "_on_rudderSlider_value_changed").bind(pRudderSlider));
	pRudderSlider->set_ticks(11);
	pRudderSlider->set_ticks_on_borders(false);

// Przypisanie wskaŸników na kontrolki g³êbokoœci
	pDiveSpeedRudder = get_node<HSlider>(pTacticalTier->PathsToRepo.Dive_hslider);
	if (!pDiveSpeedRudder)
		UtilityFunctions::printerr("Slider DiveSpeed_hslider does not exist");
	pDiveSpeedRudder_label = get_node<Label>(pTacticalTier->PathsToRepo.DiveSpeed_label);
	if (!pDiveSpeedRudder_label)
		UtilityFunctions::printerr("Label DiveSpeed_label does not exist");
	pDepthRudder = get_node<VSlider>(pTacticalTier->PathsToRepo.Dive_vslider);
	if (!pDepthRudder)
		UtilityFunctions::printerr("Slider Dive_vslider does not exist");
	pDIVE_button = get_node<Button>(pTacticalTier->PathsToRepo.DIVE_button);
	if (!pDIVE_button)
		UtilityFunctions::printerr("Button DIVE does not exist");
	else
		pDIVE_button->connect("pressed", Callable(this, "_on_DIVEButton_pressed").bind(pDIVE_button));
	
	pSurface_button = get_node<Button>(pTacticalTier->PathsToRepo.Surface_button);
	if (!pSurface_button)
		UtilityFunctions::printerr("Button Surface does not exist");
	else
		pSurface_button->connect("pressed", Callable(pTacticalTier, "_on_SurfaceButton_pressed").bind(pSurface_button));
	
	pPerisc_button = get_node<Button>(pTacticalTier->PathsToRepo.Perisc_button);
	if (!pPerisc_button)
		UtilityFunctions::printerr("Button Perisc does not exist");
	else
		pPerisc_button->connect("pressed", Callable(pTacticalTier, "_on_PeriscButton_pressed").bind(pPerisc_button));
	
	pCrashDive_button = get_node<Button>(pTacticalTier->PathsToRepo.CrashDive_button);
	if (!pCrashDive_button)
		UtilityFunctions::printerr("Button CrashDive does not exist");
	else
		pCrashDive_button->connect("pressed", Callable(pTacticalTier, "_on_CrashDive_pressed").bind(pCrashDive_button));
	
	pEMBlowing_button = get_node<Button>(pTacticalTier->PathsToRepo.EMBlowing_button);
	if (!pEMBlowing_button)
		UtilityFunctions::printerr("Button EMBlowing does not exist");
	else
		pEMBlowing_button->connect("pressed", Callable(pTacticalTier, "_on_EMBlowing_pressed").bind(pEMBlowing_button));

	pPlusTen_button = get_node<Button>(pTacticalTier->PathsToRepo.PlusTen_button);
	if (!pPlusTen_button)
		UtilityFunctions::printerr("Button PlusTen does not exist");
	else
		pPlusTen_button->connect("pressed", Callable(pTacticalTier, "_on_DepthPlusMinus_pressed").bind(pPlusTen_button));

	pPlusTwenty_button = get_node<Button>(pTacticalTier->PathsToRepo.PlusTwenty_button);
	if (!pPlusTwenty_button)
		UtilityFunctions::printerr("Button PlusTwenty does not exist");
	else
		pPlusTwenty_button->connect("pressed", Callable(pTacticalTier, "_on_DepthPlusMinus_pressed").bind(pPlusTwenty_button));

	pPlusThirty_button = get_node<Button>(pTacticalTier->PathsToRepo.PlusThirty_button);
	if (!pPlusThirty_button)
		UtilityFunctions::printerr("Button PlusThirty does not exist");
	else
		pPlusThirty_button->connect("pressed", Callable(pTacticalTier, "_on_DepthPlusMinus_pressed").bind(pPlusThirty_button));

	pPlusFifty_button = get_node<Button>(pTacticalTier->PathsToRepo.PlusFifty_button);
	if (!pPlusFifty_button)
		UtilityFunctions::printerr("Button PlusFifty does not exist");
	else
		pPlusFifty_button->connect("pressed", Callable(pTacticalTier, "_on_DepthPlusMinus_pressed").bind(pPlusFifty_button));

	pMinusTen_button = get_node<Button>(pTacticalTier->PathsToRepo.MinusTen_button);
	if (!pMinusTen_button)
		UtilityFunctions::printerr("Button MinusTen does not exist");
	else
		pMinusTen_button->connect("pressed", Callable(pTacticalTier, "_on_DepthPlusMinus_pressed").bind(pMinusTen_button));

	pMinusTwenty_button = get_node<Button>(pTacticalTier->PathsToRepo.MinusTwenty_button);
	if (!pMinusTwenty_button)
		UtilityFunctions::printerr("Button MinusTwenty does not exist");
	else
		pMinusTwenty_button->connect("pressed", Callable(pTacticalTier, "_on_DepthPlusMinus_pressed").bind(pMinusTwenty_button));

	pMinusThirty_button = get_node<Button>(pTacticalTier->PathsToRepo.MinusThirty_button);
	if (!pMinusThirty_button)
		UtilityFunctions::printerr("Button MinusThirty does not exist");
	else
		pMinusThirty_button->connect("pressed", Callable(pTacticalTier, "_on_DepthPlusMinus_pressed").bind(pMinusThirty_button));

	pMinusFifty_button = get_node<Button>(pTacticalTier->PathsToRepo.MinusFifty_button);
	if (!pMinusFifty_button)
		UtilityFunctions::printerr("Button MinusFifty does not exist");
	else
		pMinusFifty_button->connect("pressed", Callable(pTacticalTier, "_on_DepthPlusMinus_pressed").bind(pMinusFifty_button));
	
	// Tymczasowe przypisanie wskaŸników na etykiety MaxDepth i TimePass
	pMaxDepth_val = get_node<Label>(pTacticalTier->PathsToRepo.MaxDepth_val);
	pTimePass_val = get_node<Label>(pTacticalTier->PathsToRepo.TimePass_val);
	pMaxDepth_title = get_node<Label>(pTacticalTier->PathsToRepo.MaxDepth_title);
	pTimePass_title = get_node<Label>(pTacticalTier->PathsToRepo.TimePass_title);
}

void TG_HUD::_process(double delta)
{
	
}

//											Obs³uga przycisków prêdkoœci (31.01.2025)
/*=============================================================================================================================

=============================================================================================================================*/
void TG_HUD::_on_engineButton_pressed(Variant pSender)
{
	Button* pButton = Object::cast_to<Button>(pSender);
	if (pButton != pCurrentSpeedButton)
	{
		if (pCurrentSpeedButton)
			pCurrentSpeedButton->set_pressed(false);
		pPreviousSpeedButton = pCurrentSpeedButton;
		pCurrentSpeedButton = pButton;
		pCurrentSpeedButton->set_pressed(true);
		pCurrentSpeedButton->grab_focus();
	}
}

//											Obs³uga steru kierunku (07.02.2025)
/*=============================================================================================================================

=============================================================================================================================*/
void TG_HUD::_on_rudderSlider_value_changed(int iVal)
{
	if (pRudderSetting_val)
	{
		UtilityFunctions::print(iVal);
		/*char buffer[50];
		snprintf(buffer, sizeof(buffer), "%.02f%", pRudderSlider->get_value());
		String sRudder = buffer;*/
		pRudderSetting_val->set_text(String::num_int64(iVal));
	}
	rudderSetting = (float)iVal;
}

//											Obs³uga przycisku ZANU¯ENIE (10.03.2025)
/*=============================================================================================================================

=============================================================================================================================*/
void TG_HUD::_on_DIVEButton_pressed(Variant pSender)
{
	Button* pButton = Object::cast_to<Button>(pSender);
	if (pButton->is_pressed()) return;
	
	pButton->set_pressed(true);
}


//								Uaktualnienie wartoœci  labela Speed_val (20.01.2025)
/*=============================================================================================================================
 - wywo³ywana w T_PlayerUboot::_process
=============================================================================================================================*/
void TG_HUD::UpdateSpeed_val(float iSpeed)
{
	if (pSpeed_val)
	{
		char buffer[50];
		snprintf(buffer, sizeof(buffer), "%.02f%", iSpeed);
		String sSpeed = buffer;
		pSpeed_val->set_text(sSpeed + " knots");
	}
	else
		UtilityFunctions::printerr("Label Speed_val does not exist");
}

//								Aktualizacja wartoœci  labela Course_val (08.02.2025)
/*=============================================================================================================================
 - wywo³ywana w T_PlayerUboot::_process
=============================================================================================================================*/
void TG_HUD::UpdateCourse_val(float iCourse)
{
	if (!pCourse_val) return;
	
	char buffer[50];
	snprintf(buffer, sizeof(buffer), "%.02f", iCourse);
	String sCourse = buffer;
	pCourse_val->set_text(sCourse);
}

//								Aktualizacja wartoœci  labela depth_val (09.02.2025)
/*=============================================================================================================================
 - wywo³ywana w T_PlayerUboot::_process
=============================================================================================================================*/
void TG_HUD::UpdateDepth_val(float iDepth)
{
	if (!pDepth_val) return;
	char buffer[50];
	snprintf(buffer, sizeof(buffer), "%.02f", iDepth);
	String sDepth = buffer;
	pDepth_val->set_text(sDepth);
}

void TG_HUD::UpdateMaxDepth_val(float iMaxDepth)
{
	if (!pMaxDepth_val) return;
	char buffer[50];
	snprintf(buffer, sizeof(buffer), "%.02f", iMaxDepth);
	String sMaxDepth = buffer;
	pMaxDepth_val->set_text(sMaxDepth);
}

void TG_HUD::UpdateTimePass_val(float iTimePass)
{
	if (!pTimePass_val) return;
	char buffer[50];
	snprintf(buffer, sizeof(buffer), "%.02f", iTimePass);
	String sTimePass = buffer;
	pTimePass_val->set_text(sTimePass);
}

//										Obs³uga klawiszy g³êbokoœci (09.02.2025)
/*=============================================================================================================================
 - wywo³ywana w T_PlayerUboot::_process
=============================================================================================================================*/
int TG_HUD::HandleDepthKeys()
{
	return pInput->get_action_strength("sub_dec_depth") - pInput->get_action_strength("sub_inc_depth");
}

