#include "TG_PlayerUboot.h"


#include <engine.hpp>						// for Engine::get_singleton()->is_editor_hint()
#include <scene_tree.hpp>					// for get_tree()
#include <window.hpp>						// for get_root()
//#include <node_path.hpp>					// for NodePath
//#include <node.hpp>							// for get_node_or_null()


void TG_PlayerUboot::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("GetAcceleration"), &TG_PlayerUboot::GetAcceleration);
	ClassDB::bind_method(D_METHOD("SetAcceleration", "p_Acceleration"), &TG_PlayerUboot::SetAcceleration);
	ClassDB::bind_method(D_METHOD("GetDeacceleration"), &TG_PlayerUboot::GetDeacceleration);
	ClassDB::bind_method(D_METHOD("SetDeacceleration", "p_Deacceleration"), &TG_PlayerUboot::SetDeacceleration);
	ClassDB::bind_method(D_METHOD("GetSurfaceMaxVelocity"), &TG_PlayerUboot::GetSurfaceMaxVelocity);
	ClassDB::bind_method(D_METHOD("SetSurfaceMaxVelocity", "p_MaxVelocity"), &TG_PlayerUboot::SetSurfaceMaxVelocity);
	ClassDB::bind_method(D_METHOD("GetSubmergedMaxVelocity"), &TG_PlayerUboot::GetSubmergedMaxVelocity);
	ClassDB::bind_method(D_METHOD("SetSubmergedMaxVelocity", "p_MaxVelocity"), &TG_PlayerUboot::SetSubmergedMaxVelocity);
	//ClassDB::bind_method(D_METHOD("_on_tg_hud_speed_telegraph_button_pressed_signal"), &T_PlayerUboot::_on_tg_hud_speed_telegraph_button_pressed_signal);
	
	ClassDB::add_property("TG_PlayerUboot", PropertyInfo(Variant::FLOAT, "Acceleration"), "SetAcceleration", "GetAcceleration");
	ClassDB::add_property("TG_PlayerUboot", PropertyInfo(Variant::FLOAT, "Deacceleration"), "SetDeacceleration", "GetDeacceleration");
	ClassDB::add_property("TG_PlayerUboot", PropertyInfo(Variant::FLOAT, "SurfaceMaxVelocity"), "SetSurfaceMaxVelocity", "GetSurfaceMaxVelocity");
	ClassDB::add_property("TG_PlayerUboot", PropertyInfo(Variant::FLOAT, "SubmergedMaxVelocity"), "SetSubmergedMaxVelocity", "GetSubmergedMaxVelocity");
}

//										Konstruktor U-Boota Gracza (11.01.2025)
/*=============================================================================================================================

=============================================================================================================================*/
TG_PlayerUboot::TG_PlayerUboot()
{
	if (!pSpeedTelegraph)
		pSpeedTelegraph = new T_SpeedTelegraph();
}

//												Destruktor (11.01.2025)
/*=============================================================================================================================

=============================================================================================================================*/
TG_PlayerUboot::~TG_PlayerUboot()
{
	delete pSpeedTelegraph;
	pSpeedTelegraph = nullptr;
}

