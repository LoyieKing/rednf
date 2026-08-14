// df_guild_r — GuildCargo (split from source/guild per ORIG layout)
// ORIG 的 GuildCargo TU 中 STGuildCargoLog 无用户析构（InsertHistory 局部对象
// 不调用 D1Ev、无 EH 清理块；D1Ev/D2Ev 弱符号由其它 TU 的头文件内联定义提供），
// 与 DNFFunctionLib.h 中声明析构的 TU 构成 ODR 差异——按二进制证据原样复现。
#define DNF_GUILD_ODR_TRIVIAL_CARGOLOG_DTOR
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

// 按 ORIG 布局的字段覆盖视图（避免 char* 算术产生 add 指令形态）
struct GuildCargoLayout
{
    char pad[0x18d8];
    int m_capacity;      // +0x18d8
    int m_field18dc;     // +0x18dc
    int m_guildKey;      // +0x18e0
    unsigned char m_load; // +0x18e4
    char rest[0x1910 - 0x18e8];
};

#pragma pack(push,1)
// 按 ORIG 布局的 DnfItemInfo 字段覆盖视图（+1 为 item_id，0x35 字节/项）
struct GuildCargoItemLayout
{
    unsigned char m_seal;          // +0x0
    unsigned int m_itemId;         // +0x1
    unsigned char m_attr;          // +0x5
    unsigned int m_addInfo;        // +0x6
    unsigned short m_endurance;    // +0xa
    unsigned int m_extendInfo;     // +0xc
    unsigned char m_abilityType;   // +0x10
    unsigned short m_abilityValue; // +0x11
    char m_rest[0x35 - 0x13];      // +0x13
};
#pragma pack(pop)

STGuildCargoLog::STGuildCargoLog()
{
    memset(this, 0, sizeof(STGuildCargoLog));
}

STGuildCargoDBInfo::STGuildCargoDBInfo()
{
    m_capacity = 0;
}

unsigned char CGuildCargo::IsLoadComplete()
{
    return ((GuildCargoLayout*)this)->m_load;
}

void CGuildCargo::SetCapacity(unsigned int capacity)
{
    ((GuildCargoLayout*)this)->m_capacity = (int)capacity;
}

int CGuildCargo::GetCapacity()
{
    return ((GuildCargoLayout*)this)->m_capacity;
}

void* CGuildCargo::GetGuildCargoDBInfo()
{
    return this;
}

CGuildCargo::CGuildCargo()
{
    Reset();
}

CGuildCargo::~CGuildCargo()
{
    Reset();
}

void CGuildCargo::Reset()
{
    m_info.m_capacity = 0;
    m_itemCount = 0;
    m_guildKey = 0;
    m_load = 0;
    m_history.clear();
    memset(&m_info, 0, 0x18d8);
}

void CGuildCargo::SetGuildInfo(int guildKey)
{
    ((GuildCargoLayout*)this)->m_guildKey = guildKey;
}

bool CGuildCargo::IsValidSlot(int slot)
{
    if (slot < 0 || ((GuildCargoLayout*)this)->m_capacity <= slot || 0x77 < slot)
    {
        return 0;
    }
    return 1;
}

int CGuildCargo::AddItem(DnfItemInfo& info, int slot, int count)
{
    if (m_info.m_items[slot].m_itemId == 0)
    {
        m_info.m_items[slot] = info;
        m_itemCount += 1;
        DNF_LOG_SCOPE_LINE(0x4b,"./log/GuildCargo",
            "AddItem SUCCESS - GUILD:%d, CHARAC:%d, ITEM:%d, SLOT:%d",
            m_guildKey, count, info.m_itemId, slot);
        return 0xc1;
    }
    DNF_LOG_SCOPE_LINE(0x54,"./log/GuildCargo",
        "AddItem ITEM ALREADY EXIST - GUILD:%d, CHARAC:%d, INSERT ITEM:%d, SLOT:%d, AREADY ITEM:%d",
        m_guildKey, count, info.m_itemId, slot,
        m_info.m_items[slot].m_itemId);
    return 0xc9;
}

