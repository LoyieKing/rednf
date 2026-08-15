// df_guild_r — DNFGuild (split from source/guild per ORIG layout)
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

#include "DNFGuild.h"
#include "BlackUser.h"
#include "CashObject.h"
#include "DNFAppConfig.h"
#include "DNFAppStartInit.h"
#include "DNFAppStopInit.h"
#include "DNFApplication.h"
#include "DNFDBServer.h"
#include "DNFFloatingPointExceptSig.h"
#include "DNFGameServer.h"
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
#include <new>

// 局部布局视图（仅 DNFGuild.cpp 内部使用，不改共享头；用于按 ORIG 字段偏移生成直接位移访问）
struct __attribute__((packed)) GuildNoticeInfoFull
{
    char m_raw0[0xa];
    unsigned int m_channel;  // +0xa
    unsigned int m_charNo;   // +0xe
    unsigned int m_guildKey; // +0x12
    char m_body[0xbd];       // +0x16
    char m_padD3;            // +0xd3
    char m_rest[0x65];       // +0xd4
};

// CGuild today-member 块（+0x66ec），与 SetTodayGuildMember 的 _TmFull 一致：
// 9×uint + ushort + byte = 0x27 字节
struct __attribute__((packed)) CGuildTodayMemberBlock
{
    unsigned int m_charNo;    // +0x0 (0x66ec)
    unsigned int m_name0;     // +0x4 (0x66f0)
    unsigned int m_name1;     // +0x8
    unsigned int m_name2;     // +0xc
    unsigned int m_name3;     // +0x10
    unsigned int m_name4;     // +0x14
    unsigned int m_name5;     // +0x18
    unsigned int m_name6;     // +0x1c
    unsigned int m_name7;     // +0x20
    unsigned short m_field24; // +0x24 (0x6710)
    unsigned char m_field26;  // +0x26 (0x6712)
};
#include "SystemTimeHandler.h"
#include "TcpNetSystem.h"
#include "WebEvent.h"

// CGuild 布局镜像（仅本 TU 使用，字节级对齐 DNFGuild.h 中 CGuild 布局；
// 供按真实成员形态还原 ORIG 代码生成用）
struct __attribute__((packed)) CGuildMemberNameView
{
    char m_pad[0xd];
    unsigned int m_charNo;      // +0xd（绝对偏移 = 基址 + 0xdd）
    char m_name[0x1e];          // +0x11
};

struct __attribute__((packed)) CGuildMemberGradeView
{
    char m_pad[0x14];
    unsigned char m_grade;      // +0x14
};

struct __attribute__((packed)) CGuildSubMasterView
{
    char m_pad[0x4d];
    unsigned char m_cnt;             // +0x4d（= m_dbInfo.m_info.m_subMasterCnt）
    unsigned int m_subGuildMaster[5]; // +0x4e
};

// 成员记录数组视图（2026-08-12 R43：与 ORIG 代码生成形态逐字节对齐）。
// ORIG 以「成员数组 + 字段位移」形式访问（基址 0xd0/0xf0，步长 0x41）：
//   - 主视图 0xd0：charNo@+0xd、name@+0x11
//   - 附加视图 0xf0：job@+0xf、growth@+0x10、level@+0x11、sex@+0x13、
//     grade@+0x14、changableTime@+0x15、memo@+0x19
struct __attribute__((packed)) CGuildMemberMainRec
{
    char m_pad[0xd];
    unsigned int m_charNo;      // +0xd
    char m_name[0x1d];          // +0x11
    char m_pad2[0x41 - 0x2e];   // +0x2e .. 0x40
};
struct CGuildMemberMainArray
{
    char m_pad[0xd0];
    CGuildMemberMainRec m_members[300];
};
struct __attribute__((packed)) CGuildMemberExtraRec
{
    char m_pad0[0xf];
    char m_job;                 // +0xf
    char m_growth;              // +0x10
    unsigned short m_level;     // +0x11
    char m_sex;                 // +0x13
    unsigned char m_grade;      // +0x14
    unsigned int m_changableTime; // +0x15
    char m_memo[0x14];          // +0x19
    char m_pad1[0x41 - 0x2d];   // +0x2d .. 0x40
};
struct CGuildMemberExtraArray
{
    char m_pad[0xf0];
    CGuildMemberExtraRec m_members[300];
};

int g_guildDBProcessDay = 0;

void* CGuild::operator new(unsigned int size)
{
    return m_GuildmemPool_.alloc();
}

void CGuild::operator delete(void* p)
{
    m_GuildmemPool_.free(p);
}

void CGuild::operator delete(void* p, unsigned int size)
{
    m_GuildmemPool_.free(p, size);
}

void CGuild::AddGuildMemberPoint(unsigned int charNo, unsigned int point)
{
}

MemPool<CGuild> CGuild::m_GuildmemPool_(10000);

ST_Notice_Guild_Enter::ST_Notice_Guild_Enter()
{
    m_guildKey = 0;
    m_dbid = 0;
    m_charNo = 0;
    memset(m_guildName, 0, 0x17);
    memset(m_charName, 0, 0x1e);
}

unsigned int CGuild::GetGuildKey()
{
    return m_guildKey;
}

void RandomOption::reset()
{
    m_f0.reset();
    m_f3.reset();
    m_f6.reset();
    m_s9.reset();
    m_f10.reset();
    m_s13.reset();
}

void RandomOptionField::reset()
{
    m_data[0] = 0;
    m_data[1] = 0;
    m_data[2] = 0;
}

void RandomOptionSeed::reset()
{
    m_data[0] = 0;
}

#pragma pack(push,1)
struct DnfItemInfoResetLayout
{
    unsigned char m0;            // +0x0
    unsigned int m1;             // +0x1
    unsigned char m5;            // +0x5
    unsigned int m6;             // +0x6
    unsigned short m10;          // +0xa
    unsigned int m12;            // +0xc
    unsigned char m16;           // +0x10
    unsigned short m17;          // +0x11
};
#pragma pack(pop)

void DnfItemInfo::reset()
{
    m_seal = 0;
    m_itemId = 0;
    m_attr = 0;
    m_addInfo = 0;
    m_endurance = 0;
    m_extendInfo = 0;
    m_abilityType = 0;
    m_abilityValue = 0;
    m_opt.reset();
    m_up.reset();
    m_res.reset();
}

DnfItemInfo::DnfItemInfo()
{
    reset();
}

UpgradeSeparateInfo::UpgradeSeparateInfo()
{
    reset();
}

void UpgradeSeparateInfo::reset()
{
    struct Bits { unsigned char a:5, b:1, c:2; };
    reinterpret_cast<Bits*>(m_data)->a = 0;
    reinterpret_cast<Bits*>(m_data)->b = 0;
    reinterpret_cast<Bits*>(m_data)->c = 0;
}

ReservedCapacity::ReservedCapacity()
{
    reset();
}

void ReservedCapacity::reset()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
}

void CGuild::SetPowerSide(unsigned char side)
{
    if (IsSetGuildDBFlag(4))
    {
        m_dBSaveFlag = 1;
        m_dbInfo.m_info.m_powerSide = side;
    }
}

STGuildMemberProxy::STGuildMemberProxy()
{
    m_no = 0;
    m_job = 0xff;
    m_growType = 0xff;
    m_lev = 0xffff;
    m_sex = 0;
    m_grade = 0;
    m_lastPlayTime = 0;
    memset(m_name, 0, 0x1e);
    memset(m_memo, 0, 0x15);
}

#pragma pack(push,1)
struct STGuildMemberCharacData_Layout
{
    char pad0x0[0x1];
    unsigned char m1;
    unsigned char m2;
};
#pragma pack(pop)
STGuildMemberCharacData::STGuildMemberCharacData()
{
    m_field0 = 0xff;
    m_field1 = 255;
    m_masterFlag = 0;
    memset(m_name, 0, 0x1e);
}

ST_GuildCreateFromWeb::ST_GuildCreateFromWeb()
{
    m_guildKey = 0;
    m_charNo = 0;
    memset(m_rest, 0, 0x17);
}

ST_Notice_Guild_Secede::ST_Notice_Guild_Secede()
{
    m_guildKey = 0;
    m_dbid = 0;
    m_charNo = 0;
    m_secedeFlag = 0;
    memset(m_guildName, 0, 0x17);
    memset(m_charName, 0, 0x1e);
}

STGuildCallInfo::STGuildCallInfo()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field5 = 0;
    memset(m_data, 0, sizeof(m_data));
}

void CGuild::SetGuildDBFlag(unsigned short flag)
{
    m_guildDBFlag |= flag;
}

bool CGuild::IsSetGuildDBFlag(unsigned short flag)
{
    return (m_guildDBFlag & flag) == flag;
}

unsigned char CGuild::GetGuildLevel()
{
    return m_dbInfo.m_info.m_guildLevel;
}

char* CGuild::GetGuildName()
{
    return m_dbInfo.m_info.m_guildName;
}

unsigned char CGuild::GetGuildRank()
{
    return m_dbInfo.m_info.m_guildRank;
}

unsigned int CGuild::GetGuildExp()
{
    return m_dbInfo.m_info.m_guildExp;
}

bool CGuild::IsGuildMaster(unsigned int dbid)
{
    if (GetMasterId() == dbid)
    {
        return true;
    }
    return false;
}

unsigned char CGuild::GetCurSubGuildMasterCnt()
{
    return m_dbInfo.m_info.m_subMasterCnt;
}

unsigned short CGuild::GetTotalCnt_Of_GuildDBInfo()
{
    return m_dbInfo.m_info.m_totalCnt;
}

unsigned char CGuild::GetPowerSide()
{
    return m_dbInfo.m_info.m_powerSide;
}

void CGuild::SetPowerSecedeTime(unsigned int time)
{
    m_dbInfo.m_info.m_powerSecedeTime = time;
}

STGuildDBInfo* CGuild::GetGuildDBInfo()
{
    return &m_dbInfo;
}

bool CGuild::IsExistGuildAgit()
{
    if (m_dbInfo.m_info.m_agitFlag != 0)
    {
        return true;
    }
    return false;
}

void CGuild::SetGuildAgitFlag(bool flag)
{
    if (flag != 0)
    {
        m_dbInfo.m_info.m_agitFlag = 1;
    }
    else
    {
        m_dbInfo.m_info.m_agitFlag = 0;
    }
}

void CGuild::SetGuildAgitLevelUp()
{
    m_agitInfo.m_agitLevel++;
}

CGuildCargo* CGuild::GetGuildCargo()
{
    return &m_cargo;
}

CGuildBoard* CGuild::GetGuildBoard()
{
    return &m_board;
}

unsigned char CGuild::GetDBSaveFlag()
{
    return m_dBSaveFlag;
}

void CGuild::EnableDBSaveFlag()
{
    m_dBSaveFlag = 1;
}

void CGuild::SetTodayGuildMember(STTodayGuildMember& member)
{
    this->m_board.m_today = *(const CGuildBoard::TodayMemberBlock*)&member;
}

CGuild::CGuild(unsigned int guildKey)
    : m_guildKey(guildKey),
      m_guildDBFlag(0),
      m_totalCnt(0),
      m_dbInfo(),
      m_agitInfo(),
      m_field4d92(0),
      m_saveIntervalCnt(0),
      m_dBSaveFlag(0),
      m_changable(),
      m_changableTickCnt(0),
      m_cargo(),
      m_board()
{
    m_field4d70 = 300;
    m_field4d72 = 300;
    m_field4d74 = 300;
    memset(m_guildMessage, 0, 0x64);
    memset((void*)&m_agitInfo, 0, sizeof(m_agitInfo));
}