//											Przygotowanie U-Boota do pracy (21.01.2025)
/*=============================================================================================================================
(updated 26.01.2025)
	- dodane przypisanie wskaŸnika na T_TacticalTier
	- dodane przypisanie wskaŸników na przyciski prêdkoœci
=============================================================================================================================*/
void TG_PlayerUboot::_ready()
{
	if (Engine::get_singleton()->is_editor_hint()) return;

// Przypisanie wskaŸnika na child_node - Sub
	pSub = Object::cast_to<RigidBody3D>(get_child(0));
	if (!pSub)
	{
		UtilityFunctions::printerr("TG_PlayerUboot: not child node with sub");
	}
	pRudder = Object::cast_to<Marker3D>(pSub->find_child("Propeller"));
	if (!pRudder)
	{
		UtilityFunctions::printerr("TG_PlayerUboot: not child node with rudder");
	}
// Przypisanie wskaŸnika na HUD, który jest dzieckiem g³ównego wêz³a
	auto pRoot = get_tree()->get_root();				// get_tree() zwaraca typ SceneTree, get_root() zwraca typ Window
	auto pNode = pRoot->get_child(0);					// get_child(0) zwraca typ Node g³ównego wêz³a
	//auto pHUD_node = pNode->get_node_or_null("TG_HUD");
	auto pHUD_node = get_node_or_null("../../TG_HUD");
	if (pHUD_node)
	{
		pHUD = Object::cast_to<TG_HUD>(pHUD_node);
		pSpeedTelegraph->SetHUD(pHUD);
		pSpeedTelegraph->SetCurrentSpeedButton(pHUD->GetCurrentSpeedButton());
	}
	else
	{
		UtilityFunctions::printerr("TG_PlayerUboot: not child node with HUD");
	}

// Przypisanie wskaŸnika na T_TacticalTier, który jest dzieckiem g³ównego wêz³a
	//auto pTT_node = pNode->get_node_or_null("TG_TacticalTier");
	auto pTT_node = get_node_or_null("../../");
	if (pTT_node)
	{
		pTacticalTier = Object::cast_to<TG_TacticalTier>(pTT_node);
	}
	else
	{
		UtilityFunctions::printerr("TG_PlayerUboot: not TG_TacticalTier");
	}

	UtilityFunctions::print("PlayerUboot ready");
}

//											Obs³uga Okrêtu Gracza (07.02.2025)
/*=============================================================================================================================

=============================================================================================================================*/
void TG_PlayerUboot::_process(double delta)
{
	if (Engine::get_singleton()->is_editor_hint()) return;

	ProcessSpeedTelegraph(delta);
	Move(delta);
	Turn(delta);
	Dive(delta);
	
	
	

	
}

//									Update speed telegraph and get desired speed (10.03.2025)
/*=============================================================================================================================
- wywo³ywana w T_PlayerUboot::_process
=============================================================================================================================*/
void TG_PlayerUboot::ProcessSpeedTelegraph(const double iDelta)
{
	// Update speed telegraph and get desired speed
	pSpeedTelegraph->Update(iDelta, isSurface);
	if (pSpeedTelegraph->GetTelegraphState() == tsReadyToChange)
	{
		pSpeedTelegraph->ResetTelegraphState();
		pSpeedTelegraph->SyncProperTelegraph();
		if ((int)pSpeedTelegraph->GetProperTelegraph() < 20)
		{
			float maxVelocity = 0.0f;
			if ((int)pSpeedTelegraph->GetProperTelegraph() < 10)
			{
				maxVelocity = prop_SurfaceMaxVelocity;
			}
			else
			{
				maxVelocity = prop_SubmergedMaxVelocity;
			}
			enTelegraph properTelegraph = pSpeedTelegraph->GetProperTelegraph();
			enTelegraph operatingTelegraph = pSpeedTelegraph->GetOperatingTelegraph();
			desiredSpeed = vThrottle.at(static_cast<int>(pSpeedTelegraph->GetProperTelegraph())) / 100.0f * maxVelocity;
			desiredSpeed = desiredSpeed;
		}
	}
}

//											Przemieszczenie Okrêtu Gracza (07.02.2025)
/*=============================================================================================================================
(updated 10.03.2025)
	- przeniesiono tutaj obs³ugê zmiany prêdkoœci
	- rezygnacja ze wspó³pracy z GDScript
(updated 01.03.2025)
	- dostosowanie do wspó³pracy z GDScript
=============================================================================================================================*/
void TG_PlayerUboot::Move(const double iDelta)
{
	if (!pSub)
	{
		UtilityFunctions::printerr("TG_PlayerUboot: not child node with sub");
		return;
	}

// Acceleration and deceleration
	if (actualSpeed < desiredSpeed)
	{
		actualSpeed += prop_Acceleration * iDelta;
		if (actualSpeed > desiredSpeed)
			actualSpeed = desiredSpeed;

	}
	else if (actualSpeed > desiredSpeed)
	{
		actualSpeed -= prop_Deacceleration * iDelta;
		if (actualSpeed < desiredSpeed)
			actualSpeed = desiredSpeed;

	}

	if (actualSpeed == 0.0f)
		return;
// Ustalenie kierunku ruchu
	float direction = 0.0f;
	if (actualSpeed > 0.0f)
		direction = 1.0f;
	else
		direction = -1.0f;

// Przekszta³cenie prêdkoœci na si³ê
	auto globalTransform = pSub->get_global_transform();
	auto mass = pSub->get_mass();
	auto forceFactor = actualSpeed * 110.0f * mass;							// actualSpeed, force factor and mass
	forceFactor *= iDelta;
	Vector3 force = globalTransform.basis.get_column(2) * forceFactor;
	pSub->apply_central_force(force);

// Aktualizacja prêdkoœci
	auto linvel = pSub->get_linear_velocity();
	Vector2 linvel2D(linvel.x, linvel.z);
	auto length = linvel2D.length();
	auto realSpeed = length;
	realSpeed = pTacticalTier->ConvertMetersToKnots(realSpeed);
	pHUD->UpdateSpeed_val(realSpeed*direction);
}

