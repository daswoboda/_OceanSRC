#include "buoyancyBody.h"
#include "BuoyancyProbe.h" // Za³o¿enie: BuoyancyProbe3D jest klas¹ C++

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/classes/node.hpp> // Dla get_parent() i rzutowania
#include <godot_cpp/variant/string_name.hpp> // Dla StringName

#include <scene_tree.hpp>					// for get_tree()
#include <window.hpp>						// for get_root()


#include <cmath>

using namespace godot;

BuoyancyBody::BuoyancyBody() {
    buoyancy_multiplier = 1.0;
    buoyancy_power = 1.5;
    environment = nullptr;
    submerged_drag_linear = 0.05;
    submerged_drag_angular = 0.1;

    submerged = false;
    submerged_probes_count = 0;
    _displayed_null_ocean_warning = false;
    _displayed_no_wave_method_warning = false;
}

BuoyancyBody::~BuoyancyBody() {}

void BuoyancyBody::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_buoyancy_multiplier", "multiplier"), &BuoyancyBody::set_buoyancy_multiplier);
    ClassDB::bind_method(D_METHOD("get_buoyancy_multiplier"), &BuoyancyBody::get_buoyancy_multiplier);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "buoyancy_multiplier"), "set_buoyancy_multiplier", "get_buoyancy_multiplier");

    ClassDB::bind_method(D_METHOD("set_buoyancy_power", "power"), &BuoyancyBody::set_buoyancy_power);
    ClassDB::bind_method(D_METHOD("get_buoyancy_power"), &BuoyancyBody::get_buoyancy_power);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "buoyancy_power", PROPERTY_HINT_RANGE, "0.5,10.0,0.001"), "set_buoyancy_power", "get_buoyancy_power");

    ClassDB::bind_method(D_METHOD("set_environment", "env"), &BuoyancyBody::set_environment);
    ClassDB::bind_method(D_METHOD("get_environment"), &BuoyancyBody::get_environment);
    // PROPERTY_HINT_RESOURCE_TYPE mo¿e nadal dzia³aæ z nazw¹ klasy GDScript, jeœli jest ona zarejestrowana przez `class_name`
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "environment", PROPERTY_HINT_NODE_TYPE, "Node"), "set_environment", "get_environment"); // Zmieniono hint na ogólniejszy "Node" dla bezpieczeñstwa, mo¿na spróbowaæ "OceanEnvironment"

    ClassDB::bind_method(D_METHOD("set_submerged_drag_linear", "drag"), &BuoyancyBody::set_submerged_drag_linear);
    ClassDB::bind_method(D_METHOD("get_submerged_drag_linear"), &BuoyancyBody::get_submerged_drag_linear);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "submerged_drag_linear"), "set_submerged_drag_linear", "get_submerged_drag_linear");

    ClassDB::bind_method(D_METHOD("set_submerged_drag_angular", "drag"), &BuoyancyBody::set_submerged_drag_angular);
    ClassDB::bind_method(D_METHOD("get_submerged_drag_angular"), &BuoyancyBody::get_submerged_drag_angular);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "submerged_drag_angular"), "set_submerged_drag_angular", "get_submerged_drag_angular");

    ClassDB::bind_method(D_METHOD("add_probe", "probe"), &BuoyancyBody::add_probe);
    ClassDB::bind_method(D_METHOD("remove_probe", "probe"), &BuoyancyBody::remove_probe);

    ClassDB::bind_method(D_METHOD("is_submerged"), &BuoyancyBody::is_submerged);
    ClassDB::bind_method(D_METHOD("get_submerged_probes_count"), &BuoyancyBody::get_submerged_probes_count);
}

void BuoyancyBody::set_buoyancy_multiplier(double p_multiplier) { buoyancy_multiplier = p_multiplier; }
double BuoyancyBody::get_buoyancy_multiplier() const { return buoyancy_multiplier; }
void BuoyancyBody::set_buoyancy_power(double p_power) { buoyancy_power = p_power; }
double BuoyancyBody::get_buoyancy_power() const { return buoyancy_power; }

