#pragma once
#ifndef TG_ESCORTS_H
#define TG_ESCORTS_H

#include <node3D.hpp>
#include <vector>

#include "TG_TacticalTier.h"
#include "TG_Convoy.h"
#include "ShipVessel.h"

class TG_TacticalTier;
class TG_Convoy;
class EscortShip;
class DetectionSystem;
enum class DetectionMode;


using namespace godot;
using namespace std;

inline float deg_to_rad(float degrees)							{ return degrees * (Math_PI / 180.0f); }
inline float rad_to_deg(float radians)							{ return radians * (180.0f / Math_PI); }


//										System wykrywania dla eskorty
/*=============================================================================================================================
- Zarz�dza wykrywaniem U-boot�w przez wszystkie eskortowce
- Koordynuje informacje mi�dzy jednostkami
=============================================================================================================================*/
class DetectionSystem
{
public:
	struct DetectionReport
	{
		Vector3 position;
		float confidence;      // 0-1
		double timestamp;
		int reportingShipID;
		DetectionMode detectionMethod;
	};

private:
	std::vector<DetectionReport> activeReports;
	const double REPORT_TIMEOUT = 30.0; // Raporty wygasaj� po 30 sekundach

public:
	void AddReport(const DetectionReport& report);
	void UpdateReports(double currentTime);
	std::vector<DetectionReport> GetReportsInArea(Vector3 center, float radius);
	bool IsPositionAlreadyReported(Vector3 position, float threshold = 100.0f);
};

//										Klasa obs�uguj�ca Eskort� Konwoju (15.05.2025)
/*=============================================================================================================================
- klasa Godota obs�uguj�ca ca�� Eskort� Konwoju
(updated 17.05.2025)
	- dodana metoda ExternalizeEscorts() do wizualizacji okr�t�w eskorty w oknie Godota
	- dodana tablica wska�nik�w na w�z�y Eskortowc�w (vEscortNodes)
	- dodana metoda ClearVecPlacedEscorts() do czyszczenia tablicy vPlacedEscorts
	- dodana metoda ClearVecEscortNodes() do czyszczenia tablicy vEscortNodes
=============================================================================================================================*/
class TG_Escorts : public Node3D
{
	GDCLASS(TG_Escorts, Node3D)

protected:
	// Wi�zanie metod
	static void _bind_methods();

public:
	// Konstruktor i destruktor
	TG_Escorts();
	~TG_Escorts();

	// Metody Godota
	void _ready() override;
	void _process(double delta) override;
	
	// Settery i gettery
	//void SetPlacedEscorts(const vector<TG_EscortShip>& placedEscorts) { vPlacedEscorts = placedEscorts; }	// Setter for vPlacedEscorts
	void SetConvoy(TG_Convoy* convoy)								{ pConvoy = convoy; }
	TG_Convoy* GetConvoy()											const { return pConvoy; }
	const vector<EscortShip*>& VecPlacedEscorts()					const { return vPlacedEscorts; }	// Getter for vPlacedEscorts
	const vector<Node3D*>& VecEscortNodes()							const { return vEscortNodes; }		// Getter for vEscortNodes
	void ClearVecEscortNodes()										{ vEscortNodes.clear(); }			// Czyszczenie tablicy vEscortNodes
	void ClearVecPlacedEscorts()									{ vPlacedEscorts.clear(); }			// Czyszczenie tablicy vPlacedEscorts

	// Metody
	void PlaceEscorts(const Rectangle& convoyRect, int numEscorts, double escortOffsetDistance, double basePatrolRadius);	// Metoda do umieszczania okr�t�w eskorty w szyku konwoju
	Vector2 CalculateOffsetPoint(Vector2 p, Vector2 lineStart, Vector2 lineEnd, float offsetDistance, Vector2 convoyCenter);			// Metoda do obliczania punktu offsetowego wzd�u� linii
	void ExternalizeEscorts();													// Wizualizacja okr�t�w eskorty w oknie Godota
	void AddEscort(unsigned short iID, Vector3 iPos, PatrolZone iZone);			// Dodanie okr�tu eskorty do tablicy vPlacedEscorts
	void DeleteEscortNodes();													// Usuwanie w�z��w eskorty

	void UpdateDetectionSystem(double delta);
	void ProcessDetectionReport(const DetectionSystem::DetectionReport& report);
	DetectionSystem* GetDetectionSystem()								{ return &detectionSystem; }


private:
	// Pola
	TG_TacticalTier* pTacticalTier{ nullptr };		// Wska�nik na w�ze� T_TacticalTier
	TG_Convoy* pConvoy{ nullptr };                  // Wska�nik do konwoju, kt�ry eskortujemy
	vector<EscortShip*> vPlacedEscorts{};			// tablica pozycji okr�t�w eskorty do formoania szyku Eskorty
	vector<Node3D*> vEscortNodes{};					// tablica wska�nik�w na w�z�y Eskortowc�w

	DetectionSystem detectionSystem;
};



#endif