
#pragma once


namespace TopicsMqtt
{
    // Etats du panneau RAM
    constexpr const char* NIV_GB =
        "RAM/panneau/etats/NivGB";

    constexpr const char* NIV_PB =
        "RAM/panneau/etats/NivPB";

    constexpr const char* TMP_PB =
        "RAM/panneau/etats/TmpPB";

    // Etat des valves
    constexpr const char* VALVE_GB =
        "RAM/panneau/etats/ValveGB";

    constexpr const char* VALVE_PB =
        "RAM/panneau/etats/ValvePB";

    constexpr const char* VALVE_EC =
        "RAM/panneau/etats/ValveEC";

    constexpr const char* VALVE_EF =
        "RAM/panneau/etats/ValveEF";

    constexpr const char* VALVE_EEC =
        "RAM/panneau/etats/ValveEEC";

    constexpr const char* VALVE_EEF =
        "RAM/panneau/etats/ValveEEF";

    // Etat de la pompe et du mode
    constexpr const char* POMPE =
        "RAM/panneau/etats/Pompe";

    constexpr const char* MODE =
        "RAM/panneau/etats/Mode";

    // Commandes du superviseur
    constexpr const char* CMD_MODE =
        "RAM/panneau/cmd/Mode";

    constexpr const char* CMD_NIV_GB =
        "RAM/panneau/cmd/ConsNivGB";

    constexpr const char* CMD_NIV_PB =
        "RAM/panneau/cmd/ConsNivPB";

    constexpr const char* CMD_TMP_PB =
        "RAM/panneau/cmd/ConsTmpPB";

    constexpr const char* CMD_VALVE_GB =
        "RAM/panneau/cmd/ValveGB";

    constexpr const char* CMD_VALVE_PB =
        "RAM/panneau/cmd/ValvePB";

    constexpr const char* CMD_VALVE_EC =
        "RAM/panneau/cmd/ValveEC";

    constexpr const char* CMD_VALVE_EF =
        "RAM/panneau/cmd/ValveEF";

    constexpr const char* CMD_VALVE_EEC =
        "RAM/panneau/cmd/ValveEEC";

    constexpr const char* CMD_VALVE_EEF =
        "RAM/panneau/cmd/ValveEEF";

    constexpr const char* CMD_POMPE =
        "RAM/panneau/cmd/Pompe";
}