int CGuildCargo::CalcItemCount()
{
    m_itemCount = 0;
    for (int i = 0; i < 0x78; i++)
    {
        if (m_info.m_items[i].m_itemId != 0)
        {
            m_itemCount = m_itemCount + 1;
        }
    }
    return m_itemCount;
}

int CGuildCargo::InsertItem(DnfItemInfo& info, int& slot, int count, unsigned char a, int b)
{
    if (!IsValidSlot(slot))
    {
        return 0xc4;
    }
    if (a == 1)
    {
        int existingSlot = GetSpecificItemSlot(info.m_itemId);
        if (existingSlot != -1)
        {
            int oldCount = m_info.m_items[existingSlot].m_addInfo;
            int addCount = info.m_addInfo;
            if (oldCount + addCount <= count)
            {
                m_info.m_items[existingSlot].m_addInfo =
                    m_info.m_items[existingSlot].m_addInfo + info.m_addInfo;
                DNF_LOG_SCOPE_LINE(0x89,"./log/GuildCargo",
                    "InsertItem STACKABLE ADD SUCCESS - GUILD:%d, CHARAC:%d, ITEM:%d, OLD:%d, ADD:%d, CURR:%d",
                    m_guildKey, b, info.m_itemId,
                    oldCount, addCount, m_info.m_items[existingSlot].m_addInfo);
                slot = existingSlot;
                return 0xc1;
            }
            DNF_LOG_SCOPE_LINE(0x96,"./log/GuildCargo",
                "InsertItem STACKABLE ADD OVER STACK LIMIT - GUILD:%d, CHARAC:%d, ITEM:%d, OLD:%d, ADD:%d, LMT:%d, CURR:%d",
                m_guildKey, b, info.m_itemId,
                oldCount, addCount, count, m_info.m_items[existingSlot].m_addInfo);
            slot = existingSlot;
            return 200;
        }
    }
    int ret = AddItem(info, slot, b);
    return ret;
}

int CGuildCargo::CheckInsertItem(int itemId, int count, int slot, unsigned char stackable,
                                 int maxStack)
{
    if (!IsValidSlot(slot))
    {
        return 0xc4;
    }
    if (stackable == 1)
    {
        int existingSlot = GetSpecificItemSlot(itemId);
        if (existingSlot != -1)
        {
            if ((int)m_info.m_items[existingSlot].m_addInfo + count > maxStack)
            {
                return 200;
            }
            return 0xc1;
        }
    }
    if (m_info.m_items[slot].m_itemId != 0)
    {
        return 0xc9;
    }
    return 0xc1;
}

int CGuildCargo::DeleteItem(DnfItemInfo& info, int slot, int count, unsigned char a, int b, int c)
{
    if (!IsValidSlot(slot))
    {
        return 0xc4;
    }
    if (m_info.m_items[slot].m_itemId == 0 ||
        m_info.m_items[slot].m_itemId != count)
    {
        return 0xca;
    }
    info = m_info.m_items[slot];
    if (a == 1)
    {
        info.m_addInfo = b;
        int oldCount = m_info.m_items[slot].m_addInfo;
        int subCount = info.m_addInfo;
        if (oldCount < subCount)
        {
            return 199;
        }
        m_info.m_items[slot].m_addInfo =
            m_info.m_items[slot].m_addInfo - info.m_addInfo;
        if (m_info.m_items[slot].m_addInfo == 0)
        {
            m_info.m_items[slot].reset();
        }
        DNF_LOG_SCOPE_LINE(0xee,"./log/GuildCargo",
            "DeleteItem STACKABLE DELETE SUCCESS(Stackable) - GUILD:%d, CHARAC:%d, ITEM:%d, OLD:%d, SUB:%d, CURR:%d",
            m_guildKey, c, info.m_itemId,
            oldCount, subCount, m_info.m_items[slot].m_addInfo);
    }
    else
    {
        m_info.m_items[slot].reset();
        DNF_LOG_SCOPE_LINE(0xfa,"./log/GuildCargo",
            "DeleteItem STACKABLE DELETE SUCCESS(Equip) - GUILD:%d, CHARAC:%d, ITEM:%d",
            m_guildKey, c, info.m_itemId);
    }
    return 0xc1;
}