void BuoyancyBody::set_environment(Node* p_env) { // << ZMIANA: Typ na Node*
    environment = p_env;
    // Zresetuj flagi ostrze¿eñ, gdy œrodowisko jest ustawiane (nawet na null)
    _displayed_null_ocean_warning = false;
    _displayed_no_wave_method_warning = false;
}
Node* BuoyancyBody::get_environment() const { return environment; } // << ZMIANA: Typ na Node*

void BuoyancyBody::set_submerged_drag_linear(double p_drag) { submerged_drag_linear = p_drag; }
double BuoyancyBody::get_submerged_drag_linear() const { return submerged_drag_linear; }
void BuoyancyBody::set_submerged_drag_angular(double p_drag) { submerged_drag_angular = p_drag; }
double BuoyancyBody::get_submerged_drag_angular() const { return submerged_drag_angular; }

void BuoyancyBody::_add_ocean_environment_ancestor()
{
	// Nie jestem w stanie uzyskaæ dostêpu do wêz³a OceanEnvironment
	// Próbowa³em wiele sposobów, ale nie mogê znaleŸæ poprawnego wêz³a
	// Na razie muszê ustawiaæ Environment rêcznie w edytorze, co jest niewygodne

	// Znalaz³em inny sposób, aby znaleŸæ OceanEnvironment w drzewie
	// Jest on implementowany w _ready(), wiêc nie ma potrzeby tutaj

}

void BuoyancyBody::_physics_process(double delta)
{
    if (Engine::get_singleton()->is_editor_hint())
    {
        return;
    }

    if (!environment)
    {
        if (!_displayed_null_ocean_warning)
        {
            UtilityFunctions::push_warning("BuoyancyBody: OceanEnvironment not set. Attempting to find ancestor in _enter_tree or assign manually.");
            _displayed_null_ocean_warning = true;
        }
        return;
    }
    // Jeœli dotarliœmy tutaj, environment nie jest null, wiêc resetujemy flagê ostrze¿enia o null
    _displayed_null_ocean_warning = false;

    // << ZMIANA: Sprawdzanie metody "get_wave_height" na wêŸle GDScript
    static const StringName get_wave_height_method = StringName("get_wave_height");
    if (!environment->has_method(get_wave_height_method)) {
        if (!_displayed_no_wave_method_warning) {
            UtilityFunctions::push_warning("BuoyancyBody3D: Assigned OceanEnvironment node does not have the 'get_wave_height' method.");
            _displayed_no_wave_method_warning = true;
        }
        return;
    }
    // Jeœli dotarliœmy tutaj, metoda istnieje, resetujemy flagê ostrze¿enia o braku metody
    _displayed_no_wave_method_warning = false;

    Variant gv_variant = ProjectSettings::get_singleton()->get_setting("physics/3d/default_gravity_vector");
    Vector3 gv = gv_variant.operator Vector3();

    submerged_probes_count = 0;
    submerged = false;

    for (int i = 0; i < _buoyancy_probes.size(); ++i) {
        BuoyancyProbe* probe = Object::cast_to<BuoyancyProbe>(_buoyancy_probes[i]);
        if (!probe) {
            continue;
        }

        // << ZMIANA: Wywo³anie metody GDScript `get_wave_height`
        Variant wave_height_variant = environment->call(get_wave_height_method, probe->get_global_position(), probe->get_max_cascade(), probe->get_height_sampling_steps());
        double wave_height = 0.0;

        if (wave_height_variant.get_type() == Variant::FLOAT || wave_height_variant.get_type() == Variant::INT) {
            wave_height = wave_height_variant.operator double();
        }
        else {
            UtilityFunctions::push_warning("BuoyancyBody3D: 'get_wave_height' from OceanEnvironment did not return a number for probe at " + probe->get_global_position());
            // Kontynuuj z wave_height = 0.0 lub inn¹ logik¹ obs³ugi b³êdu
        }

        double depth = Math::clamp((probe->get_global_position().y - wave_height), -10000.0, 0.0);
        double current_buoyancy_force_magnitude = std::pow(std::abs(depth), buoyancy_power);

        if (depth < 0.0) {
            submerged = true;
            submerged_probes_count++;
            double multiplier = buoyancy_multiplier * probe->get_buoyancy_multiplier();
            Vector3 force = -gv * current_buoyancy_force_magnitude * multiplier * delta; // Uwaga: si³a zale¿y od 'delta' fizyki
            apply_force(force, probe->get_global_position() - get_global_position());
        }
    }
}

