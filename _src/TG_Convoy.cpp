#include "TG_Convoy.h"
#include <godot_cpp/core/class_db.hpp>

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <scene_tree.hpp>					// for get_tree()
#include <window.hpp>						// for get_root()
#include <packed_scene.hpp>					// for PackedScene
#include <resource.hpp>						// for Resource
#include <resource_loader.hpp>				// for ResourceLoader	
using namespace godot;

//										Wiązanie metod i właściwości (xx.xx.2025)
/*=============================================================================================================================

=============================================================================================================================*/
void TG_Convoy::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_ConvoyName"), &TG_Convoy::get_ConvoyName);
	ClassDB::bind_method(D_METHOD("set_ConvoyName", "p_convoyName"), &TG_Convoy::set_ConvoyName);
	ClassDB::bind_method(D_METHOD("get_InitNoMerchants"), &TG_Convoy::get_InitNoMerchants);
	ClassDB::bind_method(D_METHOD("set_InitNoMerchants", "p_initNoMerchants"), &TG_Convoy::set_InitNoMerchants);
	ClassDB::bind_method(D_METHOD("get_InitNoEscorts"), &TG_Convoy::get_InitNoEscorts);
	ClassDB::bind_method(D_METHOD("set_InitNoEscorts", "p_initNoEscorts"), &TG_Convoy::set_InitNoEscorts);
	ClassDB::bind_method(D_METHOD("get_Course"), &TG_Convoy::get_Course);
	ClassDB::bind_method(D_METHOD("set_Course", "p_course"), &TG_Convoy::set_Course);
	ClassDB::bind_method(D_METHOD("get_Speed"), &TG_Convoy::get_Speed);
	ClassDB::bind_method(D_METHOD("set_Speed", "p_speed"), &TG_Convoy::set_Speed);
	ClassDB::bind_method(D_METHOD("get_FormationSpacing"), &TG_Convoy::get_FormationSpacing);
	ClassDB::bind_method(D_METHOD("set_FormationSpacing", "p_formationSpacing"), &TG_Convoy::set_FormationSpacing);
	
	ClassDB::add_property("TG_Convoy", PropertyInfo(Variant::STRING, "convoyName"), "set_ConvoyName", "get_ConvoyName");
	ClassDB::add_property("TG_Convoy", PropertyInfo(Variant::INT, "initNoMerchants"), "set_InitNoMerchants", "get_InitNoMerchants");
	ClassDB::add_property("TG_Convoy", PropertyInfo(Variant::INT, "initNoEscorts"), "set_InitNoEscorts", "get_InitNoEscorts");
	ClassDB::add_property("TG_Convoy", PropertyInfo(Variant::FLOAT, "course"), "set_Course", "get_Course");
	ClassDB::add_property("TG_Convoy", PropertyInfo(Variant::FLOAT, "speed"), "set_Speed", "get_Speed");
	ClassDB::add_property("TG_Convoy", PropertyInfo(Variant::VECTOR2I, "formationSpacing"), "set_FormationSpacing", "get_FormationSpacing");
	
	ADD_SIGNAL(MethodInfo("position_changed", PropertyInfo(Variant::OBJECT, "node"), PropertyInfo(Variant::VECTOR2, "new_pos")));
}

//												Konstruktor (xx.xx.2025)
/*=============================================================================================================================

=============================================================================================================================*/
TG_Convoy::TG_Convoy()
{
	// Initialize any variables here.
	if (Engine::get_singleton()->is_editor_hint())
	{
		set_process_mode(Node::ProcessMode::PROCESS_MODE_DISABLED);
	}
	// time_passed = 0.0;
	// amplitude = 1.0;
}

//												Destruktor (xx.xx.2025)
/*=============================================================================================================================

=============================================================================================================================*/
TG_Convoy::~TG_Convoy()
{
	// Add your cleanup here.
}

