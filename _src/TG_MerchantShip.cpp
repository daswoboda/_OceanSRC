#include "TG_MerchantShip.h"

#include <math.hpp>
#include <utility_functions.hpp>        // Dla UtilityFunctions::print
#include <rigid_body3d.hpp>             // Ensure this is included for PhysicsDirectBodyState3D
#include <physics_direct_body_state3d.hpp>

using namespace godot;

//									            Konstruktor (24.05.2025)
/*=============================================================================================================================

=============================================================================================================================*/
TG_MerchantShipX::TG_MerchantShipX()
{
    pConvoyBox = nullptr;
    initialized_offset = false;
    linear_follow_strength = 10.0;  // Nale�y dostroi�
    angular_follow_strength = 5.0; // Nale�y dostroi�
    linear_damping_factor = 0.8;    // Nale�y dostroi� (0-1)
    angular_damping_factor = 0.8;   // Nale�y dostroi� (0-1)
    max_linear_correction_force = 1000.0; // Nale�y dostroi�
    max_angular_correction_torque = 1000.0; // Nale�y dostroi�

    // Ustawienie custom_integrator na true jest konieczne, je�li chcemy w pe�ni kontrolowa� si�y
    // Je�li chcemy tylko dodawa� si�y do standardowej integracji, mo�na zostawi� false.
    // Dla tego przypadku, gdzie aktywnie korygujemy pozycj�, true jest zazwyczaj lepsze.
    set_use_custom_integrator(true);
}

//									            Destruktor (24.05.2025)
/*=============================================================================================================================

=============================================================================================================================*/
TG_MerchantShipX::~TG_MerchantShipX()
{
}

//									    Wi�zanie metod i w�a�ciwo�ci (24.05.2025)
/*=============================================================================================================================

=============================================================================================================================*/
void TG_MerchantShipX::_bind_methods()
{
    // Bindowanie akcesor�w, je�li potrzebne
    ClassDB::bind_method(D_METHOD("set_linear_follow_strength", "p_strength"), &TG_MerchantShipX::set_linear_follow_strength);
    ClassDB::bind_method(D_METHOD("get_linear_follow_strength"), &TG_MerchantShipX::get_linear_follow_strength);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "linear_follow_strength"), "set_linear_follow_strength", "get_linear_follow_strength");

    ClassDB::bind_method(D_METHOD("set_angular_follow_strength", "p_strength"), &TG_MerchantShipX::set_angular_follow_strength);
    ClassDB::bind_method(D_METHOD("get_angular_follow_strength"), &TG_MerchantShipX::get_angular_follow_strength);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "angular_follow_strength"), "set_angular_follow_strength", "get_angular_follow_strength");

    ClassDB::bind_method(D_METHOD("set_linear_damping_factor", "p_factor"), &TG_MerchantShipX::set_linear_damping_factor);
    ClassDB::bind_method(D_METHOD("get_linear_damping_factor"), &TG_MerchantShipX::get_linear_damping_factor);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "linear_damping_factor"), "set_linear_damping_factor", "get_linear_damping_factor");

    ClassDB::bind_method(D_METHOD("set_angular_damping_factor", "p_factor"), &TG_MerchantShipX::set_angular_damping_factor);
    ClassDB::bind_method(D_METHOD("get_angular_damping_factor"), &TG_MerchantShipX::get_angular_damping_factor);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "angular_damping_factor"), "set_angular_damping_factor", "get_angular_damping_factor");

    ClassDB::bind_method(D_METHOD("set_max_linear_correction_force", "p_force"), &TG_MerchantShipX::set_max_linear_correction_force);
    ClassDB::bind_method(D_METHOD("get_max_linear_correction_force"), &TG_MerchantShipX::get_max_linear_correction_force);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_linear_correction_force"), "set_max_linear_correction_force", "get_max_linear_correction_force");

    ClassDB::bind_method(D_METHOD("set_max_angular_correction_torque", "p_torque"), &TG_MerchantShipX::set_max_angular_correction_torque);
    ClassDB::bind_method(D_METHOD("get_max_angular_correction_torque"), &TG_MerchantShipX::get_max_angular_correction_torque);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_angular_correction_torque"), "set_max_angular_correction_torque", "get_max_angular_correction_torque");
}