CGuild::~CGuild()
{
    m_guildDBFlag = 0;
    m_guildKey = 0;
    m_field4d92 = 0;
    m_saveIntervalCnt = 0;
    m_dBSaveFlag = 0;
    m_totalCnt = 0;
    m_changableTickCnt = 0;
    m_changable.clear();
    m_members.clear();
}

bool CGuild::InsertGuildMember(unsigned int charNo, CUser* user)
{
    if (user == 0)
    {
        return 0;
    }
    user->AttachGuild(this);
    std::pair<std::map<unsigned int, CUser*>::iterator, bool> r =
        m_members.insert(std::make_pair(charNo, user));
    if (r.second == 0)
    {
        DNF_LOG_SCOPE_LINE(0x7b,"./log/GuildMember",
            "[INSERT_ERR]\tAlready Exist : Guild Key : %d\tChar Key : %d,\tChar Name : %s\tLogin Mem Cnt : %d\n",
            GetGuildKey(), charNo, user->GetCharName(), m_members.size());
        return 0;
    }
    return 1;
}

bool CGuild::DeleteGuildMember(unsigned int charNo, CUser* user)
{
    if (user == 0)
    {
        return 0;
    }
    if (m_members.empty())
    {
        return 0;
    }
    if (m_members.erase(charNo) != 0)
    {
        if (user->IsSetGuildMemFlag(2) != 1)
        {
            user->SetGuildMemFlag(8);
        }
        return 1;
    }
    // ORIG：erase 失败时先 find，仅当成员仍存在才打日志
    std::map<unsigned int, CUser*>::iterator it = m_members.find(charNo);
    if (it == m_members.end())
    {
        DNF_LOG_SCOPE_LINE(0xaf,"./log/Except",
            "CGuild::DeleteGuildMember\tException Break Possible! Or Check Using Function FindUser() or FindUser_CharNo()\tGuild Key : %d\tChar Key : %d,\tChar Name : %s\tLogin Mem Cnt : %d\n",
            GetGuildKey(), charNo, user->GetCharName(), m_members.size());
    }
    return 0;
}

CUser* CGuild::FindGuildMember(unsigned int charNo)
{
    std::map<unsigned int, CUser*>::iterator it;
    if (m_members.empty())
    {
        return 0;
    }
    it = m_members.find(charNo);
    if (m_members.end() != it)
    {
        return it->second;
    }
    return 0;
}

bool CGuild::IsEmpty()
{
    if (m_members.empty())
    {
        return true;
    }
    return false;
}

void CGuild::QueryGuild(CServerHandler* handler, unsigned int charNo)
{
    if ((m_guildDBFlag & 4) == 0)
    {
        handler->QueryGuild(m_guildKey, charNo);
        DNF_LOG_SCOPE_LINE(0xf5, "./log/Guild", "[QUERY]  Guild Key : %d\n", m_guildKey);
        m_guildDBFlag |= 2;
    }
    return;
}

void CGuild::LoadGuild(STGuildDBInfoOnly& info, char* name)
{
    if ((m_guildDBFlag & 4) == 0 && (m_guildDBFlag & 2) != 0)
    {
        int local18 = m_dbInfo.m_info.m_guildPoint;
        char local11 = 0;
        int local10 = 0;
        while (local10 < (int)info.m_skillLearnCnt &&
               199 < info.m_skills[local10].m0 &&
               info.m_skills[local10].m0 < 0xd1)
        {
            local11++;
            local10++;
        }
        if ((int)local11 != (int)info.m_skillLearnCnt)
        {
            DNF_LOG_SCOPE_LINE(0x11b,"./log/GuildSkill", "Guild Skill Learn Error(%d)(%d)",
                (int)info.m_skillLearnCnt, (int)local11);
            info.m_skillLearnCnt = (unsigned char)local11;
        }
        memcpy(&m_dbInfo.m_info, (char*)&info, 0xbd);
        SetGuildMessage(name);
        if (local18 != 0 && (int)info.m_guildPoint < local18)
        {
            m_dbInfo.m_info.m_guildPoint = local18;
        }
        m_guildDBFlag |= 4;
    }
}

void CGuild::SaveGuild(unsigned char flag, CServerHandler* handler, unsigned int param)
{
    if ((m_guildDBFlag & 4) != 0)
    {
        Packet_Monitor_SAVE_Guild pkt;
        pkt.m_flag = flag;
        pkt.m_guildKey = m_guildKey;
        pkt.m_param = param;
        memcpy(&pkt.m_info, &m_dbInfo.m_info, 0xbd);
        handler->SendToDB(&pkt);
    }
}

void CGuild::DBGuildSaveProcess(CServerHandler* handler)
{
    if (m_dBSaveFlag == 0)
    {
        return;
    }
    if ((m_guildDBFlag & 4) == 0)
    {
        return;
    }
    m_saveIntervalCnt++;
    if (m_saveIntervalCnt > 1)
    {
        if (m_members.empty())
        {
            const char* gname = GetGuildName();
            unsigned int key = m_guildKey;
            CMyFileLog log(__FUNCTION__, 0x171);
            log("./log/Except", "[SAVE_INTERVAL]  Guild Key : %d\tGuild Name : %s\t\n",
                key, gname);
            m_saveIntervalCnt = 0;
        }
        else
        {
            CUser* user = m_members.begin()->second;
            if (user == 0)
            {
                CMyFileLog log(__FUNCTION__, 0x17c);
                log("./log/Except", "[SAVE_INTERVAL]  pclUser is NULL!");
                m_saveIntervalCnt = 0;
                return;
            }
            if (user->GetGameServer() != 0)
            {
                unsigned int exp = GetGuildExp();
                unsigned int key = m_guildKey;
                CMyFileLog log(__FUNCTION__, 0x183);
                log("./log/Guild", "GUILD EXP   Guild Key : %d, Guild Exp : %d", key, exp);
                SaveGuild((unsigned char)user->GetGameServer()->GetGroupNo(), handler, 0);
                SendGuildInfoToMembers(false);
                DBSaveGuildMembers((unsigned char)user->GetGameServer()->GetGroupNo(), handler, 2);
            }
            m_saveIntervalCnt = 0;
            m_dBSaveFlag = 0;
        }
    }
}

void CGuild::ResetGuildPointRank()
{
    m_dbInfo.m_info.m_guildRank = 0;
    m_dbInfo.m_info.m_guildPoint = 0;
    if (m_members.empty())
    {
        return;
    }
    for (std::map<unsigned int, CUser*>::iterator it = m_members.begin();
         it != m_members.end(); ++it)
    {
        CUser* user;
        if ((user = it->second) == 0) { continue; }
        user->ResetGuildPoint();
    }
}

void CGuild::DBGuildSave(unsigned char flag, CServerHandler* handler, unsigned int param)
{
    if (m_dBSaveFlag != 0)
    {
        if ((m_guildDBFlag & 4) != 0)
        {
            DNF_LOG_SCOPE_LINE(0x1ba,"./log/Guild", "GUILD EXP   Guild Key : %d, Guild Exp : %d",
                m_guildKey, GetGuildExp());
            SaveGuild(flag, handler, param);
            m_saveIntervalCnt = 0;
            m_dBSaveFlag = 0;
        }
    }
    else
    {
        return;
    }
}

void CGuild::DBSavePowerSecedeTime(unsigned char flag, CServerHandler* handler)
{
    if ((m_guildDBFlag & 4) != 0)
    {
        Packet_DBMW_Save_Power_Secede_Time pkt;
        pkt.m_flag = flag;
        pkt.m_key = m_guildKey;
        handler->SendToDB(&pkt);
    }
}

void CGuild::DBGuildMemberSave(CUser* user, unsigned char flag, CServerHandler* handler,
                               unsigned char param)
{
    if ((m_guildDBFlag & 4) != 0)
    {
        user->SetGuildMemFlag(0x10);
        user->SaveGuildMember(flag, m_guildKey, handler, param);
    }
}

void CGuild::DBSaveGuildMembers(unsigned char flag, CServerHandler* handler, unsigned char param)
{
    if ((m_guildDBFlag & 4) == 0)
        return;
    if (m_members.empty())
        return;
    for (std::map<unsigned int, CUser*>::iterator it = m_members.begin();
         it != m_members.end(); ++it)
    {
        CUser* user = it->second;
        if (user)
        {
            user->SaveGuildMember(flag, m_guildKey, handler, param);
        }
    }
}

void CGuild::AddGuildPoint(unsigned short point)
{
    if ((m_guildDBFlag & 4) != 0)
    {
        m_dBSaveFlag = 1;
        unsigned int oldPoint = m_dbInfo.m_info.m_guildPoint;
        m_dbInfo.m_info.m_guildPoint += point;
        if (m_dbInfo.m_info.m_guildPoint < oldPoint)
        {
            m_dbInfo.m_info.m_guildPoint = oldPoint;
        }
    }
}

void CGuild::AddGuildExpUntilLimit(unsigned int exp, unsigned int limit)
{
    if ((m_guildDBFlag & 4) != 0)
    {
        m_dBSaveFlag = 1;
        unsigned int old = m_dbInfo.m_info.m_guildExp;
        m_dbInfo.m_info.m_guildExp += exp;
        if (limit < m_dbInfo.m_info.m_guildExp)
        {
            m_dbInfo.m_info.m_guildExp = limit;
        }
        if (m_dbInfo.m_info.m_guildExp < old)
        {
            m_dbInfo.m_info.m_guildExp = old;
        }
        DNF_LOG_SCOPE_LINE(0x246,"./log/Guild",
            "GUILD EXP UNTIL LIMIT : guild key(%d), old exp(%d), add exp(%d), guild exp(%d), exp_limit(%d)",
            GetGuildKey(), old, exp, m_dbInfo.m_info.m_guildExp, limit);
    }
}

void CGuild::AddGuildExp(unsigned int exp)
{
    if ((m_guildDBFlag & 4) != 0)
    {
        m_dBSaveFlag = 1;
        unsigned int old = m_dbInfo.m_info.m_guildExp;
        m_dbInfo.m_info.m_guildExp += exp;
        if (m_dbInfo.m_info.m_guildExp < old)
        {
            m_dbInfo.m_info.m_guildExp = old;
        }
    }
}

void CGuild::AddPowerWarPoint(unsigned int point)
{
    if ((m_guildDBFlag & 4) != 0)
    {
        m_dBSaveFlag = 1;
        m_dbInfo.m_info.m_powerWarPoint += point;
        if (99999999 < m_dbInfo.m_info.m_powerWarPoint)
        {
            m_dbInfo.m_info.m_powerWarPoint = 99999999;
        }
        DNF_LOG_SCOPE_LINE(0x277,"./log/Guild",
            "GUILD POWERWAR POINT : guild key(%d), add powerwar point(%d), guild powerwar point(%d)",
            GetGuildKey(), point, m_dbInfo.m_info.m_powerWarPoint);
    }
}

void CGuild::SubPowerWarPoint(unsigned int point)
{
    if ((m_guildDBFlag & 4) != 0)
    {
        m_dBSaveFlag = 1;
        if (point < m_dbInfo.m_info.m_powerWarPoint)
        {
            m_dbInfo.m_info.m_powerWarPoint -= point;
        }
        else
        {
            m_dbInfo.m_info.m_powerWarPoint = 0;
        }
        DNF_LOG_SCOPE_LINE(0x290,"./log/Guild",
            "GUILD POWERWAR POINT : guild key(%d), sub powerwar point(%d), guild powerwar point(%d)",
            GetGuildKey(), point, m_dbInfo.m_info.m_powerWarPoint);
    }
}

