#include "ShipVessel.h"

#include <marker3d.hpp>
#include <input.hpp>
#include <engine.hpp>
#include <class_db.hpp>
#include <utility_functions.hpp>        // Dla push_warning
#include <node.hpp>                     // Dla get_node_or_null
#include <cylinder_mesh.hpp>			// for CylinderMesh
#include <standard_material3d.hpp>		// for StandardMaterial3D
#include <scene_tree.hpp>				// for get_tree()
#include <time.hpp>                    // Dla get_ticks_msec

#include <cmath> // Ensure <cmath> is included for mathematical functions

using namespace godot;

//// Define the deg_to_rad function
//inline float deg_to_rad(float degrees)
//{
//    return degrees * static_cast<float>(Math_PI) / 180.0f;
//}

// Inicjalizacja statycznej sk�adowej (je�li nie zrobiono tego w BuoyancyBody3D)
//const StringName ShipVessel::get_wave_height_method = StringName("get_wave_height");


ShipVessel::ShipVessel()
{
    //thrust_power_main = 10.0;
    //max_rudder_force = 10.0;

    //propeller_marker_node = nullptr;
    //rudder_marker_node = nullptr; // Inicjalizacja wska�nika steru

    //// Inicjalizacja StringName dla akcji
    //action_thrust_forwards = StringName("ship_thrust_main_forwards");
    //action_thrust_backwards = StringName("ship_thrust_main_backwards");
    //action_steer_left = StringName("ship_steering_main_left");
    //action_steer_right = StringName("ship_steering_main_right");
}

ShipVessel::~ShipVessel() {}

void ShipVessel::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_thrust_power_main", "power"), &ShipVessel::set_thrust_power_main);
    ClassDB::bind_method(D_METHOD("get_thrust_power_main"), &ShipVessel::get_thrust_power_main);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "thrust_power_main"), "set_thrust_power_main", "get_thrust_power_main");

    ClassDB::bind_method(D_METHOD("set_max_rudder_force", "force"), &ShipVessel::set_max_rudder_force);
    ClassDB::bind_method(D_METHOD("get_max_rudder_force"), &ShipVessel::get_max_rudder_force);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_rudder_force"), "set_max_rudder_force", "get_max_rudder_force");

    ClassDB::bind_method(D_METHOD("set_propeller_marker_path", "path"), &ShipVessel::set_propeller_marker_path);
    ClassDB::bind_method(D_METHOD("get_propeller_marker_path"), &ShipVessel::get_propeller_marker_path);
    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "propeller_marker_path", PROPERTY_HINT_NODE_PATH_VALID_TYPES, "Marker3D"), "set_propeller_marker_path", "get_propeller_marker_path");

    ClassDB::bind_method(D_METHOD("set_rudder_marker_path", "path"), &ShipVessel::set_rudder_marker_path);
    ClassDB::bind_method(D_METHOD("get_rudder_marker_path"), &ShipVessel::get_rudder_marker_path);
    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "rudder_marker_path", PROPERTY_HINT_NODE_PATH_VALID_TYPES, "Marker3D"), "set_rudder_marker_path", "get_rudder_marker_path");
}

void ShipVessel::set_thrust_power_main(double power) { thrust_power_main = power; }
double ShipVessel::get_thrust_power_main() const { return thrust_power_main; }
void ShipVessel::set_max_rudder_force(double force) { max_rudder_force = force; }
double ShipVessel::get_max_rudder_force() const { return max_rudder_force; }

void ShipVessel::set_propeller_marker_path(const NodePath& path) {
    propeller_marker_path = path;
    if (is_inside_tree()) { // Je�li jeste�my ju� w drzewie, spr�buj pobra� w�ze�
        propeller_marker_node = Object::cast_to<Marker3D>(get_node_or_null(propeller_marker_path));
    }
}
NodePath ShipVessel::get_propeller_marker_path() const { return propeller_marker_path; }

