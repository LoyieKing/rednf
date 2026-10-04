#ifndef DNF_GAME_PACKET_Packet_Loading_Time_Report_Statistics_H_
#define DNF_GAME_PACKET_Packet_Loading_Time_Report_Statistics_H_

#include <string.h>
#include "PacketHeader.h"

#pragma pack(push, 1)

class Packet_Loading_Time_Report_Statistics : public PacketHeader
{
public:
    Packet_Loading_Time_Report_Statistics();

    struct Slot9 {
        unsigned short m_link0;
        unsigned short m_link1;
        unsigned short m_a;
        unsigned char m_b;
        unsigned short m_c;
    };
    struct SlotB {
        unsigned short m_a;
        unsigned char m_b;
        unsigned int m_c;
        unsigned int m_d;
    };

    unsigned int m_times[9];    // +0x0a
    union {
        struct {
            unsigned int m_accId;       // +0x2e
            short m_countA;             // +0x32
            char m_data[0x806 - 0x34];  // +0x34
        };
        struct {
            char m_pad30[2];            // +0x2e
            Slot9 m_rowsA[100];         // +0x30
        };
        struct {
            char m_padB[0x3b8 - 0x2e];  // +0x2e
            short m_countB;             // +0x3b8
            SlotB m_rowsB[100];         // +0x3ba
        };
    };
};

#pragma pack(pop)

static_assert(sizeof(Packet_Loading_Time_Report_Statistics) == 0x806, "Packet_Loading_Time_Report_Statistics layout");

#endif