//									Przygotowanie obiektu TG_Convoy (26.03.2025)
/*=============================================================================================================================
(updated 17.05.2025)
	- dodano wskaźnik na TG_Escorts
=============================================================================================================================*/
/// <summary>
/// Inicjalizuje wskaźniki do kluczowych węzłów potomnych i nadrzędnych po uruchomieniu węzła TG_Convoy.
/// </summary>
void TG_Convoy::_ready()
{
	/*if (Engine::get_singleton()->is_editor_hint()) return;
	
	bool visible = is_visible_in_tree();*/

// Przypisanie wskaźnika na ConvoyBox, który jest dzieckiem tego węzła
	pConvoyBox = Object::cast_to<CSGBox3D>(find_child("ConvoyBox"));
	if (!pConvoyBox)
	{
		UtilityFunctions::printerr("TG_Convoy: not child node ConvoyShape");
	}
	//pConvoyBox->set_size(Vector3(100, 1, 100));
	
// Przypisanie wskaźnika na HUD, który jest dzieckiem głównego węzła
	auto pHUD_node = get_node_or_null("../TG_HUD");
	if (pHUD_node)
	{
		pHUD = Object::cast_to<TG_HUD>(pHUD_node);
	}
	else
	{
		UtilityFunctions::printerr("TG_Convoy: not child node with HUD");
	}

// Przypisanie wskaźnika na T_TacticalTier, który jest dzieckiem głównego węzła
	auto pTT_node = get_node_or_null("../");
	if (pTT_node)
	{
		pTacticalTier = Object::cast_to<TG_TacticalTier>(pTT_node);
	}
	else
	{
		UtilityFunctions::printerr("TG_Convoy: not TG_TacticalTier");
	}
	
// Przypisanie wskaźnika na TG_Escort, który jest dzieckiem tego węzła
	auto pEscort_node = get_node_or_null("ConvoyBox/TG_Escorts");
	if (pEscort_node)
	{
		pEscorts = Object::cast_to<TG_Escorts>(pEscort_node);
	}
	else
	{
		UtilityFunctions::printerr("TG_Convoy: not TG_Escorts");
	}


	UtilityFunctions::print("TG_Convoy ready");
}

//											Przetwarzanie fizyki (24.05.2025)
/*=============================================================================================================================
- silnik wywoła automatycznie tę metodę w każdej klatce gry, jeśli jest aktywna
- Przetwarzanie fizyki odbywa się w stałym tempie, domyślnie 60 razy na sekundę. Jest to niezależne od faktycznej
	liczby klatek na sekundę w grze i zapewnia płynne działanie fizyki. 
=============================================================================================================================*/
//void TG_Convoy::_physics_process(double delta)
//{
//	if (Math::is_zero_approx(courseDirection.length_squared())) return; // Brak kierunku, nie ruszaj się
//	
//	Vector3 direction_normalized = courseDirection.normalized();
//	Vector3 frame_velocity = direction_normalized * prop_Speed;
//
//	Transform3D current_transform = get_global_transform();
//	current_transform.origin += frame_velocity * delta;
//
//	// Orientacja lidera w kierunku kursu
//	// Używamy Y-up jako wektora "w górę" dla look_at
//	Basis target_basis = Basis::looking_at(direction_normalized, Vector3(0, 1, 0));
//	current_transform.basis = current_transform.basis.slerp(target_basis, turnSpeedRadPerSec * delta);
//
//	set_global_transform(current_transform);
//}