//									Przygotowanie obiektu TG_MerchantShip (24.05.2025)
/*=============================================================================================================================

=============================================================================================================================*/
void TG_MerchantShipX::_ready() 
{
    Node* parent_node = get_parent(); // Powinien to by� ConvoyBox
    if (parent_node) 
    {
        pConvoyBox = Object::cast_to<Node3D>(parent_node);
    }

    if (pConvoyBox) 
    {
        initial_local_transform = get_transform(); // Transformacja lokalna wzgl�dem ConvoyBox
        initialized_offset = true;
    }
    else 
    {
        UtilityFunctions::printerr("ConvoyShip: Nie uda�o si� znale�� w�z�a kotwicz�cego formacji (rodzica). Statek nie b�dzie pod��a� za formacj�.");
    }
}

//									    Integracja si� fizycznych (24.05.2025)
/*=============================================================================================================================

=============================================================================================================================*/
void TG_MerchantShipX::_integrate_forces(PhysicsDirectBodyState3D* state)
{
    if (!initialized_offset || !pConvoyBox) 
    {
        // Je�li use_custom_integrator jest true, a my nic nie robimy, statek b�dzie sta� w miejscu (lub dryfowa�, je�li s� inne si�y)
        // Mo�na doda� standardow� integracj� si�, je�li jest potrzebna w tym przypadku:
        // state->integrate_forces(); // To wywo�a standardow� fizyk� Godota (grawitacja, t�umienie itp.)
        return;
    }

    time_accumulator += state->get_step();

    // 1. Obliczenie docelowego globalnego transformu statku
    Transform3D anchor_global_transform = pConvoyBox->get_global_transform();
    Transform3D target_ship_global_transform = anchor_global_transform * initial_local_transform;

    // 2. Korekcja liniowa (pozycji)
    Vector3 current_pos = state->get_transform().origin;
    Vector3 target_pos = target_ship_global_transform.origin;
    Vector3 pos_error = target_pos - current_pos;

    // Proporcjonalno-r�niczkuj�cy (PD) kontroler dla pozycji
    Vector3 desired_velocity = pos_error * linear_follow_strength;
    Vector3 velocity_error = desired_velocity - state->get_linear_velocity();
    Vector3 force_to_apply = velocity_error * (1.0 / state->get_step()) * get_mass() * (1.0 - linear_damping_factor); // Si�a = m * a = m * dv/dt
    // Mno�enie przez (1-damping) to uproszczone t�umienie
    force_to_apply = force_to_apply.limit_length(max_linear_correction_force * get_mass()); // Limit si�y
    state->apply_central_force(force_to_apply);


    // 3. Korekcja k�towa (orientacji)
    Basis current_basis = state->get_transform().basis;
    Basis target_basis = target_ship_global_transform.basis;
    Quaternion q_current = current_basis.get_rotation_quaternion();
    Quaternion q_target = target_basis.get_rotation_quaternion();
    Quaternion q_error = q_target * q_current.inverse(); // Obr�t z obecnej do docelowej

    Vector3 axis;
    real_t angle;
    q_error.get_axis_angle(axis, angle);
    if (axis.length_squared() < CMP_EPSILON) { // Ma�y k�t, unikaj dzielenia przez zero
        axis = Vector3(0, 0, 0); // Brak osi obrotu
        angle = 0.0;
    }
    else {
        axis = axis.normalized();
    }

    // Proporcjonalno-r�niczkuj�cy (PD) kontroler dla orientacji
    Vector3 desired_angular_velocity = axis * angle * angular_follow_strength;
    Vector3 angular_velocity_error = desired_angular_velocity - state->get_angular_velocity();
    Vector3 torque_to_apply = state->get_inverse_inertia_tensor().inverse().xform(angular_velocity_error * (1.0 / state->get_step()) * (1.0 - angular_damping_factor));

    torque_to_apply = torque_to_apply.limit_length(max_angular_correction_torque * get_mass()); // Limit momentu (masa dla uproszczenia skali)
    state->apply_torque(torque_to_apply);

    //// 4. Zastosowanie dodatkowych si� fizycznych (np. symulacja fal)
    //Vector3 wave_force = calculate_wave_force_component(state, state->get_step());
    //state->apply_central_force(wave_force); // Lub apply_force w okre�lonym punkcie

    //Vector3 wave_torque = calculate_wave_torque_component(state, state->get_step());
    //state->apply_torque(wave_torque);

    // Je�li custom_integrator jest true, musimy sami zintegrowa� si�y, je�li nie u�ywamy apply_force/torque
    // Jednak apply_force/torque/impulse wewn�trz _integrate_forces dodaj� si�y/momenty do akumulator�w,
    // kt�re s� nast�pnie u�ywane przez wewn�trzny integrator silnika fizyki na ko�cu kroku,
    // o ile nie wywo�amy state->integrate_forces() sami wcze�niej lub nie ustawimy go na ca�kowicie custom.
    // W tym przypadku, poniewa� ustawili�my custom_integrator=true, a u�ywamy apply_central_force i apply_torque,
    // Godot nadal powinien zintegrowa� te zaaplikowane si�y.
    // Je�li chcieliby�my ca�kowicie pomin�� standardow� integracj� Godota i np. bezpo�rednio ustawia� state->set_linear_velocity(),
    // wtedy musieliby�my te� sami obs�u�y� state->set_transform().
    // W dokumentacji jest napisane: "By default, it works in addition to the usual physics behavior,
    // but custom_integrator allows you to disable the default behavior and write custom force integration for a body." [4]
    // Oznacza to, �e je�li custom_integrator jest true, standardowe si�y (grawitacja, t�umienie) nie s� stosowane,
    // ale silnik nadal integruje pr�dko�ci i pozycje na podstawie si�, kt�re my zaaplikujemy w _integrate_forces.
}

