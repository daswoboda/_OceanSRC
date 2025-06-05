#pragma once
#ifndef TG_MERCHANTSHIP_H
#define TG_MERCHANTSHIP_H

#include <node3D.hpp>
#include <rigid_body3d.hpp>									// for RigidBody
#include <vector>

#include "TG_TacticalTier.h"
#include "TG_Convoy.h"

class TG_TacticalTier;
class TG_Convoy;


using namespace godot;
using namespace std;

//										Klasa obs�uguj�ca statek handlowy (24.05.2025)
/*=============================================================================================================================
- klasa Godota obs�uguj�ca pojedynczy statek handlowy
- pod klas� podpi�ty jest model 3D okr�tu jako RigidBody
=============================================================================================================================*/
class TG_MerchantShipX : public RigidBody3D
{
	GDCLASS(TG_MerchantShipX, RigidBody3D)

private:
    
    Transform3D initial_local_transform; // Wzgl�dna transformacja do ConvoyBox
    bool initialized_offset;

    // Parametry do strojenia pod��ania
    double linear_follow_strength;
    double angular_follow_strength;
    double linear_damping_factor;  // Wsp�czynnik t�umienia liniowego dla pod��ania
    double angular_damping_factor; // Wsp�czynnik t�umienia k�towego dla pod��ania
    double max_linear_correction_force;
    double max_angular_correction_torque;

    // Parametry dla symulacji fal (przyk�adowe)
    double time_accumulator;

    // Pola
    short ID{ 0 };							// ID okr�tu eskortowego
    String name;							// Nazwa okr�tu eskortowego
    Node3D* pShipNode{ nullptr };			// Wska�nik do modelu 3D okr�tu eskortowego
    Node3D* pConvoyBox{};                   // Wska�nik do w�z�a ConvoyBox
    Vector3 originPosition{};				// Pozycja relatywna w stosunku do konwoju
    Vector3 position{};						// Pozycja rzeczywista okr�tu eskortowego
    //TG_Escorts* pEscortsNode{ nullptr };	// Wska�nik do w�z�a TG_Escorts


protected:
    static void _bind_methods();

public:
    TG_MerchantShipX();
    ~TG_MerchantShipX();

    void _ready() override;
    void _integrate_forces(PhysicsDirectBodyState3D* state) override;

    // Settery i gettery
    void SetID(short iID) { ID = iID; }
    short GetID()									const { return ID; }
    void SetOriginPosition(Vector3 origin) { originPosition = origin; }
    Vector3 GetOriginPosition()						const { return originPosition; }
    void SetPosition(Vector3 pos) { position = pos; set_position(pos); }
    Vector3 GetPosition()							const { return position; }
    void SetName(const String& shipName) { name = shipName; }
    String GetName()								const { return name; }
    void SetShipNode(Node3D* pNode) { pShipNode = pNode; }
    Node3D* GetShipNode()							const { return pShipNode; }
    
    // Akcesory do parametr�w (opcjonalne, je�li maj� by� ustawiane z edytora)
    void set_linear_follow_strength(double p_strength);
    double get_linear_follow_strength() const;
    void set_angular_follow_strength(double p_strength);
    double get_angular_follow_strength() const;
    void set_linear_damping_factor(double p_factor);
    double get_linear_damping_factor() const;
    void set_angular_damping_factor(double p_factor);
    double get_angular_damping_factor() const;
    void set_max_linear_correction_force(double p_force);
    double get_max_linear_correction_force() const;
    void set_max_angular_correction_torque(double p_torque);
    double get_max_angular_correction_torque() const;

};


#endif