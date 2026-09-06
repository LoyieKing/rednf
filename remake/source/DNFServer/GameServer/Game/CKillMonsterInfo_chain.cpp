// ============================================================================
// CKillMonsterInfo_chain.cpp —— 击杀/奖励链第三阶依赖（LinkKillChain 批次）。
//
// 覆盖 8 个 ORIG 真实符号 + 3 个链上内部依赖（全部经 docs/class_func_reports/
// 对应报告 ## 汇编 + ## 反编译 C 完整读取后忠实转写；无桩）：
//   CKillMonsterInfo::{Check,Register}BossKill        T 0x0854d184 / 0x0854d418
//   CKillMonsterInfo::{Check,Register}APCBossKill     T 0x0854d1e8 / 0x0854d48c
//   CKillMonsterInfo::{Check,Register}NamedKill       T 0x0854d1b6 / 0x0854d452
//   CKillMonsterInfo::GetDungeonDifficultyFlag        T 0x0854d21a（链上依赖）
//   CKillMonsterInfo::{Check,Register}MonsterKill     T 0x0854d0ea / 0x0854d24c（依赖）
//   CHellPartyItemStatistic::SendHellPartyItemStatistic T 0x0860fcc0
//   GetInstanceHellPartyItemStatistic                 T 0x0860fcb6
//
// CKillMonsterInfo 权威头 CKillMonsterInfo.h（继承 charac_expand::CData，
// 三个 std::map<uint,uchar> 子对象位于 +0x8/+0x20/+0x38，ctor 报告实证）；
// 上述谓词/登记/难度标志均以真实成员函数定义（桥清零，b21 批次）。
//
// CHellPartyItemStatistic 同样无权威头；SendHellPartyItemStatistic 不访问任何
// 成员（this 仅占位），单例 instance 为函数级 static（ORIG .bss _ZZ33...E8instance
// @0x0947334c，下一符号距 0x24 → sizeof 0x24）。
//
// 统计包 Packet_HellParty_Statistic_Item（ORIG ctor 0x0860ff9e =
// PacketHeader(0xc31, 0x2a)）本工程仅前向声明（Statistics.h），此处 TU 局部补全；
// 字段语义沿用 StatisticManager::WriteHellPartyStatisticItem 的 Wire 视图。
// ============================================================================

#include <map>
#include "CKillMonsterInfo.h"
#include "../../../shared/packet/include/Packet_HellParty_Statistic_Item.h"
#include <string.h>

#include "CBoosterGage.h"          // charac_expand::CData（alter）
#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h         // 经 WarField.h 引入 CAICharacterList/CDataManager 宿主
#include "CDataManager.h"          // G_CDataManager / find_monster / m_aiCharacterList
#include "LogManager.h"            // LogManager::logFormat
#include "CServerProxyMgr.h"      // CServerProxyMgr<T>::GetServerProxy 完整模板
#include "PacketHeader.h"          // PacketHeader(ushort, ushort)
#include "InterfacePacketBuf.h"    // PacketGuard 发送域接口
#include "GlobalData.h"            // GlobalData::s_statistic_proxy_mgr
#include "CStatisticServerProxy.h" // CStatisticServerProxy::SendPacket
#include "CStreamGuard.h"         // CStreamGuard(Stream*,bool) / Stream
#include "MsgQueueMgr.h"          // MsgQueueMgr::put(DB_Q, CStreamGuard&)
#include "SigTypes_C.h"           // SIG_LOAD_KILL_MONSTER_INFO

// ============================================================================
// CKillMonsterInfo 域 —— 桥清零：原 freestanding asm(_ZN…) shim 转为真实成员
// 函数（权威头 CKillMonsterInfo.h，返回型 void→bool 按 ORIG 报告；mangled 不含
// 返回型零 ABI 风险）。槽 map 经成员 m_bossKills/m_namedKills/m_apcBossKills
// 直接访问（+0x8/+0x20/+0x38），与 ORIG add $0x8/0x20/0x38 同形。
// ============================================================================

// ---- GetDungeonDifficultyFlag —— ORIG T 0x0854d21a -------------------------
// diff<=4 ? (1<<diff) : 0（shl %cl 位型实证；返回 int，写 map 值时按 byte 截断）。
int CKillMonsterInfo::GetDungeonDifficultyFlag(unsigned char diff)
{
    if (diff > 4)
        return 0;
    return 1 << diff;
}

