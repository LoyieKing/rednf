// ============================================================================
// LinkMiscSecuTimerSolo.cpp —— 杂项/安全/定时器域剩余链接期未定义符号补齐。
// 各符号 ORIG 地址注释随注（nm -C df_game_r 核实）；实现逐指令转写自
// objdump。本文件由 LinkMiscSecuTimerSolo 批次注册（cmake/game/CMakeLists.txt）。
// 符号归属：
//   Timer* 域（TimerDispatcher.h 声明；dispatch_sig 已由 TimerDispatcher.cpp 提供）
//   STAIInfoScript（STMonsterScript.h 内联）/ GetLinearDataForMonsterAbility
//   CServerProxyMgr<CGuildServerProxy>::GetServerProxy（模板显式实例化）
//   competition_reward_result ctor（PacketTypes.h）
//   getScriptFileFullPath / findFileWithServerToken（RDAR 脚本域）
//   processModuleTagSet（RDAR 脚本模块标签域）
//   user_creature::CCreatureMgr::CheckDiedCreature（CInventory.h 声明）
//   CAutoMarketConditionsControl::LoadDatabase + GetMarketConditionRate +
//     SendNotiSetDurability 族 + DB_UpdateAutoMarketConditionsControl::makeRequest
// ============================================================================

#include "TimerDispatcher.h"
#include "TimerQueue.h"

#include "STMonsterScript.h"

#include "CServerProxyMgr.h"
#include "CGuildServerProxy.h"
#include "CGuildServerProxy.h"   // CEventManager 完整定义（CServerProxyMgr 模板析构）

#include "PacketTypes.h"

#include "CInventory.h"          // CCreatureMgr
#include "CCreature.h"           // CCreature::CheckDiedCreature

#include "CDataManager.h"        // CAutoMarketConditionsControl
#include "CEnvironment.h"
#include "GameWorld.h"
#include "GameTypes.h"           // cMyTrace
#include "CreatureScript.h"      // g_moduleNameTag
#include "DNFLexWrapper.h"       // ScanType
#include "LogManager.h"

#include "GlobalData.h"           // s_stream_pool / s_msgq_mgr / s_systemTime_
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "SigTypes.h"             // SIG_UPDATE_AUTO_MARKET_CONDITIONS_CONTROL
#include "InterDispatcher.h"   // Inter_MonitorServerEventStart
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"

#include <time.h>
#include <string>
#include <string>
#include <vector>
#include <map>

#include "../../../Library/Include/Core/SecureStdio.h"   // ss_strcpy / ss_strcat

// ---------------------------------------------------------------------------
// RDAR 脚本域全局（定义于 ScriptExtras.cpp；ORIG B 0x949b168 / 0x949b180）
// ---------------------------------------------------------------------------
extern std::vector<std::string> g_szScriptFilePriorityBaseDirectory;
extern char g_szScriptFileBaseDirectory[0x104];
// PackSet 未建模（ScriptExtras.cpp 同注）：pack 分支以 access 等价回退。
static bool dnf_packset_is_exist(const char* path)
{
    return access(path, 0) == 0;
}

// ===========================================================================
// findFileWithServerToken  ORIG T 0x88bb7db
// 路径"dir/name"已在 ORIG 无 '/' 时用 '.' 定位分隔符；把分隔符后段插入
// "server" 子目录名（dir/"server"+name），存在（access / PackSet）则改写
// 路径并返回 true；否则还原路径返回 false。
// ===========================================================================
bool findFileWithServerToken(char* path)
{
    std::string pathStr(path);
    std::string::size_type pos = pathStr.find_last_of("/");
    if (pos == std::string::npos)
        pos = pathStr.find_last_of("\\");
    if (pos == std::string::npos)
        return false;

    std::string full = pathStr.substr(0, pos + 1);
    full += "(R)";
    full += pathStr.substr(pos + 1, pathStr.size());

    ss_strcpy(path, 0x104, full.c_str());
    if (dnf_packset_is_exist(path))
        return true;

    ss_strcpy(path, 0x104, pathStr.c_str());
    return false;
}

