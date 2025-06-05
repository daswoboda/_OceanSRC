#include "TG_Wolfs.h"
#include <godot_cpp/core/class_db.hpp>

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void TG_Wolfs::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_WolfsNumber"), &TG_Wolfs::get_WolfsNumber);
	ClassDB::bind_method(D_METHOD("set_WolfsNumber", "p_WolfsNumber"), &TG_Wolfs::set_WolfsNumber);
	ClassDB::add_property("TG_Wolfs", PropertyInfo(Variant::INT, "WolfsNumber"), "set_WolfsNumber", "get_WolfsNumber");
	ADD_SIGNAL(MethodInfo("position_changed", PropertyInfo(Variant::OBJECT, "node"), PropertyInfo(Variant::VECTOR2, "new_pos")));
}

TG_Wolfs::TG_Wolfs()
{
	// Initialize any variables here.
	if (Engine::get_singleton()->is_editor_hint())
	{
		set_process_mode(Node::ProcessMode::PROCESS_MODE_DISABLED);
	}
	// time_passed = 0.0;
	// amplitude = 1.0;
}

TG_Wolfs::~TG_Wolfs()
{
	// Add your cleanup here.
}

void TG_Wolfs::_ready()
{
	if (Engine::get_singleton()->is_editor_hint()) return;
	UtilityFunctions::print("TG_Wolfs ready");
}

void TG_Wolfs::_process(double delta)
{
	// UtilityFunctions::print("hola mundo desde GDEXTENSION");
	// time_passed += delta;
	// Vector2 new_position = Vector2(10.0 + (10.0 * sin(time_passed * 2.0)), 10.0 + (10.0 * cos(time_passed * 1.5));
	// set_position(new_position * amplitude);
	// time_emit += delta;
	// if (time_emit > 1.0) {
	// 	emit_signal("position_changed", this, new_position);
	// 	time_emit = 0.0;
	// }
}

void TG_Wolfs::set_WolfsNumber(const int WolfsNumber)
{
	wolfsNumber = WolfsNumber;
}

int TG_Wolfs::get_WolfsNumber() const
{
	return wolfsNumber;
}