// ---- CheckMonsterKill(map const&, mobIdx, diff) —— ORIG T 0x0854d0ea -------
// find(mobIdx)!=end 且 (diffFlag & 记录值) 非 0 → true。
bool CKillMonsterInfo::CheckMonsterKill(
    std::map<unsigned int, unsigned char> const& kills,
    unsigned int mobIdx, unsigned char diff)
{
    std::map<unsigned int, unsigned char>::const_iterator it =
        kills.find(mobIdx);
    if (it == kills.end())
        return false;
    return (GetDungeonDifficultyFlag(diff) & it->second) != 0;
}

// ---- RegisterMonsterKill(map&, mobIdx, diff, isAPC) —— ORIG T 0x0854d24c ---
// diff>4 → false；isAPC ? CAICharacterList::get==0 拒绝 : find_monster==0 拒绝；
// 已有记录 → flag |= diffFlag 并 alter()（charac_expand::CData::m_bIsSave=1）；
// 否则 insert(make_pair(idx,diffFlag))，插入失败记日志返回 false，成功 alter()。
bool CKillMonsterInfo::RegisterMonsterKill(
    std::map<unsigned int, unsigned char>& kills,
    unsigned int mobIdx, unsigned char diff, bool isAPC)
{
    if (diff > 4)
        return false;

    if (isAPC)
    {
        if (G_CDataManager()->m_aiCharacterList->get(mobIdx)
            == 0)
            return false;
    }
    else
    {
        if (G_CDataManager()->find_monster(static_cast<int>(mobIdx)) == 0)
            return false;
    }

    const unsigned char flag = static_cast<unsigned char>(
        GetDungeonDifficultyFlag(diff));
    std::map<unsigned int, unsigned char>::iterator it = kills.find(mobIdx);
    if (it != kills.end())
    {
        it->second = static_cast<unsigned char>(it->second | flag);
        alter();
        return true;
    }
    std::pair<std::map<unsigned int, unsigned char>::iterator, bool> ins =
        kills.insert(std::make_pair(mobIdx, flag));
    if (!ins.second)
    {
        LogManager::logFormat(
            1, "KillMonsterInfo.cpp",
            "bool CKillMonsterInfo::RegisterMonsterKill(killMonsterMap&, "
            "unsigned int, unsigned char, bool)",
            199, "KillMonsterInfo::RegisterMonsterKill Fail(%u, %u)", mobIdx,
            static_cast<unsigned int>(diff));
        return false;
    }
    alter();
    return true;
}

// ---- 六个公开谓词 -----------------------------------------------------------
// Check/Register{BossKill,NamedKill,APCBossKill} 分别转发至 +0x8/+0x20/+0x38
// 槽 map；Register 的 APC 标志按 ORIG：Boss=false、Named=false、APCBoss=true
// （见各报告反编译 C）。槽地址由 this 直接偏移（与 ORIG add $0x8/0x20/0x38 同形）。
bool CKillMonsterInfo::CheckBossKill(unsigned int mobIdx, unsigned char diff)
{
    return CheckMonsterKill(m_bossKills, mobIdx, diff);
}

bool CKillMonsterInfo::RegisterBossKill(unsigned int mobIdx, unsigned char diff)
{
    return RegisterMonsterKill(m_bossKills, mobIdx, diff, false);
}

bool CKillMonsterInfo::CheckNamedKill(unsigned int mobIdx, unsigned char diff)
{
    return CheckMonsterKill(m_namedKills, mobIdx, diff);
}

bool CKillMonsterInfo::RegisterNamedKill(unsigned int mobIdx, unsigned char diff)
{
    return RegisterMonsterKill(m_namedKills, mobIdx, diff, false);
}

bool CKillMonsterInfo::CheckAPCBossKill(unsigned int mobIdx, unsigned char diff)
{
    return CheckMonsterKill(m_apcBossKills, mobIdx, diff);
}

bool CKillMonsterInfo::RegisterAPCBossKill(unsigned int mobIdx,
                                           unsigned char diff)
{
    return RegisterMonsterKill(m_apcBossKills, mobIdx, diff, true);
}

// ============================================================================
// CHellPartyItemStatistic 域
// ============================================================================

