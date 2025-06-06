#include "g_FreeCamera3D.h"

#include <godot_cpp/core/math.hpp>
#include <godot_cpp/classes/input_event_mouse_motion.hpp>	// for InputEventMouseMotion
#include <godot_cpp/classes/input_event_key.hpp>			// for KEY_ESCAPE
#include <godot_cpp/classes/scene_tree.hpp>					// for get_tree()->quit()
#include <godot_cpp/classes/engine.hpp>						// for Engine::get_singleton()->is_editor_hint()

void g_FreeCamera3D::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("GetRotationSpeed"), &g_FreeCamera3D::GetRotationSpeed);
	ClassDB::bind_method(D_METHOD("SetRotationSpeed", "iSpeed"), &g_FreeCamera3D::SetRotationSpeed);
	ClassDB::bind_method(D_METHOD("GetMouseSensitivity"), &g_FreeCamera3D::GetMouseSensitivity);
	ClassDB::bind_method(D_METHOD("SetMouseSensitivity", "iSensitivity"), &g_FreeCamera3D::SetMouseSensitivity);
	ClassDB::bind_method(D_METHOD("GetSlowMovementSpeed"), &g_FreeCamera3D::GetSlowMovementSpeed);
	ClassDB::bind_method(D_METHOD("SetSlowMovementSpeed", "iSpeed"), &g_FreeCamera3D::SetSlowMovementSpeed);
	ClassDB::bind_method(D_METHOD("GetFastMovementSpeed"), &g_FreeCamera3D::GetFastMovementSpeed);
	ClassDB::bind_method(D_METHOD("SetFastMovementSpeed", "iSpeed"), &g_FreeCamera3D::SetFastMovementSpeed);

	ClassDB::add_property("g_FreeCamera3D", PropertyInfo(Variant::FLOAT, "RotationSpeed"), "SetRotationSpeed", "GetRotationSpeed");
	ClassDB::add_property("g_FreeCamera3D", PropertyInfo(Variant::FLOAT, "MouseSensitivity"), "SetMouseSensitivity", "GetMouseSensitivity");
	ClassDB::add_property("g_FreeCamera3D", PropertyInfo(Variant::FLOAT, "FastMovementSpeed"), "SetFastMovementSpeed", "GetFastMovementSpeed");
	ClassDB::add_property("g_FreeCamera3D", PropertyInfo(Variant::FLOAT, "SlowMovementSpeed"), "SetSlowMovementSpeed", "GetSlowMovementSpeed");

}

g_FreeCamera3D::g_FreeCamera3D()
{
}

g_FreeCamera3D::~g_FreeCamera3D()
{
	
}

void g_FreeCamera3D::_ready()
{
	pInput = Input::get_singleton();
}

//										Obs³uga kamery (11.01.2025)
/*=============================================================================================================================

(updated 08.02.2025)
	- dodny szybki i wolny ruch kamery
	- dodano ruch kamery w górê i w dó³
=============================================================================================================================*/
void g_FreeCamera3D::_process(double delta)
{
	if (Engine::get_singleton()->is_editor_hint()) return; // Early return if we are in editor

	// Strafe the camera
	float rotation_amount = pInput->get_action_strength("free_cam_right") - pInput->get_action_strength("free_cam_left");
	if (rotation_amount)
	{
		//rotate(Vector3(0, 1, 0), rotation_amount * -1 * rotation_speed * delta);
		translate(Vector3{ rotation_amount, 0.0f, 0.0f }); //Translate is actually implemented like this: position = position + offset
	}

	// Move the camera forward and backward
	float camDir = pInput->get_action_strength("free_cam_backward") - pInput->get_action_strength("free_cam_forward");
	if (camDir)
	{
		float velocity = prop_slowMovementSpeed * camDir * delta;
		if (pInput->get_action_strength("free_cam_speed"))
			velocity = prop_fastMovementSpeed * camDir * delta;
		translate(Vector3{0.0f, 0.0f, velocity}); //Translate is actually implemented like this: position = position + offset
	}

	// Move the camera up and down
	float camUpDown = pInput->get_action_strength("free_cam_up") - pInput->get_action_strength("free_cam_down");
	if (camUpDown)
	{
		float velocity = prop_slowMovementSpeed * camUpDown * delta;
		if (pInput->get_action_strength("free_cam_speed"))
			velocity = prop_fastMovementSpeed * camUpDown * delta;
		translate(Vector3{ 0.0f, velocity, 0.0f }); //Translate is actually implemented like this: position = position + offset
	}

	// Mouse look
	if (pInput->is_mouse_button_pressed(MOUSE_BUTTON_RIGHT))
	{
		// Vector2 mouse_motion = pInput->get_mouse_motion()
		Vector2 mouse_motion = pInput->get_last_mouse_velocity();
		/*rotate_x(mouse_motion.y * -1 * mouseSensitivity * delta);
		rotate_y(mouse_motion.x * -1 * mouseSensitivity * delta);*/
		rotate_y(-mouse_motion.x * prop_mouseSensitivity);
		rotate_object_local(Vector3{ 1,0,0 }, mouse_motion.y * prop_mouseSensitivity);
	}
} 

void g_FreeCamera3D::_input(const Ref<InputEvent> event)
{
	if (event->is_pressed())
	{
		InputEventKey* key_event = static_cast<InputEventKey*>(*event);
		if (key_event->is_pressed())
		{
			if (key_event->get_keycode() == KEY_ESCAPE)
			{
				get_tree()->quit();
			}
		}
	}
	
	
}