void BuoyancyBody::_integrate_forces(PhysicsDirectBodyState3D* state) 
{
    if (submerged) {
        // Proporcjonalne t³umienie, niezale¿ne od delta, tak jak w oryginalnym kodzie GDScript
        // state->get_step() to delta dla kroku fizyki
        state->set_linear_velocity(state->get_linear_velocity() * (1.0 - submerged_drag_linear * state->get_step()));
        state->set_angular_velocity(state->get_angular_velocity() * (1.0 - submerged_drag_angular * state->get_step()));
        // Jeœli t³umienie ma byæ jak w oryginalnym GDScript (nie mno¿one przez delta/step):
        // state->set_linear_velocity(state->get_linear_velocity() * (1.0 - submerged_drag_linear));
        // state->set_angular_velocity(state->get_angular_velocity() * (1.0 - submerged_drag_angular));
        // Wybra³em wersjê skalowan¹ przez state->get_step() dla bardziej typowego zachowania t³umienia w fizyce. Dostosuj wg potrzeb.
    }
}

void BuoyancyBody::_enter_tree() 
{
  
}

void BuoyancyBody::_exit_tree() {
    // Nie ma potrzeby zerowania 'environment' tutaj, chyba ¿e jest to Ref<T>
    // Jeœli 'environment' zosta³o przypisane rêcznie, pozostanie.
    // Jeœli zosta³o znalezione w drzewie, wskaŸnik stanie siê nieprawid³owy, jeœli wêze³ zostanie usuniêty,
    // ale logika w _physics_process powinna to obs³u¿yæ przez sprawdzanie 'is_instance_valid(environment)'.
    // Na razie pozostawiam bez zmian; Godot powinien zarz¹dzaæ wêz³ami.
    // environment = nullptr; // Mo¿na rozwa¿yæ, jeœli powoduje problemy
}

//											Przygotowanie komponentu BuoyancyBody (31.05.2025)
/*=============================================================================================================================

=============================================================================================================================*/
void BuoyancyBody::_ready()
{
    auto pRoot = get_tree()->get_root();
    auto pOceanEnv = pRoot->find_child("OceanEnvironment", true, false);
    
    if (pOceanEnv)
    {
        /*StringName nodeName2 = pOceanEnv->get_class();
        std::string nodeNameStr2 = String(nodeName2).utf8().get_data();
        UtilityFunctions::print("Nazwa wêz³a: " + nodeName2);*/
        environment = pOceanEnv;
    }
    else
    {
        UtilityFunctions::printerr("BuoyancyBody: not found OceanEnvironment");
    }

}

void BuoyancyBody::add_probe(BuoyancyProbe* probe) {
    if (probe) {
        if (_buoyancy_probes.find(probe) == -1) {
            _buoyancy_probes.append(probe);
        }
    }
}

void BuoyancyBody::remove_probe(BuoyancyProbe* probe) { // << ZMIANA: Typ na BuoyancyBody
    int index = _buoyancy_probes.find(probe);
    if (index != -1) {
        _buoyancy_probes.remove_at(index);
    }
}

PackedStringArray BuoyancyBody::_get_configuration_warnings() { // << ZMIANA: Typ na BuoyancyBody
    PackedStringArray warnings;
    if (_buoyancy_probes.is_empty()) {
        warnings.push_back("BuoyancyBody: This node has no BuoyancyProbes. Add BuoyancyProbe3D children and ensure they register via add_probe or are found by the body.");
    }
    Node* current_env = get_environment(); // U¿yj gettera
    if (!current_env) {
        warnings.push_back("BuoyancyBody: OceanEnvironment is not assigned. It will attempt to find an ancestor named 'OceanEnvironment' on _enter_tree, or you can assign it manually in the editor.");
    }
    else {
        if (!current_env->has_method("get_wave_height")) {
            warnings.push_back("BuoyancyBody: The assigned 'environment' node does not have a 'get_wave_height' method, which is required for buoyancy calculations.");
        }
    }
    return warnings;
}

bool BuoyancyBody::is_submerged() const { return submerged; } // << ZMIANA: Typ na BuoyancyBody
int BuoyancyBody::get_submerged_probes_count() const { return submerged_probes_count; } // << ZMIANA: Typ na BuoyancyBody