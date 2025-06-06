#include "TG_Escorts.h"

#include <packed_scene.hpp>					// for PackedScene
#include <resource.hpp>						// for Resource
#include <resource_loader.hpp>				// for ResourceLoader
#include <scene_tree.hpp>					// for get_tree()
#include <window.hpp>						// for get_root()
#include <time.hpp>				            // for Time

void TG_Escorts::_bind_methods()
{
}

TG_Escorts::TG_Escorts()
{
}

TG_Escorts::~TG_Escorts()
{
}

//											Przygotowanie Escorty (15.05.2025)
/*=============================================================================================================================

=============================================================================================================================*/
/// <summary>
/// Inicjalizacja wêz³a eskorty. Przypisuje wskaŸnik do T_TacticalTier i wywo³uje metodê gotowoœci.
/// </summary>
void TG_Escorts::_ready()
{
	// if (Engine::get_singleton()->is_editor_hint()) return;

	// Przypisanie wskaŸnika na T_TacticalTier, który jest dzieckiem g³ównego wêz³a
	//auto pTT_node = pNode->get_node_or_null("TG_TacticalTier");
	auto pTT_node = get_node_or_null("../../");
	if (pTT_node)
	{
		pTacticalTier = Object::cast_to<TG_TacticalTier>(pTT_node);
	}
	else
	{
		UtilityFunctions::printerr("TG_Escorts: not TG_TacticalTier");
	}

    // Przypisz konwój do Eskorty - Pobierz wskaŸnik do konwoju przez rodzica
    Node* parent = get_parent();
    if (parent)
    {
        TG_Convoy* ipConvoy = Object::cast_to<TG_Convoy>(parent->get_parent());
        if (ipConvoy)
        {
            SetConvoy(ipConvoy);  // Changed pEscortShip->SetConvoy to SetConvoy
        }
    }
    else
    {
        UtilityFunctions::printerr("TG_Escorts: not TG_Convoy");
	}

	UtilityFunctions::print("TG_Escorts ready");
}

void TG_Escorts::_process(double delta)
{
}