// ===========================================================================
// getScriptFileFullPath  ORIG T 0x88bba4e
// 依次用优先根目录表（g_szScriptFilePriorityBaseDirectory）拼 dir/name，
// 命中 access/findFileWithServerToken 即采用；否则落到
// g_szScriptFileBaseDirectory。out 尺寸由 size 限制。
// ===========================================================================
void getScriptFileFullPath(char* out, unsigned int size, const char* dir,
                           const char* name)
{
    if (!g_szScriptFilePriorityBaseDirectory.empty())
    {
        int i = static_cast<int>(g_szScriptFilePriorityBaseDirectory.size()) - 1;
        while (true)
        {
            ss_strcpy(out, size,
                      g_szScriptFilePriorityBaseDirectory[i].c_str());
            if (dir != 0)
                ss_strcat(out, size, dir);
            if (name != 0)
                ss_strcat(out, size, name);
            if (access(out, 0) == 0 || findFileWithServerToken(out))
                return;
            if (--i < 0)
                break;
        }
        out[0] = 0;
    }
    ss_strcpy(out, size, g_szScriptFileBaseDirectory);
    if (dir != 0)
        ss_strcat(out, size, dir);
    if (name != 0)
        ss_strcat(out, size, name);
    if (access(out, 0) != 0)
        findFileWithServerToken(out);
}

// ===========================================================================
// processModuleTagSet  ORIG T 0x08a97637（RDARScriptSkill.cpp 模块标签域）
// 解析 "<module>"/"<sub>" 标签对：moduleId/subId 输出 + 前置 tag 消费。
// type 覆写为 3（ORIG 直接改参数）；标签表 g_moduleNameTag[0x20]（开/闭对）。
// ===========================================================================
// ORIG T 0x08a97637（mangled _Z19processModuleTagSetRiS_PSsib：int&,int& 形参）。
bool processModuleTagSet(int& moduleId, int& subId, std::string* tag,
                         int type, bool flag)
{
    type = 3;
    bool subMatched = false;
    moduleId = -1;
    subId = -1;
    int i = 0;
    for (; i <= 0x10; ++i)
    {
        if (*tag == g_moduleNameTag[2 * i] ||
            *tag == g_moduleNameTag[2 * i + 1])
        {
            moduleId = i;
            subMatched = (*tag == g_moduleNameTag[2 * i + 1]);
            if (i != type && type <= 0x10 &&
                g_moduleNameTag[2 * type] == g_moduleNameTag[2 * i])
            {
                moduleId = type;
            }
            break;
        }
    }

    if (moduleId != -1)
    {
        if (moduleId == type)
        {
            if (!subMatched)
                subId = type;
        }
        else if (g_moduleNameTag[2 * moduleId] == "[dungeon]")
        {
            if (subMatched)
            {
                subId = 3;
                LogManager::logFormat(0, "../RDARScriptSkill.cpp",
                                      __FUNCTION__, 0xc0,
                                      "skillscript separating : \xbc\xba\xb0\xf8 : \xb1\xe2\xba\xbb\xb0\xaa \xb8\xf0\xb5\xe2\xc5\xc2\xb1\xd7\xc0\xcc\xb9\xc7\xb7\xce \xc1\xf8\xc7\xe0 tag %s",
                                      tag->c_str());
            }
        }
        else if (g_moduleNameTag[2 * moduleId] == "[pvp]")
        {
            if (type == 9 || type == 5)
                subId = 4;
        }
        else
        {
            bool foundSub = false;
            bool matched = false;
            do
            {
                matched = false;
                ScanType(*tag, false);
                for (int j = 0; j <= 0x10; ++j)
                {
                    if (*tag == g_moduleNameTag[2 * j + 1])
                    {
                        foundSub = true;
                        matched = true;
                        break;
                    }
                }
            } while (!matched);
            if (!foundSub)
            {
                LogManager::logFormat(0, "../RDARScriptSkill.cpp",
                                      __FUNCTION__, 0xf3,
                                      "ERROR : skillscript separating : \xbf\xa1\xb7\xaf : \xc0\xdf\xb8\xf8\xb5\xc8 \xb8\xf0\xb5\xe2\xb1\xb8\xba\xd0\xc5\xc2\xb1\xd7 \xbb\xe7\xbf\xeb %s",
                                      tag->c_str());
            }
            return 0;
        }
    }

    if (subMatched)
        return 0;
    if (flag && moduleId != -1)
        ScanType(*tag, false);
    return 1;
}

