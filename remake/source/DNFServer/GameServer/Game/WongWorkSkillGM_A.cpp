// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 WongWorkSkillGM_A/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <string.h>

#include "SkillSlot.h"          // CSkillChanger 类定义 / CUser / NumberToString / G_CDataManager
// [Lane37CUser 2026-09-10 解环] SkillSlot.h 已把 `#include "CUser.h"` 改为前向声明
//   （解 CUser.h ⇄ SkillSlot.h 环）⇒ 依赖传递获得 CUser 完整定义的 TU 需自行直补。
#include "CUser.h"              // CUser 完整定义（本 TU 解引用 CUser 成员）
#include "Quest.h"              // Quest（+0xb8 字段）/ RewardItemInfo 完整型
#include "QuestClear.h"         // WongWork::CQuestClear::enumQuestClear 回调链
#include "CDataManager.h"       // find_quest / get_level_exp
#include "CMyFileLog.h"         // AppendGM_Sys 日志
#include "CGuildServerProxy.h"  // WongWork::CGMAccounts / stGMInfo_t 权威声明

namespace WongWork
{

// ORIG T 0x0810933c loadGMAccounts(char const*)：恒返回 1。
#include "WongWorkSkillGM_A/loadGMAccounts.inc"
{
    return 1;
}

// ============================================================================
// ORIG T 0x08609d10 CheckCondition(CUser const*)
// exp < get_level_exp(level) || get_level_exp(level+1) < exp → 不可执行
// ============================================================================
#include "WongWorkSkillGM_A/CheckCondition.inc"
{
    // ORIG 0x08609d10：get_charac_level ×2、G_CDataManager ×2、
    // getCurCharacExp 两条件各取一次（P48 无 CSE）；结果槽 -0xc/-0x10。
    unsigned int levelExp = static_cast<unsigned int>(
        G_CDataManager()->get_level_exp(user->get_charac_level()));
    unsigned int nextLevelExp = static_cast<unsigned int>(
        G_CDataManager()->get_level_exp(user->get_charac_level() + 1));
    // 双物化岛：bVar1 = (c1||c2) 经 mov$1/mov$0，再 `return !bVar1` 二次物化
    // （+0x76 test %al;je / +0x7a mov$0 / +0x81 mov$1）。
    // N96 清绑：原 `register bool b __asm__("al")` 已删。ORIG b 无槽 ⇒ 按
    // P226扩展/P316① 用语句表达式包住短路条件。清绑前后 44=44、与 ORIG 逐条全等
    // （仅 .o 重定位地址差）⇒ 残差 0；具名 bool 局部形为 45（+1 槽物化）。
    if (({ (user->getCurCharacExp() < levelExp)
            || (nextLevelExp < user->getCurCharacExp()); }))
        return false;
    return true;
}
// ============================================================================
// ORIG T 0x08609d9c CountElixer(CUser*)：重置四组数据（8/组首）、置就绪标志，
// 统计用户物品并应用双树 kind 数据。
// ============================================================================
#include "WongWorkSkillGM_A/CountElixer.inc"
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
#include "WongWorkSkillGM_A/GetElixerCount.inc"
{
    return m_data.m_elixerCount;
}

// ORIG W 0x08697816 GetNeedElixerCount()：返回 this+0x48。
#include "WongWorkSkillGM_A/GetNeedElixerCount.inc"
{
    return m_data.m_needElixerCount;
}

// ORIG W 0x08697822 GetNeedElixerAbility()：返回 this+0x6c。
#include "WongWorkSkillGM_A/GetNeedElixerAbility.inc"
{
    return m_data.m_needElixerAbility;
}

// ============================================================================
// ORIG T 0x0860a868 _addSPRelatedItem(int*,int*,int,int)
// 0x407 → *a += count；0x40e → *b += count；
// 其余且 initialized：命中 SP 书索引时 needCount[i] += count、
// needAbility[i] = g_nElixerAbility[i]（直接赋值，与 ORIG store 一致）。
// ============================================================================
#include "WongWorkSkillGM_A/_addSPRelatedItem.inc"
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
#include "WongWorkSkillGM_A/_addSFPRelatedItem.inc"
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
// [P3S3 定稿] N82 已解（帧 0x64→0x54、槽与符号全对齐）：
//   ① rewards 提为 `const std::vector<...>*`：ORIG 0x0860a7aa/0x0860a7bf 的
//      begin/end 目标 = `_ZNKSt6vectorI14RewardItemInfoSaIS0_EE5beginEv/3endEv`
//      （const 版）且**无** iterator→const_iterator 转换 ctor 调用 ⇒ 我方旧形
//      多出 `__normal_iterator<const T*>::C1<T*>` + 槽 -0x3c（帧 0x64）。
//   ② end 缓存在 for-init 双声明：ORIG 0x0860a7bf 的 end() 只调一次
//      （0x0860a843 条件处仅 `ne`）⇒ it -0x34 / end -0x38 与 ORIG 逐槽一致，
//      `ne` 符号亦降为 2 模板参 const 版（ORIG 0x083e5e57 同名）。
//   残余 N107（唯一 diff，gap +5）：ORIG **六组**形参自增 `mov 0xc(%ebp),%eax;
//   lea 0x4(%eax),%edx; mov %edx,0xc(%ebp)`（2 条/组，含 0x0860a727 第 6 组读
//   user 后亦自增 ⇒ 我方同步补第 6 次 `args += 4`，our 114→115，gap +6→+5）vs
//   4.4 恒 RMW 折叠 `addl $0x4,0xc(%ebp)`（1 条/组）。已实测六形均不可复制 lea
//   形：`args += 4` / `args = args + 4` / `args = &args[4]` /
//   `(char*)((int)args+4)` / `(char*)((int**)args+1)` 全产 addl；
//   `(char*)((unsigned int)args + 4u)`（P5 类型轴旋钮③）确能解除 RMW 折叠但产
//   `add $0x4,%eax; mov %eax,0xc(%ebp); mov 0xc(%ebp),%eax`（多 1 条 reload）
//   ⇒ 劣化（our 123、gap -3），已回滚。判 N 封顶（4.1↔4.4 固有）。
//   历史：[A28 round5] rewards 预清两句式（ORIG 0x0860a781 `movl $0x0,-0x14`，P198①）。
// ============================================================================
#include "WongWorkSkillGM_A/_fnEnumQuestClear.inc"
{
    // b28d-L3 复测（P0/P183）：ORIG 0x0860a6da 为**形参自增指针**形 —— 每个槽
    //   `mov 0xc(%ebp),%eax; lea 0x4(%eax),%edx; mov %edx,0xc(%ebp); mov (%eax),%eax;
    //    mov %eax,-0x30…(%ebp)`（6 连，偏移步进 4）⇒ 直接对 args 做 `+= 4` 步进，
    //   不设 `slots` 中间层（旧形多一具名槽 -0x34 且读形为 `mov -0x34(%ebp),%eax;
    //   add $0x4,%eax`）。
    int* spA = *(int**)args;
    args += 4;
    int* spB = *(int**)args;
    args += 4;
    int* sfpA = *(int**)args;
    args += 4;
    int* sfpB = *(int**)args;
    args += 4;
    CSkillChanger* self = *reinterpret_cast<CSkillChanger**>(args);
    args += 4;
    CUser* user = *reinterpret_cast<CUser**>(args);
    args += 4;                            // ORIG 0x0860a727-0x0860a72d：第 6 组读后亦自增

    Quest const* quest = G_CDataManager()->find_quest(static_cast<int>(questIdx));
    if (quest == 0)
        return true;
    if (quest->m_fieldB8 != 0 && quest->m_fieldB8 != 0x19)
        return true;

    // ORIG 0x0860a781：`movl $0x0,-0x14(%ebp)` 预清后再 call（P198① 两句式）。
    // const 视图 + end 缓存：见顶部 [P3S3 定稿] ①②（ORIG 0x0860a7aa/0x0860a7bf）。
    const std::vector<RewardItemInfo>* rewards = 0;
    rewards = user->getUserQuestRewardItem(quest, user);   // ORIG _ZN5CUser22…PKS_
    for (std::vector<RewardItemInfo>::const_iterator it = rewards->begin(),
             end = rewards->end();
         it != end; ++it)
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
#include "WongWorkSkillGM_A/_CountQuestSPBook.inc"
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
#include "WongWorkSkillGM_A/appendGM.inc"
{
    int dead_frame;                        // ORIG sub $0x10,%esp + leave（0x10 无读写帧）
    (void)&dead_frame;                     // P31 dead local 帧差对齐（探针实证取址保槽）
    return 0;
}

// ORIG T 0x08109524 AppendGM_Sys(unsigned,char)：入列 {mid,(int)(signed char)}
// 并记 "./log/Init" 文件日志 "GM List Add mid:%s"。
#include "WongWorkSkillGM_A/AppendGM_Sys.inc"
{
    // ORIG：POD 直接赋值（stGMInfo_t 无用户 ctor，ORIG nm 无 C1 符号）。
    CGMAccounts::stGMInfo_t info = {};   // ORIG 零填 -0x18/-0x14 两槽后覆写
    info.mid = mid;
    info.type = static_cast<unsigned int>(static_cast<int>(type));
    m_gmList.push_back(info);

    // ORIG 0x08109524 段：`call NumberToString; mov %eax,%ebx`（ctor 前求值 + %ebx
    //   跨 CMyFileLog ctor 保活）⇒ P227 全表达式临时链形（具名 log 对象/语句表达式
    //   两形在 4.4 下把结果落槽 -0xc，多 1 store + 1 reload）。
    // ORIG 0x0810957b 立即数符号为 `_ZZN8WongWork11CGMAccounts12AppendGM_SysEjcE12__FUNCTION__`
    //   （P139：手写字面量不生成该局部静态符号 ⇒ 仅ORIG 差集）⇒ 传 __FUNCTION__。
    CMyFileLog(__FUNCTION__, 0xcd)("./log/Init", "GM List Add mid:%s",
                                   NumberToString(mid, 0));
}

}  // namespace WongWork
