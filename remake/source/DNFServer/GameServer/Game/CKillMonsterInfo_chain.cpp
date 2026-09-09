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
// CHellPartyItemStatistic 权威头 CHellPartyItemStatistic.h；本 TU 提供
// SendHellPartyItemStatistic 真实成员定义（桥清零）。单例 GetInstanceHellPartyItemStatistic
// 为自由函数，提供真实定义（函数级 static，ORIG .bss _ZZ33...E8instance
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
#include "CHellPartyItemStatistic.h"  // 地狱派对统计（桥清零，权威头）

// ============================================================================
// CKillMonsterInfo 域 —— 桥清零：原 freestanding asm(_ZN…) shim 转为真实成员
// 函数（权威头 CKillMonsterInfo.h，返回型 void→bool 按 ORIG 报告；mangled 不含
// 返回型零 ABI 风险）。槽 map 经成员 m_bossKills/m_namedKills/m_apcBossKills
// 直接访问（+0x8/+0x20/+0x38），与 ORIG add $0x8/0x20/0x38 同形。
// ============================================================================

// ORIG C1 0x0854c9b4：CData 风格 vptr 后三 map 默认构造 + _reset()。
CKillMonsterInfo::CKillMonsterInfo()
{
    _reset();
}

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
// [b28d-solo-CKillMonsterInfo_chain 判词 · AE] ORIG 0x0854d0ea（49 条）：
// const_iterator::operator!=（ORIG `ne(PKi,RKi)`）、字节宽度判定
// `test %al,%al; setne`（uchar 物化）、共享 false 尾块均对齐（P167 CLEAR）。
// 关键旋钮（P231/P248 档②）：`register unsigned char rec = it->second;` +
// 条件 RHS 侧 `(unsigned char)GetDungeonDifficultyFlag(diff)` —— 使 ORIG 0854d149
// 的 `movzbl 0x4(%eax),%ebx` 单段直落 callee-saved 复现；无 register 时 4.4 恒
// `movzbl …,%eax; mov %eax,%ebx`（50/49），RHS 不加 uchar cast 则丢 setne 段。
// 现 49=49 助记符/槽位逐条全等，残余仅 .o 重定位地址显示（P264）。
bool CKillMonsterInfo::CheckMonsterKill(
    std::map<unsigned int, unsigned char> const& kills,
    unsigned int mobIdx, unsigned char diff)
{
    std::map<unsigned int, unsigned char>::const_iterator it =
        kills.find(mobIdx);
    // [b28d-7 P167] ORIG 调 const_iterator::operator!=（0x0854d10x），
    // 旧版 == 产生另一实例（调用点差集 MISSING/EXTRA 各 1）。
    if (it != kills.end())
    {
        // ORIG 0x0854d1xx：`and %ebx,%eax; test %al,%al`（字节宽度判定）⇒
        // 结果按 unsigned char 收窄。
        register unsigned char rec = it->second;
        if ((rec & (unsigned char)GetDungeonDifficultyFlag(diff)) != 0)
            return true;
        return false;
    }
    return false;
}

