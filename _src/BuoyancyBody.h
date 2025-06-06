#pragma once
#ifndef BUOYANCYBODY_H
#define BUOYANCYBODY_H

#include <rigid_body3d.hpp>
#include <physics_direct_body_state3d.hpp>
#include <packed_string_array.hpp>
#include <binder_common.hpp>

#include "TG_TacticalTier.h"
//#include "TG_Convoy.h"

class TG_TacticalTier;
class TG_Convoy;
class BuoyancyProbe;

using namespace godot;
using namespace std;

//										Klasa obs³uguj¹ca statek handlowy (24.05.2025)
/*=============================================================================================================================
- klasa Godota obs³uguj¹ca pojedynczy statek handlowy
- pod klasê podpiêty jest model 3D okrêtu jako RigidBody
=============================================================================================================================*/
class BuoyancyBody : public RigidBody3D
{
    GDCLASS(BuoyancyBody, RigidBody3D)


protected:
    static void _bind_methods();

public:
    BuoyancyBody();
    ~BuoyancyBody();

    void _physics_process(double delta) override;
    void _integrate_forces(PhysicsDirectBodyState3D* state) override;
    void _enter_tree() override;
    void _exit_tree() override;
    void _ready() override;
    PackedStringArray _get_configuration_warnings();

    // Settery i Gettery dla eksportowanych w³aœciwoœci
    void set_buoyancy_multiplier(double p_multiplier);
    double get_buoyancy_multiplier() const;

    void set_buoyancy_power(double p_power);
    double get_buoyancy_power() const;

    void set_environment(Node* p_env); // << ZMIANA: Typ na godot::Node*
    Node* get_environment() const;     // << ZMIANA: Typ na godot::Node*

    void set_submerged_drag_linear(double p_drag);
    double get_submerged_drag_linear() const;

    void set_submerged_drag_angular(double p_drag);
    double get_submerged_drag_angular() const;

    void add_probe(BuoyancyProbe* probe);
    void remove_probe(BuoyancyProbe* probe);

    bool is_submerged() const;
    int get_submerged_probes_count() const;

private:
	// Pola klasy
    double buoyancy_multiplier;
    double buoyancy_power;
    Node* environment; // << ZMIANA: Typ na godot::Node*
    double submerged_drag_linear;
    double submerged_drag_angular;

    bool submerged;
    int submerged_probes_count;

    TypedArray<Object> _buoyancy_probes;
    bool _displayed_null_ocean_warning;
    bool _displayed_no_wave_method_warning;

	TG_TacticalTier* pTacticalTier{};       // WskaŸnik na TG_TacticalTier
	
    // Metody prywatne
    void _add_ocean_environment_ancestor();


};


#endif