// df_guild_r — DNFUser (split from source/guild per ORIG layout)
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
#include <stdio.h>
#include <utility>
#include <stdlib.h>
#include <sys/time.h>
#include <new>

#include "DNFUser.h"
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
#include "DNFUser1Sig.h"
#include "DNFUserManager.h"
#include "DNFVersion.h"
#include "GMAccounts.h"
#include "GuildBoard.h"
#include "GuildCargo.h"
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

#pragma pack(push,1)
struct STGuildMemerDBInfo_Layout
{
    char pad0x0[0x15];
    unsigned char m15;
    unsigned int m16;   // +0x16（CUser 内 +0x60）
};
#pragma pack(pop)

struct CUserGuildPointLayout
{
    char pad[0x60];
    unsigned int m_guildPoint;   // +0x60
};

typedef std::map<unsigned int, CBlackUser*> UserBlackMap;

#pragma pack(push,1)
struct PktSetGuildKeyLayout
{
    char pad0x0[0xa];
    int m_channel;          // +0xa
    unsigned int m_grade;   // +0xe
    unsigned int m_guildKey;// +0x12
};
struct PktSendAllUserInfoLayout
{
    char pad0x0[0xa];
    unsigned int m_guildKey; // +0xa
    unsigned int m_charNo;   // +0xe
    int m_channel;           // +0x12
};
struct NoticeGuildMemberInfoLayout
{
    unsigned short m_packetId;    // +0
    unsigned short m_packetSize;  // +2
    char pad0x4[6];               // +4..+9
    int m_channel;                // +0xa
    unsigned int m_charNo;        // +0xe
    char m_info[0x1a];            // +0x12
};
#pragma pack(pop)

void* CUser::operator new(unsigned int size)
{
    return m_UsermemPool_.alloc();
}

void CUser::operator delete(void* p)
{
    m_UsermemPool_.free(p);
}

void CUser::operator delete(void* p, unsigned int size)
{
    m_UsermemPool_.free(p, size);
}

MemPool<CUser> CUser::m_UsermemPool_(28000);

CUser::CUser()
    : m_dbid(0), m_charNo(0), m_gameServer(0), m_tcpGameServer(0), m_posState(0),
      m_channel(-1), m_job(0xff), m_growthType(0xff), m_guildMemFlag(0xffff),
      m_field3a(1), m_guild(0), m_field48(0), m_guildDBInfo(), m_blackList(),
      m_field7c(0), m_field7e(0x7f), m_field80(0), m_field84(0)
{
    memset(m_charInfo, 0, sizeof(m_charInfo));
    memset(m_field3b, 0, sizeof(m_field3b));
}

CUser::~CUser()
{
    m_gameServer = 0;
    m_tcpGameServer = 0;
    m_dbid = 0;
    m_charNo = 0;
    m_channel = -1;
    ResetGuild();
    m_job = 0xff;
    m_growthType = 0xff;
    m_guildMemFlag = 0xffff;
    memset(m_charInfo, 0, sizeof(m_charInfo));
    m_posState = 0;
    m_field7c = 0;
    m_field3a = 1;
}

void CUser::QueryGuildMember(CServerHandler* handler)
{
    if (m_gameServer != 0)
    {
        handler->QueryGuildMember((unsigned char)m_gameServer->GetGroupNo(), m_charNo);
        m_field48 |= 2;
    }
}

void CUser::LoadGuildMember(unsigned int guildKey, STGuildMemerDBInfo& info)
{
    if ((m_field48 & 8) != 0)
    {
        return;
    }
    if (m_guild == 0 || m_guild->GetGuildKey() != guildKey)
    {
        return;
    }
    if ((m_field48 & 2) != 0)
    {
        memcpy(&m_guildDBInfo, &info, 0x1a);
        m_field48 |= 4;
        if (m_guild->GetMasterId() == GetUniqCharNo() && m_guildDBInfo.m_data[0x15] != 1)
        {
            register int grade = (unsigned char)m_guildDBInfo.m_data[0x15];
            register unsigned int charNo = GetUniqCharNo();
            CMyFileLog log(__FUNCTION__, 0x8d);
            log("./log/GuildMember", "[Master_Err]\tGuild_K(%d)\tChar_No(%d)\tGrade(%d)", guildKey,
                charNo, grade);
            m_guildDBInfo.m_data[0x15] = 1;
        }
    }
}

