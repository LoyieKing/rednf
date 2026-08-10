// df_guild_r — GuildCargo (split from source/guild per ORIG layout)
#include <string.h>
#include <time.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>
#include "tinyxml.h"
#include "Packet_Monitor_Call_Guild_Members_ToChannel.h"
#include "Packet_Monitor_Call_Guild_Members_ToChannel_Next.h"
#include "Packet_Monitor_Call_Guild_All_Members_ToChannel.h"
#include "Packet_Monitor_Call_Guild_All_Members_ToChannel_Next.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

#include "GuildCargo.h"
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
#include "MemPool.h"
#include "MemoryCashManager.h"
#include "PacketCounter.h"
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

STGuildCargoLog::STGuildCargoLog()
{
    memset((char*)this, 0, 0x30);
}

STGuildCargoDBInfo::STGuildCargoDBInfo()
{
    for (int i = 0; i < 0x78; i++) { new ((char*)this + 0x0 + i * 0x35) DnfItemInfo; }
    *(unsigned int*)((char*)this + 0x18d8) = 0;
}

bool CGuildCargo::IsLoadComplete()
{
    return *(char*)((char*)this + 0x18e4) != 0;
}

void CGuildCargo::SetCapacity(unsigned int capacity)
{
    *(unsigned int*)((char*)this + 0x18d8) = capacity;
}

int CGuildCargo::GetCapacity()
{
    return *(int*)((char*)this + 0x18d8);
}

void* CGuildCargo::GetGuildCargoDBInfo()
{
    return this;
}

CGuildCargo::CGuildCargo()
{
    new (m_data) STGuildCargoDBInfo();
    new (m_data + 0x18e8) std::deque<STGuildCargoLog>();
    Reset();
}

CGuildCargo::~CGuildCargo()
{
    Reset();
    ((std::deque<STGuildCargoLog>*)(m_data + 0x18e8))->~deque();
}

void CGuildCargo::Reset()
{
    *(int*)((char*)this + 0x18d8) = 0;
    *(int*)((char*)this + 0x18dc) = 0;
    *(int*)((char*)this + 0x18e0) = 0;
    *(char*)((char*)this + 0x18e4) = 0;
    ((std::deque<STGuildCargoLog>*)(m_data + 0x18e8))->clear();
    memset(m_data, 0, 0x18d8);
}

void CGuildCargo::SetGuildInfo(int guildKey)
{
    *(int*)((char*)this + 0x18e0) = guildKey;
}

int CGuildCargo::IsValidSlot(int slot)
{
    return !(slot < 0 || *(int*)((char*)this + 0x18d8) <= slot || 0x77 < slot);
}

int CGuildCargo::AddItem(DnfItemInfo& info, int slot, int count)
{
    if (*(int*)((char*)this + slot * 0x35 + 1) == 0)
    {
        memcpy((char*)this + slot * 0x35, &info, 0x35);
        *(int*)((char*)this + 0x18dc) += 1;
        DNF_LOG_SCOPE_LINE(0x4b,"./log/GuildCargo",
            "AddItem SUCCESS - GUILD:%d, CHARAC:%d, ITEM:%d, SLOT:%d",
            *(int*)((char*)this + 0x18e0), count, *(int*)((char*)&info + 1), slot);
        return 0xc1;
    }
    DNF_LOG_SCOPE_LINE(0x54,"./log/GuildCargo",
        "AddItem ITEM ALREADY EXIST - GUILD:%d, CHARAC:%d, INSERT ITEM:%d, SLOT:%d, AREADY ITEM:%d",
        *(int*)((char*)this + 0x18e0), count, *(int*)((char*)&info + 1), slot,
        *(int*)((char*)this + slot * 0x35 + 1));
    return 0xc9;
}

int CGuildCargo::CalcItemCount()
{
    *(int*)((char*)this + 0x18dc) = 0;
    for (int i = 0; i < 0x78; i++)
    {
        if (*(int*)((char*)this + i * 0x35 + 1) != 0)
        {
            *(int*)((char*)this + 0x18dc) += 1;
        }
    }
    return *(int*)((char*)this + 0x18dc);
}