//											Skrêcanie Okrêtu Gracza (08.02.2025)
/*=============================================================================================================================
(updated 10.03.2025)
	- przeniesiono tutaj ca³¹ obs³ugê skrêcania
=============================================================================================================================*/
void TG_PlayerUboot::Turn(const double iDelta)
{
	if (!pSub)
	{
		UtilityFunctions::printerr("TG_PlayerUboot: not child node with sub");
		return;
	}
	if (actualSpeed == 0.0f)
		return;
	
	// Odczytanie nastawy steru kierunku z HUD
	pSub->call("set_turn_force", 0.0f);
	if (pHUD->GetRudderSetting() != rudder)
	{
		rudder = pHUD->GetRudderSetting();
		rudderSetting = rudder;
		rudderSetting *= 0.5f;
	}
	if (rudder != 0.0f && actualSpeed != 0.0f)
	{
		float force = rudderSetting;
		force *= prop_TurnForce;
		force *= iDelta;
		force *= -1.0f;
		pSub->call("set_turn_force", force);			// przekazaæ si³y skrêtu do skryptu GD
		//Turn(Vector3{ 0.0f, -rudderSetting, 0.0f }, delta);
		pHUD->UpdateCourse_val(pTacticalTier->ConvertAngleTo360(pSub->get_rotation_degrees().y));
	}
}

//										Zmiana g³êbokoœci Okrêtu Gracza (09.02.2025)
/*=============================================================================================================================
(updated 16.03.2025)
	- dodano sprawdzwnie osi¹gniêcia docelowej g³êbokoœci
(updated 10.03.2025)
	- przeniesiono tutaj ca³¹ obs³ugê zmiany g³êbokoœci
=============================================================================================================================*/
void TG_PlayerUboot::Dive(const double iDelta)
{
	pSub->call("set_falling_force", 0.0f);
	if (!pSub) return;

	if (isSurface && pHUD->GetDIVEButtonPressed())
	{
		DiveFromSurface(iDelta);
	}

	if (!isDivingPossible) return;
	
// Change depth
	if (pHUD->GetDepthRudderSetting() < 0)
	{
		pSub->set_mass(100000.0f);
		pSub->call("set_falling_force", -prop_DiveForce - ((pHUD->GetDiveSpeedRudderVal() - 1) * 0.45f));
		isRequiredDepthStabilization = true;
	}
	else if (pHUD->GetDepthRudderSetting() > 0)
	{
		pSub->set_mass(10000.0f);
		pSub->call("set_falling_force", prop_DiveForce + ((pHUD->GetDiveSpeedRudderVal() - 1) * 0.45f));
		isRequiredDepthStabilization = true;
	}
	else if (pHUD->GetDepthRudderSetting() == 0 && !isSurface && isRequiredDepthStabilization)
	{
		StabilizeDepth(pSub->get_global_position().y);

	}

// Update depth
	auto depth = pSub->get_global_position().y;
	actualDepth = depth;
	pHUD->UpdateDepth_val(depth);

// Coming to surface
	if (depth < 3.0f && !isSurface)
	{
		if (pHUD->GetDepthRudderSetting() > 0)
		{
			ComingToSurface(iDelta);
		}
	}

// Check if the desired depth has been reached
	if (desiredDepth == actualDepth || (actualDepth < desiredDepth + 0.2f && actualDepth > desiredDepth - 0.2f))
	{
		isRequiredDepthStabilization = true;
		pHUD->ResetDIVEButton();
		pHUD->SetDepthRudder(0);
		desiredDepth = -1.0f;
	}
	
// Check if sub is on surface
	if (depth > 3.0f)
	{
		isSurface = false;
	}
	else
	{
		isSurface = true;
	}
	

// Aktualizacja prêdkoœci opadania/wznoszenia
	/*auto linvel = pSub->get_linear_velocity();
	pHUD->UpdateSpeed_val(linvel.y);*/

	TestDepth(pSub->get_global_position().y, iDelta);
}

