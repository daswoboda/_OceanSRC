#pragma once
#ifndef T_TacticalTier_h
#define T_TacticalTier_h

#include <variant> // dla std::variant

#include <node3D.hpp>
#include "TG_HUD.h"
#include "TG_PlayerUboot.h"
//#include "T_SpeedTelegraph.h"

class TG_HUD;
class TG_PlayerUboot;

using namespace godot;

//										Struktura Prostok�ta (15.05.2025)
/*=============================================================================================================================
- powered by Gemini
(updated 18.05.2025)
	- poprawki oblicze� do wsp�rz�dnych osi Godota
=============================================================================================================================*/
/// <summary>
/// Struktura reprezentuj�ca prostok�t oraz metody do obliczania jego w�a�ciwo�ci geometrycznych.
/// </summary>
struct Rectangle
{
	Vector2 topLeft;
	real_t width;
	real_t height;

	Vector2 GetCenter()							const { return Vector2{ static_cast<real_t>(topLeft.x - width / 2.0), static_cast<real_t>(topLeft.y - height / 2.0) }; }
	float GetPerimeter()						const { return 2.0 * (width + height); }
	std::vector<Vector2> GetCorners()			const { return {topLeft,{topLeft.x - width, topLeft.y},{topLeft.x - width, topLeft.y - height},{topLeft.x, topLeft.y - height}}; }	// lewy g�rny, prawy g�rny, prawy dolny, lewy dolny
	float GetArea()								const { return width * height; }
	std::vector<Vector2> GetSideMidpoints()		const;		// Obliczenie �rodk�w 4 bok�w Prostok�ta
};

//										Struktura Okr�gu (15.05.2025)
/*=============================================================================================================================
- powered by Gemini
=============================================================================================================================*/
struct Circle
{
	Vector2 center;
	float radius;
};

//										Struktura Obszaru Patrolowego (15.05.2025)
/*=============================================================================================================================
- powered by Gemini
=============================================================================================================================*/
struct PatrolZone
{
	std::variant<Rectangle, Circle> zone;
	// Mo�na doda� pole typu enum, aby �atwiej identyfikowa� aktywny typ,
	// chocia� std::visit lub std::holds_alternative s� preferowane.
};

//										Struktura Okr�tu Eskortowego (15.05.2025)
/*=============================================================================================================================
- powered by Gemini
=============================================================================================================================*/
struct stEscortShip
{
	Vector2 position;
	PatrolZone patrolZone;		// Lub bezpo�rednio Circle/Rectangle, je�li uproszczono
};