void TG_Convoy::_physics_process(double delta)
{
	if (Math::is_zero_approx(courseDirection.length_squared()))
	{
		return; // Brak kierunku, nie ruszaj się
	}

	// Aktualizuj obrót konwoju
	UpdateConvoyRotation(delta);

	if (Math::is_zero_approx(courseDirection.length_squared())) {
		return; // Brak kierunku, nie ruszaj się
	}

	Vector3 direction_normalized = courseDirection.normalized();

	// Konwersja prędkości z węzłów na metry na sekundę
	double speed_mps = prop_Speed * KNOTS_TO_MPS;
	Vector3 frame_velocity = direction_normalized * speed_mps;

	Transform3D current_transform = get_global_transform();
	current_transform.origin += frame_velocity * delta;

	// Orientacja lidera w kierunku kursu
	Basis target_basis = Basis::looking_at(-direction_normalized, Vector3(0, 1, 0));
	current_transform.basis = current_transform.basis.slerp(target_basis, turnSpeedRadPerSec * delta);

	set_global_transform(current_transform);
	
	//Vector3 direction_normalized = courseDirection.normalized();

	//// Konwersja prędkości z węzłów na metry na sekundę
	//double speed_mps = prop_Speed * KNOTS_TO_MPS;
	//Vector3 frame_velocity = direction_normalized * speed_mps;

	//Transform3D current_transform = get_global_transform();
	//current_transform.origin += frame_velocity * delta;

	//// Orientacja lidera w kierunku kursu
	//// ZMIANA: Używamy -direction_normalized, ponieważ looking_at kieruje oś -Z węzła
	//// w stronę podanego kierunku. Aby oś +Z węzła była skierowana w direction_normalized,
	//// oś -Z musi być skierowana w -direction_normalized.
	//Basis target_basis = Basis::looking_at(-direction_normalized, Vector3(0, 1, 0));
	//current_transform.basis = current_transform.basis.slerp(target_basis, turnSpeedRadPerSec * delta);


	//set_global_transform(current_transform);
}

//										Ustawienie kursu konwoju (27.03.2025)
/*=============================================================================================================================
- ustawienie kursu konwoju
- ustawienie kąta obrotu ConvoyBoxa
=============================================================================================================================*/
void TG_Convoy::set_Course(const float i_course)
{
	/*prop_Course = i_course;
	if (pConvoyBox)
	{
		pConvoyBox->set_rotation(Vector3(0.0, prop_Course, 0.0));
	}*/

	// Użyj nowej funkcji do płynnego obrotu
	//RotateConvoyToCourse(i_course);

	SetCourseImmediate(i_course); // Ustawienie kursu natychmiastowo, bez animacji obrotu
}

float TG_Convoy::GetCurrentCourseDegrees() const
{
	float degrees = rad_to_deg(current_course);
	while (degrees < 0) degrees += 360.0f;
	while (degrees >= 360.0f) degrees -= 360.0f;
	return degrees;
}

float TG_Convoy::GetTargetCourseDegrees() const
{
	float degrees = rad_to_deg(target_course);
	while (degrees < 0) degrees += 360.0f;
	while (degrees >= 360.0f) degrees -= 360.0f;
	return degrees;
}

bool TG_Convoy::IsConvoyTurning() const
{
	return is_turning;
}

void TG_Convoy::SetCourseImmediate(float new_course_degrees)
{
	// Normalizacja kursu do zakresu [0, 360)
	while (new_course_degrees < 0) new_course_degrees += 360.0f;
	while (new_course_degrees >= 360.0f) new_course_degrees -= 360.0f;

	prop_Course = new_course_degrees;
	current_course = target_course = deg_to_rad(new_course_degrees);
	is_turning = false;

	if (pConvoyBox)
	{
		pConvoyBox->set_rotation(Vector3(0.0f, -current_course, 0.0f));
	}

	// Nawigacyjny system współrzędnych
	courseDirection = Vector3(
		Math::sin(current_course),    // X - Wschód/Zachód
		0.0f,                        // Y - Góra/Dół
		Math::cos(current_course)     // Z - Północ/Południe
	);
}

String TG_Convoy::GetTurnDirection() const
{
	if (!is_turning) return "NONE";

	float current_deg = GetCurrentCourseDegrees();
	float target_deg = GetTargetCourseDegrees();
	float diff = target_deg - current_deg;

	if (diff > 180.0f) diff -= 360.0f;
	else if (diff < -180.0f) diff += 360.0f;

	if (diff > 0) return "CLOCKWISE (STARBOARD)";
	else return "COUNTER-CLOCKWISE (PORT)";
}