// processModuleTagSet(std::string*, int&)  ORIG T 0x0896a87b（RDARScriptSkill.cpp）
// 局部 static 标签表（函数局部 _ZZ19processModuleTagSetPSsRiE13moduleNameTag，
// guard 惰性构造）：10 个开/闭标签对（[expert job none] 对不参与主循环）。
// i=1..4 命中开标签（arr[2i]）写 out=i 返回 0；命中闭标签（arr[2i+1]）返回 0；
// 否则返回 1。
// 验收（§6.2）：gap 125 全部位于 guard 内 static string[10] initializer-list
// 构造块——4.1 每元素独立 allocator temp 槽（-0x26..-0x1d）+双层倒计数循环+
// 逐元素 EH cleanup 链，4.4.7 复用 temp 槽合并循环（libstdc++ 版本内部结构
// 差，跨版本 N 类，源码层无合法写法可复现）；可达路径调用差集与 ORIG 一致
// （5×~string 差全在构造失败 landing pad，不执行）。parse 主体逐行同形，
// 仅 arr[i*2] 寻址 4.1 折 `shl $3` vs 4.4 `add+shl $2`（N1 常量折叠族，1 行）。
// ===========================================================================
bool processModuleTagSet(std::string* tag, int& out)
{
    static const std::string moduleNameTag[10] = {
        "[expert job none]", "[/expert job none]",
        "[magic enchant]",   "[/magic enchant]",
        "[alchemist]",       "[/alchemist]",
        "[disjointer]",      "[/disjointer]",
        "[doll controller]", "[/doll controller]"
    };
    for (int i = 1; i <= 4; ++i)
    {
        if (*tag == moduleNameTag[2 * i])
        {
            out = i;
            return 0;
        }
        if (*tag == moduleNameTag[2 * i + 1])
            return 0;
    }
    return 1;
}
// GetLinearDataForMonsterAbility  ORIG T 0x08891bae
// diff>=span → high；diff<=0 → low；否则 low + (high-low)*diff/span。
// ===========================================================================
int GetLinearDataForMonsterAbility(int low, int high, int diff, int span)
{
    if (diff >= span)
        return high;
    if (diff <= 0)
        return low;
    return low + (high - low) * diff / span;
}

// ===========================================================================
// Timer 域注册/生命周期。RegistNextTimer 族（TimerNatType/TimerNatTypeUser/
// TimerQueueSizeLog/TimerStatisticsDetailChannelInfo）已由 TimerDispatcher.cpp
// 权威定义（本文件重复实现于 2026-08-29 删除，避免多重定义）。
// 其余 Timer 生命周期 ctor/dtor 仍在本 TU。
// ===========================================================================

// TimerBossDieTimeout ctor W 0x0863c7fa / dtor W 0x0863fe2e（仅 vptr）。
TimerBossDieTimeout::TimerBossDieTimeout()
{
}

TimerBossDieTimeout::~TimerBossDieTimeout()
{
}

// TimerStartVillageAttacked ctor W 0x0863cd92 / dtor W 0x0863ee98。
TimerStartVillageAttacked::TimerStartVillageAttacked()
{
}

TimerStartVillageAttacked::~TimerStartVillageAttacked()
{
}

// ===========================================================================
// CServerProxyMgr<CGuildServerProxy>::GetServerProxy 显式实例化
// （ORIG W 0x0811d3b8；模板定义在 CServerProxyMgr.h，缺实例化点）。
// ===========================================================================
template CGuildServerProxy*
CServerProxyMgr<CGuildServerProxy>::GetServerProxy(ENUM_SERVER_GROUP);