//								Klasa przechowuj�ca �cie�ki do w�z��w (25.01.2025)
/*=============================================================================================================================
	- klasa noGODOT
=============================================================================================================================*/
class T_PathsToRepo
{
public:
    const NodePath Speed_val{ "BottomScreenPanel/HBoxContainer/Speed_val" };			                // �cie�ka do etykiety pr�dko�ci
	const NodePath Belka_label{ "U-Boot Control Panel/VBox/Belka/Belka_label" };	                    // �cie�ka do labela Belka_label
	const NodePath Telegraph_label{ "U-Boot Control Panel/VBox/Telegraph_tab/Telegraph_label" };	    // �cie�ka do labela Telegraph_label
	const NodePath zF_button{ "U-Boot Control Panel/VBox/EngineButtons_container/zF_button" };          // �cie�ka do przycisku zF_button
	const NodePath zST_button{ "U-Boot Control Panel/VBox/EngineButtons_container/zST_button" };        // �cie�ka do przycisku zST_button
	const NodePath zS_button{ "U-Boot Control Panel/VBox/EngineButtons_container/zS_button" };          // �cie�ka do przycisku zS_button
	const NodePath zD_button{ "U-Boot Control Panel/VBox/EngineButtons_container/zD_button" };          // �cie�ka do przycisku zD_button
	const NodePath STOP_button{ "U-Boot Control Panel/VBox/EngineButtons_container/STOP_button" };      // �cie�ka do przycisku STOP_button
	const NodePath vD_button{ "U-Boot Control Panel/VBox/EngineButtons_container/vD_button" };          // �cie�ka do przycisku vD_button
	const NodePath vS_button{ "U-Boot Control Panel/VBox/EngineButtons_container/vS_button" };          // �cie�ka do przycisku vS_button
	const NodePath vST_button{ "U-Boot Control Panel/VBox/EngineButtons_container/vST_button" };        // �cie�ka do przycisku vST_button
	const NodePath vF_button{ "U-Boot Control Panel/VBox/EngineButtons_container/vF_button" };          // �cie�ka do przycisku vF_button
	const NodePath vFL_button{ "U-Boot Control Panel/VBox/EngineButtons_container/vFL_button" };		// �cie�ka do przycisku vFL_button
	const NodePath Rudder_label{ "U-Boot Control Panel/VBox/Rudder_panel/Rudder_vbox/RudderVal_label" };// �cie�ka do labela Rudder_label
	const NodePath Rudder_slider{ "U-Boot Control Panel/VBox/Rudder_panel/Rudder_vbox/Rudder_slider" };	// �cie�ka do suwaka Rudder_slider
	const NodePath Dive_hslider{ "U-Boot Control Panel/VBox/Dive_panel/Dive_hslider" };					// �cie�ka do suwaka Dive_hslider
	const NodePath DiveSpeed_label{ "U-Boot Control Panel/VBox/Dive_panel/Dive_hslider/DiveSpeed_label" };// �cie�ka do labela Dive_label
	const NodePath Dive_vslider{ "U-Boot Control Panel/VBox/Dive_panel/Dive_vslider" };					// �cie�ka do Dive_vslider
	const NodePath DIVE_button{ "U-Boot Control Panel/VBox/Dive_panel/DIVE_button" };					// �cie�ka do DIVE_button
	const NodePath Surface_button{ "U-Boot Control Panel/VBox/Dive_panel/Dive_hbox/Surfce_button" };				// �cie�ka do Surface_button
	const NodePath Perisc_button{ "U-Boot Control Panel/VBox/Dive_panel/Dive_hbox/Perisc_button" };				// �cie�ka do Perisc_button
	const NodePath CrashDive_button{ "U-Boot Control Panel/VBox/Dive_panel/Dive_hbox/Crash_button" };				// �cie�ka do CrashDive_button
	const NodePath EMBlowing_button{ "U-Boot Control Panel/VBox/Dive_panel/Dive_hbox/EmBlow_button" };			// �cie�ka do EMBlowing_button
	const NodePath PlusTen_button{ "U-Boot Control Panel/VBox/Dive_panel/Dive_grid/Plus10_button" };			// �cie�ka do PlusTen_button
	const NodePath PlusTwenty_button{ "U-Boot Control Panel/VBox/Dive_panel/Dive_grid/Plus20_button" };		// �cie�ka do PlusTwenty_button
	const NodePath PlusThirty_button{ "U-Boot Control Panel/VBox/Dive_panel/Dive_grid/Plus30_button" };		// �cie�ka do PlusThirty_button
	const NodePath PlusFifty_button{ "U-Boot Control Panel/VBox/Dive_panel/Dive_grid/Plus50_button" };		// �cie�ka do PlusFifty_button
	const NodePath MinusTen_button{ "U-Boot Control Panel/VBox/Dive_panel/Dive_grid/Minus10_button" };		// �cie�ka do MinusTen_button
	const NodePath MinusTwenty_button{ "U-Boot Control Panel/VBox/Dive_panel/Dive_grid/Minus20_button" };	// �cie�ka do MinusTwenty_button
	const NodePath MinusThirty_button{ "U-Boot Control Panel/VBox/Dive_panel/Dive_grid/Minus30_button" };	// �cie�ka do MinusThirty_button
	const NodePath MinusFifty_button{ "U-Boot Control Panel/VBox/Dive_panel/Dive_grid/Minus50_button" };	// �cie�ka do MinusFifty_button
	const NodePath OceanEnv{ "OceanEnvironment" };
	// Sceny
	const String LibertyShipScene{ "res://_Assets/Ships/Liberty_ship_2.tscn" };								// �cie�ka do sceny statku Liberty
	// Tymczasowe
	const NodePath MaxDepth_title{ "BottomScreenPanel/HBoxContainer/MaxDepth_title" };			//
	const NodePath MaxDepth_val{ "BottomScreenPanel/HBoxContainer/MaxDepth_val" };
	const NodePath TimePass_title{ "BottomScreenPanel/HBoxContainer/TimePass_title" };
	const NodePath TimePass_val{ "BottomScreenPanel/HBoxContainer/TimePass_val" };
};

//									Klasa konsoliduj�ca warstw� taktyczn� (25.01.2025)
/*=============================================================================================================================
	- klasa GODOT
(updated 31.05.2025)
	- dodany wska�nik na OceanEnvironment i getter
(updated 10.02.2025)
	- dodana funkcja ConvertPosToDepth()
=============================================================================================================================*/
class TG_TacticalTier : public Node3D
{
    GDCLASS(TG_TacticalTier, Node3D)

protected:
    // Wi�zanie metod
    static void _bind_methods();

public:
    // Konstruktor i destruktor
    TG_TacticalTier() {};
    ~TG_TacticalTier() {};
    // Metody Godota
    void _ready() override;
    void _process(double delta) override;
	void _on_SurfaceButton_pressed(Variant pSender);
	void _on_PeriscButton_pressed(Variant pSender);
	void _on_CrashDive_pressed(Variant pSender);
	void _on_EMBlowing_pressed(Variant pSender);
	void _on_DepthPlusMinus_pressed(Variant pSender);
	// Metody publiczne
	inline float ConvertKnotsToMPS(float iKnots)		const { return iKnots * 0.514444f; }    // Konwersja w�z��w na metry na sekund�
	inline float ConvertMetersToKnots(float iMeters)	const { return iMeters / 0.514444f; }    // Konwersja metr�w na w�z�y
	inline float ConvertAngleTo360(float iAngle)		const { iAngle = 360.0f - iAngle; return iAngle > 360.0f ? iAngle-360.0f : iAngle; }    // Konwersja k�ta na zakres 0-360
	Node* GetOceanEnv()									const { return pOceanEnvironment; }	// Pobranie wska�nika na OceanEnvironment
	// Repos classes
	T_PathsToRepo PathsToRepo{};
private:
	// Pola
	TG_HUD* pHUD{ nullptr };					// Wska�nik na HUD
	TG_PlayerUboot* pPlayerUboot{ nullptr };	// Wska�nik na U-Boota Gracza
	Node* pOceanEnvironment{};					// Wska�nik na OceanEnvironment
};




#endif