void TG_Convoy::RotateConvoyToCourse(float new_course_degrees)
{
	// Normalizacja kursu do zakresu [0, 360)
	while (new_course_degrees < 0) new_course_degrees += 360.0f;
	while (new_course_degrees >= 360.0f) new_course_degrees -= 360.0f;

	// Konwersja stopni na radiany (dostosowane do kierunku wskazówek zegara)
	// 0° = Północ, 90° = Wschód, 180° = Południe, 270° = Zachód
	target_course = deg_to_rad(new_course_degrees);

	// Sprawdzenie czy potrzebny jest obrót
	float current_degrees = rad_to_deg(current_course);
	while (current_degrees < 0) current_degrees += 360.0f;
	while (current_degrees >= 360.0f) current_degrees -= 360.0f;

	// Oblicz różnicę kąta w kierunku wskazówek zegara
	float angle_diff = new_course_degrees - current_degrees;

	// Znajdź najkrótszą drogę obrotu (w kierunku lub przeciwnie do wskazówek zegara)
	if (angle_diff > 180.0f) {
		angle_diff -= 360.0f;  // Obróć w lewo (przeciwnie do wskazówek)
	}
	else if (angle_diff < -180.0f) {
		angle_diff += 360.0f;  // Obróć w prawo (zgodnie ze wskazówkami)
	}

	if (Math::abs(angle_diff) > 0.57f) // Tolerancja ~0.01 radiana w stopniach
	{
		is_turning = true;
		prop_Course = new_course_degrees; // Aktualizuj właściwość

		// Ustaw kierunek ruchu na podstawie nowego kursu (nawigacyjny system współrzędnych)
		// 0° = +Z (Północ), 90° = +X (Wschód)
		courseDirection = Vector3(
			Math::sin(target_course),    // X - Wschód/Zachód
			0.0f,                        // Y - Góra/Dół
			Math::cos(target_course)     // Z - Północ/Południe
		);
	}
}

void TG_Convoy::UpdateConvoyRotation(double delta)
{
	if (!is_turning) return;

	float current_degrees = rad_to_deg(current_course);
	float target_degrees = rad_to_deg(target_course);

	// Normalizacja do zakresu [0, 360)
	while (current_degrees < 0) current_degrees += 360.0f;
	while (current_degrees >= 360.0f) current_degrees -= 360.0f;
	while (target_degrees < 0) target_degrees += 360.0f;
	while (target_degrees >= 360.0f) target_degrees -= 360.0f;

	// Oblicz różnicę kąta
	float angle_diff = target_degrees - current_degrees;

	// Znajdź najkrótszą drogę obrotu
	if (angle_diff > 180.0f) {
		angle_diff -= 360.0f;
	}
	else if (angle_diff < -180.0f) {
		angle_diff += 360.0f;
	}

	if (Math::abs(angle_diff) < 0.57f) // Tolerancja ~1 stopień
	{
		// Osiągnięto cel
		current_course = target_course;
		is_turning = false;
		return;
	}

	// Oblicz krok obrotu w stopniach na sekundę
	float rotation_step_degrees = rad_to_deg(course_turn_speed) * delta;

	if (Math::abs(angle_diff) < rotation_step_degrees)
	{
		current_course = target_course;
		is_turning = false;
	}
	else
	{
		// Obróć w kierunku najkrótszej drogi
		//float new_current_degrees = current_degrees + Math::sign(angle_diff) * rotation_step_degrees;
		float new_current_degrees = current_degrees - Math::sign(angle_diff) * rotation_step_degrees;

		// Normalizacja
		while (new_current_degrees < 0) new_current_degrees += 360.0f;
		while (new_current_degrees >= 360.0f) new_current_degrees -= 360.0f;

		current_course = deg_to_rad(new_current_degrees);
	}

	// Zastosuj obrót do ConvoyBoxa (Godot używa Y-up, więc obrót wokół osi Y)
	if (pConvoyBox)
	{
		pConvoyBox->set_rotation(Vector3(0.0f, current_course, 0.0f));
	}

	// Aktualizuj kierunek ruchu (nawigacyjny system współrzędnych)
	courseDirection = Vector3(
		Math::sin(current_course),    // X - Wschód/Zachód
		0.0f,                        // Y - Góra/Dół  
		Math::cos(current_course)     // Z - Północ/Południe
	);
}