//							Metoda do umieszczania okrêtów eskorty w szyku konwoju (15.05.2025)
/*=============================================================================================================================
- powered by Gemini
=============================================================================================================================*/
/// <summary>
/// Rozmieszcza statki eskortuj¹ce wokó³ prostok¹tnego konwoju, wyznaczaj¹c ich pozycje oraz strefy patrolowe na podstawie liczby eskort, odsuniêcia i bazowego promienia patrolu.
/// </summary>
/// <param name="convoyRect">Prostok¹t reprezentuj¹cy obszar konwoju, wokó³ którego maj¹ byæ rozmieszczone eskorty.</param>
/// <param name="numEscorts">Liczba statków eskortuj¹cych do rozmieszczenia.</param>
/// <param name="escortOffsetDistance">Odsuniêcie (w jednostkach) pozycji eskorty od obwodu konwoju.</param>
/// <param name="basePatrolRadius">Bazowy promieñ strefy patrolowej dla pojedynczej eskorty (przed adaptacj¹ do liczby eskort).</param>
/// <returns>Wektor obiektów EscortShip, z których ka¿dy zawiera pozycjê eskorty oraz przypisan¹ strefê patrolow¹.</returns>
void TG_Escorts::PlaceEscorts
                             (
                                 const Rectangle& convoyRect,
                                 int numEscorts,
                                 double escortOffsetDistance,
                                 double basePatrolRadius         // Bazowy promieñ dla adaptacyjnej strefy
                             ) 
{
    //std::vector<TG_EscortShip*> escorts;
	if (numEscorts <= 0) return;                    // Nie ma eskort do umieszczenia

    // Oblicz potrzebne w³aœciwoœci konwoju
	Vector2 convoyCenter = convoyRect.GetCenter();
	double perimeter = convoyRect.GetPerimeter(); // Oblicz obwód prostok¹ta
    std::vector<Vector2> vCorners = convoyRect.GetCorners(); // 0:TopL, 1:TopR, 2:BottomR, 3:BottomL
    std::vector<Vector2> midpoints = convoyRect.GetSideMidpoints();

    // Wektor do przechowywania pocz¹tkowych punktów na obwodzie
    std::vector<Vector2> perimeterPoints;

    // --- Krok 1: Wyznacz pocz¹tkowe punkty na obwodzie ---
    if (numEscorts == 1) {
        // Przypadek 1 eskorty: œrodek górnego boku (front)
        perimeterPoints.push_back({ convoyCenter.x, vCorners[0].y }); // Zmieniono na vCorners[0]
    }
    else if (numEscorts == 2) {
        // Przypadek 2 eskort: œrodki górnego i dolnego boku
        perimeterPoints.push_back(midpoints[0]); // Góra
        perimeterPoints.push_back(midpoints[1]); // Dó³ - zmieniono na vCorners[3]
    }
    else if (numEscorts == 3) {
        // Przypadek 3 eskort: œrednie górnego, lewego i prawego boku
        perimeterPoints.push_back({ midpoints[0] }); // Góra - zmieniono na vCorners[0]
        perimeterPoints.push_back({ midpoints[1] }); // Lewo - zmieniono na vCorners[1]
        perimeterPoints.push_back({ midpoints[2] }); // Prawo - zmieniono na vCorners[2]
    }
    else if (numEscorts == 4) {
        // Przypadek 4 eskort: œrodki wszystkich boków
        //std::vector<Vector2> midpoints = convoyRect.GetSideMidpoints();
        perimeterPoints = midpoints;
    }
    else {
        // Przypadek ogólny (numEscorts > 4): Równomiernie wzd³u¿ obwodu
        double spacing = perimeter / numEscorts;
        double currentDistance = 0.0; // Dystans od punktu startowego (np. TL)
        Vector2 currentPoint = vCorners[0];
        int currentSegment = 0; // 0: góra, 1: prawo, 2: dó³, 3:` lewo

        std::vector<Vector2> segmentStarts = { vCorners[0], vCorners[1], vCorners[2], vCorners[3]};
        std::vector<Vector2> segmentEnds = { vCorners[1], vCorners[2], vCorners[3], vCorners[0]};
        std::vector<double> segmentLengths = {
            convoyRect.width, convoyRect.height, convoyRect.width, convoyRect.height
        };

        for (int i = 0; i < numEscorts; ++i) {
            double targetDistance = (static_cast<double>(i) + 0.5) * spacing; // Celuj w œrodek interwa³u
            double remainingTarget = targetDistance - currentDistance;

            // Przesuwaj siê wzd³u¿ segmentów, a¿ znajdziesz w³aœciwy
            while (remainingTarget > segmentLengths[currentSegment]) {
                remainingTarget -= segmentLengths[currentSegment];
                currentDistance += segmentLengths[currentSegment];
                currentSegment = (currentSegment + 1) % 4;
                currentPoint = segmentStarts[currentSegment];
            }

            // Oblicz pozycjê na bie¿¹cym segmencie
            double segmentDx = segmentEnds[currentSegment].x - segmentStarts[currentSegment].x;
            double segmentDy = segmentEnds[currentSegment].y - segmentStarts[currentSegment].y;
            double fraction = (segmentLengths[currentSegment] > 1e-9) ? (remainingTarget / segmentLengths[currentSegment]) : 0.0;

            Vector2 pointOnPerimeter = {
                static_cast<real_t>(segmentStarts[currentSegment].x + segmentDx * fraction),
                static_cast<real_t>(segmentStarts[currentSegment].y + segmentDy * fraction)
            };
            perimeterPoints.push_back(pointOnPerimeter);
        }
    }

    // --- Krok 2: Zastosuj odsuniêcie i utwórz strefy patrolowe ---
    int iterationCount = 0;
    for (const auto& p_perimeter : perimeterPoints)
    {
        //// ZnajdŸ segment, na którym le¿y punkt (lub najbli¿szy segment dla œrodków)
        //// (Uproszczenie: zak³adamy, ¿e znamy orientacjê - mo¿na to wywnioskowaæ dok³adniej)
        //Vector2 lineStart, lineEnd;
        //// Logika determinuj¹ca lineStart i lineEnd na podstawie p_perimeter
        //// np. sprawdzaj¹c, do którego boku jest najbli¿ej lub na którym le¿y.
        //// Dla przypadków N=1..4 jest to prostsze. Dla N>4 wymaga wiêcej logiki.
        //// Przyk³ad uproszczony - wymaga dopracowania identyfikacji segmentu:
        //if (std::abs(p_perimeter.y - vCorners[0].y) < 1e-6 && p_perimeter.x >= vCorners[0].x && p_perimeter.x <= vCorners[1].x) { // Góra
        //    lineStart = vCorners[0]; lineEnd = vCorners[1];
        //}
        //else if (std::abs(p_perimeter.x - vCorners[1].x) < 1e-6 && p_perimeter.y >= vCorners[0].y && p_perimeter.y <= vCorners[2].y) { // Prawo
        //    lineStart = vCorners[1]; lineEnd = vCorners[2];
        //}
        //else if (std::abs(p_perimeter.y - vCorners[2].y) < 1e-6 && p_perimeter.x >= vCorners[3].x && p_perimeter.x <= vCorners[2].x) { // Dó³
        //    lineStart = vCorners[2]; lineEnd = vCorners[3];
        //}
        //else { // Lewo
        //    lineStart = vCorners[3]; lineEnd = vCorners[0];
        //}

		
        Vector2 lineStart, lineEnd;
        if (iterationCount == 0)
        { // Góra
            lineStart = vCorners[0]; lineEnd = vCorners[1];
        }
        else if (iterationCount == 1)
        { // Prawo
            lineStart = vCorners[1]; lineEnd = vCorners[2];
        }
        else if (iterationCount == 2) 
        { // Dó³
            lineStart = vCorners[2]; lineEnd = vCorners[3];
        }
        else { // Lewo
            lineStart = vCorners[3]; lineEnd = vCorners[0];
        }
		

        // Oblicz finaln¹ pozycjê eskorty
        Vector2 finalPosition = CalculateOffsetPoint(p_perimeter, lineStart, lineEnd, escortOffsetDistance, convoyCenter);

        // Oblicz adaptacyjny rozmiar strefy (przyk³ad dla okrêgu)
        double calculatedRadius = basePatrolRadius / std::sqrt(static_cast<double>(numEscorts));

        // Gwarancja braku przeciêcia - przyciêcie promienia
        double finalRadius = std::min(calculatedRadius, escortOffsetDistance * 0.99);
        if (finalRadius < 0) finalRadius = 0; // Promieñ nie mo¿e byæ ujemny

        // Utwórz strefê patrolow¹ (tutaj jako Circle)
        Circle zoneShape = { finalPosition, finalRadius };
        PatrolZone patrolZone; // Zak³adaj¹c, ¿e PatrolZone u¿ywa std::variant
        patrolZone.zone = zoneShape;

        // Opcjonalne: Dodatkowe sprawdzenie przeciêcia (powinno byæ false)
        // if (doesIntersect(zoneShape, convoyRect)) {
        //     // Logika obs³ugi b³êdu - to nie powinno siê zdarzyæ przy poprawnym przyciêciu
        // }

        // Dodaj statek eskortuj¹cy do wyniku
        AddEscort(iterationCount, Vector3(finalPosition.x, 0.0f, finalPosition.y), patrolZone);
		

        ++iterationCount;
    }

    return;
}

