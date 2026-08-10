#ifndef PACKETCOUNTER_H_
#define PACKETCOUNTER_H_

#include <queue>
#include <string>
#include "PacketHeader.h"
#include "Thread.h"

#include "PacketHeader.h"

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
class Packet_DB_Load_Request_Guild_Board_Write : public PacketHeader {
public:
    Packet_DB_Load_Request_Guild_Board_Write();
    char m_data[0xb2];
};

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
class Packet_Guild_Attendance_Info_Reply : public PacketHeader {
public:
    Packet_Guild_Attendance_Info_Reply();
    char m_data[0x25];
};

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
