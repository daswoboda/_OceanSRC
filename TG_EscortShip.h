#pragma once
#ifndef TG_ESCORTSHIP_H
#define TG_ESCORTSHIP_H

#include <node3D.hpp>
#include <rigid_body3d.hpp>									// for RigidBody
#include <marker3d.hpp>										// for Marker3D
#include <vector>

#include "TG_HUD.h"
#include "TG_TacticalTier.h"
#include "TG_Escorts.h"

class TG_TacticalTier;
class TG_HUD;
class TG_Escorts;


using namespace godot;
using namespace std;

//										Klasa obs�uguj�ca Okr�t eskortowy (22.03.2025)
/*=============================================================================================================================
- klasa Godota obs�uguj�ca pojedynczy okr�t eskortowy
- pod klas� podpi�ty jest model 3D okr�tu jako RigidBody
=============================================================================================================================*/
class TG_EscortShip : public Node3D
{
	GDCLASS(TG_EscortShip, Node3D)

protected:
	// Wi�zanie metod
	static void _bind_methods();

public:
	// Konstruktor i destruktor
	TG_EscortShip();
	~TG_EscortShip();
	// Metody Godota
	void _ready() override;
	void _process(double delta) override;
	// Settery i gettery
	void SetID(short iID)							{ ID = iID; }
	short GetID()									const { return ID; }
	void SetOriginPosition(Vector3 origin)			{ originPosition = origin; }
	Vector3 GetOriginPosition()						const { return originPosition; }
	void SetPosition(Vector3 pos)					{ position = pos; set_position(pos); }
	Vector3 GetPosition()							const { return position; }
	void SetName(const String& shipName)			{ name = shipName; }
	String GetName()								const { return name; }
	void SetShipNode(Node3D* pNode)					{ pShipNode = pNode; }
	Node3D* GetShipNode()							const { return pShipNode; }
	void SetPatrolZone(const PatrolZone& patrol)	{ patrolZone = patrol; }
	PatrolZone GetPatrolZone()						const { return patrolZone; }
	// Metody
	void ExternalizePatrolZone();					// Wizualizacja obszaru patrolowego
	
private:
	// Pola
	short ID{ 0 };							// ID okr�tu eskortowego
	String name;							// Nazwa okr�tu eskortowego
	Node3D* pShipNode{ nullptr };			// Wska�nik do modelu 3D okr�tu eskortowego
	Vector3 originPosition{};				// Pozycja relatywna w stosunku do konwoju
	Vector3 position{};						// Pozycja rzeczywista okr�tu eskortowego
	PatrolZone patrolZone{};				// Obszar patrolowy
	TG_Escorts* pEscortsNode{ nullptr };	// Wska�nik do w�z�a TG_Escorts
};

#endif