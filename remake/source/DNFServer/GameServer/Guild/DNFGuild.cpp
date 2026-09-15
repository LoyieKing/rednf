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

// 成员名按权威定义（Game/Inven_Item.h）改写；偏移与调用序与 ORIG
// _ZN12RandomOption5resetEv 0x08089620 逐条一致（+0/+3/+6/+9/+0xa/+0xd）。
void RandomOption::reset()
{
    m_fields[0].reset();   // +0x00（原 m_f0）
    m_fields[1].reset();   // +0x03（原 m_f3）
    m_fields[2].reset();   // +0x06（原 m_f6）
    m_seed9.reset();       // +0x09（原 m_s9）
    m_fielda.reset();      // +0x0a（原 m_f10）
    m_seedd.reset();       // +0x0d（原 m_s13）
}

void RandomOptionField::reset()
{
    m_field0 = 0;   // 原 m_data[0]
    m_field1 = 0;   // 原 m_data[1]
    m_field2 = 0;   // 原 m_data[2]
}

void RandomOptionSeed::reset()
{
    m_field = 0;    // 原 m_data[0]
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
    reinterpret_cast<Bits*>(&m_field)->a = 0;   // 原 m_data（权威定义中为 union 别名 m_field）
    reinterpret_cast<Bits*>(&m_field)->b = 0;
    reinterpret_cast<Bits*>(&m_field)->c = 0;
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

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-NEAR-0013 已消除）
void CGuild::SetTodayGuildMember(STTodayGuildMember& member)
{
    // 试形记录（2026-09-15）：两侧显式同型 reinterpret 视图无法翻正
    // `mov 0x8(%ebp),%eax` / `mov 0xc(%ebp),%edx` 装载序（4.4 恒先载 RHS 地址，
    // 与 N76 镜像）⇒ 回退原形，按 N76 镜像族封顶。
    // [P235 双视角] 同型赋值（STTodayGuildMember::m_today 与 m_board.m_today 同类型）
    this->m_board.m_today = member.m_today;
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

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0113 已消除）
bool CGuild::InsertGuildMember(unsigned int charNo, CUser* user)
{
    // [P222] ORIG：user==0 直跳尾 return 0 块（否定早返改正向包裹）
    if (user != 0)
    {
        user->AttachGuild(this);
        std::pair<std::map<unsigned int, CUser*>::iterator, bool> r =
            m_members.insert(std::make_pair(charNo, user));
        if (r.second != 0)
        {
            return 1;
        }
        {
            DNF_LOG_SCOPE_LINE(0x7b,"./log/GuildMember",
                "[INSERT_ERR]\tAlready Exist : Guild Key : %d\tChar Key : %d,\tChar Name : %s\tLogin Mem Cnt : %d\n",
                GetGuildKey(), charNo, user->GetCharName(), m_members.size());
        }
        // [P214] ORIG：日志块后独立 return 0（生成 mov $0; jmp 尾块）
        return 0;
    }
    return 0;
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

// [DNF-NONIDENTICAL] DNF-GLD-DIFF-0146 | guild | 与ORIG差异=DIFF | CGuild::LoadGuild | 详见 function_reports/guild/_ZN6CGuild9LoadGuildER17STGuildDBInfoOnlyPc.md
// [已试 2026-09-15 StaticsRest2] ORIG 尾部有 `jmp eb 01` + `nop`（if 的 join 残留），
//   试过 ① if 后加 `return;`（→ 我方多 jmp+nop，113 条）② 空 `else {}`（无变化）
//   ③ 额外复合块 + return（仍多一条 nop）④ D 形态三层配对（循环体内裸块 + 函数尾 `return;`，113 条）
//   ⇒ 四种位置/形态均无效；Main 已将本件列入「尾 jmp 不来自显式 return」反例清单，登记死胡同。
// [2026-09-15 W3-GuildRest 实测] 残差 = ORIG 在 `m_guildDBFlag |= 4;` 之后多一条**跳至紧邻下一条**
//   的 `jmp`，且 ORIG 两条守卫跳转目标差 1 字节（`jne #0x16a` / `je #0x16b`，我方同目标）
//   ⇒ 复合条件 `&&` 的两个假出口在 ORIG 属**两个不同基本块** ⇒ 块尾标签/尾跳族。
//   已试并回退：块尾补 `return;`（void 尾语句被 GCC 消除，112/111 与 ndiff 3 不变）✗
//   ⇒ 与 §5.19-D「块尾标签次序族」（monitor NoticeMemberLogin_Out）同族，源码层无靶点。
void CGuild::LoadGuild(STGuildDBInfoOnly& info, char* name)
{
    if ((m_guildDBFlag & 4) == 0 && (m_guildDBFlag & 2) != 0)
    {
        int local18 = m_dbInfo.m_info.m_guildPoint;
        char local11 = 0;
        int local10 = 0;
        while (local10 < (int)info.m_skillLearnCnt)
        {
            if (199 < info.m_skills[local10].m0 &&
                info.m_skills[local10].m0 < 0xd1)
            {
            }
            else
            {
                break;
            }
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

// [DNF-NONIDENTICAL] DNF-GLD-DIFF-0117 | guild | 与ORIG差异=DIFF | CGuild::DBGuildSaveProcess | 详见 function_reports/guild/_ZN6CGuild18DBGuildSaveProcessEP14CServerHandler.md
// [已试 2026-09-15 StaticsRest2] 三个 CMyFileLog 临时槽：ORIG `-0x1c/-0x24/-0x28`，我方 `-0x1c/-0x20/-0x28`
//   ⇒ log1(0x171) 与 log2(0x17c) 的槽互换；帧相同、槽集合各有一项独占 ⇒「槽互换族」，
//   占位法对该族必劣化（判据表已证），未找到可改的声明位置（log 是宏/构造临时，无声明）⇒ 登记死胡同。
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
    if (++m_saveIntervalCnt > 1)
    {
        if (m_members.empty())
        {
            // [P227/P329] ORIG：变参在 ctor 前求值并跨 ctor 保活
            CMyFileLog(__FUNCTION__, 0x171)("./log/Except",
                "[SAVE_INTERVAL]  Guild Key : %d\tGuild Name : %s\t\n",
                m_guildKey, GetGuildName());
            m_saveIntervalCnt = 0;
        }
        else
        {
            CUser* user = m_members.begin()->second;
            if (user == 0)
            {
                CMyFileLog(__FUNCTION__, 0x17c)("./log/Except",
                    "[SAVE_INTERVAL]  pclUser is NULL!");
                m_saveIntervalCnt = 0;
                return;
            }
            if (user->GetGameServer() != 0)
            {
                CMyFileLog(__FUNCTION__, 0x183)("./log/Guild",
                    "GUILD EXP   Guild Key : %d, Guild Exp : %d",
                    m_guildKey, GetGuildExp());
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

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0118 已消除）
void CGuild::DBSaveGuildMembers(unsigned char flag, CServerHandler* handler, unsigned char param)
{
    // [P-nested-guard] 双早返 → 单尾岛復现
    if ((m_guildDBFlag & 4) != 0)
    {
        if (m_members.empty())
            return;
        for (std::map<unsigned int, CUser*>::iterator it = m_members.begin();
             it != m_members.end(); ++it)
        {
            CUser* user;
            if ((user = it->second) == 0)
            {
                continue;
            }
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

// [DNF-IDENTICAL（2026-09-15 复核：已 identical ✓）] DNF-GLD-DIFF-0142 | guild | 与ORIG差异=DIFF | CGuild::NoticeGuildMasterDelegateToMembers | 详见 function_reports/guild/_ZN6CGuild34NoticeGuildMasterDelegateToMembersEPc.md
void CGuild::NoticeGuildMasterDelegateToMembers(char* name)
{
    if ((m_guildDBFlag & 4) != 0)
    {
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
    else
    {
        CMyFileLog(__FUNCTION__, 0x2bd)("./log/GuildErr",
            "GUILD_INFO : NoticeGuildMasterDelegateToMembers, Guild Key(%d)", m_guildKey);
    }
}

void CGuild::SendGuildInfoToMembers(bool flag)
{
    Packet_Monitor_Notice_Guild_Info pkt;
    CUser* member;
    pkt.m_guildKey = m_guildKey;
    memcpy(&pkt.m_info, &m_dbInfo.m_info, 0xbd);
    pkt.m_padD3 = (char)flag;
    int n = strlen(m_guildMessage);
    if (n <= 0x64)
    {
        memcpy(pkt.m_rest, m_guildMessage, n);
    }
    else
    {
        memcpy(pkt.m_rest, m_guildMessage, 0x64);
    }
    for (std::map<unsigned int, CUser*>::iterator it = m_members.begin();
         it != m_members.end(); ++it)
    {
        if ((member = it->second) == 0)
        {
            continue;
        }
        pkt.m_channel = member->GetIdByChannel();
        pkt.m_charNo = member->GetUniqCharNo();
        member->SendToGameserver((char*)&pkt, 0x139);
        ReplyGuildMembers(member);
    }
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0140 已消除）
void CGuild::SendGuildNameChangeToMembers()
{
    Packet_Guild_Notice_Guild_Name_Change pkt;
    pkt.m_guildKey = m_guildKey;
    memcpy(pkt.m_name, m_dbInfo.m_info.m_guildName, 0x16);
    for (std::map<unsigned int, CUser*>::iterator it = m_members.begin();
         it != m_members.end(); ++it)
    {
        CUser* u;
        if ((u = it->second) == 0)
        {
            continue;
        }
        pkt.m_channel = u->GetIdByChannel();
        pkt.m_charNo = u->GetUniqCharNo();
        u->SendToGameserver((char*)&pkt, 0x2d);
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

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0127 已消除）
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
            CUser* u;
            if ((u = it->second) == 0)
            {
                continue;
            }
            // [P354] ORIG 两处 `call; xor $1; test; je` ⇒ 源码为 !A && !B 否定式守卫
            if (!IsGuildMaster(u->GetUniqCharNo()) &&
                !IsSubGuildMaster(u->GetUniqCharNo()))
            {
                continue;
            }
            pkt.m_channel = u->GetIdByChannel();
            pkt.m_charNo = u->GetUniqCharNo();
            u->SendToGameserver((char*)&pkt, 0x139);
        }
    }
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0119 已消除）| CGuild::SendToGuildForMail | 详见 function_reports/guild/_ZN6CGuild18SendToGuildForMailEv.md
void CGuild::SendToGuildForMail()
{
    if ((m_guildDBFlag & 4) != 0)
    {
        Packet_Monitor_Notify_New_Mail pkt;
        for (std::map<unsigned int, CUser*>::iterator it = m_members.begin();
             it != m_members.end(); ++it)
        {
            CUser* m;
            if ((m = it->second) == 0)
            {
                continue;
            }
            pkt.me = m->GetIdByChannel();
            pkt.ma = m->GetUniqCharNo();
            m->SendToGameserver((char*)&pkt, 0x12);
        }
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

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0110 已消除）
bool CGuild::BuyGuildSkill(int skillId, int slot, short param, unsigned int charNo)
{
    if ((m_guildDBFlag & 4) != 0)
    {
    if ((int)m_dbInfo.m_info.m_guildSkillPoint < (int)(short)param)
    {
        // [P227/P329] ORIG：变参在 ctor 前求值并跨 ctor 保活（全表达式临时链）
        CMyFileLog(__FUNCTION__, 0x3d2)("./log/GuildSkill",
            "BUY_SKILL_1, GKey(%d) , Idx(%d), lev(%d), gsp(%d)",
            GetGuildKey(), skillId, slot, (unsigned int)m_dbInfo.m_info.m_guildSkillPoint);
        return 0;
    }
    if (m_dbInfo.m_info.m_guildFund < charNo)
    {
        CMyFileLog(__FUNCTION__, 0x3d9)("./log/GuildSkill",
            "BUY_SKILL_1, GKey(%d) , Idx(%d), lev(%d), guildfund(%d)",
            GetGuildKey(), skillId, slot, (unsigned int)m_dbInfo.m_info.m_guildFund);
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
            CMyFileLog(__FUNCTION__, 0x3ee)("./log/GuildSkill",
                "BUY_SKILL_ERR, GKey(%d) , Learn Cnt(%d)", GetGuildKey(),
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
    // [P297 反条件 if/else] ORIG：jl 直跳「置 0」尾块，减法路径直落
    if (m_dbInfo.m_info.m_guildSkillPoint >= param)
    {
        m_dbInfo.m_info.m_guildSkillPoint -= (unsigned short)param;
    }
    else
    {
        m_dbInfo.m_info.m_guildSkillPoint = 0;
    }
    m_dBSaveFlag = 1;
    CMyFileLog(__FUNCTION__, 0x403)("./log/GuildSkill",
        "BUY_SKILL, GKey(%d) , Idx(%d), lev(%d), gsp(%d)", GetGuildKey(),
        skillId, slot, (unsigned int)m_dbInfo.m_info.m_guildSkillPoint);
    return 1;
    }
    return 0;
}

// [DNF-NONIDENTICAL] DNF-GLD-DIFF-0109 | guild | 与ORIG差异=DIFF | CGuild::GuildLevelUp | 详见 function_reports/guild/_ZN6CGuild12GuildLevelUpEP14CServerHandlerP5CUser.md
// [已试 2026-09-15 StaticsRest2] 两处根因：①空 then+else 的分支方向（ORIG `je` / 我方 `jne`+`mov $2`）
//   ②`m_flags |= 2` 我方多一条中转 mov 且重载 this（ORIG 单次载入）。试过 `m_flags = m_flags | 2`
//   （显式展开）⇒ 逐字节不变；类型已是 unsigned char（R6 无靶点）⇒ 登记「分支方向 + this 保活」族。
int CGuild::GuildLevelUp(CServerHandler* handler, CUser* user)
{
    if (user == 0)
    {
        return 2;
    }
    if ((m_guildDBFlag & 4) != 0)
    {
    // [P354 已试] 空 then+else（本形 nd=25/177 条）优于 De Morgan 正极性守卫
    //   （`!=`&&`==0`：nd=25 但 178 条且多 `xor $1`；`!f()`：多 `xor $1`）—— 保持原形
    // [2026-09-15 StaticsRest2] ORIG 该处为「整体 `||` 值物化」：
    //   `call GetUniqCharNo; call GetMasterId; cmp; je .1; call GetUniqCharNo; call IsSubGuildMaster;
    //    test %al,%al; je .0; .1: mov $0x1,%eax; jmp .t; .0: mov $0x0,%eax; .t: test %al,%al; je else`
    //   我方无物化（`jne <T>; mov $0x2,%eax; jmp <T>`）⇒ 属「条件物化形态族」。
    //   已试并回退：①空 then/De Morgan（P354，无改善）② `!f()` 单侧取反（多 `xor $1`）
    //   ③ 交换 `||` 操作数次序（ndiff 6→**25** 劣化）。⇒ 登记死胡同。
    // [2026-09-15 W3-GuildRest 机理 D 变体（**保留，未 identical**）] ORIG 假出口 **直落函数尾**的 `return 2`
    //   （`je <尾部 mov $0x2,%eax>`，全函数只物化一份 2）⇒ then 之内即整个后续 body，
    //   `return 2` 只在函数尾出现一次。
    if (user->GetUniqCharNo() == GetMasterId() ||
        IsSubGuildMaster(user->GetUniqCharNo()))
    {
    if (m_dbInfo.m_info.m_guildLevel + 1 > 0x10)
    {
        // [P329] ORIG：nextLev 不具名（`lea 0x1(%eax),%ebx` 寄存器保活，无 -0x14 槽）
        CMyFileLog(__FUNCTION__, 0x41d)("./log/GuildModify",
            "CGuild::GuildLevelUp Err (%d)", m_dbInfo.m_info.m_guildLevel + 1);
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
        CMyFileLog(__FUNCTION__, 0x445)("./log/Except",
            "CGuild::GuildLevelUp : pclUser->GetGameServer() == 0");
    }
    SendGuildInfoToMembers(false);
    CMyFileLog(__FUNCTION__, 0x44a)("./log/GuildModify",
        "CGuild::GuildLevelUp(%d : %d)GSP(%d)", GetGuildKey(),
        m_dbInfo.m_info.m_guildLevel, m_dbInfo.m_info.m_guildSkillPoint);
    return 0;
    }
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

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0129 已消除）
void CGuild::NoticeEnterToGuildMember(char* info)
{
    // [P-nested-guard] 双早返 → 单尾岛復现
    if (!((m_guildDBFlag & 4) == 0 && (m_guildDBFlag & 2) == 0))
    {
        if (m_members.empty())
        {
            return;
        }
        Packet_Monitor_Notice_Guild_Enter_ToUser pkt;
        memcpy(&pkt.m_info, info, 0x46);
        for (std::map<unsigned int, CUser*>::iterator it = m_members.begin();
             // [2026-09-15 实测] 该件循环增量切后置 `it++` 令 ndiff 1→33（劣化）⇒ 保持 `++it`；
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
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0131 已消除）
void CGuild::NoticeSecedeToGuildMember(char* info)
{
    // [P-nested-guard] 双早返 → 单尾岛復现
    if ((m_guildDBFlag & 4) != 0)
    {
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
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0141 已消除）| CGuild::NoticeMarkChangeToGuildMember | 详见 function_reports/guild/_ZN6CGuild29NoticeMarkChangeToGuildMemberEj.md
void CGuild::NoticeMarkChangeToGuildMember(unsigned int charNo)
{
    if ((m_guildDBFlag & 4) != 0)
    {
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
            pkt.m_channel = member->GetIdByChannel();
            pkt.m_charNo = member->GetUniqCharNo();
            pkt.m_charNo2 = charNo;
            DNF_LOG_SCOPE_AT(__FUNCTION__, 0x4bf, "./log/Web",
                "[GUILD MARK CHANGE] Send to game server. (channel:%d, character:%u, guildkey:%d)\n",
                pkt.m_channel, pkt.m_charNo, pkt.m_charNo2);
            member->SendToGameserver((char*)&pkt, 0x16);
        }
    }
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0139 已消除）
void CGuild::NoticeChatMsgToGuildMembers(unsigned int charNo, char* msg, int len,
                                         const char* name)
{
    if (len >= 0x100)
    {
        return;
    }
    // [P-nested-guard] 后两个守卫内联（4.4 按早返数发岛；ORIG 只两岛）
    if ((m_guildDBFlag & 4) != 0)
    {
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
            u->SendToGameserver((char*)&pkt, pkt.packetSize);
        }
    }
}

// [DNF-NONIDENTICAL] DNF-GLD-DIFF-0145 | guild | 与ORIG差异=DIFF | CGuild::NoticeChatMsgToGuildMembersHyperLink | 详见 function_reports/guild/_ZN6CGuild36NoticeChatMsgToGuildMembersHyperLinkEjPcihPK19hyperlink_item_infoPKc.md
void CGuild::NoticeChatMsgToGuildMembersHyperLink(unsigned int charNo, char* msg, int len,
                                                  unsigned char type,
                                                  const hyperlink_item_info* link,
                                                  const char* name)
{
    if (len >= 0x100)
    {
        return;
    }
    // [P-nested-guard] 后两个守卫内联（4.4 按早返数发岛；ORIG 只两岛）
    if ((m_guildDBFlag & 4) != 0)
    {
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
                // m_items@+0x31 = m_type(+0x30)+1：ORIG 以 add $0x30 / add $1 两步取址
                // （试过改写为 `pkt.m_items + i * 0x68`：ndiff 5→10，已回退）
                memcpy((char*)&pkt.m_type + 1 + i * 0x68,
                       (char*)link + i * 0x68, 0x68);
            }
            pkt.m_len = (char)len;
            memcpy(pkt.m_msg, msg, (size_t)len);
            pkt.packetSize = (unsigned short)(len + 0x16a);
            user->SendToGameserver((char*)&pkt, pkt.packetSize);
        }
    }
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0138 已消除）
void CGuild::DismissGuildMemberAndNotice(int group)
{
    // [P-nested-guard] 双早返 → 单尾岛復现
    if ((m_guildDBFlag & 4) != 0)
    {
        if (m_members.empty())
            return;
        Packet_Monitor_Notice_Guild_Dismiss_ToUser notice;
        for (std::map<unsigned int, CUser*>::iterator it = m_members.begin();
             it != m_members.end(); ++it)
        {
            CUser* user;
            if ((user = it->second) == 0)
            {
                continue;
            }
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

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0133 已消除）| CGuild::NoticeGuildMemberLogin_Out | 详见 function_reports/guild/_ZN6CGuild26NoticeGuildMemberLogin_OutEP5CUserc.md
void CGuild::NoticeGuildMemberLogin_Out(CUser* user, char flag)
{
    // [P360] 第一组保持 `||`（ORIG 此处为 bool 物化 `jne;mov $1;jmp;mov $0`）
    if (user == 0 || user->GetGameServer() == 0)
    {
        return;
    }
    if ((m_guildDBFlag & 4) != 0)
    {
        if (m_members.empty())
        {
            return;
        }
        {
        Packet_Monitor_Notice_Guild_Member_Login_out pkt;
        for (std::map<unsigned int, CUser*>::iterator it = m_members.begin();
             it != m_members.end(); ++it)
        {
            CUser* member;
            if ((member = it->second) == 0)
            {
                continue;
            }
            if (member == user)
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
    }
}

// [2026-09-15 W3-GuildRest 机理 H 实测] 三处残差同为地址编码族：
//   ORIG `mov %edx,%eax; shl $2; add %edx,%eax; mov %eax,%ebx; add 0xc(%ebp),%ebx`（add 形）
//   我方 `mov 0xc(%ebp),%ecx; …; lea (%ecx,%eax,1),%ebx`（lea 索引形）。
//   已试并回退：① `(Rec*)((unsigned int)info + count*5)` 整型加法 ⇒ 102→106、ndiff 6→13 ✗
//   ② `info->m_members[count].m_channel` 下标形 ⇒ 104、ndiff 31 ✗。
//   ⇒ 与 SetSubGuildMaster 尾部同属「add/lea alternative 选择族」，源码层无靶点。
// [DNF-NONIDENTICAL] DNF-GLD-DIFF-0123 | guild | 与ORIG差异=DIFF | CGuild::ReplyGuildMembersToWeb | 详见 function_reports/guild/_ZN6CGuild22ReplyGuildMembersToWebEP24STGuildMemberWebConnInfo.md
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
                // [P348/P183] ORIG 无 rec 具名局部（指针在 %ebx 保活，地址重物化）
                (info->m_members + count)->m_charNo = it->second->GetUniqCharNo();
                if (it->second->GetGameServer() != 0)
                {
                    (info->m_members + count)->m_channel = it->second->GetGameServer()->GetChannelNo();
                }
                count++;
            }
        }
    }
    return count;
}

// [DNF-NONIDENTICAL] DNF-GLD-DIFF-0114 | guild | 与ORIG差异=DIFF | CGuild::ReplyGuildMembers | 详见 function_reports/guild/_ZN6CGuild17ReplyGuildMembersEP5CUser.md
void CGuild::ReplyGuildMembers(CUser* user)
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
        Packet_Monitor_Call_Guild_Members_ToChannel pkt;
        pkt.m_guildKey = m_guildKey;
        memcpy(pkt.m_guildName, m_dbInfo.m_info.m_guildName, 0x16);
        pkt.m_guildPoint = m_dbInfo.m_info.m_guildPoint;
        pkt.m_totalCnt = m_dbInfo.m_info.m_totalCnt;
        int count = 0;
        std::map<unsigned int, CUser*>::iterator it;
        it = m_members.begin();
        while (it != m_members.end())
        {
            pkt.m_members[count].f.m_job = it->second->GetJob();
            pkt.m_members[count].f.m_growthType = it->second->GetGrowthType();
            pkt.m_members[count].f.m_level = it->second->GetLevel();
            memcpy(pkt.m_members[count].f.m_name, it->second->GetCharName(), 0x1d);
            pkt.m_members[count].f.m_sex = it->second->GetSex();
            if (it->second->GetGameServer() != 0)
            {
                pkt.m_members[count].f.m_channel =
                    it->second->GetGameServer()->GetChannelNo();
            }
            memcpy(pkt.m_members[count].f.m_guildMemDBInfo,
                   (char*)it->second->GetGuildMemDBInfo(), 0x14);
            pkt.m_members[count].f.m_grade =
                (char)it->second->GetGuildMemDBInfo()->m_grade;
            if (it->second->IsBlackUser(user->GetUniqCharNo()))
            {
                pkt.m_members[count].f.m_blackUser = 1;
            }
            count++;
            if (0x5f < count)
            {
                break;
            }
            ++it;
        }
        pkt.m_count = (unsigned char)count;
        pkt.packetSize = (unsigned short)(pkt.m_count * 0x3f + 0x34);
        pkt.m_channel = user->GetIdByChannel();
        pkt.m_charNo = user->GetUniqCharNo();
        user->SendToGameserver((char*)&pkt, pkt.packetSize);
        if (it != m_members.end())
        {
            Packet_Monitor_Call_Guild_Members_ToChannel_Next pkt2;
            pkt2.m_guildKey = m_guildKey;
            count = 0;
            std::map<unsigned int, CUser*>::iterator it2;
            it2 = it;
            while (it2 != m_members.end())
            {
                pkt2.m_members[count].f.m_job = it2->second->GetJob();
                pkt2.m_members[count].f.m_growthType = it2->second->GetGrowthType();
                pkt2.m_members[count].f.m_level = it2->second->GetLevel();
                memcpy(pkt2.m_members[count].f.m_name, it2->second->GetCharName(), 0x1d);
                pkt2.m_members[count].f.m_sex = it2->second->GetSex();
                if (it2->second->GetGameServer() != 0)
                {
                    pkt2.m_members[count].f.m_channel =
                        it2->second->GetGameServer()->GetChannelNo();
                }
                memcpy(pkt2.m_members[count].f.m_guildMemDBInfo,
                       (char*)it2->second->GetGuildMemDBInfo(), 0x14);
                pkt2.m_members[count].f.m_grade =
                    (char)it2->second->GetGuildMemDBInfo()->m_grade;
                if (it2->second->IsBlackUser(user->GetUniqCharNo()))
                {
                    pkt2.m_members[count].f.m_blackUser = 1;
                }
                count++;
                ++it2;
                if (0x5f < count)
                {
                    pkt2.m_count = (unsigned char)count;
                    pkt2.packetSize = 0x17b7;
                    pkt2.m_channel = user->GetIdByChannel();
                    pkt2.m_charNo = user->GetUniqCharNo();
                    user->SendToGameserver((char*)&pkt2, pkt2.packetSize);
                    count = 0;
                }
            }
            if (count != 0)
            {
                pkt2.m_count = (unsigned char)count;
                pkt2.packetSize = (unsigned short)(pkt2.m_count * 0x3f + 0x17);
                pkt2.m_channel = user->GetIdByChannel();
                pkt2.m_charNo = user->GetUniqCharNo();
                user->SendToGameserver((char*)&pkt2, pkt2.packetSize);
            }
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

// [DNF-NONIDENTICAL] DNF-GLD-DIFF-0120 | guild | 与ORIG差异=DIFF | CGuild::ReplyGuildAllMembers | 详见 function_reports/guild/_ZN6CGuild20ReplyGuildAllMembersEP5CUser.md
void CGuild::ReplyGuildAllMembers(CUser* user)
{
    if (user == 0 || (m_guildDBFlag & 4) == 0 || (m_guildDBFlag & 0x10) == 0)
    {
        return;
    }
    Packet_Monitor_Call_Guild_All_Members_ToChannel pkt;
    pkt.m_guildKey = m_guildKey;
    memcpy(pkt.m_guildName, m_dbInfo.m_info.m_guildName, 0x16);
    pkt.m_guildPoint = m_dbInfo.m_info.m_guildPoint;
    pkt.m_totalCnt = m_totalCnt;
    CUser* m;
    int idx;
    int count = 0;
    if (m_dbInfo.m_info.m_totalCnt != m_totalCnt)
    {
        DNF_LOG_SCOPE_LINE(0x63b,"./log/GuildModify", "Error Guild Member Cnt Not Valid , (%d)/(%d)",
            (unsigned int)m_dbInfo.m_info.m_totalCnt,
            (unsigned int)m_totalCnt);
        m_dbInfo.m_info.m_totalCnt = m_totalCnt;
    }
    for (idx = 0; idx < (int)m_totalCnt; idx++)
    {

        pkt.m_members[count].f.m_job = m_dbInfo.m_members[idx].m_job;
        pkt.m_members[count].f.m_growthType = m_dbInfo.m_members[idx].m_growType;
        pkt.m_members[count].f.m_level = (short)m_dbInfo.m_members[idx].m_lev;
        memcpy(pkt.m_members[count].f.m_name, m_dbInfo.m_members[idx].m_name, 0x1d);
        pkt.m_members[count].f.m_sex = m_dbInfo.m_members[idx].m_sex;
        pkt.m_members[count].f.m_grade = m_dbInfo.m_members[idx].m_grade;
        pkt.m_members[count].f.m_pad3b = m_dbInfo.m_members[idx].m_lastPlayTime;
        if ((m = FindGuildMember(m_dbInfo.m_members[idx].m_no)) != 0)
        {
            pkt.m_members[count].f.m_growthType = m->GetGrowthType();
            pkt.m_members[count].f.m_level = (short)m->GetLevel();
            if (m->GetGameServer() == 0)
            {
                DNF_LOG_SCOPE_LINE(0x658,"./log/Except",
                    "CGuild::ReplyGuildMembers() Guild Key : %d\tGuild Name : %s\tDB Id : %d\tChar Id : %d\n",
                    GetGuildKey(), GetGuildName(), m->GetDBID(), m_dbInfo.m_members[idx].m_no);
            }
            else
            {
                pkt.m_members[count].f.m_channel = m->GetGameServer()->GetChannelNo();
                memcpy(pkt.m_members[count].f.m_guildMemDBInfo, (char*)m->GetGuildMemDBInfo(), 0x14);
                pkt.m_members[count].f.m_grade = m->GetGuildMemDBInfo()->m_grade;
            }
            if (m->IsBlackUser(user->GetUniqCharNo()) != 0)
            {
                pkt.m_members[count].f.m_blackUser = 1;
            }
        }
        else
        {
            memcpy(pkt.m_members[count].f.m_guildMemDBInfo, m_dbInfo.m_members[idx].m_memo, 0x14);
        }
        count++;
        if (0x5f < count)
        {
            break;
        }
    }
    pkt.m_count = (unsigned char)count;
    pkt.packetSize = (unsigned short)(pkt.m_count * 0x3f + 0x34);
    pkt.m_channel = user->GetIdByChannel();
    pkt.m_charNo = user->GetUniqCharNo();
    user->SendTcpGameserver((PacketHeader*)&pkt);
    if (count < (int)m_totalCnt)
    {
        Packet_Monitor_Call_Guild_All_Members_ToChannel_Next pkt2;
        pkt2.m_guildKey = m_guildKey;
        count = 0;
        for (int i = idx + 1; i < (int)m_totalCnt; i++)
        {

            pkt2.m_members[count].f.m_job = m_dbInfo.m_members[i].m_job;
            pkt2.m_members[count].f.m_growthType = m_dbInfo.m_members[i].m_growType;
            pkt2.m_members[count].f.m_level = (short)m_dbInfo.m_members[i].m_lev;
            memcpy(pkt2.m_members[count].f.m_name, m_dbInfo.m_members[i].m_name, 0x1d);
            pkt2.m_members[count].f.m_sex = m_dbInfo.m_members[i].m_sex;
            pkt2.m_members[count].f.m_grade = m_dbInfo.m_members[i].m_grade;
            pkt2.m_members[count].f.m_pad3b = m_dbInfo.m_members[i].m_lastPlayTime;
            if ((m = FindGuildMember(m_dbInfo.m_members[i].m_no)) != 0)
            {
                pkt2.m_members[count].f.m_growthType = m->GetGrowthType();
                pkt2.m_members[count].f.m_level = (short)m->GetLevel();
                if (m->GetGameServer() == 0)
                {
                    DNF_LOG_SCOPE_LINE(0x695,"./log/Except",
                        "CGuild::ReplyGuildMembers() Guild Key : %d\tGuild Name : %s\tDB Id : %d\tChar Id : %d\n",
                        GetGuildKey(), GetGuildName(), m->GetDBID(), m_dbInfo.m_members[i].m_no);
                }
                else
                {
                    pkt2.m_members[count].f.m_channel = m->GetGameServer()->GetChannelNo();
                    memcpy(pkt2.m_members[count].f.m_guildMemDBInfo, (char*)m->GetGuildMemDBInfo(), 0x14);
                    pkt2.m_members[count].f.m_grade = m->GetGuildMemDBInfo()->m_grade;
                }
                if (m->IsBlackUser(user->GetUniqCharNo()) != 0)
                {
                    pkt2.m_members[count].f.m_blackUser = 1;
                }
            }
            else
            {
                pkt2.m_members[count].f.m_job = 0xff;
                memset(pkt2.m_members[count].f.m_guildMemDBInfo, 0, 0x14);
                pkt2.m_members[count].f.m_sex = 0;
            }
            count++;
            if (0x5f < count)
            {
                pkt2.m_count = (unsigned char)count;
                pkt2.packetSize = 0x17b7;
                pkt2.m_channel = user->GetIdByChannel();
                pkt2.m_charNo = user->GetUniqCharNo();
                user->SendTcpGameserver((PacketHeader*)&pkt2);
                count = 0;
            }
        }
        if (count != 0)
        {
            pkt2.m_count = (unsigned char)count;
            pkt2.packetSize = (unsigned short)(pkt2.m_count * 0x3f + 0x17);
            pkt2.m_channel = user->GetIdByChannel();
            pkt2.m_charNo = user->GetUniqCharNo();
            user->SendTcpGameserver((PacketHeader*)&pkt2);
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

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0128 已消除）
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
            memcpy(&m_dbInfo.m_members[m_totalCnt].m_no, proxy, (size_t)param * 0x41);
            m_totalCnt = (unsigned short)(m_totalCnt + param);
        }
    }
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0125 已消除）
bool CGuild::LoadGuildOneMemberProxy(CUser* user)
{
    if ((m_guildDBFlag & 4) != 0 && (m_guildDBFlag & 8) != 0)
    {
        if (m_totalCnt <= 0x12b)
        {
            m_dbInfo.m_members[m_totalCnt].m_no = user->GetUniqCharNo();
            memcpy(m_dbInfo.m_members[m_totalCnt].m_name, user->GetCharName(), 0x1d);
            m_dbInfo.m_members[m_totalCnt].m_job = user->GetJob();
            m_dbInfo.m_members[m_totalCnt].m_growType = user->GetGrowthType();
            m_dbInfo.m_members[m_totalCnt].m_sex = user->GetSex();
            m_dbInfo.m_members[m_totalCnt].m_lev = user->GetLevel();
            m_totalCnt++;
        }
        if (m_totalCnt > 0x12b)
        {
            CMyFileLog(__FUNCTION__, 0x73d)("./log/GuildErr",
                "Guild Member Cnt Full Or Over : G Key(%d), Cnt(%d)", m_guildKey,
                (unsigned int)m_totalCnt);
            if (m_totalCnt > 0x12c)
            {
                m_totalCnt = 0x12c;
            }
        }
        m_dbInfo.m_info.m_totalCnt = m_totalCnt;
        return 1;
    }
    return 0;
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0126 已消除）
bool CGuild::LoadGuildOneMemberProxy(STGuildMemberProxy& proxy)
{
    if ((m_guildDBFlag & 4) != 0 && (m_guildDBFlag & 0x10) != 0)
    {
        if (m_totalCnt <= 0x12b)
        {
            memcpy(&m_dbInfo.m_members[m_totalCnt].m_no, &proxy, 0x41);
            m_totalCnt++;
        }
        if (m_totalCnt > 0x12b)
        {
            CMyFileLog(__FUNCTION__, 0x719)("./log/GuildErr",
                "Guild Member Cnt Full Or Over : G Key(%d), Cnt(%d)", m_guildKey,
                (unsigned int)m_totalCnt);
            if (m_totalCnt > 0x12c)
            {
                m_totalCnt = 0x12c;
            }
        }
        m_dbInfo.m_info.m_totalCnt = m_totalCnt;
        return 1;
    }
    return 0;
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0111 已消除）| CGuild::AddGuildMember | 详见 function_reports/guild/_ZN6CGuild14AddGuildMemberER21ST_Notice_Guild_EnterP5CUser.md
void CGuild::AddGuildMember(ST_Notice_Guild_Enter& info, CUser* user)
{
    if (user == 0)
    {
        return;
    }
    if ((m_guildDBFlag & 4) != 0 && (m_guildDBFlag & 0x10) != 0)
    {
        if (m_members.empty())
        {
            return;
        }
        // [P183反向] ORIG 具名指针局部 p=&m_totalCnt（-0xc），每次解引用
        unsigned short* idx = &m_totalCnt;
        m_dbInfo.m_members[*idx].m_no = info.m_charNo;
        memcpy(m_dbInfo.m_members[*idx].m_name, info.m_charName, 0x1d);
        m_dbInfo.m_members[*idx].m_job = user->GetJob();
        m_dbInfo.m_members[*idx].m_growType = user->GetGrowthType();
        m_dbInfo.m_members[*idx].m_lev = user->GetLevel();
        (*idx)++;
        if (300 < *idx)
        {
            *idx = 300;
        }
        m_dbInfo.m_info.m_totalCnt = *idx;
    }
}

// [DNF-NONIDENTICAL] DNF-GLD-DIFF-0115 | guild | 与ORIG差异=DIFF | CGuild::SecedeProxyMember | 详见 function_reports/guild/_ZN6CGuild17SecedeProxyMemberER22ST_Notice_Guild_Secede.md
// [已试 2026-09-15 StaticsRest2] 我方序言 `push %ebx; sub $0x24` vs ORIG `push %esi; push %ebx; sub $0x20`
//   ⇒ ORIG 多用一个 callee-saved（%esi）保活某值，导致我方 `mov %eax,%edx; sub …; mov %edx,%eax`
//   双中转。属寄存器分配族（帧差来自 push 数，非多余局部）。
//   已试删除 `unsigned short* cnt = &m_totalCnt;` 并内联全部 `*cnt` ⇒ ndiff 23→39（更差，已回退）
//   ⇒ 该具名指针局部是必需的（与 P-H 实证一致），登记死胡同。
//   [2026-09-15 W3-GuildRest 追加] 机理 B 反推：ORIG 的 `push %esi` 来自该 cnt 指针跨 memmove 调用
//   保活 ⇒ 试 `register unsigned short* cnt __asm__("esi") = &m_totalCnt;`
//   ⇒ 111→**112**、ndiff 35→**41 劣化** ✗（%esi 亦被复用）⇒ 已回退，双路径均封死。
void CGuild::SecedeProxyMember(ST_Notice_Guild_Secede& info)
{
    if ((m_guildDBFlag & 4) == 0 || (m_guildDBFlag & 0x10) == 0)
    {
        return;
    }
    if (m_members.empty())
    {
        return;
    }
    {
        unsigned short* cnt = &m_totalCnt;
        if (*cnt < 0x12d)
        {
            for (unsigned int i = 0; i < (unsigned int)*cnt; i++)
            {
                if (m_dbInfo.m_members[i].m_no == (int)info.m_charNo)
                {
                    if ((unsigned int)*cnt - i != 1)
                    {
                        memmove(&m_dbInfo.m_members[i],
                                &m_dbInfo.m_members[i + 1],
                                ((unsigned int)*cnt - i - 1) * 0x41);
                    }
                    (*cnt)--;
                    if (300 < *cnt)
                    {
                        *cnt = 0;
                    }
                    m_totalCnt = *cnt;
                    m_dbInfo.m_info.m_totalCnt = *cnt;
                    return;
                }
            }
        }
    }
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0112 已消除）
bool CGuild::ChangeGuildMaster(CServerHandler* handler, CUser* user, unsigned int charNo)
{
    if ((m_guildDBFlag & 4) != 0 && charNo != 0)
    {
        m_dbInfo.m_info.m_masterId = charNo;
        ChangeUnconnectedGuildMemberGrade(charNo, 1);
        CUser* member;
        if ((member = FindGuildMember(charNo)) != 0)
        {
            member->ChangeGuildMemberGrade(1);
            GetGuildBoard()->sendMessageToDBMW_GuildMasterChanging(
                handler, user, member->GetCharName());
            strncpy(m_dbInfo.m_info.m_masterName, member->GetCharName(), 0x14);
        }
        else
        {
            GetGuildBoard()->sendMessageToDBMW_GuildMasterChanging(handler, user, getUnconnectedGuildMemberName(charNo));
            if (getUnconnectedGuildMemberName(charNo) != 0)
            {
                strncpy(m_dbInfo.m_info.m_masterName, getUnconnectedGuildMemberName(charNo), 0x14);
            }
        }
        return 1;
    }
    return 0;
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

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-NEAR-0015 已消除）
void CGuild::ChangeUnconnectedGuildMemberGrade(unsigned int charNo, int grade)
{
    if ((m_guildDBFlag & 4) != 0 && (m_guildDBFlag & 0x10) != 0)
    {
        for (int i = 0; i < m_totalCnt; i++)
        {
            if (m_dbInfo.m_members[i].m_no == charNo)
            {
                m_dbInfo.m_members[i].m_grade = (unsigned char)grade;
                return;
            }
        }
    }
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-NEAR-0014 已消除）
char* CGuild::getUnconnectedGuildMemberName(unsigned int charNo)
{
    if ((m_guildDBFlag & 4) != 0)
    {
        if ((m_guildDBFlag & 0x10) != 0)
        {
            for (int i = 0; i < m_totalCnt; i++)
            {
                if (m_dbInfo.m_members[i].m_no == charNo)
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

// [2026-09-15 W3-GuildRest 机理 B 实测] `register bool over __asm__("al")`（seta 落 %al）
//   使 ndiff 6→4→**1**（残余唯一差异 = `add %edx,%eax` vs 我方 `lea (%edx,%eax,1),%eax`，
//   即 i386 后端 add/lea alternative 选择族：需 op0==op1==目标，源码层不可控）。
//   另试：`register int ni __asm__("edx") = ~i;`（106→108 劣化 ✗）、
//   `register unsigned int ucnt __asm__("eax") = *cnt;`（ndiff→1 但 add 仍在）、
//   `ucnt = ucnt + ~i;` / `ucnt += ~i;`（均仍 lea）⇒ 属后端指令选择族，已回退。
// [DNF-NONIDENTICAL] DNF-GLD-DIFF-0116 | guild | 与ORIG差异=DIFF | CGuild::SetSubGuildMaster | 详见 function_reports/guild/_ZN6CGuild17SetSubGuildMasterEjb.md
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

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0135 已消除）| CGuild::NotifyMessageToGuildMember | 详见 function_reports/guild/_ZN6CGuild26NotifyMessageToGuildMemberEv.md
void CGuild::NotifyMessageToGuildMember()
{
    if ((m_guildDBFlag & 4) != 0)
    {
        if (m_members.empty())
        {
            return;
        }
        else
        {
            Packet_Guild_Notify_Message_To_Guild_Mem pkt;
            CUser* u;
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

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0151 已消除）
bool CGuild::PopGuildMemberChanglableInfo(unsigned int charNo,
                                          STGuildMemberChangableInfo& info) const
{
    if (m_changable.empty())
    {
        return false;
    }
    std::map<unsigned int, STGuildMemberChangableInfo>::const_iterator it =
        m_changable.find(charNo);
    if (m_changable.end() != it)
    {
        // ORIG：仅拷贝 map 值的首 dword（其余字段 ORIG 未初始化/不使用）
        info = it->second;
        return true;
    }
    return false;
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0137 已消除）
void CGuild::UpdateChangableInfoProcess()
{
    if (IsSetGuildDBFlag(4) && IsSetGuildDBFlag(0x10))
    {
        m_changableTickCnt++;
        // ORIG 该条件位把比较结果 bool 物化（seta %al; test %al,%al; je）——
        // 4.4 仅在 while/for 条件位物化，故用 while+break 复现（P225/P351）。
        while (9 < (unsigned char)m_changableTickCnt)
        {
            for (int i = 0; i < m_totalCnt; i++)
            {
                STGuildMemberChangableInfo info;
                if (PopGuildMemberChanglableInfo(m_dbInfo.m_members[i].m_no, info) != 0)
                {
                    m_dbInfo.m_members[i].m_lastPlayTime = info.m_time;
                }
            }
            m_changable.clear();
            m_changableTickCnt = 0;
            break;
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

// [DNF-NONIDENTICAL] DNF-GLD-DIFF-0130 | guild | 与ORIG差异=DIFF | CGuild::ChangeGuildMemberCharName | 详见 function_reports/guild/_ZN6CGuild25ChangeGuildMemberCharNameEjPc.md
bool CGuild::ChangeGuildMemberCharName(unsigned int charNo, char* name)
{
    bool result = false;
    if (IsSetGuildDBFlag(4))
    {
        for (int i = 0; i <= 299; i++)
        {
            if (m_dbInfo.m_members[i].m_no == charNo)
            {
                memset(m_dbInfo.m_members[i].m_name, 0, 0x1e);
                memcpy(m_dbInfo.m_members[i].m_name, name, 0x1d);
                result = true;
            }
        }
        if (m_board.m_today.m_charNo == charNo)
        {
            memset(&m_board.m_today.m_name0, 0, 0x1e);
            // [2026-09-15 W4-GuildRest 机理 P] 名字缓冲改用数组视图（GuildBoard.h union）：
            //   ORIG 此处为数组取址融合 `lea 0x66f0(%eax),%edx`，原 `(char*)&m_name0` 生成 `add`。
            strncpy(m_board.m_today.m_nameBytes, name, 0x1d);
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

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0121 已消除）
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
            // [N] ORIG `mov 0xd(%eax),%ebx` 单段直落；register/内联两形均反劣（2→12）⇒ 封顶
            if (m_dbInfo.m_members[i].m_no == user->GetUniqCharNo())
            {
                memset(m_dbInfo.m_members[i].m_memo, 0, 0x15);
                memcpy(m_dbInfo.m_members[i].m_memo, memo, 0x14);
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

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0136 已消除）
void CGuild::SendGuildAgitInfoToMembers()
{
    Packet_Channel_Guild_Agit_Info pkt;
    pkt.m_guildKey = m_guildKey;
    memcpy(&pkt.m_info, &m_agitInfo, 1);
    for (std::map<unsigned int, CUser*>::iterator it = m_members.begin();
         it != m_members.end(); ++it)
    {
        CUser* u;
        if ((u = it->second) == 0)
        {
            continue;
        }
        pkt.m_channel = u->GetIdByChannel();
        pkt.m_charNo = u->GetUniqCharNo();
        u->SendToGameserver((char*)&pkt, 0x17);
    }
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0143 已消除）
void CGuild::NotifyCreateGuildAgitToGuildMember(unsigned int charNo)
{
    // [P-nested-guard] 两个独立早返会让 4.4 各发一个尾岛（ORIG 4.1 只留 1 个）：
    //   改「首个守卫正向包裹 + 内层早返」复现 ORIG 单岛 + 守卫直跳 epilogue。
    if ((m_guildDBFlag & 4) != 0)
    {
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
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0144 已消除）
void CGuild::NotifyDeleteGuildAgitToGuildMember(unsigned int charNo)
{
    // [P-nested-guard] 双早返 → 单尾岛復现（同 NotifyCreateGuildAgitToGuildMember）
    if ((m_guildDBFlag & 4) != 0)
    {
        if (m_members.empty())
        {
            return;
        }
        Packet_Channel_Delete_Guild_Agit pkt;
        pkt.m12 = charNo;
        pkt.m16 = 0;
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

// [DNF-NONIDENTICAL] DNF-GLD-NEAR-0012 | guild | 与ORIG差异=NEAR | CGuild::AddGuildFund | 详见 function_reports/guild/_ZN6CGuild12AddGuildFundEj.md
// [已试 2026-09-15 StaticsRest2] 两个分支的 CMyFileLog 临时槽整体镜像：ORIG `-0x28`（if 分支）/`-0x30`（else），
//   我方反之。帧相同、槽集合相同 ⇒「槽互换族」，改声明位置不适用（log 为构造临时）⇒ 登记死胡同。
void CGuild::AddGuildFund(unsigned int fund)
{
    if ((m_guildDBFlag & 4) != 0)
    {
        m_dBSaveFlag = 1;
    // [2026-09-15 StaticsRest2] 本件差异（`lea -0x28/-0x30` 两个 log 临时槽镜像）经四步判据：
    //   ① 空差集 ✓ ② 差异落在槽操作数 ✓ ⇒ 名义上属互换族/B3。**但实测 B3 变体劣化**：
    //   交换 `fundPtr`/`levelPtr` 声明次序 ⇒ ndiff 4→**15**（已回退）。
    //   回退后复查发现真差异是**求值序**：ORIG `add $0xc0,%eax`（= &m_guildLevel，level 先）
    //   vs 我方 `add $0x3b,%eax`（= &m_guildFund，fund 先）⇒ 即 `*levelPtr` 与 `*fundPtr`
    //   地址求值的先后，属**求值序族**，与声明次序无关（交换后 GCC 仍先算 fundPtr）
    //   ⇒ 登记为「求值序族」，B3 在本件无靶点。
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
        // [2026-09-15 NEAR-Guild 复验] 唯一差异 = 两处 `CMyFileLog` **表达式临时**的栈槽互换
        //   （ORIG then→-0x28 / else→-0x30；我方反之）。两侧帧 0x5c、槽集合、指令数全同。
        //   本轮追加 2 形态（均**逐条无变化**，已回退）：V1 裸块包住 then 的 log 语句 ·
        //   V2 裸块包住 else 的 log 语句 ⇒ 确认 **块层次/释放时机不是该槽序的自变量**；
        //   叠加前泳道「交换 fundPtr/levelPtr 声明次序 ⇒ 4→15」⇒ 共 3 形态全负。
        //   ⇒ 定案：**GCC -O0 临时槽分配器内部顺序**（codegen-only），本 TU 无靶点 ✓
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

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0132 已消除）
void CGuild::NotifyAllTodayGuildMember()
{
    CUser* user = 0;
    Packet_Notify_Today_Guild_Member pkt;
    pkt.m_guildKey = m_guildKey;
    // ORIG：整块 0x27 字节结构拷贝（单次 this 装载 + %edx 中转），非逐字段赋值
    pkt.m_today = m_board.m_today;
    for (std::map<unsigned int, CUser*>::iterator it = m_members.begin();
         it != m_members.end(); ++it)
    {
        if ((user = it->second) == 0)
        {
            continue;
        }
        pkt.m_channel = user->GetIdByChannel();
        pkt.m_charNo = user->GetUniqCharNo();
        user->SendToGameserver((char*)&pkt, 0x3d);
    }
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0122 已消除）
void CGuild::NotifyTodayGuildMember(CUser* user)
{
    Packet_Notify_Today_Guild_Member pkt;
    pkt.m_guildKey = m_guildKey;
    // ORIG：整块 0x27 字节结构拷贝（单次 this 装载 + %edx 中转），非逐字段赋值
    pkt.m_today = m_board.m_today;
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

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0134 已消除）
void CGuild::NotifyAllAchieveAttendance(unsigned int charNo, unsigned int phase)
{
    CUser* user = 0;
    Packet_Achieve_Guild_Attendance pkt;
    pkt.m_charNo = charNo;
    pkt.m_phase = phase;
    for (std::map<unsigned int, CUser*>::iterator it = m_members.begin();
         it != m_members.end(); ++it)
    {
        if ((user = it->second) == 0)
        {
            continue;
        }
        pkt.m_channel = user->GetIdByChannel();
        pkt.m_charNo2 = user->GetUniqCharNo();
        user->SendToGameserver((char*)&pkt, 0x1a);
    }
}

STGuildSkill::STGuildSkill()
{
    m0 = -1;
    m4 = 0xff;
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-GLD-DIFF-0101、DNF-GLD-DIFF-0102 已消除）
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
    // [位域具名视图] ORIG = `movzbl 0x1c(%eax),%edx; or/and; mov %dl,0x1c(%eax)` 就地掩码
    //   （dbmw 侧同源实现已验证 identical）
    struct GuildAbilityView
    {
        char h[0x1c];
        unsigned char b0 : 1;  // +0x1c bit0
        unsigned char b1 : 1;  // +0x1c bit1
        unsigned char pad : 6; // +0x1c bits2-7
    } __attribute__((packed));
    ((GuildAbilityView*)this)->b0 = 1;
    ((GuildAbilityView*)this)->b1 = 0;
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
    // 原 m_data（权威定义中为 union 别名 m_field）；ORIG 0x080a03f0 = movzx;and $0x1f
    return (unsigned char)(m_field & 0x1f);
}
