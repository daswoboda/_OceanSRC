#pragma once
#ifndef g_FreeCamera3D_h
#define g_FreeCamera3D_h

#include <camera3D.hpp>
#include <input.hpp>
#include <input_event.hpp>

using namespace godot;

//										Klasa kamery (11.01.2025)
/*=============================================================================================================================
- klasa Godota obs³uguj¹ca kamerê

(updated 08.02.2025)
	- zmienne szybkoœci przekszta³cone na properties Godota
	- dodny szybki i wolny ruch kamery
=============================================================================================================================*/
class g_FreeCamera3D : public Camera3D
{
    GDCLASS(g_FreeCamera3D, Camera3D)

protected:
    // Wi¹zanie metod
    static void _bind_methods();

public:
    // Konstruktor i destruktor
    g_FreeCamera3D();
    ~g_FreeCamera3D();
    // Metody Godota
    void _ready() override;
    void _process(double delta) override;
    void _input(const Ref<InputEvent> event);
    // Settery i gettery
	void SetRotationSpeed(float iSpeed)             { prop_rotationSpeed = iSpeed; }
	float GetRotationSpeed()						const { return prop_rotationSpeed; }
	void SetMouseSensitivity(float iSensitivity)    { prop_mouseSensitivity = iSensitivity; }
	float GetMouseSensitivity()						const { return prop_mouseSensitivity; }
	void SetSlowMovementSpeed(float iSpeed)         { prop_slowMovementSpeed = iSpeed; }
	float GetSlowMovementSpeed()					const { return prop_slowMovementSpeed; }
	void SetFastMovementSpeed(float iSpeed)			{ prop_fastMovementSpeed = iSpeed; }
	float GetFastMovementSpeed()					const { return prop_fastMovementSpeed; }
	
private:
    Input* pInput{ nullptr };
	float prop_rotationSpeed{ 1.1f };
	float prop_mouseSensitivity{ 0.00001f };
	float prop_slowMovementSpeed{ 50.0f };
	float prop_fastMovementSpeed{ 100.0f };
};



#endif