unsigned int CGuild::GetPowerWarPoint()
{
    return m_dbInfo.m_info.m_powerWarPoint;
}

void CGuild::NoticeGuildMasterDelegateToMembers(char* name)
{
    if ((m_guildDBFlag & 4) == 0)
    {
        CMyFileLog log(__FUNCTION__, 0x2bd);
        log("./log/GuildErr", "GUILD_INFO : NoticeGuildMasterDelegateToMembers, Guild Key(%d)",
            m_guildKey);
        return;
    }
    Packet_Guild_Notice_Guild_Master_Delegate pkt;
    memcpy(pkt.m_name, name, 0x1d);
    for (std::map<unsigned int, CUser*>::iterator it = m_members.begin();
         it != m_members.end(); ++it)
    {
        CUser* member;
        if ((member = it->second) == 0) { continue; }
        pkt.m_channel = member->GetIdByChannel();
        pkt.m_charNo = member->GetUniqCharNo();
        member->SendToGameserver((char*)&pkt, 0x30);
    }
}

void CGuild::SendGuildInfoToMembers(bool flag)
{
    Packet_Monitor_Notice_Guild_Info pkt;
    pkt.m_guildKey = m_guildKey;
    memcpy(&pkt.m_info, &m_dbInfo.m_info, 0xbd);
    pkt.m_padD3 = (char)flag;
    size_t n = strlen(m_guildMessage);
    if (n > 0x64)
    {
        n = 0x64;
    }
    memcpy(pkt.m_rest, m_guildMessage, n);
    for (std::map<unsigned int, CUser*>::iterator it = m_members.begin();
         it != m_members.end(); ++it)
    {
        CUser* member = it->second;
        if (member == 0)
        {
            continue;
        }
        pkt.m_channel = member->GetIdByChannel();
        pkt.m_charNo = member->GetUniqCharNo();
        member->SendToGameserver((char*)&pkt, 0x139);
        ReplyGuildMembers(member);
    }
}

void CGuild::SendGuildNameChangeToMembers()
{
    Packet_Guild_Notice_Guild_Name_Change pkt;
    pkt.m_guildKey = m_guildKey;
    memcpy(pkt.m_name, m_dbInfo.m_info.m_guildName, 0x16);
    for (std::map<unsigned int, CUser*>::iterator it = m_members.begin();
         it != m_members.end(); ++it)
    {
        CUser* u = it->second;
        if (u != 0)
        {
            pkt.m_channel = u->GetIdByChannel();
            pkt.m_charNo = u->GetUniqCharNo();
            u->SendToGameserver((char*)&pkt, 0x2d);
        }
    }
}

void CGuild::SendGuildInfoToMemberOnly(CUser* user)
{
    if ((m_guildDBFlag & 4) != 0)
    {
        // ORIG：直接构造 Packet_Monitor_Notice_Guild_Info（无 placement new，
        // 无 EH landing pad，u=0）。本地视图 + placement new 会引入 _Unwind_Resume。
        Packet_Monitor_Notice_Guild_Info pkt;
        pkt.m_guildKey = m_guildKey;
        memcpy(&pkt.m_info, &m_dbInfo.m_info, 0xbd);
        pkt.m_channel = user->GetIdByChannel();
        pkt.m_charNo = user->GetUniqCharNo();
        int len = strlen(m_guildMessage);
        if (len <= 100)
        {
            memcpy(pkt.m_rest, m_guildMessage, len);
        }
        else
        {
            memcpy(pkt.m_rest, m_guildMessage, 100);
        }
        user->SendToGameserver((char*)&pkt, 0x139);
    }
}

void CGuild::SendGuildInfoToManagers()
{
    if ((m_guildDBFlag & 4) != 0)
    {
        Packet_Monitor_Notice_Guild_Info pkt;
        pkt.m_guildKey = m_guildKey;
        pkt.m_padD3 = 1;
        memcpy(&pkt.m_info, &m_dbInfo.m_info, 0xbd);
        strncpy(pkt.m_rest, m_guildMessage, 100);
        for (std::map<unsigned int, CUser*>::iterator it = m_members.begin();
             it != m_members.end(); ++it)
        {
            CUser* u = it->second;
            if (u != 0)
            {
                if (IsGuildMaster(u->GetUniqCharNo()) == 1 ||
                    IsSubGuildMaster(u->GetUniqCharNo()) == 1)
                {
                    pkt.m_channel = u->GetIdByChannel();
                    pkt.m_charNo = u->GetUniqCharNo();
                    u->SendToGameserver((char*)&pkt, 0x139);
                }
            }
        }
    }
}

void CGuild::SendToGuildForMail()
{
    if ((m_guildDBFlag & 4) == 0)
    {
        return;
    }
    Packet_Monitor_Notify_New_Mail pkt;
    for (std::map<unsigned int, CUser*>::iterator it = m_members.begin();
         it != m_members.end(); ++it)
    {
        CUser* m;
        if ((m = it->second) == 0) { continue; }
        pkt.me = m->GetIdByChannel();
        pkt.ma = m->GetUniqCharNo();
        m->SendToGameserver((char*)&pkt, 0x12);
    }
}

unsigned int CGuild::GetMasterId()
{
    return (m_guildDBFlag & 2) == 0 ? 0 : m_dbInfo.m_info.m_masterId;
}

void CGuild::GuildSkillPointUp(unsigned short point)
{
    if ((m_guildDBFlag & 4) != 0)
    {
        unsigned short old = m_dbInfo.m_info.m_guildSkillPoint;
        m_dbInfo.m_info.m_guildSkillPoint += point;
        if (m_dbInfo.m_info.m_guildSkillPoint < old)
        {
            m_dbInfo.m_info.m_guildSkillPoint = old;
        }
    }
}

bool CGuild::BuyGuildSkill(int skillId, int slot, short param, unsigned int charNo)
{
    if ((m_guildDBFlag & 4) != 0)
    {
    if ((int)m_dbInfo.m_info.m_guildSkillPoint < (int)(short)param)
    {
        unsigned int gsp = (unsigned int)m_dbInfo.m_info.m_guildSkillPoint;
        unsigned int key = GetGuildKey();
        CMyFileLog log(__FUNCTION__, 0x3d2);
        log("./log/GuildSkill", "BUY_SKILL_1, GKey(%d) , Idx(%d), lev(%d), gsp(%d)",
            key, skillId, slot, gsp);
        return 0;
    }
    if (m_dbInfo.m_info.m_guildFund < charNo)
    {
        unsigned int fund = m_dbInfo.m_info.m_guildFund;
        unsigned int key = GetGuildKey();
        CMyFileLog log(__FUNCTION__, 0x3d9);
        log("./log/GuildSkill", "BUY_SKILL_1, GKey(%d) , Idx(%d), lev(%d), guildfund(%d)",
            key, skillId, slot, fund);
        return 0;
    }
    SubGuildFund(charNo);
    int found = -1;
    for (int i = 0; i <= 0xf; i++)
    {
        if ((unsigned int)m_dbInfo.m_info.m_skills[i].m0 == (unsigned int)skillId)
        {
            found = i;
            break;
        }
    }
    if (found == -1)
    {
        unsigned char* learnCnt = &m_dbInfo.m_info.m_skillLearnCnt;
        if (*learnCnt > 0xf)
        {
            CMyFileLog log("BuyGuildSkill", 0x3ee);
            log("./log/GuildSkill", "BUY_SKILL_ERR, GKey(%d) , Learn Cnt(%d)", GetGuildKey(),
                (unsigned int)*learnCnt);
            *learnCnt = 0xf;
            return 0;
        }
        m_dbInfo.m_info.m_skills[(unsigned int)*learnCnt].m0 = skillId;
        m_dbInfo.m_info.m_skills[(unsigned int)*learnCnt].m4 = (unsigned char)slot;
        *learnCnt += 1;
    }
    else
    {
        m_dbInfo.m_info.m_skills[found].m4 = (unsigned char)slot;
    }
    if (m_dbInfo.m_info.m_guildSkillPoint < param)
    {
        m_dbInfo.m_info.m_guildSkillPoint = 0;
    }
    else
    {
        m_dbInfo.m_info.m_guildSkillPoint -= (unsigned short)param;
    }
    m_dBSaveFlag = 1;
    CMyFileLog log(__FUNCTION__, 0x403);
    log("./log/GuildSkill", "BUY_SKILL, GKey(%d) , Idx(%d), lev(%d), gsp(%d)", GetGuildKey(),
        skillId, slot, (unsigned int)m_dbInfo.m_info.m_guildSkillPoint);
    return 1;
    }
    return 0;
}

int CGuild::GuildLevelUp(CServerHandler* handler, CUser* user)
{
    if (user == 0)
    {
        return 2;
    }
    if ((m_guildDBFlag & 4) != 0)
    {
    if (user->GetUniqCharNo() == GetMasterId() ||
        IsSubGuildMaster(user->GetUniqCharNo()))
    {
    }
    else
    {
        return 2;
    }
    if (m_dbInfo.m_info.m_guildLevel + 1 > 0x10)
    {
        int nextLev = m_dbInfo.m_info.m_guildLevel + 1;
        CMyFileLog log(__FUNCTION__, 0x41d);
        log("./log/GuildModify", "CGuild::GuildLevelUp Err (%d)", nextLev);
        return 3;
    }
    m_dbInfo.m_info.m_guildLevel += 1;
    GetGuildBoard()->sendMessageToDBMW_GuildLevelUP(
        handler, m_dbInfo.m_info.m_guildLevel, user);
    GuildSkillPointUp(1);
    if (m_dbInfo.m_info.m_guildLevel == 1)
    {
        m_dbInfo.m_info.m_flags |= 2;
    }
    if (user->GetGameServer() != 0)
    {
        SaveGuild((unsigned char)user->GetGameServer()->GetGroupNo(), handler, 0);
        SaveGuild((unsigned char)user->GetGameServer()->GetGroupNo(), handler, 1);
    }
    else
    {
        CMyFileLog log(__FUNCTION__, 0x445);
        log("./log/Except", "CGuild::GuildLevelUp : pclUser->GetGameServer() == 0");
    }
    SendGuildInfoToMembers(false);
    {
        unsigned short gsp = m_dbInfo.m_info.m_guildSkillPoint;
        unsigned char lev = m_dbInfo.m_info.m_guildLevel;
        unsigned int key = GetGuildKey();
        CMyFileLog log(__FUNCTION__, 0x44a);
        log("./log/GuildModify", "CGuild::GuildLevelUp(%d : %d)GSP(%d)", key, lev, gsp);
    }
    return 0;
    }
    return 2;
}

void CGuild::SendToGuild(PacketHeader* pkt)
{
    for (std::map<unsigned int, CUser*>::iterator it = m_members.begin();
         it != m_members.end(); ++it)
    {
        CUser* pUser;
        if ((pUser = it->second) == 0) { continue; }
        pUser->SendToGameserver((char*)pkt, pkt->packetSize);
    }
}

