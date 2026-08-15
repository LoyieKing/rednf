#ifndef PACKETCOUNTER_H_
#define PACKETCOUNTER_H_

#include <queue>
#include <string>
#include "PacketHeader.h"
#include "Thread.h"

#include "PacketHeader.h"
#include "GuildBoard.h"
#include "DNFGuildManager.h"

struct Packet_DB_Load_Request_Guild_Board_Delete;
struct Packet_DB_Load_Request_Guild_Board_Open;
struct Packet_DB_Load_Request_Guild_Board_Write;
struct Packet_DB_Load_Request_Web_Guild_Board_Write;
struct Packet_DB_Request_Approve_Join_Guild;
struct Packet_Guild_Attendance_Info_Reply;
struct Packet_Guild_Reply_Guild_Board;
struct Packet_Guild_Reply_Guild_Board_Delete;

// from GuildPacket.h
template<int Lo, int Hi>
class CPacketCounter
{
public:
    CPacketCounter(char* name, char* title);
    ~CPacketCounter();
    void IncrementPacketCount(int id);
    void BeforeProcess();
    void AfterProcess(int id);
    void WriteLog();
    void Reset();
    // 布局（ORIG 二进制实测，同 statics/DBMW）：+0 count / +4 t / +8 counts[0x2418] /
    // +0x9068 snapshot[0x2418] / +0x120c8 pending[0x2418] / +0x144e0 diffs[0x2418] /
    // +0x1d540 path[0x100] / +0x1d640 bProcess / +0x1d641 bInit（对齐至 0x1d644）
    union
    {
        struct
        {
            unsigned int m_count;              // +0
            time_t m_t;                        // +4
            unsigned int m_counts[0x2418];     // +8
            unsigned int m_snapshot[0x2418];   // +0x9068
            unsigned char m_pending[0x2418];   // +0x120c8
            unsigned int m_diffs[0x2418];      // +0x144e0
            char m_path[0x100];                // +0x1d540
            bool m_bProcess;                   // +0x1d640
            unsigned char m_bInit;             // +0x1d641
        };
        char m_data[0x1d644];
    };
};

// from GuildPackets.h
#pragma pack(push,1)
class Packet_DB_Load_Request_Guild_Board_Delete : public PacketHeader {
public:
    Packet_DB_Load_Request_Guild_Board_Delete();
    unsigned char m_a;        // +0xa
    unsigned int m_b;         // +0xb
    unsigned int m_c;         // +0xf
    unsigned int m_d;         // +0x13
};
#pragma pack(pop)

// from GuildPackets.h
class Packet_DB_Load_Request_Guild_Board_Open : public PacketHeader {
public:
    Packet_DB_Load_Request_Guild_Board_Open();
    unsigned int m_guildKey;   // +0xa
    unsigned int m_charNo;     // +0xe
};

// from GuildPackets.h
#pragma pack(push,1)
class Packet_DB_Load_Request_Guild_Board_Write : public PacketHeader {
public:
    Packet_DB_Load_Request_Guild_Board_Write();
    unsigned char m_a;             // +0xa
    unsigned int m_b;              // +0xb
    unsigned int m_c;              // +0xf
    unsigned int m_d;              // +0x13
    STGuildBoardDBInfo m_info;     // +0x17
};
#pragma pack(pop)

#pragma pack(push,1)
class Packet_DB_Load_Request_Web_Guild_Board_Write : public PacketHeader {
public:
    Packet_DB_Load_Request_Web_Guild_Board_Write();
    unsigned int m_guildKey;  // +0xa
    unsigned int m_charNo;    // +0xe
    unsigned int m_no;        // +0x12
} __attribute__((packed));
#pragma pack(pop)

// from GuildPackets.h
class Packet_DB_Request_Approve_Join_Guild : public PacketHeader {
public:
    Packet_DB_Request_Approve_Join_Guild();
    unsigned int m_guildKey;  // +0xa
    unsigned int m_charNo;    // +0xe
    unsigned int m_no;        // +0x12
    unsigned char m_group;    // +0x16
};

// from GuildPackets.h
#pragma pack(push,1)
class Packet_Guild_Attendance_Info_Reply : public PacketHeader {
public:
    Packet_Guild_Attendance_Info_Reply();
    unsigned int m_a;              // +0xa
    unsigned int m_b;              // +0xe
    unsigned char m_c;             // +0x12
    STAttendanceInfo m_info;       // +0x13
};
#pragma pack(pop)

// from GuildPackets.h
#pragma pack(push,1)
class Packet_Guild_Reply_Guild_Board : public PacketHeader {
public:
    Packet_Guild_Reply_Guild_Board();
    unsigned short m_a;              // +0xa
    unsigned short m_c;              // +0xc
    unsigned char m_e;               // +0xe
    unsigned int m_f;                // +0xf
    unsigned int m_13;               // +0x13
    unsigned char m_17;              // +0x17
    STGuildBoardDBInfo m_boards[10]; // +0x18（10 × 0xa5 = 0x672）
};
#pragma pack(pop)

// from GuildPackets.h
class Packet_Guild_Reply_Guild_Board_Delete : public PacketHeader {
public:
    Packet_Guild_Reply_Guild_Board_Delete();
    unsigned short m_result;  // +0xa
    unsigned int m_channel;   // +0xc
    unsigned int m_charNo;    // +0x10
};

#endif