//			Metoda do obliczenie pozycji punktu odsuniêtego prostopadle na zewn¹trz od danego boku prostok¹ta (16.05.2025)
/*=============================================================================================================================
- powered by Gemini
=============================================================================================================================*/
/// <summary>
/// Oblicza punkt odsuniêty od podanego punktu p o okreœlon¹ odleg³oœæ prostopadle do odcinka wyznaczonego przez lineStart i lineEnd, na zewn¹trz wzglêdem œrodka konwoju.
/// </summary>
/// <param name="p">Punkt, od którego wyznaczane jest odsuniêcie.</param>
/// <param name="lineStart">Pocz¹tek odcinka, wzglêdem którego wyznaczana jest normalna.</param>
/// <param name="lineEnd">Koniec odcinka, wzglêdem którego wyznaczana jest normalna.</param>
/// <param name="offsetDistance">Odleg³oœæ, o jak¹ punkt p ma zostaæ odsuniêty.</param>
/// <param name="convoyCenter">Œrodek konwoju, u¿ywany do okreœlenia kierunku odsuniêcia (na zewn¹trz konwoju).</param>
/// <returns>Nowy punkt typu Vector2, odsuniêty od p o offsetDistance prostopadle do odcinka lineStart-lineEnd, skierowany na zewn¹trz wzglêdem convoyCenter.</returns>
Vector2 TG_Escorts::CalculateOffsetPoint(Vector2 p, Vector2 lineStart, Vector2 lineEnd, float offsetDistance, Vector2 convoyCenter)
{
    // Wektor reprezentuj¹cy odcinek linii
    double dx = lineEnd.x - lineStart.x;
    double dy = lineEnd.y - lineStart.y;

    // Wektor normalny (prostopad³y)
    double nx = -dy;
    double ny = dx;

    // Normalizacja wektora normalnego
    double length = std::sqrt(nx * nx + ny * ny);
    if (length > 1e-9) { // Unikaj dzielenia przez zero dla odcinków zerowej d³ugoœci
        nx /= length;
        ny /= length;
    }
    else {
        // Obs³uga przypadku zdegenerowanego (np. zwróæ p lub obs³u¿ b³¹d)
        // Dla prostok¹ta ten przypadek nie powinien wyst¹piæ.
        return p;
    }

    // Sprawdzenie kierunku wektora normalnego (czy jest "na zewn¹trz" konwoju)
    // Wektor od œrodka konwoju do punktu p
    double vecToP_x = p.x - convoyCenter.x;
    double vecToP_y = p.y - convoyCenter.y;

    // Iloczyn skalarny wektora normalnego i wektora do p
    // Jeœli dodatni, wektor normalny jest ju¿ skierowany na zewn¹trz
    double dotProduct = nx * vecToP_x + ny * vecToP_y;

    if (dotProduct < 0) {
        // Odwróæ wektor normalny, jeœli jest skierowany do wewn¹trz
        nx = -nx;
        ny = -ny;
    }

    // Obliczenie finalnego punktu odsuniêtego
    return { static_cast<real_t>(p.x + nx * offsetDistance), static_cast<real_t>(p.y + ny * offsetDistance) };
}

