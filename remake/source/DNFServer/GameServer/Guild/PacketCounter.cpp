// df_guild_r — PacketCounter (split from source/guild per ORIG layout)
#include <stdio.h>
#include <string.h>
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Guild_Change_Power_War_Point.h"

#include "PacketCounter.h"
#include "BlackUser.h"
#include "CashObject.h"
#include "DNFAppConfig.h"
#include "DNFAppStartInit.h"
#include "DNFAppStopInit.h"
#include "DNFApplication.h"
#include "DNFDBServer.h"
#include "DNFFloatingPointExceptSig.h"
#include "DNFGameServer.h"
#include "DNFGuild.h"
#include "DNFGuildManager.h"
#include "DNFGuildServerMain.h"
#include "DNFGuildWar.h"
#include "DNFInnerMsgHandler.h"
#include "DNFKillUserConfig.h"
#include "DNFManagerServer.h"
#include "DNFMonitorServer.h"
#include "DNFNetworkThread.h"
#include "DNFPacketBuffer.h"
#include "DNFPacketDecoder.h"
#include "DNFPacketTracer.h"
#include "DNFPacketTranslater.h"
#include "DNFPowerWarConfig.h"
#include "DNFSegmentationFaultSig.h"
#include "DNFServerConfig.h"
#include "DNFServerHandler.h"
#include "DNFServerInterface.h"
#include "DNFSignal.h"
#include "DNFSignalTranslator.h"
#include "DNFSystemFailSig.h"
#include "DNFTableBase.h"
#include "DNFTcpAcceptThread.h"
#include "DNFTcpHandler.h"
#include "DNFTcpNetworkThread.h"
#include "DNFTcpSocket.h"
#include "DNFTerminateSig.h"
#include "DNFThreadInterface.h"
#include "DNFTickHandler.h"
#include "DNFUdpHandler.h"
#include "DNFUser.h"
#include "DNFUser1Sig.h"
#include "DNFUserManager.h"
#include "DNFVersion.h"
#include "GMAccounts.h"
#include "GuildBoard.h"
#include "GuildCargo.h"
#include "MemPool.h"
#include "MemoryCashManager.h"
#include "Peer.h"
#include "Power.h"
#include "PowerManager.h"
#include "PowerWar.h"
#include "PowerWarCharacInfo.h"
#include "PowerWarGuildInfo.h"
#include "Scheduler.h"
#include "ServerLoadChecker.h"
#include "ServerXml.h"
#include "SystemTimeHandler.h"
#include "TcpNetSystem.h"
#include "WebEvent.h"

template<int Lo, int Hi>
CPacketCounter<Lo, Hi>::CPacketCounter(char* name, char* title)
{
    Reset();
    *(time_t*)(m_data + 4) = time(0);
    if (name == 0)
    {
        sprintf(m_data + 0x1d540, "./log/%s", title);
    }
    else
    {
        sprintf(m_data + 0x1d540, "./log/%s/%s", name, title);
    }
    *(unsigned char*)(m_data + 0x1d640) = 1;
}

template<int Lo, int Hi>
CPacketCounter<Lo, Hi>::~CPacketCounter()
{
}

template<int Lo, int Hi>
void CPacketCounter<Lo, Hi>::IncrementPacketCount(int id)
{
    if (id < 0x2800 && 999 < id &&
        (m_data[0x1d640] == 1 ||
         *(unsigned int*)(m_data + 8 + (id - 1000) * 4) < 0xb))
    {
        *(unsigned int*)(m_data + 8 + (id - 1000) * 4) += 1;
    }
}

template<int Lo, int Hi>
void CPacketCounter<Lo, Hi>::BeforeProcess()
{
    *(int*)(m_data + 0x9068) = *(int*)m_data;
    if (*(int*)(m_data + 0x9068) == -1)
    {
        *(int*)(m_data + 0x9068) = 0;
    }
}

template<int Lo, int Hi>
void CPacketCounter<Lo, Hi>::AfterProcess(int id)
{
    if (id < 0x2800 && 999 < id &&
        (m_data[0x1d640] == 1 ||
         *(unsigned int*)(m_data + 8 + (id - 1000) * 4) < 0xb) &&
        *(int*)m_data != -1)
    {
        int prev;
        if (m_data[0x1d640] == 0)
        {
            prev = *(int*)(m_data + 0x9068);
            *(int*)(m_data + 8 + (id - 1000) * 4) += 1;
            m_data[0x11ce0 + id] = 0;
        }
        else
        {
            prev = *(int*)(m_data + 0x9068);
        }
        int diff = *(int*)m_data - prev;
        *(int*)(m_data + (id + 0x4d50) * 4) += diff;
    }
}

template<int Lo, int Hi>
void CPacketCounter<Lo, Hi>::WriteLog()
{
}

template<int Lo, int Hi>
void CPacketCounter<Lo, Hi>::Reset()
{
    memset(m_data, 0, sizeof(m_data));
}

template class CPacketCounter<1000, 10240>;