void CUser::AddGuildMemberPoint(unsigned int point)
{
    unsigned int old = ((CUserGuildPointLayout*)this)->m_guildPoint;
    ((CUserGuildPointLayout*)this)->m_guildPoint =
        ((CUserGuildPointLayout*)this)->m_guildPoint + point;
    SetGuildMemFlag(0x10);
    if (((CUserGuildPointLayout*)this)->m_guildPoint < old)
    {
        ((CUserGuildPointLayout*)this)->m_guildPoint = old;
    }
}

void CUser::SaveGuildMember(unsigned char type, unsigned int value, CServerHandler* handler,
                            unsigned char flag)
{
    if (!IsSetGuildMemFlag(0x10))
    {
        return;
    }
    if (!IsSetGuildMemFlag(4))
    {
        return;
    }
    Packet_Monitor_SAVE_Guild_Member pkt;
    pkt.m_flag = type;
    pkt.m_b = value;
    pkt.m_charNo = GetUniqCharNo();
    memcpy(&pkt.m_info, &m_guildDBInfo, 0x1a);
    pkt.m_pad2d = flag;
    handler->SendToDB(&pkt);
    ResetGuildMemFlag(0x10);
}

void CUser::ResetGuildPoint()
{
    ((CUserGuildPointLayout*)this)->m_guildPoint = 0;
}

void CUser::ResetCharInfo()
{
    m_charNo = 0;
    ResetGuild();
    m_job = 0xff;
    m_growthType = 0xff;
    m_guildMemFlag = 0xffff;
    memset(m_charInfo, 0, 0x1e);
}

void CUser::AttachGuild(CGuild* guild)
{
    m_guild = guild;
}

void CUser::ResetGuild()
{
    DetachGuild();
    memset((void*)&m_guildDBInfo, 0, 0x1a);
    m_field48 = 0;
    m_field7e = 0x7f;
    m_field80 = 0;
    m_field84 = 0;
}

void CUser::SendToGameserver(char* buf, int len)
{
    if (m_gameServer != 0)
    {
        m_gameServer->SendToServer(buf, len);
    }
}

void CUser::SendTcpGameserver(PacketHeader* pkt)
{
    if (m_tcpGameServer != 0)
    {
        char* out = m_tcpGameServer->makePacketHeader(
            pkt->packetId, pkt->packetSize);
        if (out != 0)
        {
            memcpy(out + 10, (char*)pkt + 10, pkt->packetSize - 10);
            m_tcpGameServer->SendToGameServer(out);
        }
    }
}

void CUser::SetUserInfo_CharNo(char sex, char job, short flag, unsigned int charNo, char* name)
{
    m_job = (unsigned char)sex;
    m_growthType = (unsigned char)job;
    m_guildMemFlag = (unsigned short)flag;
    m_charNo = charNo;
    memcpy(m_charInfo, name, 0x1d);
}

unsigned int CUser::GetGuildKey()
{
    return m_guild ? m_guild->GetGuildKey() : 0;
}

unsigned short CUser::GetGuildMemFlag()
{
    return m_field48;
}

void CUser::SendSetGuildKeyToUser(unsigned int guildKey, unsigned int grade)
{
    Packet_Monitor_Set_Guild_Key pkt;
    pkt.m_channel = m_channel;
    pkt.m_grade = grade;
    pkt.m_guildKey = guildKey;
    SendTcpGameserver(&pkt);
}

void CUser::SetGuildMemberMemo(const char* memo)
{
    memset((char*)this + 0x4a, 0, 0x15);
    int n = (int)strlen(memo);
    if (n <= 0x14)
    {
        memcpy((char*)this + 0x4a, memo, n);
    }
    else
    {
        memcpy((char*)this + 0x4a, memo, 0x14);
    }
}

void CUser::ChangeGuildMemberGrade(unsigned char grade)
{
    if (grade == 1 || grade == 2 || (unsigned char)m_guildDBInfo.m_data[0x15] == 1 ||
        (unsigned char)m_guildDBInfo.m_data[0x15] == 2)
    {
        DNF_LOG_SCOPE_LINE(0x183, "./log/GuildModify", "char(%s), old(%d), new(%d)", GetCharName(),
            (unsigned int)(unsigned char)m_guildDBInfo.m_data[0x15], (unsigned int)grade);
    }
    m_guildDBInfo.m_data[0x15] = (char)grade;
    SendGuildMemberDBInfo(m_guildDBInfo);
}

