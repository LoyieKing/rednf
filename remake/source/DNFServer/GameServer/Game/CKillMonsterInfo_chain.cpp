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
// CKillMonsterInfo 无权威头：完整类继承 charac_expand::CData（vptr+isSave 共 5
// 字节，pack(1)），三个 std::map<uint,uchar> 子对象位于 +0x8/+0x20/+0x38
// （ctor 报告 CKillMonsterInfo.md 实证）。本工程无该类声明点，故按 CPartyLink2.cpp
// 的 CAICharacter 先例以"视图结构 + ORIG mangled asm 符号"提供定义；调用方
// （CBattle_Field_kill_monster.cpp 的匿名 shim 声明、CParty 域）链接期解析到本 TU。
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
#include <string.h>

#include "CBoosterGage.h"          // charac_expand::CData（alter）
#include "CBattle_Field.h"         // 经 WarField.h 引入 CAICharacterList/CDataManager 宿主
#include "CDataManager.h"          // G_CDataManager / find_monster / m_aiCharacterList
#include "LogManager.h"            // LogManager::logFormat
#include "CServerProxyMgr.h"      // CServerProxyMgr<T>::GetServerProxy 完整模板
#include "PacketHeader.h"          // PacketHeader(ushort, ushort)
#include "InterfacePacketBuf.h"    // PacketGuard 发送域接口
#include "GlobalData.h"            // GlobalData::s_statistic_proxy_mgr
#include "CStatisticServerProxy.h" // CStatisticServerProxy::SendPacket

// ============================================================================
// CKillMonsterInfo 域 —— 视图布局（ORIG ctor 0x0854c9b4 实证）
// ============================================================================
namespace
{

struct KMIView
{
    void* m_vptr;                                         // +0x00（CData vptr）
    unsigned char m_isSave;                               // +0x04（CData::m_bIsSave）
    std::map<unsigned int, unsigned char> m_bossKills;      // +0x08（Boss=roleType 3）
    std::map<unsigned int, unsigned char> m_namedKills;     // +0x20（Named）
    std::map<unsigned int, unsigned char> m_apcBossKills;   // +0x38（APC Boss=roleType 8）
};

inline charac_expand::CData* kmiAsCData(void* self)
{
    return reinterpret_cast<charac_expand::CData*>(self);
}

}  // namespace

// ---- GetDungeonDifficultyFlag —— ORIG T 0x0854d21a -------------------------
// diff<=4 ? (1<<diff) : 0（shl %cl 位型实证；返回 int，写 map 值时按 byte 截断）。
int KmiGetDungeonDifficultyFlagShim(void* /*self*/, unsigned char diff)
    asm("_ZN16CKillMonsterInfo24GetDungeonDifficultyFlagEh");
int KmiGetDungeonDifficultyFlagShim(void* /*self*/, unsigned char diff)
{
    if (diff > 4)
        return 0;
    return 1 << diff;
}

// ---- CheckMonsterKill(map const&, mobIdx, diff) —— ORIG T 0x0854d0ea -------
// find(mobIdx)!=end 且 (diffFlag & 记录值) 非 0 → true。
bool KmiCheckMonsterKillShim(
    void* /*self*/, std::map<unsigned int, unsigned char> const* kills,
    unsigned int mobIdx, unsigned char diff)
    asm("_ZN16CKillMonsterInfo16CheckMonsterKillERKSt3mapIjhSt4lessIjESaISt4pairIKjhEEEjh");
bool KmiCheckMonsterKillShim(
    void* /*self*/, std::map<unsigned int, unsigned char> const* kills,
    unsigned int mobIdx, unsigned char diff)
{
    std::map<unsigned int, unsigned char>::const_iterator it = kills->find(mobIdx);
    if (it == kills->end())
        return false;
    return (KmiGetDungeonDifficultyFlagShim(0, diff) & it->second) != 0;
}

// ---- RegisterMonsterKill(map&, mobIdx, diff, isAPC) —— ORIG T 0x0854d24c ---
// diff>4 → false；isAPC ? CAICharacterList::get==0 拒绝 : find_monster==0 拒绝；
// 已有记录 → flag |= diffFlag 并 alter()（charac_expand::CData::m_bIsSave=1）；
// 否则 insert(make_pair(idx,diffFlag))，插入失败记日志返回 false，成功 alter()。
bool KmiRegisterMonsterKillShim(
    void* self, std::map<unsigned int, unsigned char>* kills,
    unsigned int mobIdx, unsigned char diff, bool isAPC)
    asm("_ZN16CKillMonsterInfo19RegisterMonsterKillERSt3mapIjhSt4lessIjESaISt4pairIKjhEEEjhb");