// ===========================================================================
// competition_reward_result::competition_reward_result  ORIG W 0x081b47a8
// （+0x00/+0x04 int 清零 + +0x08 字节清零）。
// ===========================================================================
competition_reward_result::competition_reward_result()
{
    m_0 = 0;
    m_4 = 0;
    m_8 = 0;
}

// ===========================================================================
// user_creature::CCreatureMgr::CheckDiedCreature  ORIG T 0x08339acc：
// m_equippedCreature 非空且其 CheckDiedCreature(m_user) 为真 → true。
// ===========================================================================
bool user_creature::CCreatureMgr::CheckDiedCreature()
{
    if (m_equippedCreature != 0 &&
        m_equippedCreature->CheckDiedCreature(m_user))
        return true;
    return false;
}

// ===========================================================================
// GetMarketConditionRate  ORIG T 0x082f843f
// 找市场条件档位：*outIdx = 首个 arr[i]*rate > amount 的下标（<=4 扫描；
// 全不满足为 5），钳到 nCount+1；非 0 时 *outRate = arr[*outIdx+4]。
// ===========================================================================
void GetMarketConditionRate(double rate, double amount, double* arr,
                            int nCount, int& outIdx, double& outRate)
{
    outIdx = 0;
    outRate = 1.0;
    int i = 0;
    for (i = 0; i <= 4; ++i)
    {
        if (arr[i] * rate > amount)
            break;
    }
    outIdx = i;
    if (outIdx > nCount + 1)
        outIdx = nCount + 1;
    if (outIdx != 0)
        outRate = arr[outIdx + 4];
}

// ===========================================================================
// MakeNotiSetDurability  ORIG T 0x082f84f8：header(0,0xb3) + put_int + finalize(1)。
// ===========================================================================
void MakeNotiSetDurability(PacketGuard& packet, int value)
{
    packet.put_header(0, 0xb3);
    packet.put_int(value);
    packet.finalize(true);
}

// SendNotiSetDurability(int)  ORIG T 0x082f8540：广播耐久度比率包。
void CAutoMarketConditionsControl::SendNotiSetDurability(int value)
{
    PacketGuard packet;
    MakeNotiSetDurability(packet, value);
    G_GameWorld()->send_all(packet);
}

// SendNotiSetDurability()  ORIG T 0x082f85a8：m_durCurrent*100 转 int 后广播。
void CAutoMarketConditionsControl::SendNotiSetDurability()
{
    SendNotiSetDurability(static_cast<int>(m_durCurrent * 100.0));
}

// ===========================================================================
// DB_UpdateAutoMarketConditionsControl  ORIG T 0x08433d70 makeRequest /
// 头 0x15b；SIG_UPDATE_AUTO_MARKET_CONDITIONS_CONTROL（SigTypes.h，pad 0x30）
// 逐字段填充（+0x00..+0x28，DB 侧按此布局读）。
// ===========================================================================
class DB_UpdateAutoMarketConditionsControl
{
public:
    static void makeRequest(unsigned long long a, unsigned long long b,
                            unsigned long long c, unsigned long long d,
                            int e, int f, int g)
    {
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("../RDARScript.cpp", 0x7c79), true);
        guard.operator*()->operator<<(0x15b);
        guard.operator*()->operator<<(-1);
        SIG_UPDATE_AUTO_MARKET_CONDITIONS_CONTROL* sig =
            guard->GetInBuffer<SIG_UPDATE_AUTO_MARKET_CONDITIONS_CONTROL>();
        // 具名字段（SecuLink 批次裸偏移清理；SigTypes_E.h m_values0/m_values20
        // 布局，写侧 ORIG 0x08433d70 宽度：4×8B + 3×4B）。
        sig->m_values0[0] = a;
        sig->m_values0[1] = b;
        sig->m_values0[2] = c;
        sig->m_values0[3] = d;
        sig->m_values20[0] = e;
        sig->m_values20[1] = f;
        sig->m_values20[2] = g;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
};