void ShipVessel::set_rudder_marker_path(const NodePath& path) {
    rudder_marker_path = path;
    if (is_inside_tree()) {
        rudder_marker_node = Object::cast_to<Marker3D>(get_node_or_null(rudder_marker_path));
    }
}
NodePath ShipVessel::get_rudder_marker_path() const { return rudder_marker_path; }


void ShipVessel::_ready()
{
    //BuoyancyBody::_ready(); // Wywo�aj _ready() klasy bazowej, je�li istnieje

    if (!propeller_marker_path.is_empty()) {
        propeller_marker_node = Object::cast_to<Marker3D>(get_node_or_null(propeller_marker_path));
        if (!propeller_marker_node) {
            UtilityFunctions::push_warning("MotorVesselBody3D: Propeller Marker3D not found at path: " + propeller_marker_path);
        }
    }
    else {
        UtilityFunctions::push_warning("MotorVesselBody3D: Propeller marker path is not set.");
    }

    if (!rudder_marker_path.is_empty()) {
        rudder_marker_node = Object::cast_to<Marker3D>(get_node_or_null(rudder_marker_path));
        if (!rudder_marker_node) {
            UtilityFunctions::push_warning("MotorVesselBody3D: Rudder Marker3D not found at path: " + rudder_marker_path);
        }
    }
    else {
        UtilityFunctions::push_warning("MotorVesselBody3D: Rudder marker path is not set.");
    }
}

