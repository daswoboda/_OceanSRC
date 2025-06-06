#pragma once
#ifndef TG_CONVOY_H
#define TG_CONVOY_H


#include <node3d.hpp>			// for Node3D
#include <csg_box3d.hpp>		// for CSGBox
#include <physics_direct_body_state3d.hpp>		// for PhysicsDirectBodyState3D


#include "TG_HUD.h"				// for TG_HUD
#include "TG_TacticalTier.h"	// for TG_TacticalTier
#include "TG_Escorts.h"			// for TG_Escorts
#include "TG_MerchantShip.h"

class TG_TacticalTier;
class TG_Escorts;				// for TG_Escorts
class TG_HUD;
class TG_MerchantShip;

using namespace godot;
using namespace std;

//									Klasa odpowiedzialna za konw�j (23.03.2025)
/*=============================================================================================================================
- klasa Godota obs�uguj�ca konw�j statk�w

(updated 17.05.2025)
	- dodano wska�nik na TG_Escorts
(updated 10.05.2025)
	- dodana funkcja CalculateCellCenter()
=============================================================================================================================*/
	class TG_Convoy : public Node3D {
        GDCLASS(TG_Convoy, Node3D)

    protected:
        static void _bind_methods();

    public:
        TG_Convoy();
        ~TG_Convoy();
    // Metody Godota
        //void _process(double delta) override;
        void _ready() override;
		void _physics_process(double delta) override;
		
		String get_ConvoyName()											const { return prop_ConvoyName; }
		void set_ConvoyName(const String& i_convoyName)					{ prop_ConvoyName = i_convoyName; }
		short get_InitNoMerchants()										const { return prop_InitNoMerchants; }
		void set_InitNoMerchants(const short i_initNoMerchants)			{ prop_InitNoMerchants = i_initNoMerchants; FormConvoy(); }
		short get_InitNoEscorts()										const { return prop_InitNoEscorts; }
		void set_InitNoEscorts(const short i_initNoEscorts)				{ prop_InitNoEscorts = i_initNoEscorts; }
		float get_Course()												const { return prop_Course; }
		void set_Course(const float i_course);
		float get_Speed()												const { return prop_Speed; }
		void set_Speed(const float i_speed)								{ prop_Speed = i_speed; }
		Vector2i get_FormationSpacing()									const { return prop_formationSpacing; }
		void set_FormationSpacing(const Vector2i& i_formationSpacing)	{ prop_formationSpacing = i_formationSpacing; }
		float GetCurrentCourseDegrees() const;
		float GetTargetCourseDegrees() const;
		bool IsConvoyTurning() const;
		void SetCourseImmediate(float new_course_degrees);
		
		String GetTurnDirection() const;								// Funkcja debugowania
		void RotateConvoyToCourse(float new_course_degrees);
		void UpdateConvoyRotation(double delta);

    private:
	// Sta�a konwersji w�z��w na metry na sekund�
		static constexpr double KNOTS_TO_MPS = 0.514444;
	// Properties
		String prop_ConvoyName{ "AH12" };								// nazwa konwoju
		short prop_InitNoMerchants{ 1 };								// pocz�tkowa liczba statk�w handlowych
		short prop_InitNoEscorts{ 1 };									// pocz�tkowa liczba eskortowc�w
		float prop_Course{ 0.0f };										// kurs konwoju
		float prop_Speed{ 0.0f };										// pr�dko�� konwoju
		Vector2i prop_formationSpacing{ Vector2i(500, 450) };			// odleg�o�ci pomi�dzy kolumnami i wierszami
		
     // Pola
		Vector2i columnsAndRows{ Vector2i(1, 1) };						// ilo�� kolumn i wierszy konwoju
		Vector2i formationSize{ Vector2i(1, 1) };						// rozmiar szyku konwoju
		Vector3 courseDirection{ Vector3(0, 0, 1) };					// Znormalizowany wektor kierunku
		double turnSpeedRadPerSec{ Math_PI / 4.0 };						// Pr�dko�� obrotu w radianach na sekund�
		float target_course{ 0.0f };									// Docelowy kurs w radianach
		float current_course{ 0.0f };									// Aktualny kurs w radianach
		float course_turn_speed{ 1.0f };								// Pr�dko�� obrotu w rad/s
		bool is_turning{ false };										// Czy konw�j si� obraca
		TG_TacticalTier* pTacticalTier{};								// wska�nik na warstw� taktyczn�
		TG_Escorts* pEscorts{};											// wska�nik na eskortowce
		TG_HUD* pHUD{};													// wska�nik na HUD
		CSGBox3D* pConvoyBox{};											// Node pola konwoju
		Rectangle convoyBoxRectangle{};									// prostok�t konwoju
		vector<Node3D*> vConvoyMerchants{};								// tablica wska�nik�w na handlowce
	// Metody
		void FormConvoy();												// sformowanie konwoju
		void AddMerchantToConvoy(short iColumn, short iRow, short iID);	// dodawanie handlowca do konwoju
		Vector2 CalculateCellCenter(int Column, int Rows, float spacingX, float spacingZ, int cIndex, int rIndex);	// obliczanie �rodka kom�rki
		void MoveConvoy(const double iDelta);							// poruszanie si� konwoju
		inline float deg_to_rad(float degrees)							const { return degrees * (Math_PI / 180.0f); }
		inline float rad_to_deg(float radians)							const { return radians * (180.0f / Math_PI); }
    };



#endif