#pragma pack(push,1)
struct Packet_DB_Request_Approve_Join_Guild_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned int me;
    unsigned int m12;
    unsigned char m16;
};
#pragma pack(pop)
Packet_DB_Request_Approve_Join_Guild::Packet_DB_Request_Approve_Join_Guild()
    : PacketHeader(0x1bc4, 0x17)
{
    ((Packet_DB_Request_Approve_Join_Guild_Layout*)this)->ma = 0;
    ((Packet_DB_Request_Approve_Join_Guild_Layout*)this)->me = 0;
    ((Packet_DB_Request_Approve_Join_Guild_Layout*)this)->m12 = 0;
    ((Packet_DB_Request_Approve_Join_Guild_Layout*)this)->m16 = 0;
}

Packet_Guild_Attendance_Info_Reply::Packet_Guild_Attendance_Info_Reply()
    : PacketHeader(0x1bc7, 0x2f)
{
    *(unsigned int*)((char*)this + 0x10) = 0;
    *(unsigned int*)((char*)this + 0xe) = 0;
    *(unsigned char*)((char*)this + 0x12) = 0x0;
    new ((char*)this + 0x13) STAttendanceInfo;
}

Packet_Guild_Reply_Guild_Board::Packet_Guild_Reply_Guild_Board()
    : PacketHeader(0x2328, 0x68a)
{
    for (int i = 0; i < 9; ++i)
        new ((char*)this + 0x18 + i * 0xa5) STGuildBoardDBInfo;
    *(unsigned short*)((char*)this + 0xa) = 0;
    *(unsigned short*)((char*)this + 0xc) = 0;
    *(unsigned char*)((char*)this + 0xe) = 0x0;
    *(unsigned int*)((char*)this + 0xf) = 0;
    *(unsigned int*)((char*)this + 0x13) = 0;
    *(unsigned char*)((char*)this + 0x17) = 0x0;
    memset((char*)this + 0x18, 0, 0x672);
}

#pragma pack(push,1)
struct Packet_DB_Load_Request_Guild_Board_Open_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned int me;
};
#pragma pack(pop)
Packet_DB_Load_Request_Guild_Board_Open::Packet_DB_Load_Request_Guild_Board_Open()
    : PacketHeader(0x232b, 0x12)
{
    ((Packet_DB_Load_Request_Guild_Board_Open_Layout*)this)->ma = 0;
    ((Packet_DB_Load_Request_Guild_Board_Open_Layout*)this)->me = 0;
}

Packet_DB_Load_Request_Guild_Board_Write::Packet_DB_Load_Request_Guild_Board_Write()
    : PacketHeader(0x232f, 0xbc)
{
    new ((char*)this + 0x17) STGuildBoardDBInfo;
    *(unsigned char*)((char*)this + 0x10) = 0x0;
    *(unsigned int*)((char*)this + 0xb) = 0;
    *(unsigned int*)((char*)this + 0xf) = 0;
    *(unsigned int*)((char*)this + 0x13) = 0;
}

#pragma pack(push,1)
struct Packet_DB_Load_Request_Guild_Board_Delete_Layout
{
    char pad0x0[0xa];
    unsigned char ma;
    unsigned int mb;
    unsigned int mf;
    unsigned int m13;
};
#pragma pack(pop)
Packet_DB_Load_Request_Guild_Board_Delete::Packet_DB_Load_Request_Guild_Board_Delete()
    : PacketHeader(0x2333, 0x17)
{
    ((Packet_DB_Load_Request_Guild_Board_Delete_Layout*)this)->ma = 0;
    ((Packet_DB_Load_Request_Guild_Board_Delete_Layout*)this)->mb = 0;
    ((Packet_DB_Load_Request_Guild_Board_Delete_Layout*)this)->mf = 0;
    ((Packet_DB_Load_Request_Guild_Board_Delete_Layout*)this)->m13 = 0;
}

#pragma pack(push,1)
struct Packet_Guild_Reply_Guild_Board_Delete_Layout
{
    char pad0x0[0xa];
    unsigned short ma;
    unsigned int mc;
    unsigned int m10;
};
#pragma pack(pop)
Packet_Guild_Reply_Guild_Board_Delete::Packet_Guild_Reply_Guild_Board_Delete()
    : PacketHeader(0x2332, 0x14)
{
    ((Packet_Guild_Reply_Guild_Board_Delete_Layout*)this)->ma = 0;
    ((Packet_Guild_Reply_Guild_Board_Delete_Layout*)this)->mc = 0;
    ((Packet_Guild_Reply_Guild_Board_Delete_Layout*)this)->m10 = 0;
}

#pragma pack(push,1)
struct Packet_DB_Load_Request_Web_Guild_Board_Write_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned int me;
    unsigned int m12;
};
#pragma pack(pop)
Packet_DB_Load_Request_Web_Guild_Board_Write::Packet_DB_Load_Request_Web_Guild_Board_Write()
    : PacketHeader(0x233e, 0x16)
{
    ((Packet_DB_Load_Request_Web_Guild_Board_Write_Layout*)this)->ma = 0;
    ((Packet_DB_Load_Request_Web_Guild_Board_Write_Layout*)this)->me = 0;
    ((Packet_DB_Load_Request_Web_Guild_Board_Write_Layout*)this)->m12 = 0;
}
