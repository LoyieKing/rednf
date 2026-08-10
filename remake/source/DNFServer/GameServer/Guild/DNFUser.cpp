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

void* CUser::operator new(unsigned int size)
{
    return m_UserMemPool_.alloc();
}

void CUser::operator delete(void* p)
{
    m_UserMemPool_.free(p);
}

void CUser::operator delete(void* p, unsigned int size)
{
    m_UserMemPool_.free(p, size);
}

MemPool<CUser> m_UserMemPool_(28000);

CUser::CUser()
{
    m_dbid = 0;
    m_charNo = 0;
    m_gameServer = 0;
    m_tcpGameServer = 0;
    m_posState = 0;
    m_channel = -1;
    memset(m_charInfo, 0, sizeof(m_charInfo));
    m_job = 0xff;
    m_growthType = 0xff;
    m_guildMemFlag = 0xffff;
    m_field3a = 1;
    memset(m_field3b, 0, sizeof(m_field3b));
    m_guild = 0;
    m_field48 = 0;
    memset((void*)&m_guildDBInfo, 0, sizeof(m_guildDBInfo));
    m_field7c = 0;
    m_field7e = 0x7f;
    m_field80 = 0;
    m_field84 = 0;
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
}

void CUser::QueryGuildMember(CServerHandler* handler)
{
    if (m_gameServer != 0)
    {
        handler->QueryGuildMember((unsigned char)m_gameServer->GetGroupNo(), m_charNo);
        *(unsigned short*)((char*)this + 0x48) |= 2;
    }
}

void CUser::LoadGuildMember(unsigned int guildKey, STGuildMemerDBInfo& info)
{
    if ((*(unsigned short*)((char*)this + 0x48) & 8) != 0)
    {
        return;
    }
    if (m_guild == 0 || m_guild->GetGuildKey() != guildKey)
    {
        return;
    }
    if ((*(unsigned short*)((char*)this + 0x48) & 2) == 0)
    {
        return;
    }
    memcpy(&m_guildDBInfo, &info, 0x1a);
    *(unsigned short*)((char*)this + 0x48) |= 4;
    if (m_guild->GetMasterId() == GetUniqCharNo() && *(unsigned char*)((char*)this + 0x5f) != 1)
    {
        CMyFileLog log("LoadGuildMember", 0x8d);
        log("./log/GuildMember", "[Master_Err]\tGuild_K(%d)\tChar_No(%d)\tGrade(%d)", guildKey,
            GetUniqCharNo(), *(unsigned char*)((char*)this + 0x5f));
        *(unsigned char*)((char*)this + 0x5f) = 1;
    }
}

void CUser::AddGuildMemberPoint(unsigned int point)
{
    unsigned int old = *(unsigned int*)((char*)this + 0x60);
    *(unsigned int*)((char*)this + 0x60) += point;
    SetGuildMemFlag(0x10);
    if (*(unsigned int*)((char*)this + 0x60) < old)
    {
        *(unsigned int*)((char*)this + 0x60) = old;
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
    *(unsigned char*)((char*)&pkt + 0xa) = type;
    *(unsigned int*)((char*)&pkt + 0xb) = value;
    *(unsigned int*)((char*)&pkt + 0xf) = GetUniqCharNo();
    memcpy((char*)&pkt + 0x13, (char*)this + 0x4a, 0x1a);
    *(unsigned char*)((char*)&pkt + 0x2d) = flag;
    handler->SendToDB(&pkt);
    ResetGuildMemFlag(0x10);
}

void CUser::ResetGuildPoint()
{
    *(unsigned int*)((char*)this + 0x60) = 0;
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
            *(unsigned short*)pkt, *(unsigned short*)((char*)pkt + 2));
        if (out != 0)
        {
            memcpy(out + 10, (char*)pkt + 10, *(unsigned short*)((char*)pkt + 2) - 10);
            m_tcpGameServer->SendToGameServer(out);
        }
    }
}

