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

//										Struktura Prostok¹ta (15.05.2025)
/*=============================================================================================================================
- powered by Gemini
(updated 18.05.2025)
	- poprawki obliczeñ do wspó³rzêdnych osi Godota
=============================================================================================================================*/
/// <summary>
/// Struktura reprezentuj¹ca prostok¹t oraz metody do obliczania jego w³aœciwoœci geometrycznych.
/// </summary>
struct Rectangle
{
	Vector2 topLeft;
	real_t width;
	real_t height;

	Vector2 GetCenter()							const { return Vector2{ static_cast<real_t>(topLeft.x - width / 2.0), static_cast<real_t>(topLeft.y - height / 2.0) }; }
	float GetPerimeter()						const { return 2.0 * (width + height); }
	std::vector<Vector2> GetCorners()			const { return {topLeft,{topLeft.x - width, topLeft.y},{topLeft.x - width, topLeft.y - height},{topLeft.x, topLeft.y - height}}; }	// lewy górny, prawy górny, prawy dolny, lewy dolny
	float GetArea()								const { return width * height; }
	std::vector<Vector2> GetSideMidpoints()		const;		// Obliczenie œrodków 4 boków Prostok¹ta
};

//										Struktura Okrêgu (15.05.2025)
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
	// Mo¿na dodaæ pole typu enum, aby ³atwiej identyfikowaæ aktywny typ,
	// chocia¿ std::visit lub std::holds_alternative s¹ preferowane.
};

//										Struktura Okrêtu Eskortowego (15.05.2025)
/*=============================================================================================================================
- powered by Gemini
=============================================================================================================================*/
struct stEscortShip
{
	Vector2 position;
	PatrolZone patrolZone;		// Lub bezpoœrednio Circle/Rectangle, jeœli uproszczono
};