void CGuild::NoticeEnterToGuildMember(char* info)
{
    if ((m_guildDBFlag & 4) == 0 && (m_guildDBFlag & 2) == 0)
    {
        return;
    }
    if (m_members.empty())
    {
        return;
    }
    Packet_Monitor_Notice_Guild_Enter_ToUser pkt;
    memcpy(&pkt.m_info, info, 0x46);
    for (std::map<unsigned int, CUser*>::iterator it = m_members.begin();
         it != m_members.end(); ++it)
    {
        CUser* u;
        if ((u = it->second) == 0) { continue; }
        pkt.m_a = u->GetIdByChannel();
        pkt.m_b = u->GetUniqCharNo();
        pkt.m_totalCnt = m_dbInfo.m_info.m_totalCnt;
        u->SendToGameserver((char*)&pkt, 0x5a);
    }
}

void CGuild::NoticeSecedeToGuildMember(char* info)
{
    if ((m_guildDBFlag & 4) == 0)
    {
        return;
    }
    if (m_members.empty())
    {
        return;
    }
    Packet_Monitor_Notice_Guild_Secede_ToUser pkt;
    memcpy(&pkt.m_info, info, 0x43);
    for (std::map<unsigned int, CUser*>::iterator it = m_members.begin();
         it != m_members.end(); ++it)
    {
        CUser* u;
        if ((u = it->second) == 0) { continue; }
        pkt.m_channel = u->GetIdByChannel();
        pkt.m_charNo = u->GetUniqCharNo();
        u->SendToGameserver((char*)&pkt, 0x55);
    }
}

void CGuild::NoticeMarkChangeToGuildMember(unsigned int charNo)
{
    if ((m_guildDBFlag & 4) == 0)
    {
        return;
    }
    if (m_members.empty())
    {
        return;
    }
    Packet_Monitor_Notice_Guild_Mark_Change_ToUser pkt;
    for (std::map<unsigned int, CUser*>::iterator it = m_members.begin();
         it != m_members.end(); ++it)
    {
        CUser* member;
        if ((member = it->second) == 0) { continue; }
        int channel = member->GetIdByChannel();
        unsigned int memberNo = member->GetUniqCharNo();
        pkt.m_channel = channel;
        pkt.m_charNo = memberNo;
        pkt.m_charNo2 = charNo;
        CMyFileLog log(__FUNCTION__, 0x4bf);
        log("./log/Web", "[GUILD MARK CHANGE] Send to game server. (channel:%d, character:%u, guildkey:%d)\n",
            channel, memberNo, charNo);
        member->SendToGameserver((char*)&pkt, 0x16);
    }
}

void CGuild::NoticeChatMsgToGuildMembers(unsigned int charNo, char* msg, int len,
                                         const char* name)
{
    if (len >= 0x100)
    {
        return;
    }
    if ((m_guildDBFlag & 4) == 0)
    {
        return;
    }
    if (m_members.empty())
    {
        return;
    }
    Packet_Monitor_Guild_Chat_ToUser pkt;
    for (std::map<unsigned int, CUser*>::iterator it = m_members.begin();
         it != m_members.end(); ++it)
    {
        CUser* u;
        if ((u = it->second) == 0) { continue; }
        pkt.m_channel = u->GetIdByChannel();
        pkt.m_charNo = u->GetUniqCharNo();
        memcpy(pkt.m_name, name, 0x1d);
        pkt.m_len = (unsigned char)len;
        memcpy(pkt.m_msg, msg, len);
        pkt.packetSize = (unsigned short)(len + 0x31);
        u->SendToGameserver((char*)&pkt, (unsigned int)(unsigned short)(len + 0x31));
    }
}

void CGuild::NoticeChatMsgToGuildMembersHyperLink(unsigned int charNo, char* msg, int len,
                                                  unsigned char type,
                                                  const hyperlink_item_info* link,
                                                  const char* name)
{
    if (len >= 0x100)
    {
        return;
    }
    if ((m_guildDBFlag & 4) == 0)
    {
        return;
    }
    if (m_members.empty())
    {
        return;
    }
    Packet_Monitor_Guild_Chat_ToUser_Hyper_Link pkt;
    for (std::map<unsigned int, CUser*>::iterator it = m_members.begin();
         it != m_members.end(); ++it)
    {
        CUser* user;
        if ((user = it->second) == 0) { continue; }
        pkt.m_channel = user->GetIdByChannel();
        pkt.m_charNo = user->GetUniqCharNo();
        memcpy(pkt.m_name, name, 0x1d);
        pkt.m_type = type;
        for (int i = 0; i < (int)type; i++)
        {
            memcpy(pkt.m_items + i * 0x68, (char*)link + i * 0x68, 0x68);
        }
        pkt.m_len = (char)len;
        memcpy(pkt.m_msg, msg, (size_t)len);
        pkt.packetSize = (unsigned short)(len + 0x16a);
        user->SendToGameserver((char*)&pkt, (unsigned int)(len + 0x16a));
    }
}

void CGuild::DismissGuildMemberAndNotice(int group)
{
    if ((m_guildDBFlag & 4) == 0)
        return;
    if (m_members.empty())
        return;
    Packet_Monitor_Notice_Guild_Dismiss_ToUser notice;
    for (std::map<unsigned int, CUser*>::iterator it = m_members.begin();
         it != m_members.end(); ++it)
    {
        CUser* user = it->second;
        if (user)
        {
            notice.m_channel = user->GetIdByChannel();
            notice.m_charNo = user->GetUniqCharNo();
            user->SendToGameserver((char*)&notice, 0x12);
            Packet_Guild_Exp_Book_Delete expDel;
            expDel.m_channel = user->GetIdByChannel();
            expDel.m_charNo = user->GetUniqCharNo();
            expDel.m16 = group;
            expDel.m_group = user->GetDBID();
            user->SendTcpGameserver(&expDel);
            user->ResetGuild();
        }
    }
}

void CGuild::NoticeGuildMemberLogin_Out(CUser* user, char flag)
{
    if (user == 0 || user->GetGameServer() == 0)
    {
        return;
    }
    if ((m_guildDBFlag & 4) == 0 || m_members.empty())
    {
        return;
    }
    Packet_Monitor_Notice_Guild_Member_Login_out pkt;
    for (std::map<unsigned int, CUser*>::iterator it = m_members.begin();
         it != m_members.end(); ++it)
    {
        CUser* member = it->second;
        if (member == 0 || member == user)
        {
            continue;
        }
        if (user->IsBlackUser(member->GetUniqCharNo()))
        {
            continue;
        }
        pkt.m_flag = flag;
        pkt.m_channel = member->GetIdByChannel();
        pkt.m_charNo = member->GetUniqCharNo();
        pkt.m_channelNo = (char)user->GetGameServer()->GetChannelNo();
        memcpy(pkt.m_name, user->GetCharName(), 0x1d);
        member->SendToGameserver((char*)&pkt, 0x32);
    }
}

int CGuild::ReplyGuildMembersToWeb(STGuildMemberWebConnInfo* info)
{
    int count = 0;
    if ((m_guildDBFlag & 4) != 0)
    {
        if (m_members.empty())
        {
            return 0;
        }
        std::map<unsigned int, CUser*>::iterator it;
        for (it = m_members.begin(); it != m_members.end(); ++it)
        {
            if (it->second != 0 && !(299 < count))
            {
                STGuildMemberWebConnInfo_Rec* rec = &info->m_members[count];
                rec->m_charNo = it->second->GetUniqCharNo();
                if (it->second->GetGameServer() != 0)
                {
                    rec->m_channel = it->second->GetGameServer()->GetChannelNo();
                }
                count++;
            }
        }
    }
    return count;
}

void CGuild::ReplyGuildMembers(CUser* user)
{
    if (user == 0 || (m_guildDBFlag & 4) == 0 || m_members.empty())
    {
        return;
    }
    Packet_Monitor_Call_Guild_Members_ToChannel pkt;
    char* buf = (char*)&pkt;
    *(unsigned int*)(buf + 0x12) = m_guildKey;
    memcpy(buf + 0x16, m_dbInfo.m_info.m_guildName, 0x16);
    *(unsigned int*)(buf + 0x2e) = m_dbInfo.m_info.m_guildPoint;
    *(unsigned short*)(buf + 0x32) = m_dbInfo.m_info.m_totalCnt;
    int count = 0;
    std::map<unsigned int, CUser*>::iterator it = m_members.begin();
    while (it != m_members.end())
    {
        CUser* m = it->second;
        char* rec = buf + 0x34 + count * 0x3f;
        rec[0] = m->GetJob();
        rec[1] = m->GetGrowthType();
        *(unsigned short*)(rec + 2) = m->GetLevel();
        memcpy(rec + 4, m->GetCharName(), 0x1d);
        rec[0x38] = m->GetSex();
        if (m->GetGameServer() != 0)
        {
            rec[0x37] = m->GetGameServer()->GetChannelNo();
        }
        memcpy(rec + 0x22, (char*)m->GetGuildMemDBInfo(), 0x14);
        rec[0x3a] = (char)m->GetGuildMemDBInfo()->m_grade;
        if (m->IsBlackUser(user->GetUniqCharNo()) != 0)
        {
            rec[0x39] = 1;
        }
        count++;
        ++it;
        if (0x5f < count)
        {
            break;
        }
    }
    *(unsigned char*)(buf + 0x2d) = (unsigned char)count;
    *(unsigned short*)(buf + 2) = (unsigned short)(count * 0x3f + 0x34);
    *(unsigned int*)(buf + 0xa) = user->GetIdByChannel();
    *(unsigned int*)(buf + 0xe) = user->GetUniqCharNo();
    user->SendToGameserver(buf, count * 0x3f + 0x34);
    if (it != m_members.end())
    {
        Packet_Monitor_Call_Guild_Members_ToChannel_Next pkt2;
        char* buf2 = (char*)&pkt2;
        *(unsigned int*)(buf2 + 0x12) = m_guildKey;
        int cnt2 = 0;
        while (it != m_members.end())
        {
            CUser* m = it->second;
            char* rec = buf2 + 0x17 + cnt2 * 0x3f;
            rec[0] = m->GetJob();
            rec[1] = m->GetGrowthType();
            *(unsigned short*)(rec + 2) = m->GetLevel();
            memcpy(rec + 4, m->GetCharName(), 0x1d);
            rec[0x38] = m->GetSex();
            if (m->GetGameServer() != 0)
            {
                rec[0x37] = m->GetGameServer()->GetChannelNo();
            }
            memcpy(rec + 0x22, (char*)m->GetGuildMemDBInfo(), 0x14);
            rec[0x3a] = (char)m->GetGuildMemDBInfo()->m_grade;
            if (m->IsBlackUser(user->GetUniqCharNo()) != 0)
            {
                rec[0x39] = 1;
            }
            cnt2++;
            ++it;
            if (0x5f < cnt2)
            {
                *(unsigned char*)(buf2 + 0x16) = (unsigned char)cnt2;
                *(unsigned short*)(buf2 + 2) = 0x17b7;
                *(unsigned int*)(buf2 + 0xa) = user->GetIdByChannel();
                *(unsigned int*)(buf2 + 0xe) = user->GetUniqCharNo();
                user->SendToGameserver(buf2, 0x17b7);
                cnt2 = 0;
            }
        }
        if (cnt2 != 0)
        {
            *(unsigned char*)(buf2 + 0x16) = (unsigned char)cnt2;
            *(unsigned short*)(buf2 + 2) = (unsigned short)(cnt2 * 0x3f + 0x17);
            *(unsigned int*)(buf2 + 0xa) = user->GetIdByChannel();
            *(unsigned int*)(buf2 + 0xe) = user->GetUniqCharNo();
            user->SendToGameserver(buf2, cnt2 * 0x3f + 0x17);
        }
    }
}

