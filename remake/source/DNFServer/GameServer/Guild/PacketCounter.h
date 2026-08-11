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
    char m_data[0x1d644];
};

// from GuildPackets.h
class Packet_DB_Load_Request_Guild_Board_Delete : public PacketHeader {
public:
    Packet_DB_Load_Request_Guild_Board_Delete();
    char m_data[0xd];
};

// from GuildPackets.h
class Packet_DB_Load_Request_Guild_Board_Open : public PacketHeader {
public:
    Packet_DB_Load_Request_Guild_Board_Open();
    char m_data[0xa];
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

// from GuildPackets.h
class Packet_DB_Load_Request_Web_Guild_Board_Write : public PacketHeader {
public:
    Packet_DB_Load_Request_Web_Guild_Board_Write();
    char m_data[0xc];
};

// from GuildPackets.h
class Packet_DB_Request_Approve_Join_Guild : public PacketHeader {
public:
    Packet_DB_Request_Approve_Join_Guild();
    char m_data[0xd];
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
class Packet_Guild_Reply_Guild_Board : public PacketHeader {
public:
    Packet_Guild_Reply_Guild_Board();
    char m_data[0x680];
};

// from GuildPackets.h
class Packet_Guild_Reply_Guild_Board_Delete : public PacketHeader {
public:
    Packet_Guild_Reply_Guild_Board_Delete();
    char m_data[0xa];
};

#endif