void CUser::SetUserInfo_CharNo(char sex, char job, short flag, unsigned int charNo, char* name)
{
    m_job = (unsigned char)sex;
    m_growthType = (unsigned char)job;
    *(short*)((char*)this + 0x38) = flag;
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
    *(int*)((char*)&pkt + 0xa) = m_channel;
    *(unsigned int*)((char*)&pkt + 0xe) = grade;
    *(unsigned int*)((char*)&pkt + 0x12) = guildKey;
    SendTcpGameserver(&pkt);
}

void CUser::SetGuildMemberMemo(const char* memo)
{
    memset((char*)this + 0x4a, 0, 0x15);
    size_t n = strlen(memo);
    if ((int)n < 0x15)
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
    if (grade == 1 || grade == 2 || *(unsigned char*)((char*)this + 0x5f) == 1 ||
        *(unsigned char*)((char*)this + 0x5f) == 2)
    {
        DNF_LOG_SCOPE_LINE(0x183, "./log/GuildModify", "char(%s), old(%d), new(%d)", GetCharName(),
            (unsigned int)*(unsigned char*)((char*)this + 0x5f), (unsigned int)grade);
    }
    *(unsigned char*)((char*)this + 0x5f) = grade;
    SendGuildMemberDBInfo(*(STGuildMemerDBInfo*)((char*)this + 0x4a));
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
        memcpy((char*)&pkt + 0x12, &info, 0x1a);
        *(int*)((char*)&pkt + 0xa) = GetIdByChannel();
        *(unsigned int*)((char*)&pkt + 0xe) = GetUniqCharNo();
        SendToGameserver((char*)&pkt, *(unsigned short*)((char*)&pkt + 2));
    }
}

void CUser::SetUserChangableInfo(short type, char value)
{
    *(short*)((char*)this + 0x38) = type;
    *(char*)((char*)this + 0x37) = value;
}

int CUser::RegisterToBlackList(unsigned int charNo, char* name)
{
    if (name == 0 || charNo == 0)
    {
        DNF_LOG_SCOPE_LINE(0x1c0, "./log/BlackList", "Register Err(%d)(%s)", charNo, name);
        return 0;
    }
    CBlackUser* bu = new CBlackUser;
    bu->SetBlackUser(name, (unsigned int)time(0));
    m_blackList.insert(std::make_pair(charNo, bu));
    return 1;
}

int CUser::RegisterToBlackList(unsigned int charNo, char* name, unsigned int param)
{
    if (name == 0 || charNo == 0)
    {
        return 0;
    }
    CBlackUser* bu = new CBlackUser;
    bu->SetBlackUser(name, param);
    m_blackList.insert(std::make_pair(charNo, bu));
    return 1;
}

int CUser::DeleteToBlackList(unsigned int charNo)
{
    std::map<unsigned int, CBlackUser*>::iterator it = m_blackList.find(charNo);
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
    if (!m_blackList.empty())
    {
        m_blackList.clear();
    }
}

void CUser::GetBlackList(unsigned char& count, STBlackUserDBType* list)
{
    count = 0;
    if (!m_blackList.empty())
    {
        for (std::map<unsigned int, CBlackUser*>::iterator it = m_blackList.begin();
             it != m_blackList.end(); ++it)
        {
            CBlackUser* bu = it->second;
            memcpy((char*)list + (unsigned int)count * 0x28 + 4, bu->GetName(), 0x1d);
            *(unsigned int*)((char*)list + (unsigned int)count * 0x28 + 0x24) =
                bu->GetOccurTime();
            *(unsigned int*)((char*)list + (unsigned int)count * 0x28) = it->first;
            count = (unsigned char)(count + 1);
            if (9 < count)
            {
                return;
            }
        }
    }
}

