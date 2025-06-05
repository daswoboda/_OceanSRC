#pragma once
#ifndef SHIPVESSEL_H
#define SHIPVESSEL_H

#include <node_path.hpp>
#include <marker3D.hpp>
#include <mesh_instance3d.hpp>			// for MeshInstance3D

#include "BuoyancyBody.h"       // Dziedziczymy po wczeœniej zdefiniowanym BuoyancyBody3D
#include "TG_TacticalTier.h"    // WskaŸnik do TG_TacticalTier
#include "TG_Convoy.h"
#include "TG_Escorts.h"         // WskaŸnik do TG_Escorts


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
    VISUAL,                     // Obserwacja wzrokowa - bez ograniczeñ prêdkoœci
    LISTENING,                  // Nas³uch - max 3-6 wêz³ów
    SONAR                       // Sonar - max 6-15 wêz³ów
};

enum class EscortState
{
    PATROLLING,                 // Normalne patrolowanie
    RETURNING_TO_ZONE,          // Powrót do strefy patrolowej
    TRACKING_CONTACT,           // Œledzenie wykrytego kontaktu
    INVESTIGATING               // Badanie podejrzanego kontaktu
};

// Struktura dla kontaktu sonarowego/nas³uchowego
struct SonarContact
{
    Vector3 position;
    float bearing;              // Namiar w stopniach
    float distance;             // Odleg³oœæ
    float confidence;           // Pewnoœæ kontaktu (0-1)
    double lastUpdate;          // Czas ostatniej aktualizacji
    Vector3 estimatedVelocity;
};


//										Klasa obs³uguj¹ca model Statku (28.05.2025)
/*=============================================================================================================================
- jest to klasa bazowa dla wszystkich statków w grze
- zosta³a stworzona na podstawie klasy BuoyancyBody3D,
- zosta³a przeportowana z GDScript do C++
- zawiera metody do zarz¹dzania ruchem i fizyk¹ statków
- zawiera równie¿ metody do obs³ugi wejœcia oraz dzia³ania silnika
=============================================================================================================================*/
class ShipVessel : public BuoyancyBody
{
    GDCLASS(ShipVessel, BuoyancyBody);

private:
    // Pola
    unsigned short ID{ 0 };					// ID okrêtu eskortowego
    String name{};							// Nazwa okrêtu eskortowego
    
    double thrust_power_main{};
    double max_rudder_force{};

    NodePath propeller_marker_path{};
    NodePath rudder_marker_path{}; // Oddzielna œcie¿ka dla steru

    Marker3D* propeller_marker_node{};
    Marker3D* rudder_marker_node{};

    // StringNames dla akcji InputMap dla wydajnoœci
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
    void _process(double delta) override; // Zgodnie z GDScript, ale rozwa¿ _physics_process

    // Settery i Gettery
    void set_thrust_power_main(double power);
    double get_thrust_power_main() const;

    void set_max_rudder_force(double force);
    double get_max_rudder_force() const;

    void set_propeller_marker_path(const NodePath& path);
    NodePath get_propeller_marker_path() const;

    void set_rudder_marker_path(const NodePath& path); // Setter dla œcie¿ki steru
    NodePath get_rudder_marker_path() const;       // Getter dla œcie¿ki steru
};

//										Klasa obs³uguj¹ca okrêt Eskorty (28.05.2025)
/*=============================================================================================================================
- jest to klasa pochodna dla statku eskorty
- zawiera specyficzne metody i funkcjonalnoœci dla statków eskorty
- zastêpuje wczeœniej zdefiniowan¹ klasê TG_EscortShip, dziedzicz¹c¹ po Node3D
=============================================================================================================================*/
class EscortShip : public ShipVessel
{
    GDCLASS(EscortShip, ShipVessel);

private:
    // Pola
    short ID{ 0 };							        // ID okrêtu eskortowego
    String name{};							        // Nazwa okrêtu eskortowego
    Node3D* pShipNode{ nullptr };			        // WskaŸnik do modelu 3D okrêtu eskortowego
    Vector3 originPosition{};				        // Pozycja relatywna w stosunku do konwoju
    PatrolZone patrolZone{};				        // Obszar patrolowy
	MeshInstance3D* pPatrolZoneNode{ nullptr };     // WskaŸnik do wêz³a wizualizacji PatrolZone
    TG_Escorts* pEscortsNode{ nullptr };	        // WskaŸnik do wêz³a TG_Escorts
    TG_Convoy* pConvoy{ nullptr };                  // WskaŸnik do konwoju, który eskortujemy

    // pola dla patrolowania
    Vector3 currentTargetPosition{};                // Aktualny cel ruchu
    EscortState currentState{ EscortState::PATROLLING };
    DetectionMode activeDetection{ DetectionMode::VISUAL };

    // Parametry wykrywania
    float visualRange{ 5000.0f };                   // Zasiêg wzrokowy w metrach (zale¿ny od pogody)
    float sonarRange{ 3000.0f };                    // Zasiêg sonaru w metrach
    float listeningRange{ 4000.0f };                // Zasiêg nas³uchu w metrach
    float sonarBeamWidth{ 2.0f };                   // Szerokoœæ wi¹zki sonaru w stopniach
    float sonarMaxAngle{ 45.0f };                   // Max k¹t obrotu sonaru na burtê
    float currentSonarBearing{ 0.0f };              // Aktualny namiar sonaru
    bool sonarSweepingRight{ true };                // Kierunek skanowania sonaru

    // Kontakty i wykrywanie
    std::vector<SonarContact> detectedContacts{};
    SonarContact* trackedContact{ nullptr };
    double lastDetectionUpdate{ 0.0 };
    double detectionUpdateInterval{ 1.0 };          // Co ile sekund aktualizowaæ wykrywanie

    // Parametry ruchu
    float patrolSpeed{ 12.0f };                     // Prêdkoœæ patrolowania w wêz³ach
    float maxSpeedKnots{ 25.0f };                   // Max prêdkoœæ w wêz³ach
    float returnSpeed{ 20.0f };                     // Prêdkoœæ powrotu do strefy

    // Ograniczenia prêdkoœci dla trybów wykrywania
    const float VISUAL_MAX_SPEED{ 999.0f };         // Brak ograniczeñ
    const float LISTENING_MAX_SPEED{ 6.0f };        // 3-6 wêz³ów
    const float SONAR_MAX_SPEED{ 15.0f };           // 6-15 wêz³ów
    
protected:
    static void _bind_methods();

public:
    EscortShip();
    ~EscortShip();

    void _ready() override;
	void _physics_process(double delta) override;   // U¿ywamy _physics_process dla fizyki i ruchu
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

//										Klasa obs³uguj¹ca statek Handlowy (28.05.2025)
/*=============================================================================================================================
- jest to klasa pochodna dla statku handlowego
- zawiera specyficzne metody i funkcjonalnoœci dla statków handlowych
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
    void _process(double delta) override; // Zgodnie z GDScript, ale rozwa¿ _physics_process

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
    short ID{ 0 };							// ID okrêtu eskortowego
    String name;							// Nazwa okrêtu eskortowego
    Node3D* pShipNode{ nullptr };			// WskaŸnik do modelu 3D okrêtu eskortowego
    Node3D* pConvoyBox{};                   // WskaŸnik do wêz³a ConvoyBox
    Vector3 originPosition{};				// Pozycja relatywna w stosunku do konwoju
    Vector3 position{};						// Pozycja rzeczywista okrêtu eskortowego
    TG_Escorts* pEscortsNode{ nullptr };	// WskaŸnik do wêz³a TG_Escorts
};

#endif