int CGuildCargo::InsertItem(DnfItemInfo& info, int& slot, int count, unsigned char a, int b)
{
    if (IsValidSlot(slot) != 1)
    {
        return 0xc4;
    }
    if (a == 1)
    {
        int existingSlot = GetSpecificItemSlot(*(int*)((char*)&info + 1));
        if (existingSlot != -1)
        {
            int oldCount = *(int*)((char*)this + existingSlot * 0x35 + 6);
            int addCount = *(int*)((char*)&info + 6);
            if (oldCount + addCount <= count)
            {
                *(int*)((char*)this + existingSlot * 0x35 + 6) += addCount;
                DNF_LOG_SCOPE_LINE(0x89,"./log/GuildCargo",
                    "InsertItem STACKABLE ADD SUCCESS - GUILD:%d, CHARAC:%d, ITEM:%d, OLD:%d, ADD:%d, CURR:%d",
                    *(int*)((char*)this + 0x18e0), b, *(int*)((char*)&info + 1),
                    oldCount, addCount, *(int*)((char*)this + existingSlot * 0x35 + 6));
                slot = existingSlot;
                return 0xc1;
            }
            DNF_LOG_SCOPE_LINE(0x96,"./log/GuildCargo",
                "InsertItem STACKABLE ADD OVER STACK LIMIT - GUILD:%d, CHARAC:%d, ITEM:%d, OLD:%d, ADD:%d, LMT:%d, CURR:%d",
                *(int*)((char*)this + 0x18e0), b, *(int*)((char*)&info + 1),
                oldCount, addCount, count, *(int*)((char*)this + existingSlot * 0x35 + 6));
            slot = existingSlot;
            return 200;
        }
    }
    return AddItem(info, slot, b);
}

int CGuildCargo::CheckInsertItem(int itemId, int count, int slot, unsigned char stackable,
                                 int maxStack)
{
    if (IsValidSlot(slot) != 1)
    {
        return 0xc4;
    }
    if (stackable == 1)
    {
        int existingSlot = GetSpecificItemSlot(itemId);
        if (existingSlot != -1)
        {
            if (maxStack < *(int*)((char*)this + existingSlot * 0x35 + 6) + count)
            {
                return 200;
            }
            return 0xc1;
        }
    }
    if (*(int*)((char*)this + slot * 0x35 + 1) == 0)
    {
        return 0xc1;
    }
    return 0xc9;
}

int CGuildCargo::DeleteItem(DnfItemInfo& info, int slot, int count, unsigned char a, int b, int c)
{
    if (IsValidSlot(slot) != 1)
    {
        return 0xc4;
    }
    if (*(int*)((char*)this + slot * 0x35 + 1) == 0 ||
        *(int*)((char*)this + slot * 0x35 + 1) != b)
    {
        return 0xca;
    }
    memcpy(&info, (char*)this + slot * 0x35, 0x35);
    if (a == 1)
    {
        *(int*)((char*)&info + 6) = count;
        int oldCount = *(int*)((char*)this + slot * 0x35 + 6);
        int subCount = *(int*)((char*)&info + 6);
        if (oldCount < subCount)
        {
            return 199;
        }
        *(int*)((char*)this + slot * 0x35 + 6) -= subCount;
        if (*(int*)((char*)this + slot * 0x35 + 6) == 0)
        {
            ((DnfItemInfo*)((char*)this + slot * 0x35))->reset();
        }
        DNF_LOG_SCOPE_LINE(0xee,"./log/GuildCargo",
            "DeleteItem STACKABLE DELETE SUCCESS(Stackable) - GUILD:%d, CHARAC:%d, ITEM:%d, OLD:%d, SUB:%d, CURR:%d",
            *(int*)((char*)this + 0x18e0), c, *(int*)((char*)&info + 1),
            oldCount, subCount, *(int*)((char*)this + slot * 0x35 + 6));
    }
    else
    {
        ((DnfItemInfo*)((char*)this + slot * 0x35))->reset();
        DNF_LOG_SCOPE_LINE(0xfa,"./log/GuildCargo",
            "DeleteItem STACKABLE DELETE SUCCESS(Equip) - GUILD:%d, CHARAC:%d, ITEM:%d",
            *(int*)((char*)this + 0x18e0), c, *(int*)((char*)&info + 1));
    }
    return 0xc1;
}

