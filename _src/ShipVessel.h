#pragma once
#ifndef SHIPVESSEL_H
#define SHIPVESSEL_H

#include <node_path.hpp>
#include <marker3D.hpp>
#include <mesh_instance3d.hpp>			// for MeshInstance3D

#include "BuoyancyBody.h"       // Dziedziczymy po wcze�niej zdefiniowanym BuoyancyBody3D
#include "TG_TacticalTier.h"    // Wska�nik do TG_TacticalTier
#include "TG_Convoy.h"
#include "TG_Escorts.h"         // Wska�nik do TG_Escorts


// Forward declare
class TG_TacticalTier;
class TG_Escorts;
class TG_Convoy;
struct PatrolZone; // Struktura do przechowywania obszaru patrolowego
struct stEscortShip;




using namespace godot;
using namespace std;

enum class DetectionMode
{
    VISUAL,                     // Obserwacja wzrokowa - bez ogranicze� pr�dko�ci
    LISTENING,                  // Nas�uch - max 3-6 w�z��w
    SONAR                       // Sonar - max 6-15 w�z��w
};

enum class EscortState
{
    PATROLLING,                 // Normalne patrolowanie
    RETURNING_TO_ZONE,          // Powr�t do strefy patrolowej
    TRACKING_CONTACT,           // �ledzenie wykrytego kontaktu
    INVESTIGATING               // Badanie podejrzanego kontaktu
};

// Struktura dla kontaktu sonarowego/nas�uchowego
struct SonarContact
{
    Vector3 position;
    float bearing;              // Namiar w stopniach
    float distance;             // Odleg�o��
    float confidence;           // Pewno�� kontaktu (0-1)
    double lastUpdate;          // Czas ostatniej aktualizacji
    Vector3 estimatedVelocity;
};


//										Klasa obs�uguj�ca model Statku (28.05.2025)
/*=============================================================================================================================
- jest to klasa bazowa dla wszystkich statk�w w grze
- zosta�a stworzona na podstawie klasy BuoyancyBody3D,
- zosta�a przeportowana z GDScript do C++
- zawiera metody do zarz�dzania ruchem i fizyk� statk�w
- zawiera r�wnie� metody do obs�ugi wej�cia oraz dzia�ania silnika
=============================================================================================================================*/
class ShipVessel : public BuoyancyBody
{
    GDCLASS(ShipVessel, BuoyancyBody);

private:
    // Pola
    unsigned short ID{ 0 };					// ID okr�tu eskortowego
    String name{};							// Nazwa okr�tu eskortowego
    
    double thrust_power_main{};
    double max_rudder_force{};

    NodePath propeller_marker_path{};
    NodePath rudder_marker_path{}; // Oddzielna �cie�ka dla steru

    Marker3D* propeller_marker_node{};
    Marker3D* rudder_marker_node{};

    // StringNames dla akcji InputMap dla wydajno�ci
    StringName action_thrust_forwards{};
    StringName action_thrust_backwards{};
    StringName action_steer_left{};
    StringName action_steer_right{};

    // StringName dla metody OceanEnvironment
    //static const StringName get_wave_height_method;


protected:
    static void _bind_methods();

public:
    ShipVessel();
    ~ShipVessel();

    void _ready() override;
    void _process(double delta) override; // Zgodnie z GDScript, ale rozwa� _physics_process

    // Settery i Gettery
    void set_thrust_power_main(double power);
    double get_thrust_power_main() const;

    void set_max_rudder_force(double force);
    double get_max_rudder_force() const;

    void set_propeller_marker_path(const NodePath& path);
    NodePath get_propeller_marker_path() const;

    void set_rudder_marker_path(const NodePath& path); // Setter dla �cie�ki steru
    NodePath get_rudder_marker_path() const;       // Getter dla �cie�ki steru
};

//										Klasa obs�uguj�ca okr�t Eskorty (28.05.2025)
/*=============================================================================================================================
- jest to klasa pochodna dla statku eskorty
- zawiera specyficzne metody i funkcjonalno�ci dla statk�w eskorty
- zast�puje wcze�niej zdefiniowan� klas� TG_EscortShip, dziedzicz�c� po Node3D
=============================================================================================================================*/
class EscortShip : public ShipVessel
{
    GDCLASS(EscortShip, ShipVessel);

private:
    // Pola
    short ID{ 0 };							        // ID okr�tu eskortowego
    String name{};							        // Nazwa okr�tu eskortowego
    Node3D* pShipNode{ nullptr };			        // Wska�nik do modelu 3D okr�tu eskortowego
    Vector3 originPosition{};				        // Pozycja relatywna w stosunku do konwoju
    PatrolZone patrolZone{};				        // Obszar patrolowy
	MeshInstance3D* pPatrolZoneNode{ nullptr };     // Wska�nik do w�z�a wizualizacji PatrolZone
    TG_Escorts* pEscortsNode{ nullptr };	        // Wska�nik do w�z�a TG_Escorts
    TG_Convoy* pConvoy{ nullptr };                  // Wska�nik do konwoju, kt�ry eskortujemy

    // pola dla patrolowania
    Vector3 currentTargetPosition{};                // Aktualny cel ruchu
    EscortState currentState{ EscortState::PATROLLING };
    DetectionMode activeDetection{ DetectionMode::VISUAL };

