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

//										Klasa obs³uguj¹ca statek handlowy (24.05.2025)
/*=============================================================================================================================
- klasa Godota obs³uguj¹ca pojedynczy statek handlowy
- pod klasê podpiêty jest model 3D okrêtu jako RigidBody
=============================================================================================================================*/
class TG_MerchantShipX : public RigidBody3D
{
	GDCLASS(TG_MerchantShipX, RigidBody3D)

private:
    
    Transform3D initial_local_transform; // Wzglêdna transformacja do ConvoyBox
    bool initialized_offset;

    // Parametry do strojenia pod¹¿ania
    double linear_follow_strength;
    double angular_follow_strength;
    double linear_damping_factor;  // Wspó³czynnik t³umienia liniowego dla pod¹¿ania
    double angular_damping_factor; // Wspó³czynnik t³umienia k¹towego dla pod¹¿ania
    double max_linear_correction_force;
    double max_angular_correction_torque;

    // Parametry dla symulacji fal (przyk³adowe)
    double time_accumulator;

    // Pola
    short ID{ 0 };							// ID okrêtu eskortowego
    String name;							// Nazwa okrêtu eskortowego
    Node3D* pShipNode{ nullptr };			// WskaŸnik do modelu 3D okrêtu eskortowego
    Node3D* pConvoyBox{};                   // WskaŸnik do wêz³a ConvoyBox
    Vector3 originPosition{};				// Pozycja relatywna w stosunku do konwoju
    Vector3 position{};						// Pozycja rzeczywista okrêtu eskortowego
    //TG_Escorts* pEscortsNode{ nullptr };	// WskaŸnik do wêz³a TG_Escorts


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
    
    // Akcesory do parametrów (opcjonalne, jeœli maj¹ byæ ustawiane z edytora)
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