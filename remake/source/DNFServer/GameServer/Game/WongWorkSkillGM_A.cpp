// ============================================================================
// WongWorkSkillGM_A.cpp — WongWorkA 批次 G5：CSkillChanger 追加成员 + CGMAccounts
//   独占 TU；勿在此实现 SkillSlot.cpp 已有的 10 个 CSkillChanger 符号。
// 证据来源：docs/class_func_reports/WongWork__CSkillChanger/*.md、
//           WongWork__CGMAccounts/*.md 及 objdump df_game_r（缺报告符号已标注）。
//
// 字段布局（ORIG 0x092 对象，4 组 kind 数据各 0x24）：
//   +0x00 int counts[9]        —— 用户持有 SP 书计数（m_data.m_skillBookCount）
//   +0x24 int ability[9]       —— g_nElixerAbility 表值（m_data.m_elixerAbility）
//   +0x48 int needCount[9]     —— 需求洗点书数量（union 内无具名，按偏移访问）
//   +0x6c int needAbility[9]   —— 需求洗点书能力表
//   +0x90 m_state.m_initialized —— SkillInitialize/GetCurCharacTotalSP 的 bool 入参
//                                （_addSPRelatedItem 0x0860a8a8 movzbl 0x90 读取的门控）
//   +0x91 m_state.m_reserved91  —— CountElixer 置 1 / SkillInitialize+
//                                GetCurCharacTotalSP 置 0（movb $?,0x91）
// ============================================================================

#include <string.h>

#include "SkillSlot.h"          // CSkillChanger 类定义 / CUser / NumberToString / G_CDataManager
#include "Quest.h"              // Quest（+0xb8 字段）/ RewardItemInfo 完整型
#include "QuestClear.h"         // WongWork::CQuestClear::enumQuestClear 回调链
#include "CDataManager.h"       // find_quest / get_level_exp
#include "CMyFileLog.h"         // AppendGM_Sys 日志
#include "CGuildServerProxy.h"  // WongWork::CGMAccounts / stGMInfo_t 权威声明