int CGuildCargo::MoveItem(DnfItemInfo& from, DnfItemInfo& to, int fromSlot, int fromItemId,
                          int toSlot, int toItemId, int charNo)
{
    if (!IsValidSlot(fromSlot) || !IsValidSlot(toSlot) || fromSlot == toSlot)
    {
        return 0xc4;
    }
    DNF_LOG_SCOPE_AT(__FUNCTION__, 0x115,"./log/GuildCargo",
        "Before MoveItem - GUILD:%d, CHARAC:%d, SLOT1:(%d,%d), SLOT2:(%d,%d)",
        m_guildKey, charNo, fromSlot, m_info.m_items[fromSlot].m_itemId,
        toSlot, m_info.m_items[toSlot].m_itemId);
    from = m_info.m_items[fromSlot];
    to = m_info.m_items[toSlot];
    if (from.m_itemId != fromItemId || to.m_itemId != toItemId)
    {
        return 0xca;
    }
    {
        DnfItemInfo tmp;
        tmp = m_info.m_items[fromSlot];
        m_info.m_items[fromSlot] = m_info.m_items[toSlot];
        m_info.m_items[toSlot] = tmp;
        DNF_LOG_SCOPE_AT(__FUNCTION__, 0x131,"./log/GuildCargo",
            "After MoveItem - GUILD:%d, CHARAC:%d, SLOT1:(%d,%d), SLOT2:(%d,%d)",
            m_guildKey, charNo, fromSlot, m_info.m_items[fromSlot].m_itemId,
            toSlot, m_info.m_items[toSlot].m_itemId);
        return 0xc1;
    }
    return 0xca;
}

int CGuildCargo::GetSpecificItemSlot(int itemId)
{
    for (int i = 0; (int)m_info.m_capacity > i; i++)
    {
        if (m_info.m_items[i].m_itemId == itemId)
        {
            return i;
        }
    }
    return -1;
}

void CGuildCargo::SetGuildCargoDBInfo(STGuildCargoDBInfo& info)
{
    memcpy(&m_info, &info, sizeof(m_info));
    ((GuildCargoLayout*)this)->m_load = 1;
}

void CGuildCargo::SetGuildCargoHistory(unsigned int idx, STGuildCargoLog* log)
{
    int c = idx;
    if (idx > 0x32)
    {
        c = 0x32;
    }
    for (int i = 0; i < c; i++)
    {
        m_history.push_front(log[i]);
    }
}

void CGuildCargo::SendGuildCargo(CUser* user)
{
    if (user == 0)
    {
        return;
    }
    Packet_Notice_Guild_Cargo pkt;
    pkt.m_a = user->GetIdByChannel();
    pkt.m_b = user->GetUniqCharNo();
    memcpy(&pkt.m_cargo, this, sizeof(STGuildCargoDBInfo));
    user->SendTcpGameserver(&pkt);
}

void CGuildCargo::PrintCargo(ENUM_GUILD_CARGO_BEHAVIOR behavior)
{
    DNF_LOG_SCOPE_LINE(0x18d, "./log/GuildCargo", "CARGO - g:%d,capa:%d,behavior:%d",
         m_guildKey, m_info.m_capacity, (int)behavior);
    for (int i = 0; (int)m_info.m_capacity > i; i++)
    {
        if (m_info.m_items[i].m_itemId != 0)
        {
            DNF_LOG_SCOPE_LINE(0x195, "./log/GuildCargo", "SLOT - %d,%s",
                i, PrintDnfItemInfo(m_info.m_items[i]));
        }
    }
}