int CGuildCargo::MoveItem(DnfItemInfo& from, DnfItemInfo& to, int fromSlot, int fromItemId,
                          int toSlot, int toItemId, int charNo)
{
    if (!(IsValidSlot(fromSlot) == 1 && IsValidSlot(toSlot) == 1 && fromSlot != toSlot))
    {
        return 0xc4;
    }
    int guildKey = *(int*)((char*)this + 0x18e0);
    int fromId = *(int*)((char*)this + fromSlot * 0x35 + 1);
    int toId = *(int*)((char*)this + toSlot * 0x35 + 1);
    {
        DNF_LOG_SCOPE_AT("MoveItem", 0x115,"./log/GuildCargo",
            "Before MoveItem - GUILD:%d, CHARAC:%d, SLOT1:(%d,%d), SLOT2:(%d,%d)",
            guildKey, charNo, fromSlot, fromId, toSlot, toId);
    }
    memcpy(&from, (char*)this + fromSlot * 0x35, 0x35);
    memcpy(&to, (char*)this + toSlot * 0x35, 0x35);
    if (*(int*)((char*)&from + 1) == fromItemId && *(int*)((char*)&to + 1) == toItemId)
    {
        DnfItemInfo tmp = from;
        memcpy((char*)this + fromSlot * 0x35, (char*)this + toSlot * 0x35, 0x35);
        memcpy((char*)this + toSlot * 0x35, &tmp, 0x35);
        int newFromId = *(int*)((char*)this + fromSlot * 0x35 + 1);
        int newToId = *(int*)((char*)this + toSlot * 0x35 + 1);
        {
            DNF_LOG_SCOPE_AT("MoveItem", 0x131,"./log/GuildCargo",
                "After MoveItem - GUILD:%d, CHARAC:%d, SLOT1:(%d,%d), SLOT2:(%d,%d)",
                guildKey, charNo, fromSlot, newFromId, toSlot, newToId);
        }
        return 0xc1;
    }
    return 0xca;
}

int CGuildCargo::GetSpecificItemSlot(int itemId)
{
    int cap = *(int*)((char*)this + 0x18d8);
    for (int i = 0; i < cap; i++)
    {
        if (*(int*)((char*)this + i * 0x35 + 1) == itemId)
        {
            return i;
        }
    }
    return -1;
}

void CGuildCargo::SetGuildCargoDBInfo(STGuildCargoDBInfo& info)
{
}

void CGuildCargo::SetGuildCargoHistory(unsigned int idx, STGuildCargoLog* log)
{
}

void CGuildCargo::SendGuildCargo(CUser* user)
{
    if (user == 0)
    {
        return;
    }
    Packet_Notice_Guild_Cargo pkt;
    *(unsigned int*)((char*)&pkt + 0xa) = user->GetIdByChannel();
    *(unsigned int*)((char*)&pkt + 0xe) = user->GetUniqCharNo();
    memcpy((char*)&pkt + 0x12, this, 0x18dc);
    user->SendTcpGameserver(&pkt);
}

void CGuildCargo::PrintCargo(ENUM_GUILD_CARGO_BEHAVIOR behavior)
{
    CMyFileLog log0("PrintCargo", 0x18d);
    log0("./log/GuildCargo", "CARGO - g:%d,capa:%d,behavior:%d",
         *(int*)((char*)this + 0x18e0), *(int*)((char*)this + 0x18d8),
         (int)behavior);
    for (int i = 0; i < *(int*)((char*)this + 0x18d8); i++)
    {
        if (*(int*)((char*)this + i * 0x35 + 1) != 0)
        {
            const char* itemDesc = PrintDnfItemInfo(*(DnfItemInfo*)((char*)this + i * 0x35));
            DNF_LOG_SCOPE_LINE(0x195, "./log/GuildCargo", "SLOT - %d,%s", i, itemDesc);
        }
    }
}

const char* CGuildCargo::PrintDnfItemInfo(DnfItemInfo& info)
{
    static char szBuffer[0x400];
    memset(szBuffer, 0, sizeof(szBuffer));
    unsigned char up = ((UpgradeSeparateInfo*)((char*)&info + 0x2b))->GetUpgradeSeparate();
    sprintf(szBuffer,
            "id:%d,s:%d,sc:%d,up:%d,add:%d,en:%d,ex:%d,at:%d,av:%d,sp:%d",
            *(unsigned int*)((char*)&info + 1),
            (unsigned int)(unsigned char)*(char*)&info,
            (unsigned int)((unsigned char)((char*)&info)[5] >> 5),
            (unsigned int)((unsigned char)((char*)&info)[5] & 0x1f),
            *(unsigned int*)((char*)&info + 6),
            (unsigned int)*(unsigned short*)((char*)&info + 10),
            *(unsigned int*)((char*)&info + 0xc),
            (unsigned int)(unsigned char)((char*)&info)[0x10],
            (unsigned int)*(unsigned short*)((char*)&info + 0x11),
            (unsigned int)up & 0xff);
    return szBuffer;
}

void CGuildCargo::SendGuildCargoToDBMW(CServerHandler* handler, int slot)
{
    CTcpDBServer* db = handler->GetTcpDBServer();
    char* pkt = db->makePacketHeader(0x710, 0x18ea);
    *(unsigned int*)(pkt + 0xa) = *(unsigned int*)((char*)this + 0x18e0);
    *(int*)(pkt + 0xe) = slot;
    memcpy(pkt + 0x12, this, 0x18d8);
    db->SendToServer(pkt);
}