//									Zanurzanie z powierzchni (10.03.2025)
/*=============================================================================================================================

=============================================================================================================================*/
void TG_PlayerUboot::DiveFromSurface(const double iDelta)
{
	if (!pSub) return;
	
	// Start diving from surface
	if (isSurface && !isDivingFromSurface)
	{
		isDivingFromSurface = true;
		timeToDive = timeToDiveDelay;
		pHUD->SetDepthRudder(-1);
		UtilityFunctions::print("Dive! Dive! Dive!");
	}

	// Odliczanie czasu do zanurzenia
	if (isDivingFromSurface)
	{
		timeToDive -= iDelta;
		if (timeToDive <= 0.0f)
		{
			isDivingFromSurface = false;
			isDivingPossible = true;
			isRequiredDepthStabilization = true;
			pHUD->ResetDIVEButton();
			pHUD->DisableDIVEButton(true);
			UtilityFunctions::print("Dive delay countdown finished");
			pSpeedTelegraph->SwitchToElectricEngine(); 
		}
	}
}

//								Stabilizacja g³êbokoœci zanurzenia (13.03.2025)
/*=============================================================================================================================
podaj wzór na obliczenie masy potrzebnej do utrzymania danej g³êbokoœci je¿eli: 500000 kg = 27,45 m, 300000 kg = 19,15 m, 100000 kg = 8,45 m.
=============================================================================================================================*/
void TG_PlayerUboot::StabilizeDepth(const float iDepth)
{
	if (!pSub) return;

	//auto mass = sqrt(iDepth) * 285.6f;
	auto mass = sqrt(iDepth) * 295.6f;
	mass += iDepth * 10798.8f;
	mass += 11642.3f;
	pSub->set_mass(mass);
	
	/*auto massAddOn = iDepth * 0.1f;
	massAddOn *= massForOneMeter * 0.3f;
	pSub->set_mass(massAddOn + massForOneMeter * iDepth);*/
	UtilityFunctions::print("Depth stabilized at ", iDepth);
	//UtilityFunctions::print("Mass addon: ", massAddOn);
	UtilityFunctions::print("Total mass: ", pSub->get_mass());
	isRequiredDepthStabilization = false;
}

//									Wynurzenie na powierzchniê (14.03.2025)
/*=============================================================================================================================

=============================================================================================================================*/
void TG_PlayerUboot::ComingToSurface(const double iDelta)
{
	if (!pSub) return;
	
	pSub->set_mass(3000.0f);
	isDivingPossible = false;
	isRequiredDepthStabilization = false;
	pHUD->ResetDIVEButton();
	pHUD->DisableDIVEButton(false);
	pHUD->DisableSurfaceButton(true);
	pHUD->SetDepthRudder(0);
	pHUD->DisableDepthRudder(true);
	pHUD->DisableCrashDiveButton(true);
	pSpeedTelegraph->SwitchToDieselEngine();
	UtilityFunctions::print("Coming to surface, Sir");

}

void TG_PlayerUboot::TestDepth(const float iDepth, const double iDelta)
{
	if (!pSub) return;

	timer += iDelta;
	if (iDepth > testDepth)
	{
		testDepth = iDepth;
		pHUD->UpdateMaxDepth_val(testDepth);
		timer = 0.0f;
	}
	pHUD->UpdateTimePass_val(timer);
}