//								Klasa przechowuj¹ca œcie¿ki do wêz³ów (25.01.2025)
/*=============================================================================================================================
	- klasa noGODOT
=============================================================================================================================*/
class T_PathsToRepo
{
public:
    const NodePath Speed_val{ "BottomScreenPanel/HBoxContainer/Speed_val" };			                // Œcie¿ka do etykiety prêdkoœci
	const NodePath Belka_label{ "U-Boot Control Panel/VBox/Belka/Belka_label" };	                    // Œcie¿ka do labela Belka_label
	const NodePath Telegraph_label{ "U-Boot Control Panel/VBox/Telegraph_tab/Telegraph_label" };	    // Œcie¿ka do labela Telegraph_label
	const NodePath zF_button{ "U-Boot Control Panel/VBox/EngineButtons_container/zF_button" };          // Œcie¿ka do przycisku zF_button
	const NodePath zST_button{ "U-Boot Control Panel/VBox/EngineButtons_container/zST_button" };        // Œcie¿ka do przycisku zST_button
	const NodePath zS_button{ "U-Boot Control Panel/VBox/EngineButtons_container/zS_button" };          // Œcie¿ka do przycisku zS_button
	const NodePath zD_button{ "U-Boot Control Panel/VBox/EngineButtons_container/zD_button" };          // Œcie¿ka do przycisku zD_button
	const NodePath STOP_button{ "U-Boot Control Panel/VBox/EngineButtons_container/STOP_button" };      // Œcie¿ka do przycisku STOP_button
	const NodePath vD_button{ "U-Boot Control Panel/VBox/EngineButtons_container/vD_button" };          // Œcie¿ka do przycisku vD_button
	const NodePath vS_button{ "U-Boot Control Panel/VBox/EngineButtons_container/vS_button" };          // Œcie¿ka do przycisku vS_button
	const NodePath vST_button{ "U-Boot Control Panel/VBox/EngineButtons_container/vST_button" };        // Œcie¿ka do przycisku vST_button
	const NodePath vF_button{ "U-Boot Control Panel/VBox/EngineButtons_container/vF_button" };          // Œcie¿ka do przycisku vF_button
	const NodePath vFL_button{ "U-Boot Control Panel/VBox/EngineButtons_container/vFL_button" };		// Œcie¿ka do przycisku vFL_button
	const NodePath Rudder_label{ "U-Boot Control Panel/VBox/Rudder_panel/Rudder_vbox/RudderVal_label" };// Œcie¿ka do labela Rudder_label
	const NodePath Rudder_slider{ "U-Boot Control Panel/VBox/Rudder_panel/Rudder_vbox/Rudder_slider" };	// Œcie¿ka do suwaka Rudder_slider
	const NodePath Dive_hslider{ "U-Boot Control Panel/VBox/Dive_panel/Dive_hslider" };					// Œcie¿ka do suwaka Dive_hslider
	const NodePath DiveSpeed_label{ "U-Boot Control Panel/VBox/Dive_panel/Dive_hslider/DiveSpeed_label" };// Œcie¿ka do labela Dive_label
	const NodePath Dive_vslider{ "U-Boot Control Panel/VBox/Dive_panel/Dive_vslider" };					// Œcie¿ka do Dive_vslider
	const NodePath DIVE_button{ "U-Boot Control Panel/VBox/Dive_panel/DIVE_button" };					// Œcie¿ka do DIVE_button
	const NodePath Surface_button{ "U-Boot Control Panel/VBox/Dive_panel/Dive_hbox/Surfce_button" };				// Œcie¿ka do Surface_button
	const NodePath Perisc_button{ "U-Boot Control Panel/VBox/Dive_panel/Dive_hbox/Perisc_button" };				// Œcie¿ka do Perisc_button
	const NodePath CrashDive_button{ "U-Boot Control Panel/VBox/Dive_panel/Dive_hbox/Crash_button" };				// Œcie¿ka do CrashDive_button
	const NodePath EMBlowing_button{ "U-Boot Control Panel/VBox/Dive_panel/Dive_hbox/EmBlow_button" };			// Œcie¿ka do EMBlowing_button
	const NodePath PlusTen_button{ "U-Boot Control Panel/VBox/Dive_panel/Dive_grid/Plus10_button" };			// Œcie¿ka do PlusTen_button
	const NodePath PlusTwenty_button{ "U-Boot Control Panel/VBox/Dive_panel/Dive_grid/Plus20_button" };		// Œcie¿ka do PlusTwenty_button
	const NodePath PlusThirty_button{ "U-Boot Control Panel/VBox/Dive_panel/Dive_grid/Plus30_button" };		// Œcie¿ka do PlusThirty_button
	const NodePath PlusFifty_button{ "U-Boot Control Panel/VBox/Dive_panel/Dive_grid/Plus50_button" };		// Œcie¿ka do PlusFifty_button
	const NodePath MinusTen_button{ "U-Boot Control Panel/VBox/Dive_panel/Dive_grid/Minus10_button" };		// Œcie¿ka do MinusTen_button
	const NodePath MinusTwenty_button{ "U-Boot Control Panel/VBox/Dive_panel/Dive_grid/Minus20_button" };	// Œcie¿ka do MinusTwenty_button
	const NodePath MinusThirty_button{ "U-Boot Control Panel/VBox/Dive_panel/Dive_grid/Minus30_button" };	// Œcie¿ka do MinusThirty_button
	const NodePath MinusFifty_button{ "U-Boot Control Panel/VBox/Dive_panel/Dive_grid/Minus50_button" };	// Œcie¿ka do MinusFifty_button
	const NodePath OceanEnv{ "OceanEnvironment" };
	// Sceny
	const String LibertyShipScene{ "res://_Assets/Ships/Liberty_ship_2.tscn" };								// Œcie¿ka do sceny statku Liberty
	// Tymczasowe
	const NodePath MaxDepth_title{ "BottomScreenPanel/HBoxContainer/MaxDepth_title" };			//
	const NodePath MaxDepth_val{ "BottomScreenPanel/HBoxContainer/MaxDepth_val" };
	const NodePath TimePass_title{ "BottomScreenPanel/HBoxContainer/TimePass_title" };
	const NodePath TimePass_val{ "BottomScreenPanel/HBoxContainer/TimePass_val" };
};

//									Klasa konsoliduj¹ca warstwê taktyczn¹ (25.01.2025)
/*=============================================================================================================================
	- klasa GODOT
(updated 31.05.2025)
	- dodany wskaŸnik na OceanEnvironment i getter
(updated 10.02.2025)
	- dodana funkcja ConvertPosToDepth()
=============================================================================================================================*/
class TG_TacticalTier : public Node3D
{
    GDCLASS(TG_TacticalTier, Node3D)

protected:
    // Wi¹zanie metod
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
	inline float ConvertKnotsToMPS(float iKnots)		const { return iKnots * 0.514444f; }    // Konwersja wêz³ów na metry na sekundê
	inline float ConvertMetersToKnots(float iMeters)	const { return iMeters / 0.514444f; }    // Konwersja metrów na wêz³y
	inline float ConvertAngleTo360(float iAngle)		const { iAngle = 360.0f - iAngle; return iAngle > 360.0f ? iAngle-360.0f : iAngle; }    // Konwersja k¹ta na zakres 0-360
	Node* GetOceanEnv()									const { return pOceanEnvironment; }	// Pobranie wskaŸnika na OceanEnvironment
	// Repos classes
	T_PathsToRepo PathsToRepo{};
private:
	// Pola
	TG_HUD* pHUD{ nullptr };					// WskaŸnik na HUD
	TG_PlayerUboot* pPlayerUboot{ nullptr };	// WskaŸnik na U-Boota Gracza
	Node* pOceanEnvironment{};					// WskaŸnik na OceanEnvironment
};




#endif