void CGuild::QueryGuildAllMembersProxy(CServerHandler* handler, unsigned int charNo)
{
    Packet_DB_Call_Guild_All_Members pkt;
    pkt.m_guildKey = m_guildKey;
    pkt.m_charNo = charNo;
    handler->SendToDB(&pkt);
    m_guildDBFlag |= 8;
}

void CGuild::QueryUnconnGuildMemberProxy(CServerHandler* handler, unsigned int charNo)
{
    Packet_DB_Call_Unconn_Guild_Member pkt;
    pkt.m_guildKey = m_guildKey;
    pkt.m_charNo = charNo;
    handler->SendToDB(&pkt);
}

void CGuild::ReplyGuildAllMembers(CUser* user)
{
    if (user == 0 || (m_guildDBFlag & 4) == 0 || (m_guildDBFlag & 0x10) == 0)
    {
        return;
    }
    Packet_Monitor_Call_Guild_All_Members_ToChannel pkt;
    char* buf = (char*)&pkt;
    pkt.m_fieldC = m_guildKey;
    memcpy(buf + 0x16, m_dbInfo.m_info.m_guildName, 0x16);
    pkt.m_fieldD = m_dbInfo.m_info.m_guildPoint;
    unsigned short total = m_totalCnt;
    int count = 0;
    if (m_dbInfo.m_info.m_totalCnt != m_totalCnt)
    {
        DNF_LOG_SCOPE_LINE(0x63b,"./log/GuildModify", "Error Guild Member Cnt Not Valid , (%d)/(%d)",
            (unsigned int)m_dbInfo.m_info.m_totalCnt,
            (unsigned int)m_totalCnt);
        m_dbInfo.m_info.m_totalCnt = m_totalCnt;
    }
    int idx = 0;
    for (idx = 0; idx < (int)total; idx++)
    {
        char* src = (char*)&m_dbInfo.m_members[idx];
        char* rec = buf + 0x34 + count * 0x3f;
        rec[0] = src[0x22];
        rec[1] = src[0x23];
        *(unsigned short*)(rec + 2) = *(unsigned short*)(src + 0x24);
        memcpy(rec + 4, src + 4, 0x1d);
        rec[0x38] = src[0x26];
        rec[0x3a] = src[0x27];
        *(unsigned int*)(rec + 0x3b) = *(unsigned int*)(src + 0x28);
        CUser* m = FindGuildMember(*(unsigned int*)src);
        if (m == 0)
        {
            memcpy(rec + 0x22, src + 0x2c, 0x14);
        }
        else
        {
            rec[1] = m->GetGrowthType();
            *(unsigned short*)(rec + 2) = m->GetLevel();
            if (m->GetGameServer() == 0)
            {
                DNF_LOG_SCOPE_LINE(0x658,"./log/Except",
                    "CGuild::ReplyGuildMembers() Guild Key : %d\tGuild Name : %s\tDB Id : %d\tChar Id : %d\n",
                    GetGuildKey(), GetGuildName(), m->GetDBID(), *(unsigned int*)src);
            }
            else
            {
                rec[0x37] = m->GetGameServer()->GetChannelNo();
                memcpy(rec + 0x22, (char*)m->GetGuildMemDBInfo(), 0x14);
                rec[0x3a] = (char)m->GetGuildMemDBInfo()->m_grade;
            }
            if (m->IsBlackUser(user->GetUniqCharNo()) != 0)
            {
                rec[0x39] = 1;
            }
        }
        count++;
        if (0x5f < count)
        {
            break;
        }
    }
    *(unsigned char*)(buf + 0x33) = (unsigned char)count;
    *(unsigned short*)(buf + 2) = (unsigned short)(count * 0x3f + 0x34);
    *(unsigned int*)(buf + 0xa) = user->GetIdByChannel();
    *(unsigned int*)(buf + 0xe) = user->GetUniqCharNo();
    user->SendTcpGameserver((PacketHeader*)buf);
    if (count < (int)total)
    {
        Packet_Monitor_Call_Guild_All_Members_ToChannel_Next pkt2;
        char* buf2 = (char*)&pkt2;
        *(unsigned int*)(buf2 + 0x12) = m_guildKey;
        int cnt2 = 0;
        for (int i = idx + 1; i < (int)total; i++)
        {
            char* src = (char*)&m_dbInfo.m_members[i];
            char* rec = buf2 + 0x17 + cnt2 * 0x3f;
            rec[0] = src[0x22];
            rec[1] = src[0x23];
            *(unsigned short*)(rec + 2) = *(unsigned short*)(src + 0x24);
            memcpy(rec + 4, src + 4, 0x1d);
            rec[0x38] = src[0x26];
            rec[0x3a] = src[0x27];
            *(unsigned int*)(rec + 0x3b) = *(unsigned int*)(src + 0x28);
            CUser* m = FindGuildMember(*(unsigned int*)src);
            if (m == 0)
            {
                rec[0] = (char)0xff;
                memset(rec + 0x22, 0, 0x14);
                rec[0x38] = 0;
            }
            else
            {
                rec[1] = m->GetGrowthType();
                *(unsigned short*)(rec + 2) = m->GetLevel();
                if (m->GetGameServer() == 0)
                {
                    DNF_LOG_SCOPE_LINE(0x695,"./log/Except",
                        "CGuild::ReplyGuildMembers() Guild Key : %d\tGuild Name : %s\tDB Id : %d\tChar Id : %d\n",
                        GetGuildKey(), GetGuildName(), m->GetDBID(), *(unsigned int*)src);
                }
                else
                {
                    rec[0x37] = m->GetGameServer()->GetChannelNo();
                    memcpy(rec + 0x22, (char*)m->GetGuildMemDBInfo(), 0x14);
                    rec[0x3a] = (char)m->GetGuildMemDBInfo()->m_grade;
                }
                if (m->IsBlackUser(user->GetUniqCharNo()) != 0)
                {
                    rec[0x39] = 1;
                }
            }
            cnt2++;
            if (0x5f < cnt2)
            {
                *(unsigned char*)(buf2 + 0x16) = (unsigned char)cnt2;
                *(unsigned short*)(buf2 + 2) = 0x17b7;
                *(unsigned int*)(buf2 + 0xa) = user->GetIdByChannel();
                *(unsigned int*)(buf2 + 0xe) = user->GetUniqCharNo();
                user->SendTcpGameserver((PacketHeader*)buf2);
                cnt2 = 0;
            }
        }
        if (cnt2 != 0)
        {
            *(unsigned char*)(buf2 + 0x16) = (unsigned char)cnt2;
            *(unsigned short*)(buf2 + 2) = (unsigned short)(cnt2 * 0x3f + 0x17);
            *(unsigned int*)(buf2 + 0xa) = user->GetIdByChannel();
            *(unsigned int*)(buf2 + 0xe) = user->GetUniqCharNo();
            user->SendTcpGameserver((PacketHeader*)buf2);
        }
    }
}

void CGuild::CallGuildAllMembersProxy(CUser* user, CServerHandler* handler)
{
    if (user == 0)
    {
        return;
    }
    if ((m_guildDBFlag & 4) != 0)
    {
        if (m_members.empty())
        {
            return;
        }
        if ((m_guildDBFlag & 0x10) != 0)
        {
            ReplyGuildAllMembers(user);
            return;
        }
        if ((m_guildDBFlag & 8) == 0)
        {
            QueryGuildAllMembersProxy(handler, user->GetUniqCharNo());
        }
    }
}

void CGuild::LoadGuildAllMembersProxy(STGuildMemberProxy* proxy, unsigned char flag,
                                      unsigned char param)
{
    if ((m_guildDBFlag & 4) != 0 && (m_guildDBFlag & 8) != 0)
    {
        if (flag == 0)
        {
            memcpy(&m_dbInfo.m_members[0], proxy, (size_t)param * 0x41);
            m_totalCnt = param;
        }
        else
        {
            memcpy(&((CGuildMemberMainArray*)this)->m_members[m_totalCnt].m_charNo,
                   proxy, (size_t)param * 0x41);
            m_totalCnt = (unsigned short)(m_totalCnt + param);
        }
    }
}

bool CGuild::LoadGuildOneMemberProxy(CUser* user)
{
    if ((m_guildDBFlag & 4) == 0 || (m_guildDBFlag & 8) == 0)
    {
        return 0;
    }
    if (m_totalCnt <= 0x12b)
    {
        unsigned short idx = m_totalCnt;
        ((CGuildMemberMainArray*)this)->m_members[idx].m_charNo = user->GetUniqCharNo();
        memcpy(((CGuildMemberMainArray*)this)->m_members[idx].m_name,
               user->GetCharName(), 0x1d);
        ((CGuildMemberExtraArray*)this)->m_members[idx].m_job = (char)user->GetJob();
        ((CGuildMemberExtraArray*)this)->m_members[idx].m_growth = (char)user->GetGrowthType();
        ((CGuildMemberExtraArray*)this)->m_members[idx].m_sex = (char)user->GetSex();
        ((CGuildMemberExtraArray*)this)->m_members[idx].m_level =
            (unsigned short)user->GetLevel();
        m_totalCnt++;
    }
    if (m_totalCnt > 0x12b)
    {
        CMyFileLog log("LoadGuildOneMemberProxy", 0x73d);
        log("./log/GuildErr", "Guild Member Cnt Full Or Over : G Key(%d), Cnt(%d)", m_guildKey,
            (unsigned int)m_totalCnt);
        if (m_totalCnt > 0x12c)
        {
            m_totalCnt = 0x12c;
        }
    }
    m_dbInfo.m_info.m_totalCnt = m_totalCnt;
    return 1;
}

bool CGuild::LoadGuildOneMemberProxy(STGuildMemberProxy& proxy)
{
    if ((m_guildDBFlag & 4) == 0 || (m_guildDBFlag & 0x10) == 0)
    {
        return 0;
    }
    if (m_totalCnt <= 0x12b)
    {
        memcpy(&((CGuildMemberMainArray*)this)->m_members[m_totalCnt].m_charNo, &proxy, 0x41);
        m_totalCnt++;
    }
    if (m_totalCnt > 0x12b)
    {
        CMyFileLog log(__FUNCTION__, 0x719);
        log("./log/GuildErr", "Guild Member Cnt Full Or Over : G Key(%d), Cnt(%d)", m_guildKey,
            (unsigned int)m_totalCnt);
        if (m_totalCnt > 0x12c)
        {
            m_totalCnt = 0x12c;
        }
    }
    m_dbInfo.m_info.m_totalCnt = m_totalCnt;
    return 1;
}

void CGuild::AddGuildMember(ST_Notice_Guild_Enter& info, CUser* user)
{
    if (user == 0)
    {
        return;
    }
    if ((m_guildDBFlag & 4) == 0)
    {
        return;
    }
    if ((m_guildDBFlag & 0x10) == 0)
    {
        return;
    }
    if (m_members.empty())
    {
        return;
    }
    unsigned short idx = m_totalCnt;
    m_dbInfo.m_members[idx].m_no = info.m_charNo;
    memcpy(m_dbInfo.m_members[idx].m_name, info.m_charName, 0x1d);
    m_dbInfo.m_members[idx].m_job = user->GetJob();
    m_dbInfo.m_members[idx].m_growType = user->GetGrowthType();
    m_dbInfo.m_members[idx].m_lev = user->GetLevel();
    idx++;
    if (300 < idx)
    {
        idx = 300;
    }
    m_totalCnt = idx;
    m_dbInfo.m_info.m_totalCnt = idx;
}