const char* CGuildCargo::PrintDnfItemInfo(DnfItemInfo& info)
{
    static char szBuffer[0x400];
    memset(szBuffer, 0, sizeof(szBuffer));
    sprintf(szBuffer,
            "id:%d,s:%d,sc:%d,up:%d,add:%d,en:%d,ex:%d,at:%d,av:%d,sp:%d",
            info.m_itemId,
            (unsigned int)(unsigned char)info.m_seal,
            (unsigned int)(unsigned char)(info.m_attr >> 5),
            (unsigned int)(unsigned char)(info.m_attr & 0x1f),
            info.m_addInfo,
            (unsigned int)info.m_endurance,
            info.m_extendInfo,
            (unsigned int)(unsigned char)info.m_abilityType,
            (unsigned int)info.m_abilityValue,
            (unsigned int)info.m_up.GetUpgradeSeparate());
    return szBuffer;
}

void CGuildCargo::SendGuildCargoToDBMW(CServerHandler* handler, int slot)
{
    CTcpDBServer* db = handler->GetTcpDBServer();
    struct GuildCargoToDBMWPkt {
        char hdr[0xa];
        unsigned int guildKey;
        int slot;
        char cargo[0x18d8];
    } __attribute__((packed));
    GuildCargoToDBMWPkt* p = (GuildCargoToDBMWPkt*)db->makePacketHeader(0x710, 0x18ea);
    p->guildKey = m_guildKey;
    p->slot = slot;
    memcpy(p->cargo, this, 0x18d8);
    db->SendToServer((char*)p);
}

bool CGuildCargo::IsEmpty()
{
    for (int i = 0; (int)m_info.m_capacity > i; i++)
    {
        if (m_info.m_items[i].m_itemId != 0)
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
    strncpy(log.name, name, 0x14);
    log.count = count;
    log.behavior = (unsigned char)behavior;
    log.param = param;
    register const RandomOption* o = option;
    log.opt0 = o->m_opt0;
    log.opt1 = o->m_opt1;
    log.opt2 = o->m_opt2;
    log.opt3 = o->m_opt3;
    time_t t;
    time(&t);
    log.time = t;
    m_history.push_back(log);
    DNF_LOG_SCOPE_LINE(0x202, "./log/GuildCargo",
            "InsertLog - GUILD:%d, CHARAC:%d, ITEM:%d, BEHAVIOR:%d, ETC:%d",
            m_guildKey, slot, count, (int)(char)behavior, param);
    if (0x32 < m_history.size())
    {
        m_history.pop_front();
    }
}

void CGuildCargo::GetHistory(STGuildCargoLog* out)
{
    std::deque<STGuildCargoLog>::iterator it = m_history.begin();
    int i = 0;
    while (it != m_history.end())
    {
        memcpy(out + i, &(*it), 0x30);
        ++it;
        ++i;
        if (i > 0x32)
        {
            break;
        }
    }
}

void CGuildCargo::SendHistoryToDBMW(CServerHandler* handler, ENUM_GUILD_CARGO_BEHAVIOR behavior, int slot,
                                    const char* name, int count, int param, DnfItemInfo& info)
{
    Packet_DB_Insert_Guild_Cargo_History pkt;
    pkt.m_b = m_guildKey;
    pkt.m_c = slot;
    pkt.m_d = (unsigned char)behavior;
    pkt.m_e = count;
    pkt.m_f = param;
    pkt.m_a = handler->GetServerGroupNo();
    strncpy(pkt.m_pad, name, 0x14);
    memcpy(&pkt.m_item, &info, 0x35);
    handler->SendToDB(&pkt);
}

Packet_Notice_Guild_Cargo::Packet_Notice_Guild_Cargo()
    : PacketHeader(0x712, 0x18ee), m_a(0xffffffff), m_b(0)
{
    memset(&m_cargo, 0, sizeof(m_cargo));
}

Packet_DB_Insert_Guild_Cargo_History::Packet_DB_Insert_Guild_Cargo_History()
    : PacketHeader(0x711, 0x66), m_a(0), m_b(0), m_c(0), m_d(0xff), m_e(0), m_f(0)
{
    memset(m_pad, 0, 0x15);
    memset(&m_item, 0, 0x35);
}