void ShipVessel::_process(double delta) {
    BuoyancyBody::_process(delta); // Wywo�aj _process() klasy bazowej, je�li istnieje i jest potrzebne

    // 'environment' jest odziedziczony z BuoyancyBody3D (jest to Node*)
    Node* ocean_env = get_environment(); // U�yj gettera z klasy bazowej

    if (!ocean_env || Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    // Sprawdzenie, czy environment ma wymagan� metod� (ju� robione w BuoyancyBody3D, ale mo�na powt�rzy� dla pewno�ci)
    if (!ocean_env->has_method("get_wave_height")) {
        // Ostrze�enie powinno by� ju� wygenerowane przez BuoyancyBody3D
        return;
    }

    Input* input = Input::get_singleton();

    // Logika �ruby
    if (propeller_marker_node) {
        Variant propeller_wave_height_v = ocean_env->call("get_wave_height", propeller_marker_node->get_global_position());
        double propeller_wave_height = 0.0;
        if (propeller_wave_height_v.get_type() == Variant::FLOAT || propeller_wave_height_v.get_type() == Variant::INT) {
            propeller_wave_height = propeller_wave_height_v.operator double();
        }

        if (propeller_wave_height > propeller_marker_node->get_global_position().y) { // �ruba zanurzona
            Vector3 ship_forward_vector = -get_global_transform().get_basis().get_column(2); // Zazwyczaj -Z to prz�d; // Zazwyczaj -Z to prz�d

            Vector3 prop_horizontal = ship_forward_vector;
            prop_horizontal.y = 0.0;
            if (prop_horizontal.length_squared() > 0.0001f) { // Unikaj normalizacji wektora zerowego
                prop_horizontal = prop_horizontal.normalized();
            }
            else {
                // Domy�lny kierunek, je�li statek jest idealnie pionowo (rzadkie)
                // Mo�na te� nie aplikowa� si�y w takim przypadku
                prop_horizontal = Vector3(0, 0, -1);
            }

            double prop_dot = prop_horizontal.dot(ship_forward_vector); // Jak bardzo "do przodu" jest skierowany nap�d

            Vector3 force_direction;
            if (input->is_action_pressed(action_thrust_forwards)) {
                force_direction = ship_forward_vector;
                apply_force(force_direction * thrust_power_main * prop_dot, propeller_marker_node->get_global_position() - get_global_position());
            }
            else if (input->is_action_pressed(action_thrust_backwards)) {
                force_direction = -ship_forward_vector; // Wstecz
                apply_force(force_direction * thrust_power_main * prop_dot, propeller_marker_node->get_global_position() - get_global_position());
            }
        }
    }

    // Logika steru
    if (rudder_marker_node) {
        Variant rudder_wave_height_v = ocean_env->call("get_wave_height", rudder_marker_node->get_global_position());
        double rudder_wave_height = 0.0;
        if (rudder_wave_height_v.get_type() == Variant::FLOAT || rudder_wave_height_v.get_type() == Variant::INT) {
            rudder_wave_height = rudder_wave_height_v.operator double();
        }

        if (rudder_wave_height > rudder_marker_node->get_global_position().y) { // Ster zanurzony
            Vector3 ship_right_vector = get_global_transform().get_basis().get_column(0); // X to zazwyczaj prawo

            if (input->is_action_pressed(action_steer_left)) {
                apply_force(ship_right_vector * max_rudder_force, rudder_marker_node->get_global_position() - get_global_position());
            }
            else if (input->is_action_pressed(action_steer_right)) {
                apply_force(-ship_right_vector * max_rudder_force, rudder_marker_node->get_global_position() - get_global_position());
            }
        }
    }

}


EscortShip::EscortShip()
{
    // Inicjalizacja dodatkowych p�l
	currentTargetPosition = Vector3();          // Inicjalizacja pozycji docelowej
    currentState = EscortState::PATROLLING;
    activeDetection = DetectionMode::VISUAL;
}

//									                Destruktor (28.05.2025)
/*=============================================================================================================================

=============================================================================================================================*/
EscortShip::~EscortShip()
{
    // Usuni�cie w�z�a graficznego, je�li istnieje
    if (pShipNode)
    {
        pShipNode->queue_free();
        pShipNode = nullptr;
    }
	// Usuni�cie w�z�a strefy patrolowej, je�li istnieje
    if (pPatrolZoneNode)
    {
        pPatrolZoneNode->queue_free();
        pPatrolZoneNode = nullptr;
	}
}

//									         Przygotowanie w�z�a EscortShip (31.05.2025)
/*=============================================================================================================================

=============================================================================================================================*/
void EscortShip::_ready()
{
    BuoyancyBody::_ready();         // Wywo�aj _ready() klasy bazowej, je�li istnieje
	ShipVessel::_ready();           // Wywo�aj _ready() klasy bazowej, je�li istnieje

    // Ustaw pocz�tkow� pozycj� docelow�
    ChooseNewPatrolTarget();
}

void EscortShip::_physics_process(double delta)
{
    BuoyancyBody::_physics_process(delta);
    ShipVessel::_physics_process(delta);

    if (Engine::get_singleton()->is_editor_hint()) return;

    // Aktualizuj patrolowanie i wykrywanie
    UpdatePatrolling(delta);
    UpdateDetection(delta);
    UpdateMovement(delta);
}

void EscortShip::_bind_methods()
{
}

//									        Wizualizacja obszaru patrolowego (21.05.2025)
/*=============================================================================================================================

=============================================================================================================================*/
void EscortShip::ExternalizePatrolZone()
{
    // Tworzenie wizualizacji strefy patrolowej za pomoc� CylinderMesh
    Ref<CylinderMesh> patrolZoneMesh = memnew(CylinderMesh);
    patrolZoneMesh->set_height(2.0f);
    // Sprawdzenie, czy patrolZone.zone zawiera Circle
    if (std::holds_alternative<Circle>(patrolZone.zone))
    {
        const Circle& circle = std::get<Circle>(patrolZone.zone);
        patrolZoneMesh->set_top_radius(circle.radius); // U�ycie promienia z Circle
        patrolZoneMesh->set_bottom_radius(circle.radius);
    }
    else
    {
        // Obs�uga przypadku, gdy patrolZone.zone nie jest Circle
        ERR_PRINT("PatrolZone is not a Circle. Cannot set top radius.");
        return;
    }
    patrolZoneMesh->set_radial_segments(32);

    // Tworzenie materia�u dla strefy patrolowej
    Ref<StandardMaterial3D> material = memnew(StandardMaterial3D);
    material->set_transparency(StandardMaterial3D::TRANSPARENCY_ALPHA);
    material->set_albedo(Color(0.0f, 0.5f, 1.0f, 0.3f)); // Niebieski, p�przezroczysty
    material->set_shading_mode(StandardMaterial3D::SHADING_MODE_UNSHADED); // Ustawienie trybu na "unshaded"

    // Tworzenie instancji meshu
    pPatrolZoneNode = memnew(MeshInstance3D);
    pPatrolZoneNode->set_mesh(patrolZoneMesh);
    pPatrolZoneNode->set_material_override(material);
    pPatrolZoneNode->set_name("PatrolZone");


    pPatrolZoneNode->set_position(Vector3(0, 0, 0)); // Ustawienie pozycji na (0, 0, 0)
    pPatrolZoneNode->set_rotation_degrees(Vector3(0, 0, 0)); // Ustawienie rotacji na (0, 0, 0)

    // Dodanie do drzewa sceny
    auto pEscortNode = Object::cast_to<Node3D>(this);
    pEscortNode->add_child(pPatrolZoneNode);
    pPatrolZoneNode->set_owner(get_tree()->get_edited_scene_root());
}

void EscortShip::UpdatePatrolling(double delta)
{
    switch (currentState) {
    case EscortState::PATROLLING: {
        // Sprawd� czy dotarli�my do celu
        Vector3 currentPos = get_global_position();
        float distanceToTarget = currentPos.distance_to(currentTargetPosition);

        if (distanceToTarget < 50.0f) {
            // Sprawd� czy wci�� jeste�my w strefie
            if (!IsInPatrolZone()) {
                currentState = EscortState::RETURNING_TO_ZONE;
                ReturnToPatrolZone();
            }
            else {
                // Wybierz nowy cel
                ChooseNewPatrolTarget();

                // Losowo zmie� tryb wykrywania (30% szans na zmian�)
                if (UtilityFunctions::randf() < 0.3f) {
                    int mode = UtilityFunctions::randi_range(0, 2);
                    SetDetectionMode(static_cast<DetectionMode>(mode));
                }
            }
        }
        break;
    }

    case EscortState::RETURNING_TO_ZONE: {
        if (IsInPatrolZone()) {
            currentState = EscortState::PATROLLING;
            ChooseNewPatrolTarget();
            SetDetectionMode(DetectionMode::VISUAL);
        }
        break;
    }

    case EscortState::TRACKING_CONTACT: {
        if (trackedContact) {
            TrackContact(trackedContact, delta);
        }
        else {
            currentState = EscortState::PATROLLING;
        }
        break;
    }
    }
}

void EscortShip::UpdateDetection(double delta)
{
    lastDetectionUpdate += delta;

    if (lastDetectionUpdate < detectionUpdateInterval) return;

    lastDetectionUpdate = 0.0;

    switch (activeDetection) {
    case DetectionMode::VISUAL:
        PerformVisualScan(delta);
        break;
    case DetectionMode::SONAR:
        PerformSonarScan(delta);
        break;
    case DetectionMode::LISTENING:
        PerformListeningScan(delta);
        break;
    }
}

void EscortShip::PerformSonarScan(double delta)
{
    // Sprawd� czy mo�emy u�ywa� sonaru w tym kierunku
    Vector3 sonarDirection = get_global_transform().basis.xform(
        Vector3(sin(deg_to_rad(currentSonarBearing)), 0, cos(deg_to_rad(currentSonarBearing)))
    );

    if (!CanDirectSensorTowards(sonarDirection)) {
        // Pomi� ten kierunek
        currentSonarBearing += sonarBeamWidth * (sonarSweepingRight ? 1 : -1);
        return;
    }

    // Symulacja skanowania sonarem
    // Tu nale�a�oby wykona� ray cast lub obszarowe sprawdzenie obecno�ci U-boot�w

    // Przesu� wi�zk� sonaru
    if (sonarSweepingRight) {
        currentSonarBearing += sonarBeamWidth;
        if (currentSonarBearing >= sonarMaxAngle) {
            sonarSweepingRight = false;
        }
    }
    else {
        currentSonarBearing -= sonarBeamWidth;
        if (currentSonarBearing <= -sonarMaxAngle) {
            sonarSweepingRight = true;
        }
    }
}

void EscortShip::PerformVisualScan(double delta)
{
    // Skanowanie 360 stopni
    // Zasi�g zale�ny od warunk�w (tu uproszczenie)
    float effectiveRange = visualRange;

    // Tu nale�a�oby sprawdzi� wszystkie obiekty w zasi�gu
    // i okre�li� prawdopodobie�stwo wykrycia
}

void EscortShip::PerformListeningScan(double delta)
{
    // Podobnie jak sonar, ale z wi�kszym zasi�giem i mniejsz� dok�adno�ci�
    Vector3 listeningDirection = get_global_transform().basis.xform(
        Vector3(sin(deg_to_rad(currentSonarBearing)), 0, cos(deg_to_rad(currentSonarBearing)))
    );

    if (!CanDirectSensorTowards(listeningDirection)) {
        return;
    }

    // Symulacja nas�uchu
}

bool EscortShip::CanDirectSensorTowards(Vector3 direction)
{
    // Sprawd� czy kierunek nie jest skierowany na konw�j
    if (!pConvoy) return true;

    Vector3 toConvoy = pConvoy->get_global_position() - get_global_position();
    float angleToConvoy = direction.angle_to(toConvoy);

    // Nie kieruj w stron� konwoju (k�t < 30 stopni)
    if (rad_to_deg(angleToConvoy) < 30.0f)
    {
        return false;
    }

    // Sprawd� inne eskortowce w pobli�u
    // TODO: Implementacja sprawdzania innych jednostek

    return true;
}

void EscortShip::UpdateMovement(double delta)
{
    if (currentState == EscortState::RETURNING_TO_ZONE)
    {
        // Szybki powr�t do strefy
        MoveTowardsTarget(currentTargetPosition, returnSpeed, delta);
    }
    else
    {
        // Normalny ruch z ograniczeniem pr�dko�ci
        float maxSpeed = GetMaxSpeedForCurrentMode();
        float speed = Math::min(patrolSpeed, maxSpeed);
        MoveTowardsTarget(currentTargetPosition, speed, delta);
    }
}

void EscortShip::MoveTowardsTarget(Vector3 target, float speedKnots, double delta)
{
    Vector3 currentPos = get_global_position();
    Vector3 direction = (target - currentPos).normalized();

    // Konwersja pr�dko�ci na m/s
    float speedMps = speedKnots * 0.514444f;

    // Oblicz now� pozycj�
    Vector3 movement = direction * speedMps * delta;
    set_global_position(currentPos + movement);

    // Obr�� statek w kierunku ruchu
    if (direction.length() > 0.01f)
    {
        //look_at(currentPos + direction, Vector3::UP);
        look_at(currentPos + direction, Vector3(0, 1, 0));
    }
}

float EscortShip::GetMaxSpeedForCurrentMode()
{
    switch (activeDetection) {
    case DetectionMode::VISUAL:
        return VISUAL_MAX_SPEED;
    case DetectionMode::LISTENING:
        return LISTENING_MAX_SPEED;
    case DetectionMode::SONAR:
        return SONAR_MAX_SPEED;
    default:
        return patrolSpeed;
    }
}

void EscortShip::SetDetectionMode(DetectionMode mode)
{
    // Nie zmieniaj trybu podczas �ledzenia
    if (currentState == EscortState::TRACKING_CONTACT) return;

    // Nie u�ywaj sonaru/nas�uchu je�li p�yniemy za szybko
    float currentSpeed = get_linear_velocity().length() / 0.514444f; // Konwersja na w�z�y

    if (mode == DetectionMode::LISTENING && currentSpeed > LISTENING_MAX_SPEED) {
        return;
    }
    if (mode == DetectionMode::SONAR && currentSpeed > SONAR_MAX_SPEED) {
        return;
    }

    activeDetection = mode;

    // Reset kierunku skanowania przy zmianie trybu
    currentSonarBearing = 0.0f;
    sonarSweepingRight = true;
}

void EscortShip::ChooseNewPatrolTarget()
{
    currentTargetPosition = GetRandomPointInPatrolZone();
}

Vector3 EscortShip::GetRandomPointInPatrolZone()
{
    // Zak�adaj�c �e patrolZone to Circle
    if (std::holds_alternative<Circle>(patrolZone.zone))
    {
        const Circle& circle = std::get<Circle>(patrolZone.zone);

        // Losowy punkt w kole
        float angle = UtilityFunctions::randf() * Math_TAU;
        float distance = sqrt(UtilityFunctions::randf()) * circle.radius;

        return Vector3(
            circle.center.x + distance * cos(angle),
            0.0f,
            circle.center.y + distance * sin(angle)
        );
    }

    // Dla prostok�ta lub innych kszta�t�w
    return get_global_position();
}

bool EscortShip::IsInPatrolZone()
{
    Vector3 currentPos = get_global_position();
    return IsPointInPatrolZone(currentPos);
}

bool EscortShip::IsPointInPatrolZone(Vector3 point)
{
    if (std::holds_alternative<Circle>(patrolZone.zone)) {
        const Circle& circle = std::get<Circle>(patrolZone.zone);
        Vector2 point2D(point.x, point.z);
        return point2D.distance_to(circle.center) <= circle.radius;
    }
    // Dodaj obs�ug� innych kszta�t�w
    return true;
}

void EscortShip::ReturnToPatrolZone()
{
    // Ustaw cel jako �rodek strefy patrolowej
    if (std::holds_alternative<Circle>(patrolZone.zone)) {
        const Circle& circle = std::get<Circle>(patrolZone.zone);
        currentTargetPosition = Vector3(circle.center.x, 0.0f, circle.center.y);
    }
}

void EscortShip::TrackContact(SonarContact* contact, double delta)
{
    if (!contact) return;

    // Aktualizuj pozycj� kontaktu
    double currentTime = Time::get_singleton()->get_ticks_msec() / 1000.0;
    double timeSinceUpdate = currentTime - contact->lastUpdate;

    // Je�li kontakt jest za stary, przesta� �ledzi�
    if (timeSinceUpdate > 10.0)
    {
        trackedContact = nullptr;
        currentState = EscortState::PATROLLING;
        return;
    }

    // Wykonaj ma�e wahania sonaru/nas�uchu dla okre�lenia kursu
    float trackingBearing = contact->bearing;
    float wobble = sin(currentTime * 2.0) * 5.0; // +/- 5 stopni wahania

    currentSonarBearing = trackingBearing + wobble;

    // Ustaw cel ruchu w kierunku kontaktu, ale zachowaj bezpieczn� odleg�o��
    Vector3 directionToContact = (contact->position - get_global_position()).normalized();
    float safeDistance = 500.0f; // Zachowaj 500m dystansu

    currentTargetPosition = contact->position - directionToContact * safeDistance;
}

float EscortShip::CalculateDetectionProbability(float distance, DetectionMode mode)
{
    float baseProbability = 1.0f;

    switch (mode) {
    case DetectionMode::VISUAL: {
        // Prawdopodobie�stwo maleje z kwadratem odleg�o�ci
        float maxRange = visualRange;
        if (distance > maxRange) return 0.0f;

        baseProbability = 1.0f - pow(distance / maxRange, 2);

        // Modyfikatory dla warunk�w
        // TODO: Dodaj wp�yw pogody, pory dnia, mg�y itp.

        break;
    }

    case DetectionMode::SONAR: {
        float maxRange = sonarRange;
        if (distance > maxRange) return 0.0f;

        // Sonar ma bardziej liniowy spadek skuteczno�ci
        baseProbability = 1.0f - (distance / maxRange);

        // Modyfikator dla g��boko�ci celu
        // TODO: Dodaj wp�yw g��boko�ci U-boota

        break;
    }

    case DetectionMode::LISTENING: {
        float maxRange = listeningRange;
        if (distance > maxRange) return 0.0f;

        baseProbability = 1.0f - (distance / maxRange);

        // Nas�uch jest mniej dok�adny
        baseProbability *= 0.7f;

        // Modyfikator dla pr�dko�ci celu
        // TODO: Szybszy cel = �atwiejszy do wykrycia

        break;
    }
    }

    return Math::clamp(baseProbability, 0.0f, 1.0f);
}

// Dodatkowe metody pomocnicze

void EscortShip::UpdateSonarBearingForShipRotation(float shipRotationDelta)
{
    // Skoryguj namiar sonaru przy obrocie okr�tu
    // aby pozosta� skierowany na ten sam punkt w przestrzeni
    currentSonarBearing -= rad_to_deg(shipRotationDelta);

    // Ogranicz do dozwolonego zakresu
    currentSonarBearing = Math::clamp(currentSonarBearing, -sonarMaxAngle, sonarMaxAngle);
}

bool EscortShip::IsTargetInKilwater(Vector3 targetPosition)
{
    // Sprawd� czy cel jest w kilwaterze innego okr�tu
    // Kilwater zak��ca sonar i nas�uch

    if (!pEscortsNode) return false;

    const auto& otherEscorts = pEscortsNode->VecPlacedEscorts();

    for (auto* otherShip : otherEscorts) {
        if (otherShip == this) continue;

        Vector3 otherPos = otherShip->get_global_position();
        Vector3 otherVelocity = otherShip->get_linear_velocity();

        if (otherVelocity.length() < 1.0f) continue; // Statek stoi

        // Sprawd� czy target jest za innym statkiem
        Vector3 toTarget = targetPosition - otherPos;
        Vector3 shipDirection = otherVelocity.normalized();

        float dotProduct = toTarget.dot(shipDirection);

        // Je�li cel jest z ty�u statku
        if (dotProduct < 0) {
            // Sprawd� czy jest w sto�ku kilwateru (oko�o 30 stopni)
            float angle = toTarget.angle_to(-shipDirection);
            if (rad_to_deg(angle) < 30.0f) {
                // Sprawd� odleg�o�� (kilwater do 2x zasi�gu sonaru)
                float distance = toTarget.length();
                if (distance < sonarRange * 2.0f) {
                    return true;
                }
            }
        }
    }

    return false;
}

// Metoda do raportowania kontaktu do systemu
void EscortShip::ReportContact(Vector3 position, float confidence)
{
    if (!pEscortsNode) return;

    // TODO: Przeka� informacj� do centralnego systemu wykrywania
    DetectionSystem::DetectionReport report;
    report.position = position;
    report.confidence = confidence;
    report.timestamp = Time::get_singleton()->get_ticks_msec() / 1000.0;
    report.reportingShipID = GetID();
    report.detectionMethod = activeDetection;

    // Tutaj nale�a�oby wywo�a� metod� w TG_Escorts do dodania raportu
}




void MerchantShip::_bind_methods()
{
}

//									         Przygotowanie w�z�a MerchantShip (01.06.2025)
/*=============================================================================================================================

=============================================================================================================================*/
void MerchantShip::_ready()
{
    BuoyancyBody::_ready(); // Wywo�aj _ready() klasy bazowej, je�li istnieje;
}

void MerchantShip::_process(double delta)
{
    BuoyancyBody::_process(delta); // Wywo�aj _process() klasy bazowej, je�li istnieje i jest potrzebne
	// Tutaj mo�na doda� logik� specyficzn� dla MerchantShip, je�li jest potrzebna
}