void CGuild::SecedeProxyMember(ST_Notice_Guild_Secede& info)
{
    if ((m_guildDBFlag & 4) != 0 && (m_guildDBFlag & 0x10) != 0 && !m_members.empty())
    {
        unsigned short cnt = m_totalCnt;
        if (cnt < 0x12d)
        {
            for (int i = 0; i < (int)cnt; i++)
            {
                if (m_dbInfo.m_members[i].m_no == (int)info.m_charNo)
                {
                    if ((unsigned int)cnt - (unsigned int)i != 1)
                    {
                        memmove(&m_dbInfo.m_members[i],
                                &m_dbInfo.m_members[i + 1],
                                ((unsigned int)cnt - (unsigned int)i - 1) * 0x41);
                    }
                    cnt--;
                    if (300 < cnt)
                    {
                        cnt = 0;
                    }
                    m_totalCnt = cnt;
                    m_dbInfo.m_info.m_totalCnt = cnt;
                    return;
                }
            }
        }
    }
}

bool CGuild::ChangeGuildMaster(CServerHandler* handler, CUser* user, unsigned int charNo)
{
    if ((m_guildDBFlag & 4) == 0 || charNo == 0)
    {
        return 0;
    }
    m_dbInfo.m_info.m_masterId = charNo;
    ChangeUnconnectedGuildMemberGrade(charNo, 1);
    CUser* member = FindGuildMember(charNo);
    if (member == 0)
    {
        char* name = getUnconnectedGuildMemberName(charNo);
        GetGuildBoard()->sendMessageToDBMW_GuildMasterChanging(handler, user, name);
        if (getUnconnectedGuildMemberName(charNo) != 0)
        {
            strncpy(m_dbInfo.m_info.m_masterName, getUnconnectedGuildMemberName(charNo), 0x14);
        }
    }
    else
    {
        member->ChangeGuildMemberGrade(1);
        GetGuildBoard()->sendMessageToDBMW_GuildMasterChanging(
            handler, user, member->GetCharName());
        strncpy(m_dbInfo.m_info.m_masterName, member->GetCharName(), 0x14);
    }
    return 1;
}

bool CGuild::ChangeGuildName(char* name, int flag)
{
    if ((m_guildDBFlag & 4) != 0)
    {
        strcpy(m_dbInfo.m_info.m_guildName, name);
        m_dBSaveFlag = (unsigned char)flag;
        return true;
    }
    return false;
}

void CGuild::ChangeUnconnectedGuildMemberGrade(unsigned int charNo, int grade)
{
    if ((m_guildDBFlag & 4) != 0 && (m_guildDBFlag & 0x10) != 0)
    {
        for (int i = 0; i < m_totalCnt; i++)
        {
            if (((CGuildMemberMainArray*)this)->m_members[i].m_charNo == charNo)
            {
                ((CGuildMemberExtraArray*)this)->m_members[i].m_grade =
                    (unsigned char)grade;
                return;
            }
        }
    }
}

char* CGuild::getUnconnectedGuildMemberName(unsigned int charNo)
{
    if ((m_guildDBFlag & 4) != 0)
    {
        if ((m_guildDBFlag & 0x10) != 0)
        {
            for (int i = 0; i < m_totalCnt; i++)
            {
                if (((CGuildMemberMainArray*)this)->m_members[i].m_charNo == charNo)
                {
                    return m_dbInfo.m_members[i].m_name;
                }
            }
        }
    }
    return 0;
}

bool CGuild::IsSubGuildMaster(unsigned int dbid)
{
    for (int i = 0; i < m_dbInfo.m_info.m_subMasterCnt; i++)
    {
        if (m_dbInfo.m_info.m_subGuildMaster[i] == dbid)
        {
            return true;
        }
    }
    return false;
}

void CGuild::SetSubGuildMaster(unsigned int charNo, bool flag)
{
    if (flag)
    {
        if (m_dbInfo.m_info.m_subMasterCnt + 1 < 6)
        {
            m_dbInfo.m_info.m_subGuildMaster[m_dbInfo.m_info.m_subMasterCnt++] = charNo;
        }
    }
    else
    {
        unsigned char* cnt = &m_dbInfo.m_info.m_subMasterCnt;
        for (int i = 0; i < (int)*cnt; i++)
        {
            if (m_dbInfo.m_info.m_subGuildMaster[i] == charNo)
            {
                memcpy(m_dbInfo.m_info.m_subGuildMaster + i,
                       m_dbInfo.m_info.m_subGuildMaster + i + 1,
                       ((unsigned int)*cnt + ~i) * 4);
                *cnt = (unsigned char)((int)*cnt - 1);
                register bool over = 5 < (unsigned char)*cnt;
                if (over)
                {
                    *cnt = 0;
                }
                break;
            }
        }
    }
    DNF_LOG_SCOPE_LINE(0x84e,"./log/GuildModify", "Set Sub Guild Master guild(%d) CharNo(%d) flag(%d)",
        GetGuildKey(), charNo, (unsigned int)flag);
}

void CGuild::CheckGuildSkill()
{
    STGuildDBInfo* p = &m_dbInfo;
    if (p->m_info.m_guildLevel != 0 && p->m_info.m_guildSkillPoint == 0 && p->m_info.m_skillLearnCnt == 0)
    {
        DNF_LOG_SCOPE_LINE(0x85c,"./log/GuildSkill", "Err : key(%d), lev(%d), gsp(0), cnt(0)",
            GetGuildKey(), (unsigned int)p->m_info.m_guildLevel);
    }
}

void CGuild::SetGuildMessage(char* msg)
{
    memset(m_guildMessage, 0, 0x65);
    int n = strlen(msg);
    if (n <= 100)
    {
        memcpy(m_guildMessage, msg, n);
    }
    else
    {
        memcpy(m_guildMessage, msg, 100);
    }
}

void CGuild::NotifyMessageToGuildMember()
{
    if ((m_guildDBFlag & 4) != 0)
    {
        if (m_members.empty())
        {
        }
        else
        {
            Packet_Guild_Notify_Message_To_Guild_Mem pkt;
            int len = strlen(m_guildMessage);
            if (len <= 100)
            {
                memcpy(pkt.m_msg, m_guildMessage, len);
            }
            else
            {
                memcpy(pkt.m_msg, m_guildMessage, 100);
            }
            for (std::map<unsigned int, CUser*>::iterator it = m_members.begin();
                 it != m_members.end(); ++it)
            {
                CUser* u;
                if ((u = it->second) == 0) { continue; }
                pkt.m_channel = u->GetIdByChannel();
                pkt.m_charNo = u->GetUniqCharNo();
                u->SendToGameserver((char*)&pkt, 0x77);
            }
        }
    }
}

void CGuild::IncTotalCnt_Of_GuildDBInfo()
{
    if ((m_guildDBFlag & 4) != 0)
    {
        m_dbInfo.m_info.m_totalCnt += 1;
        if (m_dbInfo.m_info.m_totalCnt > 0x12c)
        {
            m_dbInfo.m_info.m_totalCnt = 0x12c;
        }
    }
}

void CGuild::DecTotalCnt_Of_GuildDBInfo()
{
    if ((m_guildDBFlag & 4) != 0)
    {
        m_dbInfo.m_info.m_totalCnt -= 1;
        if (0x12c < m_dbInfo.m_info.m_totalCnt)
        {
            m_dbInfo.m_info.m_totalCnt = 0;
        }
    }
}

bool CGuild::InsertGuildMemberChanglableInfo(unsigned int charNo)
{
    STGuildMemberChangableInfo info;
    info.m_time = time(0);
    return m_changable.insert(std::make_pair(charNo, info)).second;
}

int CGuild::PopGuildMemberChanglableInfo(unsigned int charNo,
                                         STGuildMemberChangableInfo& info) const
{
    if (m_changable.empty())
    {
        return 0;
    }
    std::map<unsigned int, STGuildMemberChangableInfo>::const_iterator it =
        m_changable.find(charNo);
    if (it != m_changable.end())
    {
        // ORIG：仅拷贝 map 值的首 dword（其余字段 ORIG 未初始化/不使用）
        *(unsigned int*)&info = *(unsigned int*)&it->second;
        return 1;
    }
    return 0;
}

void CGuild::UpdateChangableInfoProcess()
{
    if (IsSetGuildDBFlag(4) && IsSetGuildDBFlag(0x10))
    {
        m_changableTickCnt++;
        if (9 < (unsigned char)m_changableTickCnt)
        {
            for (int i = 0; i < m_totalCnt; i++)
            {
                STGuildMemberChangableInfo info;
                if (PopGuildMemberChanglableInfo(
                        ((CGuildMemberMainArray*)this)->m_members[i].m_charNo,
                        info) != 0)
                {
                    ((CGuildMemberExtraArray*)this)->m_members[i].m_changableTime =
                        info.m_time;
                }
            }
            m_changable.clear();
            m_changableTickCnt = 0;
        }
    }
}

bool CGuild::CheckPowerSecedeTime()
{
    if (IsSetGuildDBFlag(4))
    {
        return CheckDayScheduleTimeOver(0x1e, m_dbInfo.m_info.m_powerSecedeTime);
    }
    return 0;
}

int CGuild::ChangeGuildMemberCharName(unsigned int charNo, char* name)
{
    bool result = false;
    if (IsSetGuildDBFlag(4))
    {
        for (int i = 0; i <= 299; i++)
        {
            if (((CGuildMemberMainArray*)this)->m_members[i].m_charNo == charNo)
            {
                memset(((CGuildMemberMainArray*)this)->m_members[i].m_name, 0, 0x1e);
                memcpy(((CGuildMemberMainArray*)this)->m_members[i].m_name, name, 0x1d);
                result = true;
            }
        }
        CGuildBoard::TodayMemberBlock* todayBlock = &m_board.m_today;
        if (todayBlock->m_charNo == charNo)
        {
            memset(&todayBlock->m_name0, 0, 0x1e);
            strncpy((char*)&todayBlock->m_name0, name, 0x1d);
            result = true;
            NotifyAllTodayGuildMember();
        }
        if (m_dbInfo.m_info.m_masterId == charNo)
        {
            memset(m_dbInfo.m_info.m_masterName, 0, 0x15);
            memcpy(m_dbInfo.m_info.m_masterName, name, 0x14);
            result = true;
        }
    }
    return result;
}

void CGuild::DBSaveGuildMemberUnChangableInfo(CServerHandler* handler, unsigned int a,
                                              unsigned int b, char* name)
{
    if (IsSetGuildDBFlag(4))
    {
        Packet_UnChangable_GuildInfo_Save pkt;
        pkt.m_guildKey = a;
        pkt.m_charNo = b;
        memcpy(pkt.m_name, name, 0x1d);
        handler->SendToDB(&pkt);
    }
}

void CGuild::WriteGuildMemberMemo(CUser* user, const char* memo)
{
    if (user == 0 || memo == 0)
    {
        return;
    }
    user->SetGuildMemberMemo(memo);
    if (IsSetGuildDBFlag(4) && IsSetGuildDBFlag(0x10))
    {
        for (int i = 0; i < 300; i++)
        {
            if (((CGuildMemberMainArray*)this)->m_members[i].m_charNo ==
                (unsigned int)user->GetUniqCharNo())
            {
                memset(((CGuildMemberExtraArray*)this)->m_members[i].m_memo, 0, 0x15);
                memcpy(((CGuildMemberExtraArray*)this)->m_members[i].m_memo, memo, 0x14);
                return;
            }
        }
    }
}

