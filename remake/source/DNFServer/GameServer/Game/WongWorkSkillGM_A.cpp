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
//   +0x90 m_state.m_initialized；+0x91 保留字节
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
// 定义在 SkillSlot.cpp（占位 {0}，与 ORIG D 0x093acdc0/0x093ace00 初始数据待对齐）。
extern int g_nStatUpItemIdx[];
extern int g_nElixerAbility[];

// ---- CUser::getUserQuestRewardItem 桥 ------------------------------------
// ORIG _ZN5CUser22getUserQuestRewardItemEPK5QuestPKS_ @0x0866e914：
// ORIG 反汇编/反编译显示其返回 quest 内嵌奖励 vector 地址（quest+0xec 已清除 /
// quest+0xc4 未清除），调用方直接作为 std::vector<RewardItemInfo> const* 使用
// （随后 begin/end/operator*/operator++/operator!= 全部为标准 const_iterator
// 弱符号实例，见 nm W 符号）。该静态成员声明归宿为 CUser.h（本批禁改），先以
// asm 标签桥接，待 CUser 域落地权威声明后可删除此行改直调。
std::vector<RewardItemInfo> const* bridge_getUserQuestRewardItem(
    Quest const* quest, CUser const* user)
    __asm__("_ZN5CUser22getUserQuestRewardItemEPK5QuestPKS_");

// ---- union 无具名区段访问 helper（ORIG 偏移证据见文件头注释） -------------
inline int* needElixerCountPtr(CSkillChanger* self)   // this+0x48
{
    return reinterpret_cast<int*>(reinterpret_cast<char*>(self) + 0x48);
}
inline int* needElixerAbilityPtr(CSkillChanger* self) // this+0x6c
{
    return reinterpret_cast<int*>(reinterpret_cast<char*>(self) + 0x6c);
}
}  // namespace WongWork

namespace WongWork
{

// ============================================================================
// ORIG T 0x08609d10 CheckCondition(CUser const*)
// exp < get_level_exp(level) || get_level_exp(level+1) < exp → 不可执行
// ============================================================================
bool CSkillChanger::CheckCondition(const CUser* user)
{
    const int level = user->get_charac_level();
    unsigned int levelExp =
        static_cast<unsigned int>(G_CDataManager()->get_level_exp(level));
    unsigned int nextLevelExp =
        static_cast<unsigned int>(G_CDataManager()->get_level_exp(level + 1));
    unsigned int curExp = static_cast<unsigned int>(user->getCurCharacExp());
    return !((curExp < levelExp) || (nextLevelExp < curExp));
}
// ============================================================================
// ORIG T 0x08609d9c CountElixer(CUser*)：重置四组数据（8/组首）、置就绪标志，
// 统计用户物品并应用双树 kind 数据。
// ============================================================================
void CSkillChanger::CountElixer(CUser* user)
{
    memset(reinterpret_cast<char*>(this) + 0x00, 0, 0x24);
    memset(reinterpret_cast<char*>(this) + 0x24, 0, 0x24);
    memset(reinterpret_cast<char*>(this) + 0x48, 0, 0x24);
    memset(reinterpret_cast<char*>(this) + 0x6c, 0, 0x24);
    m_state.m_kindState0 = 8;
    m_state.m_kindState1 = 8;
    m_state.m_kindState2 = 8;
    m_state.m_kindState3 = 8;
    m_state.m_initialized = 1;                     // ORIG: this[0x91] = 1
    _GetUserItemCount(user);
    _ApplyUserData(user, 0);
    _ApplyUserData(user, 1);
}

// ORIG W 0x0869780e GetElixerCount()：返回第一组计数数组（即 this）。
int* CSkillChanger::GetElixerCount()
{
    return reinterpret_cast<int*>(this);
}

// ORIG W 0x08697816 GetNeedElixerCount()：返回 this+0x48。
int* CSkillChanger::GetNeedElixerCount()
{
    return needElixerCountPtr(this);
}

// ORIG W 0x08697822 GetNeedElixerAbility()：返回 this+0x6c。
int* CSkillChanger::GetNeedElixerAbility()
{
    return needElixerAbilityPtr(this);
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
        for (int i = 1; i <= g_nStatUpItemIdx[0]; i++)
        {
            if (g_nStatUpItemIdx[i] == itemIdx)
            {
                int* needCount = needElixerCountPtr(this);
                needCount[i] = needCount[i] + count;
                int* needAbility = needElixerAbilityPtr(this);
                needAbility[i] = g_nElixerAbility[i];
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

    std::vector<RewardItemInfo> const* rewards =
        bridge_getUserQuestRewardItem(quest, user);
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
    UserQuest* quests = user->getCurCharacQuestR();
    CQuestClear* cleared =
        reinterpret_cast<CQuestClear*>(reinterpret_cast<char*>(quests) + 4);
    cleared->enumQuestClear(&_fnEnumQuestClear, &a, &b, &c, &d,
                            static_cast<CSkillChanger*>(this),
                            const_cast<CUser*>(user));
}

// ============================================================================
// CGMAccounts 三缺失成员
// ============================================================================

// ORIG T 0x0810933c loadGMAccounts(char const*)：恒返回 1。
int CGMAccounts::loadGMAccounts(char const* /*path*/)
{
    return 1;
}

// ORIG T 0x081093ee appendGM(unsigned,unsigned)：恒返回 0。
int CGMAccounts::appendGM(unsigned int /*mid*/, unsigned int /*type*/)
{
    return 0;
}

// ORIG T 0x08109524 AppendGM_Sys(unsigned,char)：入列 {mid,(int)(signed char)}
// 并记 "./log/Init" 文件日志 "GM List Add mid:%s"。
void CGMAccounts::AppendGM_Sys(unsigned int mid, char type)
{
    stGMInfo_t info(mid, static_cast<unsigned int>(static_cast<int>(type)));
    m_gmList.push_back(info);

    char* midStr = NumberToString(mid, 0);   // ORIG radix 参数为 0
    CMyFileLog log("AppendGM_Sys", 0xcd);
    log("./log/Init", "GM List Add mid:%s", midStr);
}

}  // namespace WongWork
