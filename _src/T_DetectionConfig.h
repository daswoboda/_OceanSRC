// DetectionConfig.h - Parametry konfiguracyjne systemu wykrywania
#pragma once
#ifndef DETECTION_CONFIG_H
#define DETECTION_CONFIG_H

namespace DetectionConfig
{

    // Parametry wykrywania wzrokowego
    struct VisualDetection
    {
        static constexpr float BASE_RANGE_CLEAR_DAY = 8000.0f;      // 8km w idealnych warunkach
        static constexpr float BASE_RANGE_NIGHT = 2000.0f;          // 2km w nocy
        static constexpr float BASE_RANGE_FOG = 500.0f;             // 0.5km we mgle
        static constexpr float PERISCOPE_DETECTION_MODIFIER = 0.1f;  // 10% zasiêgu dla peryskopów
        static constexpr float SURFACED_SUB_MODIFIER = 1.0f;        // 100% dla wynurzonego U-boota
    };

    // Parametry sonaru
    struct SonarParameters
    {
        static constexpr float ASDIC_RANGE = 3000.0f;               // 3km zasiêg ASDIC
        static constexpr float BEAM_WIDTH = 2.0f;                   // 2° szerokoœæ wi¹zki
        static constexpr float MAX_ROTATION_ANGLE = 45.0f;          // +/- 45° obrót
        static constexpr float PING_INTERVAL = 2.0f;                // 2s miêdzy pingami
        static constexpr float MAX_SPEED_KNOTS = 15.0f;             // Max 15 wêz³ów
        static constexpr float DEPTH_DETECTION_LIMIT = 200.0f;      // Max g³êbokoœæ wykrycia
    };

    // Parametry nas³uchu
    struct ListeningParameters
    {
        static constexpr float BASE_RANGE = 4000.0f;                // 4km zasiêg
        static constexpr float MAX_SPEED_KNOTS = 6.0f;              // Max 6 wêz³ów
        static constexpr float BEARING_ACCURACY = 5.0f;             // +/- 5° dok³adnoœæ namiaru
        static constexpr float ENGINE_NOISE_MODIFIER = 1.5f;        // Silniki diesla ³atwiej wykryæ
        static constexpr float SILENT_RUNNING_MODIFIER = 0.3f;      // Cichy bieg trudniej wykryæ
    };

    // Modyfikatory œrodowiskowe
    struct EnvironmentalFactors
    {
        static constexpr float SEA_STATE_CALM = 1.0f;               // Spokojne morze
        static constexpr float SEA_STATE_MODERATE = 0.7f;           // Umiarkowane fale
        static constexpr float SEA_STATE_ROUGH = 0.4f;              // Wzburzone morze
        static constexpr float THERMAL_LAYER_MODIFIER = 0.5f;       // Warstwa termiczna
        static constexpr float RAIN_MODIFIER = 0.8f;                // Deszcz
        static constexpr float STORM_MODIFIER = 0.3f;               // Sztorm
    };

    // Prêdkoœci i ograniczenia
    struct SpeedLimits
    {
        static constexpr float VISUAL_NO_LIMIT = 999.0f;            // Brak limitu dla obserwacji
        static constexpr float SONAR_MIN_SPEED = 6.0f;              // Min dla sonaru
        static constexpr float SONAR_MAX_SPEED = 15.0f;             // Max dla sonaru
        static constexpr float LISTENING_MIN_SPEED = 3.0f;          // Min dla nas³uchu
        static constexpr float LISTENING_MAX_SPEED = 6.0f;          // Max dla nas³uchu
    };
}

#endif // DETECTION_CONFIG_H

