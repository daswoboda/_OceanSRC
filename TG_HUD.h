#pragma once
#ifndef TG_HUD_h
#define TG_HUD_h

#include "TG_TacticalTier.h"

#include <engine.hpp>                           // for namespace godot
#include <control.hpp>                          // for Control
#include <label.hpp>                            // for label
#include <button.hpp>                           // for button
#include <h_slider.hpp>                          // for hslider
#include <v_slider.hpp>                          // for vslider
#include <input.hpp>							// for input

class TG_TacticalTier;
class T_PathsToRepo;

using namespace godot;

//								Klasa konsoliduj�ca kontrolki HUD warstwy taktycznej (19.01.2025)
/*=============================================================================================================================
	- klasa GODOT
(updated 16.03.2025)
	- dodany setter SetDiveSpeedRudderVal, SetButtonPressed
	- dodana metoda ClickButton
	- dodane wska�niki na przyciski Plus i Minus
	- dodane gettery dla przycisk�w Plus i	Minus
(updated 14.03.2025)
	- dodane kontrolki tymczasowe
	- dodane kontrolki Dive Station
(updated 28.01.2025)
	- dodany pCurrentSpeedButton
=============================================================================================================================*/
class TG_HUD : public Control
{
    GDCLASS(TG_HUD, Control)

protected:
    // Wi�zanie metod
    static void _bind_methods();

public:
    // Konstruktor i destruktor
    TG_HUD();
    ~TG_HUD();
    // Metody Godota
    void _ready() override;
    void _process(double delta) override;
	void _on_engineButton_pressed(Variant pSender);
	void _on_rudderSlider_value_changed(int iVal);
	void _on_DIVEButton_pressed(Variant pSender);
	/*void _on_z_f_button_pressed();
	void _on_z_st_button_pressed();
	void _on_button_pressed(Variant pSender, String iName);*/
    //void _input(const Ref<InputEvent> event);
	void UpdateSpeed_val(float iSpeed);
	void UpdateCourse_val(float iCourse);
	void UpdateDepth_val(float iDepth);
	void UpdateMaxDepth_val(float iMaxDepth);
	void UpdateTimePass_val(float iTimePass);
	int HandleDepthKeys();
	// Settery i gettery
	Button* GetCurrentSpeedButton()			const { return pCurrentSpeedButton; }
	Button* vFLButton()						const { return pvFL_button; }
	Button* PlusTen_button()				const { return pPlusTen_button; }
	Button* PlusTwenty_button()				const { return pPlusTwenty_button; }
	Button* PlusThirty_button()				const { return pPlusThirty_button; }
	Button* PlusFifty_button()				const { return pPlusFifty_button; }
	Button* MinusTen_button()				const { return pMinusTen_button; }
	Button* MinusTwenty_button()			const { return pMinusTwenty_button; }
	Button* MinusThirty_button()			const { return pMinusThirty_button; }
	Button* MinusFifty_button()				const { return pMinusFifty_button; }
	float GetRudderSetting()				const { return rudderSetting; }
	int GetDepthKey()						const { return depthKey; }
	bool GetDIVEButtonPressed()				const { return pDIVE_button->is_pressed(); }
	int GetDepthRudderSetting()				const { return pDepthRudder->get_value(); }
	int GetDiveSpeedRudderVal()				const { return pDiveSpeedRudder->get_value(); }