void CGuild::NotifyMemoToGuildMember(CUser* user, const char* memo)
{
    if ((m_guildDBFlag & 4) != 0)
    {
        if (m_members.empty())
        {
            return;
        }
        Packet_Guild_Notify_Guild_Member_Memo pkt;
        char* name = user->GetCharName();
        int n = (int)strlen(name);
        if ((int)n < 0x1e)
        {
            memcpy(pkt.m_name, name, n);
        }
        else
        {
            memcpy(pkt.m_name, name, 0x1d);
        }
        n = strlen(memo);
        if ((int)n < 0x15)
        {
            memcpy(pkt.m_memo, memo, n);
        }
        else
        {
            memcpy(pkt.m_memo, memo, 0x14);
        }
        pkt.m_channel = user->GetIdByChannel();
        pkt.m_charNo = user->GetUniqCharNo();
        user->SendToGameserver((char*)&pkt, 0x45);
    }
}

void CGuild::CreateGuildAgit(CServerHandler* handler, unsigned int a, unsigned int b,
                             unsigned int c, unsigned int d)
{
    if ((m_guildDBFlag & 4) != 0)
    {
        SubGuildFund(d);
        SubPowerWarPoint(c);
        SendGuildInfoToMembers(false);
        Packet_DB_Create_Guild_Agit pkt;
        pkt.m_guildKey = a;
        pkt.m_charNo = b;
        handler->SendToDB(&pkt);
    }
}

void CGuild::DeleteGuildAgit(CServerHandler* handler, unsigned int a, unsigned int b)
{
    if ((m_guildDBFlag & 4) != 0)
    {
        Packet_DB_Delete_Guild_Agit pkt;
        pkt.m_guildKey = a;
        pkt.m_charNo = b;
        handler->SendToDB(&pkt);
    }
}

void CGuild::LoadGuildAgit(CServerHandler* handler, unsigned int charNo)
{
    if ((m_guildDBFlag & 4) != 0)
    {
        Packet_DB_Load_Guild_Agit pkt;
        pkt.m_charNo = charNo;
        handler->SendToDB(&pkt);
    }
}

void CGuild::SetGuildAgitInfo(STGuildAgitDBInfo& info)
{
    if ((m_guildDBFlag & 4) != 0)
    {
        memcpy(&m_agitInfo, &info, 1);
        m_cargo.SetGuildInfo((int)m_guildKey);
    }
}

void CGuild::SendGuildAgitInfoToMembers()
{
    Packet_Channel_Guild_Agit_Info pkt;
    pkt.m_guildKey = m_guildKey;
    memcpy(&pkt.m_info, &m_agitInfo, 1);
    for (std::map<unsigned int, CUser*>::iterator it = m_members.begin();
         it != m_members.end(); ++it)
    {
        CUser* u = it->second;
        if (u != 0)
        {
            pkt.m_channel = u->GetIdByChannel();
            pkt.m_charNo = u->GetUniqCharNo();
            u->SendToGameserver((char*)&pkt, 0x17);
        }
    }
}

void CGuild::NotifyCreateGuildAgitToGuildMember(unsigned int charNo)
{
    if ((m_guildDBFlag & 4) == 0)
    {
        return;
    }
    if (m_members.empty())
    {
        return;
    }
    Packet_Channel_Create_Guild_Agit pkt;
    pkt.m_charNo = charNo;
    for (std::map<unsigned int, CUser*>::iterator it = m_members.begin();
         it != m_members.end(); ++it)
    {
        CUser* u;
        if ((u = it->second) == 0) { continue; }
        pkt.m_channel = u->GetIdByChannel();
        pkt.m_charNo2 = u->GetUniqCharNo();
        u->SendToGameserver((char*)&pkt, 0x16);
    }
}

void CGuild::NotifyDeleteGuildAgitToGuildMember(unsigned int charNo)
{
    if ((m_guildDBFlag & 4) == 0)
    {
        return;
    }
    if (m_members.empty())
    {
        return;
    }
    Packet_Channel_Delete_Guild_Agit pkt;
    pkt.m12 = charNo;
    for (std::map<unsigned int, CUser*>::iterator it = m_members.begin();
         it != m_members.end(); ++it)
    {
        CUser* u;
        if ((u = it->second) == 0) { continue; }
        pkt.ma = u->GetIdByChannel();
        pkt.me = u->GetUniqCharNo();
        u->SendToGameserver((char*)&pkt, 0x17);
    }
}

void CGuild::UpgradeGuildAgit(CServerHandler* handler, unsigned int a, unsigned int b,
                              unsigned int c, unsigned int d)
{
    if ((m_guildDBFlag & 4) != 0)
    {
        SubGuildFund(d);
        SubPowerWarPoint(c);
        SendGuildInfoToMembers(false);
        Packet_DB_Upgrade_Guild_Agit pkt;
        pkt.m_guildKey = a;
        pkt.m_charNo = b;
        handler->SendToDB(&pkt);
    }
}

void CGuild::IncPowerJoinCount()
{
    if ((m_guildDBFlag & 4) != 0)
    {
        m_dBSaveFlag = 1;
        m_dbInfo.m_info.m_powerJoinCount = (char)(m_dbInfo.m_info.m_powerJoinCount + 1);
        if (0x80 < (unsigned char)m_dbInfo.m_info.m_powerJoinCount)
        {
            m_dbInfo.m_info.m_powerJoinCount = (char)0x80;
        }
        DNF_LOG_SCOPE_LINE(0xacf,"./log/Guild", "IncPowerJoinCount Guild(%d), JoinCount(%d)",
            GetGuildKey(), (unsigned int)(unsigned char)m_dbInfo.m_info.m_powerJoinCount);
    }
}

unsigned int CGuild::GetGuildFund()
{
    return (m_guildDBFlag & 4) == 0 ? 0 : m_dbInfo.m_info.m_guildFund;
}

bool CGuild::IsCompleteGuildFund()
{
    if ((m_guildDBFlag & 4) != 0)
    {
        unsigned int* fundPtr = &m_dbInfo.m_info.m_guildFund;
        unsigned char* levelPtr = &m_dbInfo.m_info.m_guildLevel;
        if (*levelPtr > 0xf && *fundPtr > 0x98967f)
        {
            return true;
        }
        if (*fundPtr > 0x1312cff)
        {
            return true;
        }
    }
    return false;
}

void CGuild::AddGuildFund(unsigned int fund)
{
    if ((m_guildDBFlag & 4) != 0)
    {
        m_dBSaveFlag = 1;
        unsigned int* fundPtr = &m_dbInfo.m_info.m_guildFund;
        unsigned char* levelPtr = &m_dbInfo.m_info.m_guildLevel;
        *fundPtr += fund;
        if (*levelPtr > 0xf && *fundPtr > 0x989680)
        {
            *fundPtr = 0x989680;
        }
        else
        {
            if (*fundPtr > 0x1312d00)
            {
                *fundPtr = 0x1312d00;
            }
        }
        DNF_LOG_SCOPE_LINE(0xb0a,"./log/GuildFund",
            "CPacketTranslater::AddGuildFund() : GUILD_ID (%u), GUILD_FUND (%u), GOLD (%u)\n",
            GetGuildKey(), m_dbInfo.m_info.m_guildFund, fund);
    }
    else
    {
        DNF_LOG_SCOPE_LINE(0xb06,"./log/GuildFund",
            "CPacketTranslater::AddGuildFund() Error!! : GUILD_ID (%u), GUILD_FUND (%u), GOLD (%u), STATE (%d), CONNECTING_GUILD_MEMBER_CNT (%d)\n",
            GetGuildKey(), m_dbInfo.m_info.m_guildFund, fund,
            m_guildDBFlag, m_members.size());
    }
}

void CGuild::SubGuildFund(unsigned int fund)
{
    if ((m_guildDBFlag & 4) != 0)
    {
        m_dBSaveFlag = 1;
        unsigned int* fundPtr = &m_dbInfo.m_info.m_guildFund;
        if (fund >= *fundPtr)
        {
            *fundPtr = 0;
        }
        else
        {
            *fundPtr -= fund;
        }
        goto L_success;
    }
    else
    {
        DNF_LOG_SCOPE_LINE(0xb24,"./log/GuildFund",
            "CPacketTranslater::SubGuildFund() Error!! : GUILD_ID (%u), GUILD_FUND (%u), GOLD (%u), STATE (%d), CONNECTING_GUILD_MEMBER_CNT (%d)\n",
            GetGuildKey(), m_dbInfo.m_info.m_guildFund, fund,
            m_guildDBFlag, m_members.size());
        goto L_end;
    }
L_success:
    DNF_LOG_SCOPE_LINE(0xb28,"./log/GuildFund",
        "CPacketTranslater::SubGuildFund() : GUILD_ID (%u), GUILD_FUND (%u), GOLD (%u)\n",
        GetGuildKey(), m_dbInfo.m_info.m_guildFund, fund);
L_end:
    ;
}

unsigned int CGuild::IsAddableGuildFund(unsigned int fund)
{
    if ((m_guildDBFlag & 4) != 0)
    {
        unsigned int* p = &m_dbInfo.m_info.m_guildFund;
        if (m_dbInfo.m_info.m_guildLevel > 0xf)
        {
            if (*p + fund > 0x989680)
            {
                return 0x5f;
            }
        }
        else
        {
            if (*p + fund > 0x1312d00)
            {
                return 0x5f;
            }
        }
        return 0;
    }
    return 0x5f;
}

void CGuild::NotifyAllTodayGuildMember()
{
    CUser* user = 0;
    Packet_Notify_Today_Guild_Member pkt;
    pkt.m_guildKey = m_guildKey;
    pkt.m_member0 = m_board.m_today.m_charNo;
    pkt.m_member1 = m_board.m_today.m_name0;
    pkt.m_member2 = m_board.m_today.m_name1;
    pkt.m_member3 = m_board.m_today.m_name2;
    pkt.m_member4 = m_board.m_today.m_name3;
    pkt.m_member5 = m_board.m_today.m_name4;
    pkt.m_member6 = m_board.m_today.m_name5;
    pkt.m_member7 = m_board.m_today.m_name6;
    pkt.m_member8 = m_board.m_today.m_name7;
    pkt.m_memberA = m_board.m_today.m_field24;
    pkt.m_memberC = (char)m_board.m_today.m_field26;
    for (std::map<unsigned int, CUser*>::iterator it = m_members.begin();
         it != m_members.end(); ++it)
    {
        user = it->second;
        if (user == NULL)
        {
            continue;
        }
        pkt.m_channel = user->GetIdByChannel();
        pkt.m_charNo = user->GetUniqCharNo();
        user->SendToGameserver((char*)&pkt, 0x3d);
    }
}

void CGuild::NotifyTodayGuildMember(CUser* user)
{
    Packet_Notify_Today_Guild_Member pkt;
    pkt.m_guildKey = m_guildKey;
    pkt.m_member0 = m_board.m_today.m_charNo;
    pkt.m_member1 = m_board.m_today.m_name0;
    pkt.m_member2 = m_board.m_today.m_name1;
    pkt.m_member3 = m_board.m_today.m_name2;
    pkt.m_member4 = m_board.m_today.m_name3;
    pkt.m_member5 = m_board.m_today.m_name4;
    pkt.m_member6 = m_board.m_today.m_name5;
    pkt.m_member7 = m_board.m_today.m_name6;
    pkt.m_member8 = m_board.m_today.m_name7;
    pkt.m_memberA = m_board.m_today.m_field24;
    pkt.m_memberC = (char)m_board.m_today.m_field26;
    pkt.m_channel = user->GetIdByChannel();
    pkt.m_charNo = user->GetUniqCharNo();
    user->SendToGameserver((char*)&pkt, 0x3d);
}