void CUser::SendGuildMemberDBInfo(STGuildMemerDBInfo& info)
{
    if (GetUniqCharNo() == 0)
    {
        DNF_LOG_SCOPE_LINE(0x193, "./log/GuildModify", "SendGuildMemberDBInfo(), 0 == this->GetUniqCharNo() ERR");
    }
    else
    {
        Packet_Monitor_Notice_Guild_Member_Info pkt;
        memcpy(&pkt.m_info, &info, 0x1a);
        pkt.m_channel = GetIdByChannel();
        pkt.m_charNo = GetUniqCharNo();
        SendToGameserver((char*)&pkt, pkt.packetSize);
    }
}

void CUser::SetUserChangableInfo(short type, char value)
{
    m_guildMemFlag = type;
    m_growthType = value;
}

bool CUser::RegisterToBlackList(unsigned int charNo, char* name)
{
    if (name == 0 || charNo == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1c0, "./log/BlackList", "Register Err(%d)(%s)", charNo, name);
        return 0;
    }
    CBlackUser* bu = new CBlackUser;
    bu->SetBlackUser(name, (unsigned int)time(0));
    return m_blackList.insert(std::make_pair(charNo, bu)).second;
}

bool CUser::RegisterToBlackList(unsigned int charNo, char* name, unsigned int param)
{
    CBlackUser* bu = new CBlackUser;
    bu->SetBlackUser(name, param);
    return m_blackList.insert(std::make_pair(charNo, bu)).second;
}

bool CUser::DeleteToBlackList(unsigned int charNo)
{
    if (m_blackList.empty())
    {
        return 0;
    }
    UserBlackMap::iterator it = m_blackList.find(charNo);
    if (it != m_blackList.end())
    {
        delete it->second;
        m_blackList.erase(it);
        return 1;
    }
    return 0;
}

void CUser::ResetBlackList()
{
    if (m_blackList.empty())
    {
        return;
    }
    m_blackList.clear();
}

void CUser::GetBlackList(unsigned char& count, STBlackUserDBType* list)
{
    count = 0;
    if (m_blackList.empty())
    {
        return;
    }
    for (UserBlackMap::iterator it = m_blackList.begin();
         it != m_blackList.end(); ++it)
    {
        memcpy(list[count].m_name, (*it).second->GetName(), 0x1d);
        list[count].m_occurTime = (*it).second->GetOccurTime();
        list[count].m_field0 = (*it).first;
        count = (unsigned char)(count + 1);
        if (9 < count)
        {
            return;
        }
    }
}

void CUser::GetBlackList(unsigned char& count, unsigned int* list)
{
    count = 0;
    if (m_blackList.empty())
    {
        return;
    }
    for (UserBlackMap::iterator it = m_blackList.begin();
         it != m_blackList.end(); )
    {
        list[count] = (*it).first;
        count = (unsigned char)(count + 1);
        if (9 < count)
        {
            return;
        }
        ++it;
    }
}

void CUser::RegisterToCashBlackList(std::map<unsigned int, CBlackUser*>& map)
{
    for (std::map<unsigned int, CBlackUser*>::iterator it = map.begin();
         it != map.end(); ++it)
    {
        m_blackList[it->first] = it->second;
    }
}

int CUser::IsBlackUser(unsigned int charNo)
{
    if (m_blackList.empty())
    {
        return 0;
    }
    UserBlackMap::iterator it = m_blackList.find(charNo);
    return it != m_blackList.end() ? 1 : 0;
}

unsigned short CUser::GetBlackListSize()
{
    return (unsigned short)m_blackList.size();
}

void CUser::GuildInviteProcess()
{
    if (m_field7e < 2)
    {
        --m_field7e;
        register bool bVar = (m_field7e == 0 || m_field7e > 1);
        if (bVar)
        {
            SetGuildInviteFact(0, 0, 0xff);
        }
    }
}

void CUser::ChangeCharName(char* name)
{
    if (GetUniqCharNo() != 0 && GetJob() != 0)
    {
        memset(m_charInfo, 0, 0x1e);
        memcpy(m_charInfo, name, 0x1d);
    }
}

void CUser::MakeGameServerSendUserInfoPacket(unsigned int guildKey)
{
    Packet_Send_All_User_Info_Minimum_For_Guild_System pkt;
    pkt.m_guildKey = guildKey;
    pkt.m_charNo = GetUniqCharNo();
    pkt.m_channel = GetIdByChannel();
    SendTcpGameserver(&pkt);
}

STGuildMemerDBInfo::STGuildMemerDBInfo()
{
    ((STGuildMemerDBInfo_Layout*)this)->m15 = 0;
    ((STGuildMemerDBInfo_Layout*)this)->m16 = 0;
    memset((char*)this, 0, 0x15);
}

char* CUser::GetCharName()
{
    return m_charInfo;
}

