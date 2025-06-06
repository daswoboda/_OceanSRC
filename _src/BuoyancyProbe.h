#pragma once
#ifndef BUOYANCYPROBE_H
#define BUOYANCYPROBE_H

#include <marker3d.hpp>
#include <binder_common.hpp>
#include <packed_string_array.hpp>

// Forward declaration, aby unikn¹æ problemów z cyklicznymi zale¿noœciami
// Pe³ny include buoyancy_body_3d.h bêdzie w pliku .cpp
//namespace godot {
//    class BuoyancyBody3D;
//}

class BuoyancyBody;

using namespace godot;
using namespace std;

class BuoyancyProbe : public godot::Marker3D {
    GDCLASS(BuoyancyProbe, godot::Marker3D);

private:
    double buoyancy_multiplier;
    int height_sampling_steps;
    int max_cascade;

    BuoyancyBody* _buoyancy_body; // WskaŸnik do BuoyancyBody3D

    void _add_to_buoyancy_body_3d_ancestor();

protected:
    static void _bind_methods();

public:
    BuoyancyProbe();
    ~BuoyancyProbe();

    void _enter_tree() override;
    void _exit_tree() override;
    PackedStringArray _get_configuration_warnings();

    // Settery i Gettery dla eksportowanych w³aœciwoœci
    void set_buoyancy_multiplier(double p_multiplier);
    double get_buoyancy_multiplier() const;

    void set_height_sampling_steps(int p_steps);
    int get_height_sampling_steps() const;

    void set_max_cascade(int p_cascade);
    int get_max_cascade() const;

    // Opcjonalny getter dla _buoyancy_body (g³ównie do debugowania lub u¿ytku wewnêtrznego)
    BuoyancyBody* get_buoyancy_body() const;
};

#endif // BUOYANCY_PROBE_3D_H