//									Utworzenie kształtu konwoju (23.03.2025)
/*=============================================================================================================================
(updated 17.05.2025)
	- dodano wywołanie funkcji ExternalizeEscorts() w celu przeniesienia eskort do głównego węzła
	- dodano usuuwanie eskort z konwoju
(updated 10.05.2025)
	- dodane usuwanie statków z konwoju
	- zmodifikowane dodawanie statków do konwoju (dodawanie pustych komórek)
=============================================================================================================================*/
void TG_Convoy::FormConvoy()
{
// Usuń wszystkie wcześniejsze Handlowce z konwoju i wyczyść ich tablicę
	for (auto& ship : vConvoyMerchants)
	{
		if (ship)
		{
			ship->queue_free();
		}
	}
	vConvoyMerchants.clear();

// Usuń wszystkie wcześniejsze eskorty z konwoju i wyczyść ich tablicę
	if (pEscorts)
		pEscorts->DeleteEscortNodes();
	
// Zresetuj kurs konwoju
	SetCourseImmediate(0.0f);

// Skolumizowanie Konwoju
	if (pConvoyBox)
	{
		short rows, columns;
		short min, max, deviation{ 0 };
		min = sqrt(prop_InitNoMerchants);
		max = min + 1;
		if (min > 1)
			deviation = 1;
		if (min > 7)
			deviation = 2;
		if (min - deviation == 0)
			++min;
		if (min > max)
			++max;
		columns = UtilityFunctions::randi_range(min - deviation, max + deviation);
		rows = prop_InitNoMerchants / columns;
		if (rows == 0)
			rows = 1;
		// Jeśli szyk jeest mniejszy niź ilość statków to trzeba go powiększyć
		if (rows * columns < prop_InitNoMerchants)
		{
			if (UtilityFunctions::randi_range(1, 2) == 1)
				++rows;
			else
				++columns;
		}
		// Drugie sprawdzenie eleminuje buga, gdy wychodzi 5 statków, 3 kolumny i 1 wiersz i w pierwszej pętli dodana zostaje 4 kolumna
		if (rows * columns < prop_InitNoMerchants)
			++columns;
		columnsAndRows = Vector2i(columns, rows);
		formationSize.x = prop_formationSpacing.x * (columns);
		formationSize.y = prop_formationSpacing.y * (rows);
		pConvoyBox->set_size(Vector3(formationSize.x, 1.0, formationSize.y));
		convoyBoxRectangle = Rectangle{ Vector2(static_cast<float>(pConvoyBox->get_position().x + formationSize.x*0.5f), static_cast<float>(pConvoyBox->get_position().z + formationSize.y * 0.5f)), static_cast<float>(formationSize.x), static_cast<float>(formationSize.y) };

		// Ustawienie statków w szyku z uwzględnieniem losowych pustych komórek
		short merchantCount = prop_InitNoMerchants;
		short excessCells = (columns * rows) - prop_InitNoMerchants;
		for (short i = 1; i < rows + 1; ++i)
		{
			for (short j = 1; j < columns + 1; ++j)
			{
				if (merchantCount <= 0)
					break;
				if (excessCells > 0)
				{
					if (UtilityFunctions::randi_range(0, 1) == 1)
					{
						// Dodanie statku handlowego
						AddMerchantToConvoy(j, i, merchantCount);
						--merchantCount;
					}
					else
					{
						// Dodanie pustej komórki
						excessCells--;
					}
				}
				else
				{
					// Dodanie statku handlowego - nie ma nadmiaru komórek
					AddMerchantToConvoy(j, i, merchantCount);
					--merchantCount;
				}
			}
		}
		// Ustawienie eskorty w konwoju
		if (pEscorts)
		{
			pEscorts->PlaceEscorts(convoyBoxRectangle, prop_InitNoEscorts, 800, 1000.0f);
			//pEscorts->SetPlacedEscorts(vEscorts);
			//pEscorts->ExternalizeEscorts();
		}
		else
		{
			UtilityFunctions::printerr("TG_Convoy: not Escorts");
		}
		// Ustaw kurs konwoju
		SetCourseImmediate(prop_Course); // Ustawienie kursu konwoju na wartość z właściwości prop_Course
	}
}