// ORIG 包类 Packet_HellParty_Statistic_Item（ctor 0x0860ff9e = PacketHeader
// (0xc31, 0x2a)）：权威定义见 shared/packet/include/Packet_HellParty_Statistic_Item.h
//（布局：+0xa bool type / +0xb int dungeonIndex / +0xf char diff /
//  +0x10 char partyCount / +0x11 char hellpartyDiff / +0x12 int data[6]，
//  与 ORIG mangled _ZN31Packet_HellParty_Statistic_ItemC1Ev 一致）。

// ---- SendHellPartyItemStatistic —— ORIG T 0x0860fcc0 -----------------------
// dungeonDiff>4 或 scriptIdx==0 → false；否则组包经统计代理（组 0）发送 0x2a 字节。
bool HellPartySendItemStatisticShim(void* /*self*/, bool random, int dungeonIdx,
                                    int diff, int dieReason, char scriptIdx,
                                    int* statistic)
    asm("_ZN23CHellPartyItemStatistic26SendHellPartyItemStatisticEbiiicPi");
bool HellPartySendItemStatisticShim(void* /*self*/, bool random, int dungeonIdx,
                                    int diff, int dieReason, char scriptIdx,
                                    int* statistic)
{
    if (diff > 4)
        return false;
    if (scriptIdx == 0)
        return false;

    Packet_HellParty_Statistic_Item packet;
    packet.m_hellpartyType = random ? 1u : 0u;
    packet.m_dungeonIndex = static_cast<unsigned int>(dungeonIdx);
    packet.m_dungeonDiff = static_cast<unsigned char>(diff);
    packet.m_partyCount = static_cast<unsigned char>(dieReason);
    packet.m_hellpartyDiff = scriptIdx;
    memcpy(packet.m_data, statistic, 0x18);

    CStatisticServerProxy* proxy =
        GlobalData::s_statistic_proxy_mgr->GetServerProxy(
            static_cast<ENUM_SERVER_GROUP>(0));
    proxy->SendPacket(reinterpret_cast<char*>(&packet), 0x2a);
    return true;
}

// ---- GetInstanceHellPartyItemStatistic —— ORIG T 0x0860fcb6 ---------------
// 函数级 static 单例（ORIG .bss instance 0x0947334c，占 0x24 字节）。
void* GetInstanceHellPartyItemStatisticShim()
    asm("_Z33GetInstanceHellPartyItemStatisticv");
void* GetInstanceHellPartyItemStatisticShim()
{
    static unsigned char instance[0x24];
    return instance;
}


// ORIG C1 0x0854c9b4：CData 风格 vptr 后三 map 默认构造 + _reset()。
CKillMonsterInfo::CKillMonsterInfo()
{
    _reset();
}

CKillMonsterInfo::~CKillMonsterInfo()
{
}

// ORIG T 0x0854d0b8：三 map clear。
void CKillMonsterInfo::_reset()
{
    m_bossKills.clear();
    m_namedKills.clear();
    m_apcBossKills.clear();
}
// ============================================================================
// ORIG 数据序列化域 —— ConvertStructToMap / ConvertMapToStrcut / getData /
// loadData / _saveData（OrigKillMob 批次；逐报告 ## 汇编 + ## 反编译 C 转写）
// ============================================================================


// ---- ConvertStructToMap —— ORIG T 0x0854cb42 ------------------------------
// for 循环遍历 st 数组 count 项；mobIdx==0 提前返回 1。bAPC ? 经
// CAICharacterList::get 校验 : CDataManager::find_monster 校验；失败 logFormat
// 后 continue（ORIG jmp +0x158 循环增量实证）；通过则共用插入块
// insert(make_pair(mobIdx, diff))（make_pair 实参序 = 0x4(%esp) mobIdx /
// 0x8(%esp) diff 实证）。
int CKillMonsterInfo::ConvertStructToMap(
    stMonsterInfo_t* pInfo, unsigned int count,
    std::map<unsigned int, unsigned char>& outMap, bool bAPC)
{
    unsigned int mobIdx;
    unsigned char diff;
    unsigned int i;
    void* pMon;
    void* pAI;
    for (i = 0; i < count; ++i)
    {
        mobIdx = pInfo[i].mobIdx;
        if (mobIdx == 0)
            break;
        if (!bAPC)
        {
            pMon = G_CDataManager()->find_monster(mobIdx);
            if (pMon == 0)
            {
                LogManager::logFormat(
                    1, "KillMonsterInfo.cpp", __PRETTY_FUNCTION__, 0x25,
                    "ConvertStrToMap ERROR : mobIdx is not valid(%u)", mobIdx);
                continue;
            }
        }
        if (bAPC)
        {
            pAI = G_CDataManager()->m_aiCharacterList->get(mobIdx);
            if (pAI == 0)
            {
                LogManager::logFormat(
                    1, "KillMonsterInfo.cpp", __PRETTY_FUNCTION__, 0x2e,
                    "ConvertStrToMap ERROR : mobIdx is not valid(%u)", mobIdx);
                continue;
            }
        }
        diff = pInfo[i].diff;
        outMap.insert(std::make_pair(mobIdx, diff));
    }
    return 1;
}