void CGuild::QueryTodayGuildMember(CServerHandler* handler)
{
    Packet_Query_Today_Guild_Member pkt;
    pkt.m_guildKey = m_guildKey;
    handler->SendToDB(&pkt);
}

void CGuild::NotifyAllAchieveAttendance(unsigned int charNo, unsigned int phase)
{
    CUser* user = 0;
    Packet_Achieve_Guild_Attendance pkt;
    pkt.m_charNo = charNo;
    pkt.m_phase = phase;
    for (std::map<unsigned int, CUser*>::iterator it = m_members.begin();
         it != m_members.end(); ++it)
    {
        user = it->second;
        if (user)
        {
            pkt.m_channel = user->GetIdByChannel();
            pkt.m_charNo2 = user->GetUniqCharNo();
            user->SendToGameserver((char*)&pkt, 0x1a);
        }
    }
}

STGuildSkill::STGuildSkill()
{
    m0 = -1;
    m4 = 0xff;
}

STGuildDBInfoOnly::STGuildDBInfoOnly()
    : m_masterId(0), m_guildLevel(0), m_field1e(0), m_totalCnt(0),
      m_guildPoint(0), m_guildRank(0), m_guildExp(0), m_subMasterCnt(0),
      m_guildSkillPoint(0), m_skillLearnCnt(0)
{
    m_powerSide = 0;
    m_powerSecedeTime = 0;
    m_powerWarPoint = 0;
    m_agitFlag = 0;
    m_powerJoinCount = 0;
    m_guildFund = 0;
    m_fieldB9 = 0;
    memset(m_subGuildMaster, 0, 0x14);
    memset(m_guildName, 0, 0x17);
    m_flags = (unsigned char)(m_flags | 1);
    m_flags = (unsigned char)(m_flags & 0xfd);
    memset((char*)m_skills, 0, 0x50);
    memset(m_masterName, 0, 0x15);
}

STGuildDBInfo::STGuildDBInfo()
{
    // 成员 m_info / m_members[] 由编译器自动构造（STGuildDBInfoOnly ctor +
    // 300× STGuildMemberProxy ctor），与 ORIG 一致；不要再 memset 覆盖。
}

STGuildAgitDBInfo::STGuildAgitDBInfo()
{
    m_agitLevel = 0;
}

#pragma pack(push,1)
struct Packet_Monitor_Notice_Guild_Dismiss_ToUser_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned int me;
};
#pragma pack(pop)
Packet_Monitor_Notice_Guild_Dismiss_ToUser::Packet_Monitor_Notice_Guild_Dismiss_ToUser()
    : PacketHeader(0x3fc, 0x12)
{
    m_channel = 4294967295;
    m_charNo = 0;
}

Packet_Monitor_Notice_Guild_Enter_ToUser::Packet_Monitor_Notice_Guild_Enter_ToUser()
    : PacketHeader(0x3fd, 0x5a), m_a(0xffffffff), m_b(0)
{
}

Packet_Monitor_Notice_Guild_Secede_ToUser::Packet_Monitor_Notice_Guild_Secede_ToUser()
    : PacketHeader(0x3fe, 0x55), m_channel(0xffffffff), m_charNo(0)
{
}

#pragma pack(push,1)
struct Packet_Monitor_Notice_Guild_Mark_Change_ToUser_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned int me;
};
#pragma pack(pop)
Packet_Monitor_Notice_Guild_Mark_Change_ToUser::
    Packet_Monitor_Notice_Guild_Mark_Change_ToUser()
    : PacketHeader(0x3ff, 0x16)
{
    m_channel = 4294967295;
    m_charNo = 0;
}

#pragma pack(push,1)
struct Packet_Monitor_Guild_Chat_ToUser_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned int me;
    char pad0x12[0x1e];
    unsigned char m30;
};
#pragma pack(pop)
Packet_Monitor_Guild_Chat_ToUser::Packet_Monitor_Guild_Chat_ToUser()
    : PacketHeader(0x400, 0x131)
{
    m_channel = 4294967295;
    m_charNo = 0;
    m_len = 0;
    memset(m_name, 0, 0x1e);
    memset(m_msg, 0, 0x100);
}

#pragma pack(push,1)
struct Packet_DB_Call_Guild_All_Members_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned int me;
};
#pragma pack(pop)
Packet_DB_Call_Guild_All_Members::Packet_DB_Call_Guild_All_Members()
    : PacketHeader(0x425, 0x12)
{
    m_guildKey = 0;
    m_charNo = 0;
}

#pragma pack(push,1)
struct Packet_DB_Call_Unconn_Guild_Member_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned int me;
};
#pragma pack(pop)
Packet_DB_Call_Unconn_Guild_Member::Packet_DB_Call_Unconn_Guild_Member()
    : PacketHeader(0x427, 0x12)
{
    m_guildKey = 0;
    m_charNo = 0;
}

#pragma pack(push,1)
struct Packet_Monitor_Notice_Guild_Member_Login_out_Layout
{
    char pad0x0[0xa];
    unsigned char ma;
    unsigned int mb;
    unsigned int mf;
    unsigned char m13;
};
#pragma pack(pop)
Packet_Monitor_Notice_Guild_Member_Login_out::Packet_Monitor_Notice_Guild_Member_Login_out()
    : PacketHeader(0x407, 0x32)
{
    m_flag = 0;
    m_channel = 4294967295;
    m_charNo = 0;
    m_channelNo = 255;
    memset(m_name, 0, 0x1e);
}

Packet_Monitor_SAVE_Guild::Packet_Monitor_SAVE_Guild()
    : PacketHeader(0x409, 0xd0), m_flag(0xff), m_guildKey(0)
{
}

Packet_DBMW_Save_Power_Secede_Time::Packet_DBMW_Save_Power_Secede_Time()
    : PacketHeader(0x448, 0xf)
{
    (void)0;
}

Packet_Monitor_Notice_Guild_Info::Packet_Monitor_Notice_Guild_Info()
    : PacketHeader(0x40a, 0x139), m_guildKey(0)
{
    memset(m_rest, 0, sizeof(m_rest));
}

#pragma pack(push,1)
struct Packet_Guild_Notify_Guild_Member_Memo_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned int me;
};
#pragma pack(pop)
Packet_Guild_Notify_Guild_Member_Memo::Packet_Guild_Notify_Guild_Member_Memo()
    : PacketHeader(0x4d2, 0x45)
{
    m_channel = 0;
    m_charNo = 0;
    memset(m_memo, 0, 0x15);
    memset(m_name, 0, 0x1e);
}

Packet_Guild_Notify_Message_To_Guild_Mem::Packet_Guild_Notify_Message_To_Guild_Mem()
    : PacketHeader(0x42d, 0x77)
{
    memset(m_msg, 0, 0x65);
}

#pragma pack(push,1)
struct Packet_Channel_Create_Guild_Agit_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned int me;
    unsigned int m12;
};
#pragma pack(pop)
Packet_Channel_Create_Guild_Agit::Packet_Channel_Create_Guild_Agit()
    : PacketHeader(0x6dc, 0x16)
{
    m_channel = 4294967295;
    m_charNo2 = 0;
    m_charNo = 0;
}

#pragma pack(push,1)
struct Packet_DB_Create_Guild_Agit_Layout
{
    char hdr[0xa];
    unsigned int m0xa;
    unsigned int m0xe;
};
#pragma pack(pop)

Packet_DB_Create_Guild_Agit::Packet_DB_Create_Guild_Agit()
    : PacketHeader(0x6dd, 0x12)
{
    m_guildKey = 0;
    m_charNo = 0;
}

#pragma pack(push,1)
struct Packet_DB_Delete_Guild_Agit_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned int me;
};
#pragma pack(pop)
Packet_DB_Delete_Guild_Agit::Packet_DB_Delete_Guild_Agit()
    : PacketHeader(0x6df, 0x12)
{
    m_guildKey = 0;
    m_charNo = 0;
}

#pragma pack(push,1)
struct Packet_DB_Load_Guild_Agit_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
};
#pragma pack(pop)
Packet_DB_Load_Guild_Agit::Packet_DB_Load_Guild_Agit()
    : PacketHeader(0x6e1, 0xe)
{
    m_charNo = 0;
}

Packet_Channel_Guild_Agit_Info::Packet_Channel_Guild_Agit_Info()
    : PacketHeader(0x6e2, 0x17), m_channel(0), m_charNo(0), m_guildKey(0)
{
    memset(&m_info, 0, 1);
}

#pragma pack(push,1)
struct Packet_DB_Upgrade_Guild_Agit_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned int me;
};
#pragma pack(pop)
Packet_DB_Upgrade_Guild_Agit::Packet_DB_Upgrade_Guild_Agit()
    : PacketHeader(0x6e4, 0x12)
{
    m_guildKey = 0;
    m_charNo = 0;
}

Packet_Guild_Notice_Guild_Master_Delegate::Packet_Guild_Notice_Guild_Master_Delegate()
    : PacketHeader(0x43d, 0x30)
{
    memset(m_name, 0, 0x1e);
}

Packet_Guild_Notice_Guild_Name_Change::Packet_Guild_Notice_Guild_Name_Change()
    : PacketHeader(0x446, 0x2d)
{
    memset(m_name, 0, 0x17);
}

#pragma pack(push,1)
struct Packet_Query_Today_Guild_Member_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
};
#pragma pack(pop)
Packet_Query_Today_Guild_Member::Packet_Query_Today_Guild_Member()
    : PacketHeader(0x1bbf, 0xe)
{
    m_guildKey = 0;
}

#pragma pack(push,1)
struct Packet_Notify_Today_Guild_Member_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
};
#pragma pack(pop)
Packet_Notify_Today_Guild_Member::Packet_Notify_Today_Guild_Member()
    : PacketHeader(0x1bc1, 0x3d)
{
    m_guildKey = 0;
    memset(&m_member0, 0, 0x27);
}

#pragma pack(push,1)
struct Packet_Achieve_Guild_Attendance_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned int me;
};
#pragma pack(pop)
Packet_Achieve_Guild_Attendance::Packet_Achieve_Guild_Attendance()
    : PacketHeader(0x1bc8, 0x1a)
{
    m_charNo2 = 0;
    m_channel = 0;
}

#pragma pack(push,1)
struct Packet_Monitor_Guild_Chat_ToUser_Hyper_Link_Layout
{
    char pad0x0[0xa];
    unsigned int ma;
    unsigned int me;
    char pad0x12[0x1e];
    unsigned char m30;
    char pad0x31[0x138];
    unsigned char m169;
};
#pragma pack(pop)
Packet_Monitor_Guild_Chat_ToUser_Hyper_Link::Packet_Monitor_Guild_Chat_ToUser_Hyper_Link()
    : PacketHeader(0x2718, 0x26a)
{
    m_channel = 4294967295;
    m_charNo = 0;
    m_type = 0;
    m_len = 0;
    memset(m_name, 0, 0x1e);
    memset(m_msg, 0, 0x100);
    memset(m_items, 0, 0x138);
}

unsigned short CGuild::GetGuildDBFlag()
{
    return m_guildDBFlag;
}

unsigned char UpgradeSeparateInfo::GetUpgradeSeparate() const
{
    return (unsigned char)(*(unsigned char*)m_data & 0x1f);
}
