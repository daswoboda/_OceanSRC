#pragma once
#ifndef T_PLAYERUBOOT_H
#define T_PLAYERUBOOT_H

#include <node3D.hpp>
#include <rigid_body3d.hpp>									// for RigidBody
#include <marker3d.hpp>										// for Marker3D
#include <vector>

#include "T_SpeedTelegraph.h"
#include "TG_HUD.h"
#include "TG_TacticalTier.h"

class TG_TacticalTier;
class TG_HUD;
class T_SpeedTelegraph;

using namespace godot;
using namespace std;

//										Klasa obs³uguj¹ca U-Boota Gracza (11.01.2025)
/*=============================================================================================================================
(updated 16.03.2025)
	- dodane settery SetIsDivingFromSurface, SetIsDivingPossible, SetIsRequiredDepthStabilization
	- dodany getter SpeedTelegraph, GetDesiredDepth
(updated 26.01.2025)
	- dodane pola wskaŸników pTacticalTier i pHUD
=============================================================================================================================*/
class TG_PlayerUboot : public Node3D
{
    GDCLASS(TG_PlayerUboot, Node3D)

protected:
    // Wi¹zanie metod
    static void _bind_methods();

public:
    // Konstruktor i destruktor
    TG_PlayerUboot();
    ~TG_PlayerUboot();
    // Metody Godota
    void _ready() override;
    void _process(double delta) override;
	//pvoid _on_tg_hud_speed_telegraph_button_pressed_signal(Variant pButton);
	// Settery i gettery
	void SetAcceleration(float iAcceleration)		{ prop_Acceleration = iAcceleration; }
	float GetAcceleration()							const { return prop_Acceleration; }
	void SetDeacceleration(float iDeacceleration)	{ prop_Deacceleration = iDeacceleration; }
	float GetDeacceleration()						const { return prop_Deacceleration; }
	void SetSurfaceMaxVelocity(float iMaxVelocity)	{ prop_SurfaceMaxVelocity = iMaxVelocity; }
	float GetSurfaceMaxVelocity()					const { return prop_SurfaceMaxVelocity; }
	void SetSubmergedMaxVelocity(float iMaxVelocity){ prop_SubmergedMaxVelocity = iMaxVelocity; }
	float GetSubmergedMaxVelocity()					const { return prop_SubmergedMaxVelocity; }
	bool IsSurface()								const { return isSurface; }
	float GetActualDepth()							const { return actualDepth; }
	float GetDesiredDepth()							const { return desiredDepth; }
	T_SpeedTelegraph* SpeedTelegraph()				const { return pSpeedTelegraph; }
	void SetRudderSetting(float iRudder)			{ rudderSetting = iRudder; }
	void SetRudderSettingZero()						{ rudderSetting = 0.0f; }
	void IncRudderSetting()							{ rudderSetting += 5.f; if (rudderSetting > 25.f) rudderSetting = 25.f; }
	void DecRudderSetting()							{ rudderSetting -= 5.f; if (rudderSetting < -25.f) rudderSetting = -25.f; }
	void SetDesiredDepth(float iDepth)				{ desiredDepth = iDepth; }
	void SetDesiredDepthOff()						{ desiredDepth = -1.0f; }
	void SetIsDivingFromSurface(bool iIsDiving)		{ isDivingFromSurface = iIsDiving; }
	void SetIsDivingPossible(bool iIsDiving)		{ isDivingPossible = iIsDiving; }
	void SetIsRequiredDepthStabilization(bool iIsRequired) { isRequiredDepthStabilization = iIsRequired; }
	// Metody
	inline float ConvertKnotsToMPS(float iKnots)	const { return iKnots * 0.514444f; }    // Konwersja wêz³ów na metry na sekundê
private:
	// Parametry
	float prop_Acceleration{ 0.85f };				// Acceleration in knots
	float prop_Deacceleration{ 0.1f };				// Deacceleration in knots
	float prop_SurfaceMaxVelocity{ 17.7f };			// Max surface speed in knots
	float prop_SubmergedMaxVelocity{ 7.6f };		// Max submerged speed in knots
	float prop_TurnForce{ 0.1f };					// Turn force
	float prop_DiveForce{ 1.2f };					// Dive force
	
	// Pola
	TG_TacticalTier* pTacticalTier{ nullptr };		// WskaŸnik na wêze³ T_TacticalTier
	TG_HUD* pHUD{ nullptr };						// WskaŸnik na HUD
	T_SpeedTelegraph* pSpeedTelegraph{ nullptr };   // Telegraf prêdkoœci, który zostanie utworzony w konstruktorze
	vector<float> vThrottle{ -82.0f, -52.0f, -43.0f, -37.0f, 0.0f, 35.0f, 56.0f, 68.0f, 91.0f, 99.0f, -83.0f, -53.0f, -37.0f, -18.0f, 0.0f, 19.0f, 37.0f, 53.0f, 81.f, 96.0f};	// Throttle settings
	RigidBody3D* pSub{ nullptr };					// WskaŸnik na child_node - Sub
	Marker3D* pRudder{ nullptr };					// WskaŸnik na marker steru
	bool isSurface{ true };							// Czy okrêt jest na powierzchni
	bool isDivingFromSurface{ false };				// Czy okrêt rozpocz¹³ zanurzanie z powierzchni
	bool isDivingPossible{ false };					// Czy okrêt mo¿e zanurzaæ siê
	float timeToDiveDelay{ 2.0f };					// OpóŸnienie czasu do zanurzenia
	float timeToDive{ 0.0f };						// Czas do zanurzenia
	float actualSpeed{ 0.0f };						// Aktualna prêdkoœæ wêz³owa (knots)
	float desiredSpeed{ 0.0f };						// Docelowa prêdkoœæ wêz³owa (knots)
	float actualDepth{ 0.0f };						// Aktualna g³êbokoœæ (m)
	float desiredDepth{ -1.0f };					// Docelowa g³êbokoœæ (-1 brak docelowej g³êbokoœci)
	float rudder{ 0.0f };							// ster kierunku
	float rudderSetting{ 0.0f };					// nastawa steru kierunku (>30 brak nastawy)
	float eulerAngle{ 0.0f };						// kierunek okrêtu
	int processOmission{ 0 };						// pominiêcie procesu
	const float massForOneMeter{ 12120.0f };		// masa wymagana do zanurzenia o 1 metr
	bool isRequiredDepthStabilization{ false };		// Czy wymagana jest stabilizacja g³êbokoœci
	//sTypeOfUboot TypeOfUboot;
	//cPropulsion Propulsion;
	float timer{ 0.0f };							// timer
	float testDepth{ 0.0f };						// test g³êbokoœci

	// Metody prywatne
	void ProcessSpeedTelegraph(const double iDelta);
	void Move(const double iDelta);
	void Turn(const double iDelta);
	void Dive(const double iDelta);
	void DiveFromSurface(const double iDelta);		// Zanurzanie z powierzchni
	void StabilizeDepth(const float iDepth);		// Stabilizacja g³êbokoœci poprzez zmianê masy
	void ComingToSurface(const double iDelta);		// Wyp³yniêcie na powierzchniê
	void TestDepth(const float iDepth, const double iDelta);				// Test g³êbokoœci
};


#endif