//// Przyk�adowe, uproszczone metody symulacji fal
//Vector3 TG_MerchantShip::calculate_wave_force_component(PhysicsDirectBodyState3D* state, double p_delta) {
//    // Prosta si�a sinusoidalna w osi Y dla symulacji unoszenia na fali
//    float wave_y = sin(time_accumulator * wave_force_frequency * Math_TAU) * wave_force_amplitude;
//    return Vector3(0, wave_y * get_mass(), 0); // Si�a proporcjonalna do masy
//}
//
//Vector3 TG_MerchantShip::calculate_wave_torque_component(PhysicsDirectBodyState3D* state, double p_delta) {
//    // Prosty moment sinusoidalny wok� osi X i Z dla symulacji ko�ysania
//    float torque_x = sin(time_accumulator * wave_torque_frequency * Math_TAU + 0.5) * wave_torque_amplitude;
//    float torque_z = cos(time_accumulator * wave_torque_frequency * Math_TAU) * wave_torque_amplitude;
//    // Moment si�y jest bardziej skomplikowany i zale�y od tensora bezw�adno�ci, ale dla uproszczenia:
//    return Vector3(torque_x, 0, torque_z) * get_mass(); // Moment proporcjonalny do masy (uproszczenie)
//}

// Implementacje akcesor�w
void TG_MerchantShipX::set_linear_follow_strength(double p_strength) { linear_follow_strength = p_strength; }
double TG_MerchantShipX::get_linear_follow_strength() const { return linear_follow_strength; }
void TG_MerchantShipX::set_angular_follow_strength(double p_strength) { angular_follow_strength = p_strength; }
double TG_MerchantShipX::get_angular_follow_strength() const { return angular_follow_strength; }
void TG_MerchantShipX::set_linear_damping_factor(double p_factor) { linear_damping_factor = p_factor; }
double TG_MerchantShipX::get_linear_damping_factor() const { return linear_damping_factor; }
void TG_MerchantShipX::set_angular_damping_factor(double p_factor) { angular_damping_factor = p_factor; }
double TG_MerchantShipX::get_angular_damping_factor() const { return angular_damping_factor; }
void TG_MerchantShipX::set_max_linear_correction_force(double p_force) { max_linear_correction_force = p_force; }
double TG_MerchantShipX::get_max_linear_correction_force() const { return max_linear_correction_force; }
void TG_MerchantShipX::set_max_angular_correction_torque(double p_torque) { max_angular_correction_torque = p_torque; }
double TG_MerchantShipX::get_max_angular_correction_torque() const { return max_angular_correction_torque; }