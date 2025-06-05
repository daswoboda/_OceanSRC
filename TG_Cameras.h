#pragma once
#ifndef TG_Cameras_h
#define TG_Cameras_h

#include <node3D.hpp>		// for Node3D
#include <camera3D.hpp>		// for Camera3D
#include <input.hpp>		// for Input
#include <input_event.hpp>	// for InputEvent

using namespace godot;

enum enCameraMode
{
	cmNone = 0,
	cmMain,
	cmTactical
};


//									Klasa odpowiedzialna za kamery (17.03.2025)
/*=============================================================================================================================
- klasa Godota obs³uguj¹ca kamerê

=============================================================================================================================*/
class TG_Cameras : public Node3D
{
    GDCLASS(TG_Cameras, Node3D)

protected:
    // Wi¹zanie metod
    static void _bind_methods();

public:
	// Konstruktor i destruktor
	TG_Cameras() {}
	~TG_Cameras() {}
	// Metody Godota
	void _ready() override;
	void _process(double delta) override;
	void _input(const Ref<InputEvent> event);
	// Settery i gettery
	/*void SetRotationSpeed(float iSpeed) { prop_rotationSpeed = iSpeed; }
	float GetRotationSpeed()						const { return prop_rotationSpeed; }
	void SetMouseSensitivity(float iSensitivity) { prop_mouseSensitivity = iSensitivity; }
	float GetMouseSensitivity()						const { return prop_mouseSensitivity; }
	void SetSlowMovementSpeed(float iSpeed) { prop_slowMovementSpeed = iSpeed; }
	float GetSlowMovementSpeed()					const { return prop_slowMovementSpeed; }
	void SetFastMovementSpeed(float iSpeed) { prop_fastMovementSpeed = iSpeed; }
	float GetFastMovementSpeed()					const { return prop_fastMovementSpeed; }*/
	void set_MainCamera(Camera3D* ipCamera3D);
	void set_TacticalCamera(Camera3D* ipCamera3D);
	Camera3D* get_MainCamera() const				{ return prop_MainCamera; }
	Camera3D* get_TacticalCamera() const			{ return prop_TacticalCamera; }

private:
	// Pola exportowane
	Camera3D* prop_MainCamera{ nullptr };               // Kamera g³ówna
	Camera3D* prop_TacticalCamera{ nullptr };           // Kamera taktyczna
	Input* pInput{ nullptr };
	/*float prop_rotationSpeed{ 1.1f };
	float prop_mouseSensitivity{ 0.00001f };
	float prop_slowMovementSpeed{ 50.0f };
	float prop_fastMovementSpeed{ 100.0f };*/
	// Pola
	Camera3D* pCurrentCamera{ nullptr };				// Aktualnie u¿ywana kamera
	enCameraMode cameraMode{ cmNone };					// Tryb kamery
};

#endif