    // Parametry wykrywania
    float visualRange{ 5000.0f };                   // Zasi�g wzrokowy w metrach (zale�ny od pogody)
    float sonarRange{ 3000.0f };                    // Zasi�g sonaru w metrach
    float listeningRange{ 4000.0f };                // Zasi�g nas�uchu w metrach
    float sonarBeamWidth{ 2.0f };                   // Szeroko�� wi�zki sonaru w stopniach
    float sonarMaxAngle{ 45.0f };                   // Max k�t obrotu sonaru na burt�
    float currentSonarBearing{ 0.0f };              // Aktualny namiar sonaru
    bool sonarSweepingRight{ true };                // Kierunek skanowania sonaru

    // Kontakty i wykrywanie
    std::vector<SonarContact> detectedContacts{};
    SonarContact* trackedContact{ nullptr };
    double lastDetectionUpdate{ 0.0 };
    double detectionUpdateInterval{ 1.0 };          // Co ile sekund aktualizowa� wykrywanie

    // Parametry ruchu
    float patrolSpeed{ 12.0f };                     // Pr�dko�� patrolowania w w�z�ach
    float maxSpeedKnots{ 25.0f };                   // Max pr�dko�� w w�z�ach
    float returnSpeed{ 20.0f };                     // Pr�dko�� powrotu do strefy

    // Ograniczenia pr�dko�ci dla tryb�w wykrywania
    const float VISUAL_MAX_SPEED{ 999.0f };         // Brak ogranicze�
    const float LISTENING_MAX_SPEED{ 6.0f };        // 3-6 w�z��w
    const float SONAR_MAX_SPEED{ 15.0f };           // 6-15 w�z��w
    
protected:
    static void _bind_methods();

public:
    EscortShip();
    ~EscortShip();

    void _ready() override;
	void _physics_process(double delta) override;   // U�ywamy _physics_process dla fizyki i ruchu
    // Settery i gettery
    void SetID(short iID)                           { ID = iID; }
    short GetID()									const { return ID; }
    void SetOriginPosition(Vector3 origin)          { originPosition = origin; }
    Vector3 GetOriginPosition()						const { return originPosition; }
    void SetName(const String& shipName)            { name = shipName; }
    String GetName()								const { return name; }
    void SetShipNode(Node3D* pNode)                 { pShipNode = pNode; }
    Node3D* GetShipNode()							const { return pShipNode; }
    void SetPatrolZone(const PatrolZone& patrol)    { patrolZone = patrol; }
    PatrolZone GetPatrolZone()						const { return patrolZone; }
    void SetConvoy(TG_Convoy* convoy)               { pConvoy = convoy; }
    TG_Convoy* GetConvoy() const                    { return pConvoy; }
    // Metody
    void ExternalizePatrolZone();					// Wizualizacja obszaru patrolowego

    // Nowe metody
    void UpdatePatrolling(double delta);
    void UpdateDetection(double delta);
    void ChooseNewPatrolTarget();
    bool IsInPatrolZone();
    void ReturnToPatrolZone();
    void UpdateMovement(double delta);
    void SetDetectionMode(DetectionMode mode);
    float GetMaxSpeedForCurrentMode();
    void MoveTowardsTarget(Vector3 target, float speedKnots, double delta);
    void UpdateSonarBearingForShipRotation(float shipRotationDelta);
    bool IsTargetInKilwater(Vector3 targetPosition);
    void ReportContact(Vector3 position, float confidence);
    
    // Metody wykrywania
    void PerformVisualScan(double delta);
    void PerformSonarScan(double delta);
    void PerformListeningScan(double delta);
    bool CanDirectSensorTowards(Vector3 direction);
    float CalculateDetectionProbability(float distance, DetectionMode mode);
    void TrackContact(SonarContact* contact, double delta);
    
    // Pomocnicze
    Vector3 GetRandomPointInPatrolZone();
    float GetDistanceToConvoy();
    bool IsPointInPatrolZone(Vector3 point);
};

//										Klasa obs�uguj�ca statek Handlowy (28.05.2025)
/*=============================================================================================================================
- jest to klasa pochodna dla statku handlowego
- zawiera specyficzne metody i funkcjonalno�ci dla statk�w handlowych
=============================================================================================================================*/
class MerchantShip : public ShipVessel
{
    GDCLASS(MerchantShip, ShipVessel);

protected:
    static void _bind_methods();

public:
    MerchantShip() {};
    ~MerchantShip() {};

    void _ready() override;
    void _process(double delta) override; // Zgodnie z GDScript, ale rozwa� _physics_process

    // Settery i gettery
    void SetID(short iID) { ID = iID; }
    short GetID()									const { return ID; }
    void SetOriginPosition(Vector3 origin) { originPosition = origin; }
    Vector3 GetOriginPosition()						const { return originPosition; }
    void SetPosition(Vector3 pos) { position = pos; set_position(pos); }
    Vector3 GetPosition()							const { return position; }
    void SetName(const String& shipName) { name = shipName; }
    String GetName()								const { return name; }
    void SetShipNode(Node3D* pNode) { pShipNode = pNode; }
    Node3D* GetShipNode()							const { return pShipNode; }

private:
    // Pola
    short ID{ 0 };							// ID okr�tu eskortowego
    String name;							// Nazwa okr�tu eskortowego
    Node3D* pShipNode{ nullptr };			// Wska�nik do modelu 3D okr�tu eskortowego
    Node3D* pConvoyBox{};                   // Wska�nik do w�z�a ConvoyBox
    Vector3 originPosition{};				// Pozycja relatywna w stosunku do konwoju
    Vector3 position{};						// Pozycja rzeczywista okr�tu eskortowego
    TG_Escorts* pEscortsNode{ nullptr };	// Wska�nik do w�z�a TG_Escorts
};

#endif