//			                            Wizualizacja okrêtów eskorty w oknie Godota (17.05.2025)
/*=============================================================================================================================

=============================================================================================================================*/
/// <summary>
/// Wizualizuje okrêty eskortuj¹ce w edytorze Godota, dodaj¹c je jako dzieci do g³ównego wêz³a i przypisuj¹c im odpowiednie w³aœciwoœci.
/// Wêz³y eskorty s¹ tworzone na podstawie za³adowanej sceny PackedScene, a ich pozycje i strefy patrolowe s¹ ustawiane na podstawie danych z vPlacedEscorts.
/// </summary>
void TG_Escorts::ExternalizeEscorts()
{
    Ref<PackedScene> escortShip_scene = ResourceLoader::get_singleton()->load("res://_Assets/Ships/corvette_cpp.tscn");
	if (!escortShip_scene.is_valid())
    { 
        UtilityFunctions::printerr("TG_Escorts: failed to load escort ship scene");
		return;
	}

	// iteracja przez vPlacedEscorts
	int escortCount = 0;
    for (auto& pEscortShip : vPlacedEscorts)
	{
        add_child(pEscortShip);
		pEscortShip->set_name("EscortShip" + String::num(pEscortShip->GetID()));
        pEscortShip->set_owner(get_tree()->get_edited_scene_root());	// Powoduje ukazanie statku w edytorze
        pEscortShip->set_process(true);
		pEscortShip->ExternalizePatrolZone();	// Wizualizacja strefy patrolowej
        /* Ref<PackedScene> escortShip_scene = ResourceLoader::get_singleton()->load("res://_Assets/Ships/corvette_01.tscn");
        if (!escortShip_scene.is_valid())
        {
            UtilityFunctions::printerr("TG_Escorts: failed to load escort ship scene");
            return;
        }*/
        auto escortShip_node = (Node3D*)escortShip_scene->instantiate();
        
        if (escortShip_node)
        {
            //escortShip_node->set_position(pEscortShip->GetPosition());
            escortShip_node->set_name("Corvette" + String::num(pEscortShip->GetID()));
            pEscortShip->add_child(escortShip_node);
			pEscortShip->SetShipNode(escortShip_node);
			escortShip_node->set_owner(get_tree()->get_edited_scene_root());	// Powoduje ukazanie statku w edytorze
			escortShip_node->set_process(true);

        }
    }

}