//									Dodawanie handlowca do konwoju (27.03.2025)
/*=============================================================================================================================
(updated 24.05.2025)
	- dodano nowy parametr iID, który jest ID statku handlowego
	- handlowiec jest teraz klasą TG_MerchantShip, do której podpięty zostaje węzeł z RigidBody3D z obiektem statku
	- handlowce są teraz dziećmi pConvoyBox
(updated 10.05.2025)
	- dodane obliczanie środka komórki za pomocą funkcji CalculateCellCenter()
	  z zastosowaniem bloku try-catch
=============================================================================================================================*/
void TG_Convoy::AddMerchantToConvoy(short iColumn, short iRow, short iID)
{
	Ref<PackedScene> merchant_scene = ResourceLoader::get_singleton()->load("res://_Assets/Ships/LibertyShip_cpp.tscn");

	if (merchant_scene.is_valid())
	{
	// Operacje na Node3D
		auto pMerchantNode = (Node3D*)merchant_scene->instantiate();
		if (pMerchantNode)
		{
			// Ustawienie pozycji statku
			Vector2 position2D = Vector2(0.0f, 0.0f);
			try
			{
				position2D = CalculateCellCenter(columnsAndRows.x, columnsAndRows.y, prop_formationSpacing.x, prop_formationSpacing.y, iColumn - 1, iRow - 1);
				pMerchantNode->set_position(Vector3(position2D.x, 0.0f, position2D.y));
			}
			catch (const std::exception& e)
			{
				std::cerr << "Wyjatek: " << e.what() << std::endl;
			}
			pMerchantNode->set_name("MerchantShip" + String::num(iID));
			vConvoyMerchants.push_back(pMerchantNode);
			pConvoyBox->add_child(pMerchantNode);							// Dodanie statku do Sceny (do ConvoyBoxa)
			pMerchantNode->set_owner(get_tree()->get_edited_scene_root());	// Powoduje ukazanie statku w edytorze
			pMerchantNode->set_process(true);				// Włączenie przetwarzania Node3D, aby mógł działać w grze
	// Operacje na MerchantShip
			auto pMerchantShip = Object::cast_to<MerchantShip>(pMerchantNode);
			if (pMerchantShip)
			{
				pMerchantShip->SetID(iID);
				pMerchantShip->SetName(String("MerchantShip") + String::num(iID));
				pMerchantShip->SetOriginPosition(Vector3(position2D.x, 0.0f, position2D.y));
			}
		}
	}
	else
	{
		UtilityFunctions::printerr("TG_Convoy: failed to instantiate merchant ship node");
		return;
	}
	
	
	
	
	
	
//// Dodaj statek handlowy do drzewa Sceny
//	auto pMerchantShip = memnew(MerchantShip);
//	pMerchantShip->SetID(iID);
//	pMerchantShip->set_name("MerchantShip");						// Ustawienie nazwy statku handlowego
//	vConvoyMerchants.push_back(pMerchantShip);
//	pConvoyBox->add_child(pMerchantShip);							// Dodanie statku do Sceny (do ConvoyBoxa)
//	pMerchantShip->set_owner(get_tree()->get_edited_scene_root());	// Powoduje ukazanie statku w edytorze
//	
//
//// Dodanie modelu statku handlowego do pMerchantShip
//	Ref<PackedScene> libertyShip_scene = ResourceLoader::get_singleton()->load("res://_Assets/Ships/Liberty_ship.tscn");
//	if (libertyShip_scene.is_valid())
//	{
//		auto libertyShip = (Node3D*)libertyShip_scene->instantiate();
//		if (libertyShip)
//		{
//			// Ustawienie pozycji statku
//			try
//			{
//				Vector2 position = CalculateCellCenter(columnsAndRows.x, columnsAndRows.y, prop_formationSpacing.x, prop_formationSpacing.y, iColumn - 1, iRow - 1);
//				pMerchantShip->SetOriginPosition(Vector3(position.x, 0.0f, position.y));
//				pMerchantShip->SetPosition(Vector3(position.x, 0.0f, position.y));
//				//libertyShip->set_position(Vector3(position.x, 0.0f, position.y));
//			}
//			catch (const std::exception& e)
//			{
//				std::cerr << "Wyjatek: " << e.what() << std::endl;
//			}
//
//			
//			
//			// Ustawienie kursu statku
//			//libertyShip->set_rotation(Vector3(0.0, prop_Course, 0.0));
//			// Dodanie statku do konwoju
//			libertyShip->set_name("LibertyShip" + String::num(pMerchantShip->GetID()));	// Ustawienie nazwy statku
//			pMerchantShip->add_child(libertyShip);
//			libertyShip->set_owner(get_tree()->get_edited_scene_root());	// Powoduje ukazanie statku w edytorze
//			
//		}
//		else
//		{
//			UtilityFunctions::printerr("TG_Convoy: failed to instantiate LibertyShip");
//		}
//	}
//	else
//	{
//		UtilityFunctions::printerr("TG_Convoy: Nie udało się załadować sceny LibertyShip");
//	}
	
}