// ===========================================================================
// CAutoMarketConditionsControl::LoadDatabase  ORIG T 0x082f8684
// （LoadDatabase.md 全量转写）：
//   基准值 = msg+0x68 非 0 ? (msg+0x0,+0x4) :
//             ((msg+0x44,0x48)+(msg+0x3c,0x40)) - ((msg+0x18,0x1c)+(msg+0x10,0x14))
//             + (msg+0x20,0x24)；
//   三次 GetMarketConditionRate（gold/equip/dur 段）；msg+0x38 变化则通知；
//   仅 channel no==1 时 DB_UpdateAutoMarketConditionsControl 上报。
// ===========================================================================
// ===========================================================================
// Inter_MonitorServerEventStart::SendEventNotiPacket  ORIG T 0x084cf2de
// 构造 header(0,0x45) + byte(noti) + short(eventIdx) + short(interval) 包；
// user 为空广播全服（GameWorld::send_all），否则单发（CUser::Send）。
// ===========================================================================
void Inter_MonitorServerEventStart::SendEventNotiPacket(bool noti,
                                                        short eventIdx,
                                                        short interval,
                                                        CUser* user)
{
    PacketGuard packet;
    packet.put_header(0, 0x45);
    packet.put_byte(noti ? 1 : 0);
    packet.put_short((int)eventIdx);
    packet.put_short((int)interval);
    packet.finalize(true);
    if (user == 0)
        G_GameWorld()->send_all(packet);
    else
        user->Send(packet);
}
int CAutoMarketConditionsControl::LoadDatabase(
    SIG_AUTO_MARKET_CONDITIONS_CONTROL* msg)
{
    unsigned long long base = 0;
    if (msg->m_view.m_flag != 0)
    {
        base = msg->m_view.m_overGold;
    }
    else
    {
        unsigned long long v1 =
            msg->m_view.m_rows[1].m_totalGold +
            msg->m_view.m_rows[1].m_auctionGold;
        unsigned long long v2 =
            msg->m_view.m_rows[0].m_totalGold +
            msg->m_view.m_rows[0].m_auctionGold;
        base = v1 - v2 + msg->m_view.m_rows[0].m_overGold;
    }

    double amount = static_cast<double>(
        static_cast<long long>(msg->m_view.m_optimumGold));
    if (amount < 0)
        amount += 2.0;   // ORIG fldt 0x8c1fc60 校正值（-2^63 回卷路径）【推断】

    int field30 = msg->m_view.m_rows[0].m_goldPhase;
    int field34 = msg->m_view.m_rows[0].m_itemPhase;
    int field38 = msg->m_view.m_rows[0].m_durabilityPhase;

    GetMarketConditionRate(amount, static_cast<double>(base), m_goldRate,
                           field30, m_field00, m_goldBaseRate);
    GetMarketConditionRate(amount, static_cast<double>(base), m_equipProb,
                           field34, m_field380, m_equipProbBase);
    GetMarketConditionRate(amount, static_cast<double>(base), m_durRate,
                           field38, m_field3e8, m_durCurrent);

    if (field38 != m_field3e8)
        SendNotiSetDurability();

    cMyTrace trace(
        __PRETTY_FUNCTION__,
        0xae, 0);
    trace("LoadDatabase() %qd %u %d %d %d",
          static_cast<long long>(static_cast<unsigned long long>(base)),
          msg->m_view.m_optimumGoldWords[0],
          static_cast<int>(msg->m_view.m_optimumGoldWords[1]),
          m_field00, m_field380, m_field3e8);

    long long diff = static_cast<long long>(base) -
                     static_cast<long long>(amount);
    if (diff < 0)
        diff = 0;

    if (G_CEnvironment()->get_channel_no() == 1)
    {
        DB_UpdateAutoMarketConditionsControl::makeRequest(
            msg->m_view.m_rows[1].m_totalGold,
            msg->m_view.m_rows[1].m_auctionGold,
            static_cast<unsigned long long>(diff),
            msg->m_view.m_optimumGold,
            m_field00, m_field380, m_field3e8);
    }
    return 0;
}