void CUser::DetachGuild()
{
    m_guild = 0;
}

bool CUser::IsSetGuildMemFlag(unsigned short flag)
{
    return (m_field48 & flag) != 0;
}

void CUser::SetGuildMemFlag(unsigned short flag)
{
    m_field48 |= flag;
}

int CUser::GetIdByChannel()
{
    return m_channel;
}

unsigned char CUser::GetJob()
{
    return m_job;
}

void CUser::ResetGuildMemFlag(unsigned short flag)
{
    m_field48 = (unsigned short)(m_field48 & ~flag);
}

void CUser::SetGuildInviteFact(unsigned int guildId, unsigned int callerId, unsigned char fact)
{
    m_field84 = guildId;
    m_field80 = callerId;
    m_field7e = (char)fact;
}

unsigned int CUser::GetUniqCharNo()
{
    return m_charNo;
}

#pragma pack(push,1)
struct Packet_Monitor_Set_Guild_Key_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned int me;
    unsigned int m12;
};
#pragma pack(pop)
Packet_Monitor_Set_Guild_Key::Packet_Monitor_Set_Guild_Key()
    : PacketHeader(0x40b, 0x16)
{
    ((Packet_Monitor_Set_Guild_Key_Layout*)this)->ma = 4294967295;
    ((Packet_Monitor_Set_Guild_Key_Layout*)this)->me = 0;
    ((Packet_Monitor_Set_Guild_Key_Layout*)this)->m12 = 0;
}

Packet_Monitor_SAVE_Guild_Member::Packet_Monitor_SAVE_Guild_Member()
    : PacketHeader(0x40f, 0x2e), m_flag(0xff), m_b(0)
{
}

Packet_Monitor_Notice_Guild_Member_Info::Packet_Monitor_Notice_Guild_Member_Info()
    : PacketHeader(0x431, 0x2c)
{
    memset(&m_info, 0, sizeof(m_info));
}

Packet_Send_All_User_Info_Minimum_For_Guild_System::
    Packet_Send_All_User_Info_Minimum_For_Guild_System()
    : PacketHeader(0x447, 0x16)
{
}

void CUser::SetIdByChannel(int channel)
{
    m_channel = channel;
}

void CUser::SetUserPosState(unsigned char state)
{
    m_posState = state;
}

unsigned int CUser::GetDBID()
{
    return m_dbid;
}

void CUser::SetDBID(unsigned int dbid)
{
    m_dbid = dbid;
}

void CUser::SetGameServer(CGameServer* server)
{
    m_gameServer = server;
}

CGameServer* CUser::GetGameServer()
{
    return m_gameServer;
}

CTcpGameServer* CUser::GetTcpGameServer()
{
    return m_tcpGameServer;
}

void CUser::SetUniqCharNo(unsigned int charNo)
{
    m_charNo = charNo;
}

unsigned char CUser::GetGrowthType()
{
    return m_growthType;
}

unsigned char CUser::GetLevel()
{
    return (unsigned char)m_guildMemFlag;
}

void CUser::SetSex(unsigned char sex)
{
    m_field3a = sex;
}

unsigned char CUser::GetSex()
{
    return m_field3a;
}

void CUser::SetSsn(char* ssn)
{
    memcpy((char*)this + 0x3b, ssn, 6);
}

char* CUser::GetSsn()
{
    return (char*)this + 0x3b;
}

STGuildMemerDBInfo* CUser::GetGuildMemDBInfo()
{
    return &m_guildDBInfo;
}

#pragma pack(push,1)
struct CUser_IsSubGuildMaster_Layout
{
    char h[0x5f];
    unsigned char m5f;
};
#pragma pack(pop)

bool CUser::IsSubGuildMaster()
{
    // ORIG：仅比较 this+0x5f 字节 == 2（无 m_guild 解引用），打包布局复现直接偏移读取
    if (((CUser_IsSubGuildMaster_Layout*)this)->m5f == 2)
    {
        return true;
    }
    return false;
}

void CUser::SetBlackListDBFlag(unsigned short flag)
{
    m_field7c |= flag;
}

unsigned int CUser::GetGuildInviteCallerId()
{
    return m_field80;
}

unsigned int CUser::GetGuildInviteGuildId()
{
    return m_field84;
}

void CUser::SetTcpGameServer(CTcpGameServer* server)
{
    m_tcpGameServer = server;
}

void* CUser::GetMapBlackList()
{
    return &m_blackList;
}

unsigned short CUser::GetBlackListDBFlag()
{
    return m_field7c;
}