//									Obliczenie środka komórki (10.05.2025)
/*=============================================================================================================================
- powered by Gemini
=============================================================================================================================*/
Vector2 TG_Convoy::CalculateCellCenter(int Columns, int Rows, float spacingX, float spacingZ, int cIndex, int rIndex)
{
	// Walidacja podstawowych parametrów
	if (Columns <= 0)
	{
		throw std::invalid_argument("Liczba kolumn (C) musi być dodatnia.");
	}
	if (Rows <= 0)
	{
		throw std::invalid_argument("Liczba wierszy (R) musi być dodatnia.");
	}
	if (spacingX <= 0.0)
	{
		throw std::invalid_argument("Szerokość komórki (dx) musi być dodatnia.");
	}
	if (spacingZ <= 0.0)
	{
		throw std::invalid_argument("Wysokość komórki (dz) musi być dodatnia.");
	}

	// Walidacja indeksów
	if (cIndex < 0 || cIndex >= Columns)
	{
		throw std::out_of_range("Indeks kolumny (cIndeks) jest poza zakresem [0, " + std::to_string(Columns - 1) + "].");
	}
	if (rIndex < 0 || rIndex >= Rows)
	{
		throw std::out_of_range("Indeks wiersza (rIndex) jest poza zakresem [0, " + std::to_string(Rows - 1) + "].");
	}

	// Używamy static_cast<float>() dla C i R w dzieleniu, aby zapewnić dzielenie zmiennoprzecinkowe
	float centerX = spacingX * (static_cast<float>(cIndex) - static_cast<float>(Columns) / 2.0 + 0.5);
	float centerZ = spacingZ * (static_cast<float>(rIndex) - static_cast<float>(Rows) / 2.0 + 0.5);

	return Vector2(centerX, centerZ);
}

//									Poruszanie się konwoju (24.05.2025)
/*=============================================================================================================================
- powered by Gemini
=============================================================================================================================*/
void TG_Convoy::MoveConvoy(const double iDelta)
{
	//// Przekształcenie prędkości konwoju na siłę
	//auto globalTransform = pConvoyBox->get_global_transform();
	//auto mass = pConvoyBox->get_mass();
	//auto forceFactor = prop_Speed * 110.0f * mass;
	//forceFactor *= iDelta;
	//Vector3 force = globalTransform.basis.get_column(2) * forceFactor;
	//pConvoyBox->apply_central_force(force);
}