namespace WongWork
{

// ORIG T 0x0810933c loadGMAccounts(char const*)：恒返回 1。
int CGMAccounts::loadGMAccounts(char const* /*path*/)
{
    return 1;
}

// ============================================================================
// ORIG T 0x08609d10 CheckCondition(CUser const*)
// exp < get_level_exp(level) || get_level_exp(level+1) < exp → 不可执行
// ============================================================================
bool CSkillChanger::CheckCondition(const CUser* user)
{
    // ORIG 0x08609d10：get_charac_level ×2、G_CDataManager ×2、
    // getCurCharacExp 两条件各取一次（P48 无 CSE）；结果槽 -0xc/-0x10。
    unsigned int levelExp = static_cast<unsigned int>(
        G_CDataManager()->get_level_exp(user->get_charac_level()));
    unsigned int nextLevelExp = static_cast<unsigned int>(
        G_CDataManager()->get_level_exp(user->get_charac_level() + 1));
    // 双物化岛：bVar1 = (c1||c2) 经 mov$1/mov$0，再 `return !bVar1` 二次物化
    // （+0x76 test %al;je / +0x7a mov$0 / +0x81 mov$1）。register bool 强制。
    if (({ (user->getCurCharacExp() < levelExp)
            || (nextLevelExp < user->getCurCharacExp()); }))
        return false;
    return true;
}
// ============================================================================
// ORIG T 0x08609d9c CountElixer(CUser*)：重置四组数据（8/组首）、置就绪标志，
// 统计用户物品并应用双树 kind 数据。
// ============================================================================
void CSkillChanger::CountElixer(CUser* user)
{
    memset(m_data.m_elixerCount, 0, 0x24);
    memset(m_data.m_skillBookCount, 0, 0x24);
    memset(m_data.m_needElixerCount, 0, 0x24);
    memset(m_data.m_needElixerAbility, 0, 0x24);
    m_state.m_sec0.m_kindState = 8;   // 组首 +0x00/0x24/0x48/0x6c（SkillB 将 CSkillChanger 4 节结构化为 STKindSection）
    m_state.m_sec1.m_kindState = 8;
    m_state.m_sec2.m_kindState = 8;
    m_state.m_sec3.m_kindState = 8;
    m_state.m_reserved91 = 1;                      // ORIG 0x08609e41 movb $0x1,0x91(%eax)
    _GetUserItemCount(user);
    _ApplyUserData(user, (ENUM_SKILL_TREE_KIND)0);
    _ApplyUserData(user, (ENUM_SKILL_TREE_KIND)1);
}

// ORIG W 0x0869780e GetElixerCount()：返回第一组计数数组（即 this）。
int* CSkillChanger::GetElixerCount()
{
    return m_data.m_elixerCount;
}

// ORIG W 0x08697816 GetNeedElixerCount()：返回 this+0x48。
int* CSkillChanger::GetNeedElixerCount()
{
    return m_data.m_needElixerCount;
}

// ORIG W 0x08697822 GetNeedElixerAbility()：返回 this+0x6c。
int* CSkillChanger::GetNeedElixerAbility()
{
    return m_data.m_needElixerAbility;
}

// ============================================================================
// ORIG T 0x0860a868 _addSPRelatedItem(int*,int*,int,int)
// 0x407 → *a += count；0x40e → *b += count；
// 其余且 initialized：命中 SP 书索引时 needCount[i] += count、
// needAbility[i] = g_nElixerAbility[i]（直接赋值，与 ORIG store 一致）。
// ============================================================================
void CSkillChanger::_addSPRelatedItem(int* a, int* b, int itemIdx, int count)
{
    if (itemIdx == 0x407)
    {
        *a = *a + count;
    }
    else if (itemIdx == 0x40e)
    {
        *b = *b + count;
    }
    else if (m_state.m_initialized != 0)
    {
        for (int i = 1; i <= CSkillChanger::g_nStatUpItemIdx[0]; i++)
        {
            if (CSkillChanger::g_nStatUpItemIdx[i] == itemIdx)
            {
                // b28d-D66：P183 地址重物化铁律 —— ORIG 每处读写都重发完整
                // `mov this,%eax; add $0x10,%edx/%ecx; mov 0x8/0xc(%eax,%edx,4)`
                // 两步寻址（0x0860a8cb..0x0860a901），无指针局部；原具名
                // `int* needCount = ...` 缓存使 4.4 多发 `add $0x48; mov %eax,-0x8`
                // 与 `shl/add` 序列（61 vs 56 insn）。
                m_data.m_needElixerCount[i] = m_data.m_needElixerCount[i] + count;
                m_data.m_needElixerAbility[i] = CSkillChanger::g_nElixerAbility[i];
                return;
            }
        }
    }
}

// ============================================================================
// ORIG T 0x0860a918 _addSFPRelatedItem(int*,int*,int,int)
// 0x4b4 → *a += count；0x4b5 → *b += count；其余无操作。
// ============================================================================
void CSkillChanger::_addSFPRelatedItem(int* a, int* b, int itemIdx, int count)
{
    if (itemIdx == 0x4b4)
    {
        *a = *a + count;
    }
    else if (itemIdx == 0x4b5)
    {
        *b = *b + count;
    }
}

// ============================================================================
// ORIG T 0x0860a6da static _fnEnumQuestClear(uint,char*)：enumQuestClear 回调。
// va 打包区依次：int& spA,int& spB,int& sfpA,int& sfpB,this,user。
// 查任务；仅限 m_fieldB8 == 0 或 25（一般/重复类任务）统计其奖励物品；
// SP 书（0x407/0x40e 与表格项）与 SFP 书（0x4b4/0x4b5）分别累计。恒返回 true。
// ============================================================================
bool CSkillChanger::_fnEnumQuestClear(unsigned int questIdx, char* args)
{
    int** slots = reinterpret_cast<int**>(args);
    int* spA = slots[0];
    int* spB = slots[1];
    int* sfpA = slots[2];
    int* sfpB = slots[3];
    CSkillChanger* self = reinterpret_cast<CSkillChanger*>(slots[4]);
    CUser* user = reinterpret_cast<CUser*>(slots[5]);

    Quest const* quest = G_CDataManager()->find_quest(static_cast<int>(questIdx));
    if (quest == 0)
        return true;
    if (quest->m_fieldB8 != 0 && quest->m_fieldB8 != 0x19)
        return true;

    std::vector<RewardItemInfo>* rewards =
        user->getUserQuestRewardItem(quest, user);   // ORIG _ZN5CUser22…PKS_
    for (std::vector<RewardItemInfo>::const_iterator it = rewards->begin();
         it != rewards->end(); ++it)
    {
        const int itemIdx = it->m_itemIdx;   // +0x00
        const int count = it->m_count;       // +0x04
        self->_addSPRelatedItem(spA, spB, itemIdx, count);
        self->_addSFPRelatedItem(sfpA, sfpB, itemIdx, count);
    }
    return true;
}

// ============================================================================
// ORIG T 0x0860a680 _CountQuestSPBook(const CUser*,int&,int&,int&,int&)
// 经 UserQuest+0x04 的 CQuestClear 子对象遍历已完成任务并回调统计。
// ============================================================================
void CSkillChanger::_CountQuestSPBook(const CUser* user, int& a, int& b,
                                      int& c, int& d)
{
    // ORIG 无槽：getCurCharacQuestR 结果直存 eax、+0x4 经 lea 内联（P0/P183）。
    (&user->getCurCharacQuestR()->m_cleared)
        ->enumQuestClear(&_fnEnumQuestClear, &a, &b, &c, &d,
                         static_cast<CSkillChanger*>(this),
                         const_cast<CUser*>(user));
}

// ============================================================================
// CGMAccounts 三缺失成员
// ============================================================================

// ORIG T 0x081093ee appendGM(unsigned,unsigned)：恒返回 0。
int CGMAccounts::appendGM(unsigned int /*mid*/, unsigned int /*type*/)
{
    int dead_frame;                        // ORIG sub $0x10,%esp + leave（0x10 无读写帧）
    (void)&dead_frame;                     // P31 dead local 帧差对齐（探针实证取址保槽）
    return 0;
}

// ORIG T 0x08109524 AppendGM_Sys(unsigned,char)：入列 {mid,(int)(signed char)}
// 并记 "./log/Init" 文件日志 "GM List Add mid:%s"。
void CGMAccounts::AppendGM_Sys(unsigned int mid, char type)
{
    // ORIG：POD 直接赋值（stGMInfo_t 无用户 ctor，ORIG nm 无 C1 符号）。
    CGMAccounts::stGMInfo_t info = {};   // ORIG 零填 -0x18/-0x14 两槽后覆写
    info.mid = mid;
    info.type = static_cast<unsigned int>(static_cast<int>(type));
    m_gmList.push_back(info);

    // ORIG NumberToString → %ebx 跨 CMyFileLog ctor 保活（P44/P24 即时求值形）。
    // [b28d-41 判词·N82/N18a] DIFF：残差 (a) NumberToString 结果 ORIG 升格 %ebx
    // 跨 ctor 保活，4.4 恒落槽 -0xc；(b) 帧 0x48 vs 0x34、info 槽 -0x14/-0x10 vs
    // ORIG -0x18/-0x14。本轮加试 P226 裸语句表达式 `({ NumberToString(mid,0); })`
    // ——4.4 仍落槽（同 b27 判词），内联实参形令 ctor 先于 NumberToString（次序反）
    // 亦不可用 ⇒ 维持 N82/N18a 封顶。
    char* midStr = ({ NumberToString(mid, 0); });
    CMyFileLog log("AppendGM_Sys", 0xcd);
    log("./log/Init", "GM List Add mid:%s", midStr);
}

}  // namespace WongWork