void CUser::GetBlackList(unsigned char& count, unsigned int* list)
{
    count = 0;
    if (!m_blackList.empty())
    {
        for (std::map<unsigned int, CBlackUser*>::iterator it = m_blackList.begin();
             it != m_blackList.end() && count < 0xff; ++it, count++)
        {
            list[count] = it->first;
        }
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
    return m_blackList.find(charNo) != m_blackList.end() ? 1 : 0;
}

unsigned short CUser::GetBlackListSize()
{
    return (unsigned short)m_blackList.size();
}

void CUser::GuildInviteProcess()
{
    if ((char)*(char*)((char*)this + 0x7e) < 2)
    {
        *(char*)((char*)this + 0x7e) = (char)(*(char*)((char*)this + 0x7e) - 1);
        if (*(char*)((char*)this + 0x7e) == 0 || (char)*(char*)((char*)this + 0x7e) > 1)
        {
            SetGuildInviteFact(0, 0, 0xff);
        }
    }
}

void CUser::ChangeCharName(char* name)
{
    if (m_charNo != 0 && m_job != 0)
    {
        memset(m_charInfo, 0, 0x1e);
        memcpy(m_charInfo, name, 0x1d);
    }
}

void CUser::MakeGameServerSendUserInfoPacket(unsigned int guildKey)
{
    Packet_Send_All_User_Info_Minimum_For_Guild_System pkt;
    *(unsigned int*)((char*)&pkt + 0xa) = guildKey;
    *(unsigned int*)((char*)&pkt + 0xe) = GetUniqCharNo();
    *(int*)((char*)&pkt + 0x12) = GetIdByChannel();
    SendTcpGameserver(&pkt);
}

STGuildMemerDBInfo::STGuildMemerDBInfo()
{
    *(unsigned char*)((char*)this + 0x15) = 0x0;
    *(unsigned int*)((char*)this + 0x16) = 0;
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
    m_field48 = flag;
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
    m_field48 &= (unsigned short)~flag;
}

void CUser::SetGuildInviteFact(unsigned int guildId, unsigned int callerId, unsigned char fact)
{
    *(unsigned int*)((char*)this + 0x84) = guildId;
    *(unsigned int*)((char*)this + 0x80) = callerId;
    *(unsigned char*)((char*)this + 0x7e) = fact;
}

unsigned int CUser::GetUniqCharNo()
{
    return m_charNo;
}

Packet_Monitor_Set_Guild_Key::Packet_Monitor_Set_Guild_Key()
    : PacketHeader(0x40b, 0x16)
{
    *(unsigned int*)((char*)this + 0x10) = 0xffffffff;
    *(unsigned int*)((char*)this + 0xe) = 0;
    *(unsigned int*)((char*)this + 0x12) = 0;
}

Packet_Monitor_SAVE_Guild_Member::Packet_Monitor_SAVE_Guild_Member()
    : PacketHeader(0x40f, 0x2e)
{
    *(unsigned char*)((char*)this + 0x10) = 0xff;
    *(unsigned int*)((char*)this + 0xb) = 0;
    new ((char*)this + 0x13) STGuildMemerDBInfo;
}

Packet_Monitor_Notice_Guild_Member_Info::Packet_Monitor_Notice_Guild_Member_Info()
    : PacketHeader(0x431, 0x2c)
{
    new ((char*)this + 0x12) STGuildMemerDBInfo;
    memset((char*)this + 0x12, 0, 0x1a);
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
    return *(unsigned short*)((char*)this + 0x38);
}

void CUser::SetSex(unsigned char sex)
{
    *(unsigned char*)((char*)this + 0x3a) = sex;
}

unsigned char CUser::GetSex()
{
    return *(unsigned char*)((char*)this + 0x3a);
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

bool CUser::IsSubGuildMaster()
{
    return m_guild != 0 && m_guild->IsSubGuildMaster(m_dbid);
}

void CUser::SetBlackListDBFlag(unsigned short flag)
{
    m_field7c = flag;
}

unsigned int CUser::GetGuildInviteCallerId()
{
    return *(unsigned int*)((char*)this + 0x80);
}

unsigned int CUser::GetGuildInviteGuildId()
{
    return *(unsigned int*)((char*)this + 0x84);
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