bool KmiRegisterMonsterKillShim(
    void* self, std::map<unsigned int, unsigned char>* kills,
    unsigned int mobIdx, unsigned char diff, bool isAPC)
{
    if (diff > 4)
        return false;

    if (isAPC)
    {
        if (CAICharacterList::get(G_CDataManager()->m_aiCharacterList, mobIdx)
            == 0)
            return false;
    }
    else
    {
        if (G_CDataManager()->find_monster(static_cast<int>(mobIdx)) == 0)
            return false;
    }

    const unsigned char flag = static_cast<unsigned char>(
        KmiGetDungeonDifficultyFlagShim(0, diff));
    std::map<unsigned int, unsigned char>::iterator it = kills->find(mobIdx);
    if (it != kills->end())
    {
        it->second = static_cast<unsigned char>(it->second | flag);
        kmiAsCData(self)->alter();
        return true;
    }
    std::pair<std::map<unsigned int, unsigned char>::iterator, bool> ins =
        kills->insert(std::make_pair(mobIdx, flag));
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
    kmiAsCData(self)->alter();
    return true;
}

// ---- 六个公开谓词 -----------------------------------------------------------
// Check/Register{BossKill,NamedKill,APCBossKill} 分别转发至 +0x8/+0x20/+0x38
// 槽 map；Register 的 APC 标志按 ORIG：Boss=false、Named=false、APCBoss=true
// （见各报告反编译 C）。槽地址由 this 直接偏移（与 ORIG add $0x8/0x20/0x38 同形）。
// 槽地址由 this 直接偏移（与 ORIG add $0x8/0x20/0x38 同形；宏避免 O0 下
// inline 辅助函数产生 ORIG 不存在的额外 call）。
#define KMI_SLOT(self, off)                                                  \
    (reinterpret_cast<std::map<unsigned int, unsigned char>*>(              \
        reinterpret_cast<char*>(self) + (off)))
bool KmiCheckBossKillShim(void* self, unsigned int mobIdx, unsigned char diff)
    asm("_ZN16CKillMonsterInfo13CheckBossKillEjh");
bool KmiCheckBossKillShim(void* self, unsigned int mobIdx, unsigned char diff)
{
    return KmiCheckMonsterKillShim(self, KMI_SLOT(self, 0x8), mobIdx, diff);
}

bool KmiRegisterBossKillShim(void* self, unsigned int mobIdx, unsigned char diff)
    asm("_ZN16CKillMonsterInfo16RegisterBossKillEjh");
bool KmiRegisterBossKillShim(void* self, unsigned int mobIdx, unsigned char diff)
{
    return KmiRegisterMonsterKillShim(self, KMI_SLOT(self, 0x8), mobIdx, diff,
                                     false);
}

bool KmiCheckNamedKillShim(void* self, unsigned int mobIdx, unsigned char diff)
    asm("_ZN16CKillMonsterInfo14CheckNamedKillEjh");
bool KmiCheckNamedKillShim(void* self, unsigned int mobIdx, unsigned char diff)
{
    return KmiCheckMonsterKillShim(self, KMI_SLOT(self, 0x20), mobIdx, diff);
}

bool KmiRegisterNamedKillShim(void* self, unsigned int mobIdx, unsigned char diff)
    asm("_ZN16CKillMonsterInfo17RegisterNamedKillEjh");
bool KmiRegisterNamedKillShim(void* self, unsigned int mobIdx, unsigned char diff)
{
    return KmiRegisterMonsterKillShim(self, KMI_SLOT(self, 0x20), mobIdx, diff,
                                     false);
}

bool KmiCheckAPCBossKillShim(void* self, unsigned int mobIdx, unsigned char diff)
    asm("_ZN16CKillMonsterInfo16CheckAPCBossKillEjh");
bool KmiCheckAPCBossKillShim(void* self, unsigned int mobIdx, unsigned char diff)
{
    return KmiCheckMonsterKillShim(self, KMI_SLOT(self, 0x38), mobIdx, diff);
}

bool KmiRegisterAPCBossKillShim(void* self, unsigned int mobIdx,
                                unsigned char diff)
    asm("_ZN16CKillMonsterInfo19RegisterAPCBossKillEjh");
bool KmiRegisterAPCBossKillShim(void* self, unsigned int mobIdx,
                                unsigned char diff)
{
    return KmiRegisterMonsterKillShim(self, KMI_SLOT(self, 0x38), mobIdx, diff,
                                     true);
}

// ============================================================================
// CHellPartyItemStatistic 域
// ============================================================================

namespace
{

#pragma pack(push, 1)
// ORIG Packet_HellParty_Statistic_Item：header 0xa + payload 0x20 = 0x2a。
class HellPartyStatisticItemPacket : public PacketHeader
{
public:
    HellPartyStatisticItemPacket() : PacketHeader(0xc31, 0x2a) {}

    unsigned char m_hellpartyType;   // +0xa（调用点传 random 标志）
    unsigned int m_dungeonIndex;     // +0xb
    unsigned char m_dungeonDiff;     // +0xf
    unsigned char m_partyCount;      // +0x10（调用点传 dieReason）
    char m_hellpartyDiff;            // +0x11（调用点传 hellPartyScriptIdx）
    int m_data[6];                   // +0x12..+0x29（m_hellItemStatistic[6]）
};
#pragma pack(pop)

}  // namespace

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

    HellPartyStatisticItemPacket packet;
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