//			                           Dodanie okrêtu Eskorty (30.05.2025)
/*=============================================================================================================================

=============================================================================================================================*/
void TG_Escorts::AddEscort(unsigned short iID, Vector3 iPos, PatrolZone iZone)
{
    Ref<PackedScene> escortShip_scene = ResourceLoader::get_singleton()->load("res://_Assets/Ships/corvette_cpp.tscn");
    
    if (escortShip_scene.is_valid())
    {
        auto pEscortNode = (Node3D*)escortShip_scene->instantiate();
        if (pEscortNode)
        {
			pEscortNode->set_name("EscortShip" + String::num(iID));
            pEscortNode->set_position(Vector3(iPos.x, 0.0f, iPos.z)); // Ustawienie pozycji w 3D
			add_child(pEscortNode); // Dodanie wêz³a eskorty do drzewa sceny
            pEscortNode->set_owner(get_tree()->get_edited_scene_root());
            vEscortNodes.push_back(pEscortNode); // Dodanie wêz³a eskorty do wektora
            
            auto pEscortShip = Object::cast_to<EscortShip>(pEscortNode);
            if (pEscortShip)
            {
                pEscortShip->SetID(iID);
                pEscortShip->SetName(String("EscortShip") + String::num(iID));
                pEscortShip->SetOriginPosition(iPos);
                pEscortShip->SetPatrolZone(iZone);

                // Przypisz konwój do eskortowca
                // Pobierz wskaŸnik do konwoju przez rodzica
                Node* parent = get_parent();
                if (parent)
                {
                    TG_Convoy* pConvoy = Object::cast_to<TG_Convoy>(parent->get_parent());
                    if (pConvoy)
                    {
                        pEscortShip->SetConvoy(pConvoy);
                    }
                }

                pEscortShip->ExternalizePatrolZone();	// Wizualizacja strefy patrolowej
                vPlacedEscorts.push_back(pEscortShip);
            }
        }
    }
    else
    {
        UtilityFunctions::printerr("TG_Escorts: failed to instantiate escort ship node");
        return;
	}
}

void TG_Escorts::UpdateDetectionSystem(double delta)
{
    double currentTime = Time::get_singleton()->get_ticks_msec() / 1000.0;
    detectionSystem.UpdateReports(currentTime);

    // Przetwórz raporty i podejmij akcje
    // np. wyœlij najbli¿sze jednostki do zbadania kontaktu
}

//			                            Usuwanie wêz³ów eskorty (17.05.2025)
/*=============================================================================================================================
(updated 31.05.2025)
	- dodane czyszczenie wektora vEscortNodes
=============================================================================================================================*/
void TG_Escorts::DeleteEscortNodes()
{
    // Usuñ wszystkie wczeœniejsze eskorty z konwoju i wyczyœæ ich tablicê
    for (auto& escort_Ship : vPlacedEscorts)
    {
        if (escort_Ship)
        {
            escort_Ship->set_process(false);        // Zatrzymaj proces
			escort_Ship->queue_free();              // Usuñ wêze³ z drzewa
        }
    }
    for (auto& escort_Node : vEscortNodes)
    {
        if (escort_Node)
        {
			escort_Node->set_process(false);        // Zatrzymaj proces
            escort_Node->queue_free();              // Usuñ wêze³ z drzewa
        }
	}
    vEscortNodes.clear();
	vPlacedEscorts.clear();
}



void DetectionSystem::AddReport(const DetectionReport& report)
{
    // SprawdŸ czy nie ma ju¿ podobnego raportu
    if (!IsPositionAlreadyReported(report.position)) {
        activeReports.push_back(report);
    }
    else {
        // Aktualizuj istniej¹cy raport jeœli nowy ma wy¿sz¹ pewnoœæ
        for (auto& existingReport : activeReports) {
            float distance = existingReport.position.distance_to(report.position);
            if (distance < 100.0f && report.confidence > existingReport.confidence) {
                existingReport = report;
                break;
            }
        }
    }
}

void DetectionSystem::UpdateReports(double currentTime)
{
    // Usuñ wygas³e raporty
    activeReports.erase(
        std::remove_if(activeReports.begin(), activeReports.end(),
            [currentTime, this](const DetectionReport& report) {
                return (currentTime - report.timestamp) > REPORT_TIMEOUT;
            }
        ),
        activeReports.end()
    );
}

std::vector<DetectionSystem::DetectionReport>
DetectionSystem::GetReportsInArea(Vector3 center, float radius)
{
    std::vector<DetectionReport> result;

    for (const auto& report : activeReports) {
        if (report.position.distance_to(center) <= radius) {
            result.push_back(report);
        }
    }

    return result;
}

bool DetectionSystem::IsPositionAlreadyReported(Vector3 position, float threshold)
{
    for (const auto& report : activeReports) {
        if (report.position.distance_to(position) < threshold) {
            return true;
        }
    }
    return false;
}