// ---- RegisterMonsterKill(map&, mobIdx, diff, isAPC) —— ORIG T 0x0854d24c ---
// diff>4 → false；isAPC ? CAICharacterList::get==0 拒绝 : find_monster==0 拒绝；
// 已有记录 → flag |= diffFlag 并 alter()（charac_expand::CData::m_bIsSave=1）；
// 否则 insert(make_pair(idx,diffFlag))，插入失败记日志返回 false，成功 alter()。
// [b28d-solo-CKillMonsterInfo_chain 判词 · NEAR/N93] ORIG 0x0854d24c（129 条）：
//  · 已对齐：`if (!isAPC)` 极性（ORIG `movzbl isAPC; xor $1; test; je <APC>`）、
//    两校验结果具名槽（-0x10 find_monster / -0xc get + `cmpl $0x0,槽; jne`，
//    P0 槽往返）、两成功路径共享尾 `mov $1,%eax`、make_pair 右值 uchar、
//    单次 operator->（P167 CLEAR 16=16）。
//    ORIG 0854d325 `movzbl 0x4(%ebx),%esi` 单段直落亦已复现——复合赋值 RHS 写
//    `(unsigned char)GetDungeonDifficultyFlag(diff)` 即消 `mov %eax,%esi` 中转
//    （P248 档② 同族，129=129）。
//  · 残余（N93/N98 封顶）：仅 `it`(ORIG -0x34/-0x30) 与 `ins`(ORIG -0x2c/-0x28)
//    槽位互换——4.1 按声明序先声明深，4.4 按首次使用序先使用浅；it 的 find 必
//    先于 ins 的 insert，声明序翻转无效（N98），块内提头部需 ins 默认构造 ⇒
//    多 pair C1Ev call + 8 字节拷贝（违反 P215/发明符号禁令）⇒ 结构性封顶。
bool CKillMonsterInfo::RegisterMonsterKill(
    std::map<unsigned int, unsigned char>& kills,
    unsigned int mobIdx, unsigned char diff, bool isAPC)
{
    if (diff > 4)
        return false;

    // [b28d-solo-CKillMonsterInfo_chain] ORIG 0854d24c：`movzbl isAPC; xor $1;
    // test; je <APC 路径>` ⇒ 原码极性为 `if (!isAPC)`（find_monster 直落）。
    // 两个校验结果均落具名槽（`mov %eax,-0x10/-0xc(%ebp); cmpl $0x0,槽; jne`，
    // P0 槽往返 ⇒ 原码有具名局部）；直比调用结果会多 sete 物化。
    if (!isAPC)
    {
        void* pMon = G_CDataManager()->find_monster(static_cast<int>(mobIdx));
        if (pMon == 0)
            return false;
    }
    else
    {
        CAICharacter* pAI = G_CDataManager()->m_aiCharacterList->get(mobIdx);
        if (pAI == 0)
            return false;
    }

    // [b28d-7 P167] ORIG 无具名 flag 局部：两分支各自内联
    // GetDungeonDifficultyFlag(diff)（ORIG 2 次 vs 旧版 1 次），make_pair 第二参
    // 为 uchar 右值（0x0854d2xx 符号 `make_pair<unsigned int&, unsigned char>`），
    // 且记录分支用 `(*it).second`（ORIG 无 operator->，用 operator*）。
    std::map<unsigned int, unsigned char>::iterator it = kills.find(mobIdx);
    if (it != kills.end())
    {
        // ORIG 记录分支只有 1 次 operator->（复合赋值单次取址），
        // 旧版读+写两次 it->second 多 1 个调用点。
        it->second |= (unsigned char)GetDungeonDifficultyFlag(diff);
        alter();
    }
    else
    {
        // [b28d-solo-CKillMonsterInfo_chain] ORIG 两成功路径共享尾 `mov $1,%eax`
        // （记录分支 `call alter; jmp +0x1bd`，插入成功分支 fall-through）⇒
        // 尾部单一 return true，分支内不得各自 return。
        std::pair<std::map<unsigned int, unsigned char>::iterator, bool> ins =
            kills.insert(std::make_pair(
                mobIdx,
                static_cast<unsigned char>(GetDungeonDifficultyFlag(diff))));
        if (!ins.second)
        {
            LogManager::logFormat(
                1, "KillMonsterInfo.cpp",
                "bool CKillMonsterInfo::RegisterMonsterKill(killMonsterMap&, "
                "unsigned int, unsigned char, bool)",
                199, "KillMonsterInfo::RegisterMonsterKill Fail(%u, %u)",
                mobIdx, static_cast<unsigned int>(diff));
            return false;
        }
        alter();
    }
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
// [b28d-solo-CKillMonsterInfo_chain 判词 · AE] ORIG 0x0860fcc0（50 条）：
// 删具名 proxy 局部、改单条链式表达式（P52）后 `lea -0x32(%ebp),%ebx` 跨
// GetServerProxy call 保活复现，push %ebx + 帧 $0x54 + 槽 -0x28/-0x32 全对齐
// （旧版具名 proxy 帧 $0x58 无 push）。50=50 助记符/槽位逐条全等，P167 CLEAR；
// 残余仅 .o 重定位地址显示（P264）。
bool CHellPartyItemStatistic::SendHellPartyItemStatistic(
    bool random, int dungeonIdx, int diff, int dieReason, char scriptIdx,
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

    // [b28d-solo-CKillMonsterInfo_chain P52] ORIG 0850fcc0：`lea -0x32(%ebp),%ebx`
    // 在 GetServerProxy call 之前物化 &packet 并跨 call 保活，proxy 结果无槽
    // 直传（`mov %eax,(%esp)`）⇒ 必须写单条链式表达式；具名 proxy 局部多
    // `mov %eax,-0xc` + 回读且丢 push %ebx（帧 0x58 vs ORIG 0x54）。
    GlobalData::s_statistic_proxy_mgr->GetServerProxy(
        static_cast<ENUM_SERVER_GROUP>(0))
        ->SendPacket(reinterpret_cast<char*>(&packet), 0x2a);
    return true;
}

// ---- GetInstanceHellPartyItemStatistic —— ORIG T 0x0860fcb6 ---------------
// 函数级 static 单例（ORIG .bss instance 0x0947334c，占 0x24 字节）。
void* GetInstanceHellPartyItemStatistic()
{
    static unsigned char instance[0x24];
    return instance;
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
// [b28d-solo-CKillMonsterInfo_chain 判词 · AE] ORIG 0x0854cb42：100/100
// 助记符/槽位逐条全等（check_tu_routed 路由口径），P167 CLEAR。清单旧标
// NEAR ndiff=9 系 .o 重定位地址显示差（P264：字符串/函数地址未解析），
// 非助记符差异；TU 口径归 AE。
bool CKillMonsterInfo::ConvertStructToMap(
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
    // N7 封顶：唯一残差=虚调用 callee 寄存器互换（N7，探针 /tmp/l22p/vprobe*.cpp）。
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