int CGuildCargo::IsEmpty()
{
    int cap = *(int*)((char*)this + 0x18d8);
    for (int i = 0; i < cap; i++)
    {
        if (*(int*)((char*)this + i * 0x35 + 1) != 0)
        {
            return 0;
        }
    }
    return 1;
}

void CGuildCargo::InsertHistory(ENUM_GUILD_CARGO_BEHAVIOR behavior, int slot, const char* name, int count,
                                int param, const RandomOption* option)
{
    STGuildCargoLog log;
    *(char*)((char*)&log + 4) = (char)behavior;
    strncpy((char*)&log + 5, name, 0x14);
    *(int*)((char*)&log + 0x19) = count;
    *(int*)((char*)&log + 0x1d) = slot;
    *(int*)((char*)&log + 0x21) = *(int*)((char*)option + 0);
    *(int*)((char*)&log + 0x25) = *(int*)((char*)option + 4);
    *(int*)((char*)&log + 0x29) = *(int*)((char*)option + 8);
    *(short*)((char*)&log + 0x2d) = *(short*)((char*)option + 0xc);
    log.time = (int)time(0);
    std::deque<STGuildCargoLog>* hist =
        (std::deque<STGuildCargoLog>*)((char*)this + 0x18e8);
    hist->push_back(log);
    CMyFileLog fileLog("InsertHistory", 0x202);
    fileLog("./log/GuildCargo",
            "InsertLog - GUILD:%d, CHARAC:%d, ITEM:%d, BEHAVIOR:%d, ETC:%d",
            *(int*)((char*)this + 0x18e0), slot, count, (int)behavior, param);
    if (0x32 < hist->size())
    {
        hist->pop_front();
    }
}

void CGuildCargo::GetHistory(STGuildCargoLog* out)
{
    std::deque<STGuildCargoLog>* hist =
        (std::deque<STGuildCargoLog>*)((char*)this + 0x18e8);
    int i = 0;
    for (std::deque<STGuildCargoLog>::iterator it = hist->begin();
         it != hist->end() && i < 0x33; ++it, ++i)
    {
        memcpy((char*)out + i * 0x30, &(*it), 0x30);
    }
}

void CGuildCargo::SendHistoryToDBMW(CServerHandler* handler, ENUM_GUILD_CARGO_BEHAVIOR behavior, int slot,
                                    const char* name, int count, int param, DnfItemInfo& info)
{
    Packet_DB_Insert_Guild_Cargo_History pkt;
    *(unsigned char*)((char*)&pkt + 0xa) = handler->GetServerGroupNo();
    *(int*)((char*)&pkt + 0xb) = *(int*)((char*)this + 0x18e0);
    *(int*)((char*)&pkt + 0xf) = slot;
    strncpy((char*)&pkt + 0x13, name, 0x14);
    *(unsigned char*)((char*)&pkt + 0x27) = (unsigned char)behavior;
    *(int*)((char*)&pkt + 0x28) = count;
    *(int*)((char*)&pkt + 0x2c) = param;
    memcpy((char*)&pkt + 0x30, &info, 0x35);
    handler->SendToDB(&pkt);
}

Packet_Notice_Guild_Cargo::Packet_Notice_Guild_Cargo()
    : PacketHeader(0x712, 0x18ee)
{
    *(unsigned int*)((char*)this + 0x10) = 0xffffffff;
    *(unsigned int*)((char*)this + 0xe) = 0;
    new ((char*)this + 0x12) STGuildCargoDBInfo;
    memset((char*)this + 0x12, 0, 0x18dc);
}

Packet_DB_Insert_Guild_Cargo_History::Packet_DB_Insert_Guild_Cargo_History()
    : PacketHeader(0x711, 0x66)
{
    *(unsigned char*)((char*)this + 0x10) = 0x0;
    *(unsigned int*)((char*)this + 0xb) = 0;
    *(unsigned int*)((char*)this + 0xf) = 0;
    *(unsigned char*)((char*)this + 0x28) = 0xff;
    *(unsigned int*)((char*)this + 0x29) = 0;
    *(unsigned int*)((char*)this + 0x2d) = 0;
    new ((char*)this + 0x31) DnfItemInfo;
    memset((char*)this + 0x13, 0, 0x15);
    memset((char*)this + 0x31, 0, 0x35);
}

STGuildCargoLog::~STGuildCargoLog()
{
}

