#include "TG_EscortShip.h"

#include <cylinder_mesh.hpp>			// for CylinderMesh
#include <standard_material3d.hpp>		// for StandardMaterial3D
#include <mesh_instance3d.hpp>			// for MeshInstance3D
#include <scene_tree.hpp>					// for get_tree()


void TG_EscortShip::_bind_methods()
{
}

TG_EscortShip::TG_EscortShip()
{
	
}

//									Destruktor (20.05.2025)
/*=============================================================================================================================

=============================================================================================================================*/
TG_EscortShip::~TG_EscortShip()
{
	// Usuniêcie wêz³a graficznego, jeœli istnieje
	if (pShipNode)
	{
		pShipNode->queue_free();
		pShipNode = nullptr;
	}
}

void TG_EscortShip::_ready()
{
	
}

void TG_EscortShip::_process(double delta)
{
}

//									Wizualizacja obszaru patrolowego (21.05.2025)
/*=============================================================================================================================

=============================================================================================================================*/
void TG_EscortShip::ExternalizePatrolZone()
{
	// Tworzenie wizualizacji strefy patrolowej za pomoc¹ CylinderMesh
	Ref<CylinderMesh> patrolZoneMesh = memnew(CylinderMesh);
	patrolZoneMesh->set_height(2.0f);
	// Sprawdzenie, czy patrolZone.zone zawiera Circle
	if (std::holds_alternative<Circle>(patrolZone.zone))
	{
		const Circle& circle = std::get<Circle>(patrolZone.zone);
		patrolZoneMesh->set_top_radius(circle.radius); // U¿ycie promienia z Circle
		patrolZoneMesh->set_bottom_radius(circle.radius);
	}
	else 
	{
		// Obs³uga przypadku, gdy patrolZone.zone nie jest Circle
		ERR_PRINT("PatrolZone is not a Circle. Cannot set top radius.");
		return;
	}
	patrolZoneMesh->set_radial_segments(32);

	// Tworzenie materia³u dla strefy patrolowej
	Ref<StandardMaterial3D> material = memnew(StandardMaterial3D);
	material->set_transparency(StandardMaterial3D::TRANSPARENCY_ALPHA);
	material->set_albedo(Color(0.0f, 0.5f, 1.0f, 0.3f)); // Niebieski, pó³przezroczysty
	material->set_shading_mode(StandardMaterial3D::SHADING_MODE_UNSHADED); // Ustawienie trybu na "unshaded"

	// Tworzenie instancji meshu
	MeshInstance3D* patrolZoneNode = memnew(MeshInstance3D);
	patrolZoneNode->set_mesh(patrolZoneMesh);
	patrolZoneNode->set_material_override(material);
	patrolZoneNode->set_name("PatrolZone");


	patrolZoneNode->set_position(Vector3(0, 0, 0)); // Ustawienie pozycji na (0, 0, 0)
	patrolZoneNode->set_rotation_degrees(Vector3(0, 0, 0)); // Ustawienie rotacji na (0, 0, 0)

	// Dodanie do drzewa sceny
	add_child(patrolZoneNode);
	patrolZoneNode->set_owner(get_tree()->get_edited_scene_root());
}