	void ResetDIVEButton()					{ pDIVE_button->set_pressed(false); }
	void SetDepthRudder(int iVal)			{ pDepthRudder->set_value(iVal); }
	void SetDiveSpeedRudderVal(int iVal)	{ pDiveSpeedRudder->set_value(iVal); }
	void DisableDIVEButton(bool iSet)		{ pDIVE_button->set_disabled(iSet); }
	void DisableSurfaceButton(bool iSet)	{ pSurface_button->set_disabled(iSet); }
	void DisablePeriscButton(bool iSet)		{ pPerisc_button->set_disabled(iSet); }
	void DisableCrashDiveButton(bool iSet)	{ pCrashDive_button->set_disabled(iSet); }
	void DisableEMBlowingButton(bool iSet)	{ pEMBlowing_button->set_disabled(iSet); }
	void DisableDepthRudder(bool iSet)		{ pDepthRudder->set_editable(iSet); }
	void ClickButton(Button* pButton)		{ _on_engineButton_pressed(pButton); }
	void SetButtonPressed(Button* pButton, bool iSet = true)	{ pButton->set_pressed(iSet); }

private:
	Input* pInput{ nullptr };					// Wska�nik na obiekt Input
	TG_TacticalTier* pTacticalTier{ nullptr };  // Wska�nik na w�ze� T_TacticalTier
	Label* pSpeed_val{ nullptr };              // Wska�nik na kontrolk� etykiety pr�dko�ci
	Label* pCourse_val{ nullptr };             // Wska�nik na kontrolk� etykiety kursu
	Label* pDepth_val{ nullptr };              // Wska�nik na kontrolk� etykiety g��boko�ci
// Wska�niki na przyciski pr�dko�ci
	Button* pzF_button{ nullptr };             // Wska�nik na przycisk zF
	Button* pzST_button{ nullptr };            // Wska�nik na przycisk zST
	Button* pzS_button{ nullptr };             // Wska�nik na przycisk zS
	Button* pzD_button{ nullptr };             // Wska�nik na przycisk zD
	Button* pSTOP_button{ nullptr };           // Wska�nik na przycisk STOP
	Button* pvD_button{ nullptr };             // Wska�nik na przycisk vD
	Button* pvS_button{ nullptr };             // Wska�nik na przycisk vS
	Button* pvST_button{ nullptr };            // Wska�nik na przycisk vST
	Button* pvF_button{ nullptr };             // Wska�nik na przycisk vF
	Button* pvFL_button{ nullptr };            // Wska�nik na przycisk vFL
	Button* pCurrentSpeedButton{ nullptr };    // Wska�nik na aktualnie wci�ni�ty przycisk pr�dko�ci
	Button* pPreviousSpeedButton{ nullptr };   // Wska�nik na poprzednio wci�ni�ty przycisk pr�dko�ci
// Wska�niki na ster kierunku
	Label* pRudderSetting_val{ nullptr };         // Wska�nik na kontrolk� etykiety ustawienia steru
	float rudderSetting{ 0.0f };                  // Aktualne ustawienie steru
	HSlider* pRudderSlider{ nullptr };            // Wska�nik na suwak steru
	int depthKey{ 0 };                            // Klawisz do zmiany g��boko�ci
// Wska�niki na kontrolki g��boko�ci
	HSlider* pDiveSpeedRudder{ nullptr };			// Wska�nik na suwak szybko�ci zanurzania
	Label* pDiveSpeedRudder_label{ nullptr };		// Wska�nik na etykiet� pr�dko�ci zanurzania
	VSlider* pDepthRudder{ nullptr };				// Wska�nik na suwak g��boko�ci
	Button* pDIVE_button{ nullptr };				// Wska�nik na przycisk zanurzania
	Button* pSurface_button{ nullptr };				// Wska�nik na przycisk powierzchni
	Button* pPerisc_button{ nullptr };				// Wska�nik na przycisk g��boko�� peryskopowa
	Button* pCrashDive_button{ nullptr };			// Wska�nik na przycisk gwa�townego zanurzenia
	Button* pEMBlowing_button{ nullptr };			// Wska�nik na przycisk wypuszczania powietrza
	Button* pPlusTen_button{ nullptr };				// Wska�nik na przycisk +10
	Button* pPlusTwenty_button{ nullptr };			// Wska�nik na przycisk +20
	Button* pPlusThirty_button{ nullptr };			// Wska�nik na przycisk +30
	Button* pPlusFifty_button{ nullptr };			// Wska�nik na przycisk +50
	Button* pMinusTen_button{ nullptr };			// Wska�nik na przycisk -10
	Button* pMinusTwenty_button{ nullptr };			// Wska�nik na przycisk -20
	Button* pMinusThirty_button{ nullptr };			// Wska�nik na przycisk -30
	Button* pMinusFifty_button{ nullptr };			// Wska�nik na przycisk -50

// Tymczasowe kontrolki
	Label* pMaxDepth_val{ nullptr };              // Wska�nik na kontrolk� etykiety maksymalnej g��boko�ci
	Label* pTimePass_val{ nullptr };              // Wska�nik na kontrolk� etykiety czasu
	Label* pMaxDepth_title{ nullptr };               // Wska�nik na kontrolk� etykiety belki
	Label* pTimePass_title{ nullptr };           // Wska�nik na kontrolk� etykiety telegrafu
};



#endif