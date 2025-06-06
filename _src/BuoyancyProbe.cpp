#include "BuoyancyProbe.h"
#include "buoyancyBody.h" // Pe�ny include dla BuoyancyBody

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp> // Dla push_warning, print itp. (je�li potrzebne)
#include <godot_cpp/classes/node.hpp> // Dla get_parent()

using namespace godot;

BuoyancyProbe::BuoyancyProbe() {
    buoyancy_multiplier = 1.0;
    height_sampling_steps = 2;
    max_cascade = 1;
    _buoyancy_body = nullptr;
}

BuoyancyProbe::~BuoyancyProbe() {
    // _buoyancy_body jest tylko wska�nikiem, nie zarz�dzamy jego pami�ci� tutaj
}

void BuoyancyProbe::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_buoyancy_multiplier", "multiplier"), &BuoyancyProbe::set_buoyancy_multiplier);
    ClassDB::bind_method(D_METHOD("get_buoyancy_multiplier"), &BuoyancyProbe::get_buoyancy_multiplier);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "buoyancy_multiplier", PROPERTY_HINT_RANGE, "0.0,100.0,0.01"), "set_buoyancy_multiplier", "get_buoyancy_multiplier");

    ClassDB::bind_method(D_METHOD("set_height_sampling_steps", "steps"), &BuoyancyProbe::set_height_sampling_steps);
    ClassDB::bind_method(D_METHOD("get_height_sampling_steps"), &BuoyancyProbe::get_height_sampling_steps);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "height_sampling_steps", PROPERTY_HINT_RANGE, "0,10,1"), "set_height_sampling_steps", "get_height_sampling_steps");

    ClassDB::bind_method(D_METHOD("set_max_cascade", "cascade"), &BuoyancyProbe::set_max_cascade);
    ClassDB::bind_method(D_METHOD("get_max_cascade"), &BuoyancyProbe::get_max_cascade);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "max_cascade", PROPERTY_HINT_RANGE, "0,2,1"), "set_max_cascade", "get_max_cascade");

    // Mo�na te� zbindowa� get_buoyancy_body, je�li jest potrzebny dost�p z GDScript
    // ClassDB::bind_method(D_METHOD("get_buoyancy_body"), &BuoyancyProbe3D::get_buoyancy_body);
}

// Implementacje setter�w i getter�w
void BuoyancyProbe::set_buoyancy_multiplier(double p_multiplier) {
    buoyancy_multiplier = p_multiplier;
}
double BuoyancyProbe::get_buoyancy_multiplier() const {
    return buoyancy_multiplier;
}

void BuoyancyProbe::set_height_sampling_steps(int p_steps) {
    height_sampling_steps = p_steps;
}
int BuoyancyProbe::get_height_sampling_steps() const {
    return height_sampling_steps;
}

void BuoyancyProbe::set_max_cascade(int p_cascade) {
    max_cascade = p_cascade;
}
int BuoyancyProbe::get_max_cascade() const {
    return max_cascade;
}

BuoyancyBody* BuoyancyProbe::get_buoyancy_body() const {
    return _buoyancy_body;
}

void BuoyancyProbe::_add_to_buoyancy_body_3d_ancestor() {
    Node* parent_node = get_parent();
    while (parent_node) {
        BuoyancyBody* body = Object::cast_to<BuoyancyBody>(parent_node);
        if (body) {
            body->add_probe(this); // 'this' jest wska�nikiem do bie��cego obiektu BuoyancyProbe3D
            _buoyancy_body = body;
            return;
        }
        parent_node = parent_node->get_parent();
    }
    _buoyancy_body = nullptr; // Nie znaleziono przodka BuoyancyBody
}

void BuoyancyProbe::_enter_tree() {
    _add_to_buoyancy_body_3d_ancestor();
}

void BuoyancyProbe::_exit_tree() {
    if (_buoyancy_body) {
        _buoyancy_body->remove_probe(this);
        _buoyancy_body = nullptr; // Usu� referencj� po opuszczeniu drzewa
    }
}

PackedStringArray BuoyancyProbe::_get_configuration_warnings() {
    PackedStringArray warnings;
    if (!_buoyancy_body) {
        // W GDScript by�o to jedno d�ugie zdanie. Mo�na je tak zostawi� lub podzieli�.
        warnings.push_back("BuoyancyProbe3D only serves to provide a buoyancy probe to a BuoyancyBody3D derived node. Please ensure it has a BuoyancyBody3D as an ancestor in the scene tree.");
    }
    return warnings;
}