#include "TG_Cameras.h"

#include <math.hpp>
#include <input_event_mouse_motion.hpp>		// for InputEventMouseMotion
#include <input_event_key.hpp>				// for KEY_ESCAPE
#include <scene_tree.hpp>					// for get_tree()->quit()
#include <engine.hpp>						// for Engine::get_singleton()->is_editor_hint()

void TG_Cameras::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_MainCamera"), &TG_Cameras::get_MainCamera);
	ClassDB::bind_method(D_METHOD("set_MainCamera", "Camera3D"), &TG_Cameras::set_MainCamera);
	ClassDB::bind_method(D_METHOD("get_TacticalCamera"), &TG_Cameras::get_TacticalCamera);
	ClassDB::bind_method(D_METHOD("set_TacticalCamera", "Camera3D"), &TG_Cameras::set_TacticalCamera);

	ClassDB::add_property("TG_Cameras", PropertyInfo(Variant::OBJECT, "Main Camera", PROPERTY_HINT_NODE_TYPE, "Camera3D", PROPERTY_USAGE_DEFAULT), "set_MainCamera", "get_MainCamera");
	ClassDB::add_property("TG_Cameras", PropertyInfo(Variant::OBJECT, "Tactical Camera", PROPERTY_HINT_NODE_TYPE, "Camera3D", PROPERTY_USAGE_DEFAULT), "set_TacticalCamera", "get_TacticalCamera");
}

//											Przygotowanie komponentu Cameras (17.03.2025)
/*=============================================================================================================================

=============================================================================================================================*/
void TG_Cameras::_ready()
{

	pInput = Input::get_singleton();
	
// Check cameras setup
	if (prop_MainCamera == nullptr)
	{
		UtilityFunctions::printerr("Cameras: MainCamera is not set");
	}
	if (prop_TacticalCamera == nullptr)
	{
		UtilityFunctions::printerr("Cameras: TacticalCamera is not set");
	}
// Set current camera
	pCurrentCamera = prop_MainCamera;
	pCurrentCamera->set_current(true);
	cameraMode = enCameraMode::cmMain;
}

//											Obs³uga komponentu Cameras (17.03.2025)
/*=============================================================================================================================

=============================================================================================================================*/
void TG_Cameras::_process(double delta)
{
	if (Engine::get_singleton()->is_editor_hint()) return; // Early return if we are in editor

	// Switch cameras
	if (pInput->is_action_just_pressed("switch_to_main_cam"))
	{
		if (cameraMode == enCameraMode::cmTactical)
		{
			pCurrentCamera->set_current(false);
			pCurrentCamera = prop_MainCamera;
			pCurrentCamera->set_current(true);
			cameraMode = enCameraMode::cmMain;
		}
	}
	if (pInput->is_action_just_pressed("switch_to_tactical_cam"))
	{
		if (cameraMode == enCameraMode::cmMain)
		{
			pCurrentCamera->set_current(false);
			pCurrentCamera = prop_TacticalCamera;
			pCurrentCamera->set_current(true);
			cameraMode = enCameraMode::cmTactical;
		}
	}

	
	// Move the camera forward and backward
	float camDir = pInput->get_action_strength("free_cam_backward") - pInput->get_action_strength("free_cam_forward");
	if (camDir)
	{
		float velocity = 50.0f * camDir * delta;
		if (pInput->get_action_strength("free_cam_speed"))
			velocity = 100.0f * camDir * delta;
		pCurrentCamera->translate(Vector3{ 0.0f, 0.0f, velocity }); //Translate is actually implemented like this: position = position + offset
	}
	// Move the camera up and down
	float camUpDown = pInput->get_action_strength("free_cam_up") - pInput->get_action_strength("free_cam_down");
	if (camUpDown)
	{
		float velocity = 50.0f * camUpDown * delta;
		if (pInput->get_action_strength("free_cam_speed"))
			velocity = 100.0f * camUpDown * delta;
		pCurrentCamera->translate(Vector3{ 0.0f, velocity, 0.0f }); //Translate is actually implemented like this: position = position + offset
	}

	//UtilityFunctions::print(pCurrentCamera->get_position().y);
}

void TG_Cameras::_input(const Ref<InputEvent> event)
{
}

//											Setter dla MainCamera (17.03.2025)
/*=============================================================================================================================

=============================================================================================================================*/
void TG_Cameras::set_MainCamera(Camera3D* ipCamera3D)
{
	Camera3D* camera = Object::cast_to<Camera3D>(ipCamera3D);
	if (camera)
	{
		String name = ipCamera3D->get_name();
		UtilityFunctions::print(name);
		prop_MainCamera = camera;
	}
}

//											Setter dla TacticalCamera (17.03.2025)
/*=============================================================================================================================

=============================================================================================================================*/
void TG_Cameras::set_TacticalCamera(Camera3D* ipCamera3D)
{
	Camera3D* camera = Object::cast_to<Camera3D>(ipCamera3D);
	if (camera)
	{
		String name = ipCamera3D->get_name();
		UtilityFunctions::print(name);
		prop_TacticalCamera = camera;
	}
}