// ---- ConvertMapToStrcut —— ORIG T 0x0854ccbc（const）----------------------
// empty→1；size>count→0；const_iterator 默认构造后 = begin()（P28 临时中转），
// while (it != end && i < count) 逐项回写 st[i]（i*5 步长、两次独立重算）。
int CKillMonsterInfo::ConvertMapToStrcut(
    const std::map<unsigned int, unsigned char>& inMap,
    stMonsterInfo_t* pInfo, unsigned int count) const
{
    if (inMap.empty())
        return 1;
    if (inMap.size() > count)
        return 0;
    std::map<unsigned int, unsigned char>::const_iterator it;
    it = inMap.begin();
    unsigned int i = 0;
    while (it != inMap.end() && i < count)
    {
        pInfo[i].mobIdx = it->first;
        pInfo[i].diff = it->second;
        it++;
        i++;
    }
    return 1;
}

// ---- getData —— ORIG T 0x0854ce90（const，void）---------------------------
// 三 map 顺序落盘：+0x0 / +0x1d4c / +0x927c（1500 / 6000 / 1500 项 × 5 字节）。
void CKillMonsterInfo::getData(char* pData) const
{
    char* p = pData;
    ConvertMapToStrcut(m_bossKills, (stMonsterInfo_t*)p, 0x5dc);
    ConvertMapToStrcut(m_namedKills, (stMonsterInfo_t*)(p + 0x1d4c), 6000);
    ConvertMapToStrcut(m_apcBossKills, (stMonsterInfo_t*)(p + 0x927c), 0x5dc);
}

// ---- loadData —— ORIG T 0x0854cddc ----------------------------------------
// 开头虚调 vptr[+0x1c] = _reset()（ORIG vtable 槽 7 实证）；三块顺序加载，
// APC 槽 bAPC=true（第三块）；返回 true。
bool CKillMonsterInfo::loadData(CUser* pUser, char* pData)
{
    _reset();
    char* p = pData;
    ConvertStructToMap((stMonsterInfo_t*)p, 0x5dc, m_bossKills, false);
    ConvertStructToMap((stMonsterInfo_t*)(p + 0x1d4c), 6000, m_namedKills,
                       false);
    ConvertStructToMap((stMonsterInfo_t*)(p + 0x927c), 0x5dc, m_apcBossKills,
                       true);
    return true;
}

// ---- _saveData —— ORIG T 0x0854cf16 ---------------------------------------
// BigStreamPool::Acquire → CStreamGuard(stream,true) → <<0x281/<<UID →
// GetInBuffer<SIG_LOAD_KILL_MONSTER_INFO>；非空则清零 0xafcc、写 characNo、
// 三 map 落 SIG 缓冲，put(DB_Q, guard)；返回 pSig!=0。
bool CKillMonsterInfo::_saveData(CUser* pUser)
{
    CStreamGuard guard(
        GlobalData::s_big_stream_pool->Acquire("KillMonsterInfo.cpp", 0x6b),
        true);
    
    guard.operator*()->operator<<(0x281);
    guard.operator*()->operator<<((int)pUser->GetUID());

    SIG_LOAD_KILL_MONSTER_INFO* pSig =
        guard->GetInBuffer<SIG_LOAD_KILL_MONSTER_INFO>();
    if (pSig == 0)
        return false;
    memset(pSig, 0, 0xafcc);
    pSig->m_characNo = pUser->get_charac_no(-1);
    ConvertMapToStrcut(m_bossKills, (stMonsterInfo_t*)pSig, 0x5dc);
    ConvertMapToStrcut(m_namedKills,
                       pSig->m_named, 6000);
    ConvertMapToStrcut(m_apcBossKills,
                       pSig->m_apc, 0x5dc);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
}
