// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CAchievement/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 成就 CAchievement 实现（G3-3 批次，2026-08-17）。
// 语义依据 docs/class_func_reports/CAchievement.md（Ghidra C + 汇编）+ ORIG
// 反汇编交叉核对；按用户规则优先具名成员访问，不做裸偏移。
#include <cstdio>
#include <cstring>
#include <ctime>
// [P338/P327] ORIG 本 TU 有 _GLOBAL__I__ZN12CAchievementC2Ev（体为
// ios_base::Init __ioinit 构造 + __cxa_atexit，帧 sub $0x18；ORIG 0x0828d813
// __static_initialization_and_destruction_0ii 仅含 __ioinit）⇒ 原码含
// <iostream>。补该 include 以发射 ORIG 实有的 __ioinit 与 GI 符号。
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include "CMailBoxHelper.h"   // WongWork::CMailBoxHelper 权威叶子头（替代本 TU 局部镜像）
#include "CAchievement.h"

#include "CDataManager.h"
#include "QuestClear.h"      // WongWork::CQuestClear（+4 偏移访问）
#include "CItem.h"
#include "CMissionList_Charac.h"
#include "CStreamGuard.h"
#include "CSystemTime.h"
#include "GlobalData.h"
#include "InterfacePacketBuf.h"
#include "MsgQueueMgr.h"
#include "Stream.h"
#include "StreamPool.h"
#include "PacketGuard.h"

// ============================================================================
// 本 TU 需要而其它 TU 提供的符号
// ============================================================================



class CExpandEquipslot;
class CInventory;
class UserQuest;

// 本 TU 的 CUser / CUserCharacInfo / CInventory / UserQuest 局部视图
// （真实定义在 CUser.h / CInventory.h，含与 CTitleBook 冲突的局部类；
//  按 CTitleBook.cpp 同款模式避免混入）。
class CUserCharacInfo
{
public:
    CInventory* getCurCharacInvenW();
    const CInventory* getCurCharacInvenR() const;
    int getCurCharacNo() const;
    int get_charac_level() const;
};

class CUser : public CUserCharacInfo
{
public:
    enum eSendTarget
    {
        eSendTarget_0 = 0,
        eSendTarget_1 = 1
    };

    unsigned int GetUID() const;
    int get_charac_no(int type);
    ENUM_SERVER_GROUP GetServerGroup() const;
    CExpandEquipslot* GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE type) const;
    CExpandEquipslot* GetCharacExpandDataR(ENUM_CHARAC_EXPAND_TYPE type) const;
    UserQuest* getCurCharacQuestR() const;
    void Send(PacketGuard& packet);
    void SendUpdateItem(eSendTarget target, ENUM_ITEMSPACE space, int slot);
};

class UserQuest
{
public:
    // b28c-L10：与权威 CUser.h 统一（Main 代落 UserQuest::isClearQuest bool→unsigned char，
    // P174 ORIG __PRETTY_FUNCTION__ 串；返回类型不入 mangled，符号不变）。
    unsigned char isClearQuest(int questIdx) const;
};

class CInventory
{
public:
    Inven_Item* GetInvenRef(int invenType, int slot);
    Inven_Item GetInvenSlot(int invenType, int slot) const;   // ORIG _ZNK10CInventory12GetInvenSlotEii
};



// cUserHistoryLog::AchievementComplete（CUser.h 权威 cUserHistoryLog 声明，
// CAchievement.cpp 使用本地最小视图；符号 _ZN15cUserHistoryLog19AchievementCompleteEi
// 由 cUserHistoryLog.cpp 提供，ORIG 0x08685762）
class cUserHistoryLog
{
public:
    void AchievementComplete(int idx);
};


// ---- CDataManager 称号表视图（+0xa788，与 CTitleBook.cpp 相同布局） ----
struct CTitleBookInfoRaw
{
    char m_pad0[0xc];                                        // +0x00
    std::vector<stTitleSection> m_sections;                  // +0x0c（0xa794）
    std::map<int, std::vector<stTitleElement> > m_elements;  // +0x18（0xa7a0）
    char m_pad34[0x8];                                       // +0x34
    std::vector<stAdjust> m_adjustList;                      // +0x3c（0xa7c4）
};

// CDataManager 称号表视图（+0xa788 起，与 CTitleBook.cpp 相同布局）。
// 宏展开为直接偏移访问：ORIG 汇编为 lea 0xa794/0xa7a0/0xa7c4(%eax)，
// 若用 static 函数在 O0 下会生成额外 call，导致助记符序列不一致。
#define GetTitleSections(mgr) \
    (*(std::vector<stTitleSection>*)((char*)(mgr) + 0xa794))
#define GetTitleElements(mgr) \
    (*(std::map<int, std::vector<stTitleElement> >*)((char*)(mgr) + 0xa7a0))
#define GetAdjustList(mgr) \
    (*(std::vector<stAdjust>*)((char*)(mgr) + 0xa7c4))

// Quest::m_postalReward（+0x9c）的逐字段视图：ORIG 成就邮件按
// {标题, 内容, 道具 idx, addInfo} 读取（Quest.h 的 PostalReward 未具名 +0x08）。
struct PostalRewardView
{
    std::string m_title;     // +0x00
    std::string m_contents;  // +0x04
    int m_itemIdx;           // +0x08
    int m_addInfo;           // +0x0c
};

// ============================================================================
// stAchievement / stAchievement_DBData
// ============================================================================

// [P327] ORIG 的 _GLOBAL__I_ 键名为 _ZN12CAchievementC2Ev（ORIG 0x0828d853），
// 且 ORIG C1/C2 地址 0x0828b614 早于 stAchievementC1 0x0828d870 ⇒ 原码把
// CAchievement ctor 定义在本 TU 最前。GI 键名 = TU 内第一个强符号外部链接
// 实体名；把本定义前置即对齐键名（只移函数，不改构造序）。
#include "CAchievement/CAchievement.inc"
{
    // ORIG 0x828b633：m_user 置零在 map 构造之前（初始化列表，
    // 按声明顺序先于 m_achievements）；m_field24/25 在函数体
    m_field24 = 0;
    m_field25 = 0;
}

// stAchievement::stAchievement()/isComplete()、stAchievement_DBData::
// stAchievement_DBData() 已按 ORIG W 绑定改为 CAchievement.h 类内 inline 定义
//（P233；外联定义会发射 T 强符号）。

// ============================================================================
// CAchievement
// ============================================================================

#include "CAchievement/dtor_CAchievement.inc"
{
}

// ---- 触发 / 校验 / 完成 ----

#include "CAchievement/setTrigger.inc"
                              unsigned short t1, unsigned short t2)
{
    Quest* quest = G_CDataManager()->find_quest((int)id);
    if (quest == 0)
    {
        _sendSetTrigger(id, 1);
        return;
    }
    if ((quest->m_type != 7) ||
        (m_user->get_charac_level() < quest->m_levelMin))
    {
        _sendSetTrigger(id, 1);
        return;
    }
    if (isClearedAchievement(id))
    {
        _sendSetTrigger(id, 2);
        return;
    }
    // ORIG 0x828b781：_verifyTrigger 返回值 xor 1 后 je +0x12c，
    // 即 verify==1 才走 _setTrigger 路径；verify==0 → _sendSetTrigger(id,2)。
    if (!_verifyTrigger(id, t0, t1, t2))
    {
        _sendSetTrigger(id, 2);
        return;
    }
    _setTrigger(id, t0, t1, t2);
    _sendSetTrigger(id, 0);
    _checkComplete(id);
}

// [b28d-solo-CAchievement AE] 修法：ORIG 0x0828b81e 为 `mov 0xc(%ebp),%eax` +
// 三次 `cmp $imm,%eax`
// （寄存器链，P286/P251/P261）⇒ 源形是 switch（if 链在 4.4 恒 mem 直比，P209）。
// ORIG cmp 顺序 198b→19c4→1983 且两个 level 块 tail-merge 到同一地址 +0x6c。
// level 极性按 ORIG `cmp $0x45; setle %al; test; je <mov $1>` ⇒
// 源形 `if (level <= 0x45) return 0; return 1;`（P56/P292）。
#include "CAchievement/_verifyTrigger.inc"
                                  unsigned short t1, unsigned short t2)
{
    switch (id)
    {
    case 0x19c4:
    {
        time_t now = (time_t)GlobalData::s_systemTime_.getCurSec();
        struct tm local;
        localtime_r(&now, &local);
        if (local.tm_hour < 0)
        {
            break;
        }
        if (local.tm_hour > 0x14)
        {
            break;
        }
        return 0;
    }
    case 0x198b:
    case 0x1983:
        if (m_user->get_charac_level() <= 0x45)
        {
            return 0;
        }
        break;
    }
    return 1;
}

#include "CAchievement/_setTrigger.inc"
                               unsigned short t1, unsigned short t2)
{
    std::map<unsigned int, stAchievement>::iterator it =
        m_achievements.find(id);
    // ORIG 0x828b8fd：已找到分支为 fall-through（je 跳到未找到块），故用 != end 先行
    if (it != m_achievements.end())
    {
        // ORIG 0x828b916：setae（cur>=t0）为 fall-through 的 sub 分支，zero 分支在跳转目标
        if (it->second.m_trigger0 >= t0)
        {
            it->second.m_trigger0 -= t0;
        }
        else
        {
            it->second.m_trigger0 = 0;
        }
        if (it->second.m_trigger1 >= t1)
        {
            it->second.m_trigger1 -= t1;
        }
        else
        {
            it->second.m_trigger1 = 0;
        }
        if (it->second.m_trigger2 >= t2)
        {
            it->second.m_trigger2 -= t2;
        }
        else
        {
            it->second.m_trigger2 = 0;
        }
    }
    else
    {
        stAchievement ach;
        ach.m_id = id;
        Quest* quest = G_CDataManager()->find_quest((int)id);
        if (quest == 0)
        {
            return;
        }
        // ORIG 0x828b9dd：get_init 直接写 stAchievement 临时对象字段
        if (!quest->get_init_achievement_trigger(ach.m_trigger0, ach.m_trigger1,
                                                 ach.m_trigger2))
        {
            _sendSetTrigger(id, 1);
            return;
        }
        // ORIG 0x828ba5a：与已找到分支相同，setae 编码（sub 分支 fall-through）
        if (ach.m_trigger0 >= t0)
        {
            ach.m_trigger0 -= t0;
        }
        else
        {
            ach.m_trigger0 = 0;
        }
        if (ach.m_trigger1 >= t1)
        {
            ach.m_trigger1 -= t1;
        }
        else
        {
            ach.m_trigger1 = 0;
        }
        if (ach.m_trigger2 >= t2)
        {
            ach.m_trigger2 -= t2;
        }
        else
        {
            ach.m_trigger2 = 0;
        }
        m_achievements.insert(std::make_pair(ach.m_id, ach));
    }
    alter();
}

#include "CAchievement/isClearedAchievement.inc"
{
    std::map<unsigned int, stAchievement>::iterator it =
        m_achievements.find(id);
    // ORIG 0x828cc37：found 分支为 fall-through（je 跳到 return 0）
    if (it != m_achievements.end())
    {
        return it->second.isComplete();
    }
    return 0;
}

#include "CAchievement/_checkComplete.inc"
{
    if (!isClearedAchievement(id))
    {
        return;
    }
    Quest* quest = G_CDataManager()->find_quest((int)id);
    if (quest == 0)
    {
        return;
    }
    ENUM_TITLE_BOOK_CATEGORY category = (ENUM_TITLE_BOOK_CATEGORY)0;
    int slot = 0;
    int titleIdx = quest->get_title_reward();
    if (titleIdx != 0)
    {
        CTitleBook::putTitleOnly(m_user, titleIdx, category, slot);
    }
    _sendEachAchieveReward(quest);
    _sendCompleteReward(quest, category, slot);
    _sendComplete(id, titleIdx, category, slot);
    // CUser 冻结头 +0x79700 即 cUserHistoryLog（本地最小视图；本 TU 混 CTitleBook
    // 冲突类不能并入 CUser.h，保持具名视图 + 文档化基址访问）
    ((cUserHistoryLog*)((char*)m_user + 0x79700))->AchievementComplete((int)id);
}

// [b28d-solo-CAchievement AE] 修法：删 book/inven 具名局部改匿名全表达式链
//（P0/P278；ORIG 0x0828cd6x 无这两槽，GetCharacExpandData/getCurCharacInvenR
//  结果寄存器直传）⇒ 帧 0xc0、push esi+ebx、全部槽号与 ORIG 逐条对齐。
// .o 口径显示 DIFF 仅因 call 重定位标签未解析（P264）。
#include "CAchievement/_forceClear.inc"
{
    Quest* quest = G_CDataManager()->find_quest((int)id);
    // ORIG 0x0828ce5c/0x0828ce69：`je +0x238` 与 `jne +0x23b` **落点不同**
    // （各带独立尾岛）⇒ 两条独立守卫（P133①），合并短路会共落点少一岛。
    if (quest == 0)
    {
        return;
    }
    if (quest->m_type != 7)
    {
        return;
    }
    int titleIdx = quest->get_title_reward();
    if (titleIdx == 0)
    {
        return;
    }
    // ORIG 0x828ccbb：isClearedAchievement 结果 xor 1 后 je 到已清除分支，
    // 未清除分支（重新初始化 trigger+发称号）为 fall-through，故源码用 !cleared 先行。
    if (!isClearedAchievement(id))
    {
        // ORIG 0x828cccc：stAchievement 临时对象 ctor 清零，get_init 覆盖 trigger 字段
        stAchievement ach;
        quest->get_init_achievement_trigger(ach.m_trigger0, ach.m_trigger1,
                                            ach.m_trigger2);
        _setTrigger(id, ach.m_trigger0, ach.m_trigger1, ach.m_trigger2);
        CTitleBook::putTitleOnly(m_user, titleIdx);
    }
    else
    {
        ENUM_TITLE_BOOK_CATEGORY category = (ENUM_TITLE_BOOK_CATEGORY)4;
        int slot = 0;
        m_user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)14);
        CTitleBook::_getFitSlot(titleIdx, category, slot);
        // b28d-solo P0/P278：ORIG 0x0828cd6x 无 book/inven 具名槽
        //（GetCharacExpandData 结果经 %edx 直传 getTitleSlotR；getCurCharacInvenR
        //  结果经 %eax 直传 GetInvenSlot），改匿名全表达式链。
        Inven_Item titleSlot =
            ((CTitleBook*)m_user->GetCharacExpandData(
                 (ENUM_CHARAC_EXPAND_TYPE)14))
                ->getTitleSlotR(category, slot);
        if (titleSlot.m_addInfo == 0)
        {
            // ORIG 0x828cdcc 调 _ZNK15CUserCharacInfo18getCurCharacInvenREv（只读引用）
            // [b28d-D33 P215] ORIG 0x0828cc56 段无 Inven_Item::C1Ev call
            // （p167 差集 EXTRA +1 C1Ev 实证）⇒ 原码是直接初始化，非「先声明后赋值」。
            Inven_Item invenItem = m_user->getCurCharacInvenR()->GetInvenSlot(0, 0xb);
            if ((invenItem.m_addInfo == 0) ||
                (invenItem.m_addInfo != titleIdx))
            {
                CTitleBook::putTitleOnly(m_user, titleIdx);
            }
        }
    }
    alter();
}

// [b28d-solo-CAchievement N104/P223 封顶，2026-09-09 复测] 修法同 _forceClear：
// ORIG 0x0828ce40 无 book/inven 具名槽（GetCharacExpandData/getCurCharacInvenW
// 结果寄存器直传），改匿名全表达式链后帧 0x40/槽号/push esi+ebx 全部对齐
//（P0/P278）。残差 = 尾部岛数 ORIG 5 个 `nop; jmp +0x248` + 尾裸 nop vs 4.4
// 4 个（invenItem==0 与 addInfo!=titleIdx 共落点，our 168）或 6 个（our 172，
// 见下）；本函数只此一处形态差 + call 重定位标签伪差（P264）。语义等价。
// 复测四形（fresh check_tu_routed，orig=170）：①&& 短路（当前）168/4 岛；
// ②嵌套 if 168/4 岛；③否定 goto+尾标签串 172/6 岛；④早返 172/6 岛。
// P223 负例成立：ORIG 末条件直落 epilogue（+0x248）属 4.1 布局，4.4 恒给
// 中继岛 ⇒ 无法同时给出「5 岛 + 尾裸 nop」，取指令数差最小的 && 形。
#include "CAchievement/_forceReset.inc"
{
    Quest* quest = G_CDataManager()->find_quest((int)id);
    if ((quest == 0) || (quest->m_type != 7))
    {
        return;
    }
    int titleIdx = quest->get_title_reward();
    if (titleIdx == 0)
    {
        return;
    }
    if (isClearedAchievement(id))
    {
        std::map<unsigned int, stAchievement>::iterator it =
            m_achievements.find(id);
        if (it != m_achievements.end())
        {
            // ORIG 0x828ceff：stAchievement 临时对象 ctor 清零，get_init 覆盖 trigger 字段
            stAchievement ach;
            if (!quest->get_init_achievement_trigger(ach.m_trigger0,
                                                     ach.m_trigger1,
                                                     ach.m_trigger2))
            {
                return;
            }
            it->second.m_trigger0 = ach.m_trigger0;
            it->second.m_trigger1 = ach.m_trigger1;
            it->second.m_trigger2 = ach.m_trigger2;
        }
        alter();
    }
    ENUM_TITLE_BOOK_CATEGORY category = (ENUM_TITLE_BOOK_CATEGORY)4;
    int slot = 0;
    m_user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)14);
    CTitleBook::_getFitSlot(titleIdx, category, slot);
    Inven_Item* titleSlot =
        ((CTitleBook*)m_user->GetCharacExpandData(
             (ENUM_CHARAC_EXPAND_TYPE)14))
            ->getTitleSlot(category, slot);
    if (titleSlot == 0)
    {
        return;
    }
    // ORIG 0x828cff8：addInfo!=0 分支为 fall-through（reset），addInfo==0 分支 je 跳库存检查
    if (titleSlot->m_addInfo != 0)
    {
        titleSlot->reset();
    }
    else
    {
        Inven_Item* invenItem =
            ((CInventory*)m_user->getCurCharacInvenW())->GetInvenRef(0, 0xb);
        // ORIG 0x0828d032 `je +0x247`（invenItem==0 落**独立** nop 岛）与
        // 0x0828d043 `jne +0x248`（addInfo 不等**直落** epilogue）落点不同 ⇒
        // 两条独立守卫 + 尾标签（P133①/P223 多源 goto 尾标签）：
        // 多源同目标 goto 由 4.4 逐条发射，不 jump-thread。
        if (invenItem == 0)
            goto force_reset_end;
        if (invenItem->m_addInfo == titleIdx)
        {
            invenItem->reset();
            m_user->SendUpdateItem(CUser::eSendTarget_1, ENUM_ITEMSPACE_0, 0xb);
        }
    }
force_reset_end:;
}

#include "CAchievement/_getTrigger.inc"
{
    stAchievement out;
    std::map<unsigned int, stAchievement>::iterator it =
        m_achievements.find(id);
    // ORIG 0x828bb72：found 分支 fall-through 复制 it->second 到 sret，
    // not-found 分支复制构造默认 out（双返回，不合并单出口）
    if (it != m_achievements.end())
    {
        return it->second;
    }
    return out;
}

// [b28d-solo-CAchievement N95/N82 封顶] ORIG 0x0828d092 非 static 成员：
// loadData 0x0828c956 压 this + in->m_saveTime 两实参，体内 0x8(%ebp)=this
// ⇒ 解引用 +8=m_user。旧 D68「static + self 参数」版把 m_saveTime 当 this
// 解引用 = 真 UB，已修（真语义 bug）。
// ①删 ach 具名局部：ORIG 每处重读 0x8(%ebp)（P0 表达式直用）；
// ②删 list 引用缓存：ORIG 内层条件与体各自 `it->second`（每轮 2 次
//   operator->() const，P157）；③empty()→size()!=0（ORIG 0x0808e1c0
//   `_ZNKSt6vectorIiSaIiEE4sizeEv`），quest 改 const Quest* 使
//   m_triggerData[0] 走 const operator[]（ORIG 0x080ea574）；
// ④special bool 改内联复合条件（ORIG mov $1/$0 物化岛）；
// ⑤删 level 缓存：ORIG 两臂各自 get_charac_level()（P48）；
// ⑥isClearMission 实参 0x83/0x87/0x19c0 用具名局部（ORIG -0x48/-0x44/-0x1c）。
// [残差，2026-09-09 复测] 首 continue 改正向包裹后 185=185、岛数与 ORIG 逐点
// 对齐（P223：平铺三 continue 时 4.4 把 achievementIdx==-1 也岛化，多 2 条）。
// 余下残差 = ①槽位前段整体深 8（it -0x40 vs -0x38，4.4 恒预留 outgoing 对齐区
// 的版本常数 N115/N126）②实参先于 this 装载（ORIG `mov -0x2c(%ebp),%ebx;
// call getCurCharacQuestR; add $0x4,%eax` vs 4.4 后读）③N107 成员偏移取址
// add-vs-lea。P277 具名 receiver 形实测更劣（槽再深 4）⇒ 保持当前形。
#include "CAchievement/_verifyAchievement.inc"
{
    // ORIG 0x828d09e：不缓存 elements 引用，begin 一次、end 每次循环重新取
    std::map<int, std::vector<stTitleElement> >::iterator it =
        GetTitleElements(G_CDataManager()).begin();
    unsigned int achievementIdx = 0;
    unsigned int clearQuestIdx = 0;
    // ORIG 0x0828d0bd..0x0828d0d2：-0x28/-0x24/-0x20 = 0,0,4 三个死局部（后续无读，
    // P171 死写入照抄；仅复现帧槽布局与 5 条初始化序列）。
    unsigned int unusedA = 0;
    unsigned int unusedB = 0;
    unsigned int unusedC = 4;
    for (; it != GetTitleElements(G_CDataManager()).end(); ++it)
    {
        for (unsigned int i = 0; i < it->second.size(); ++i)
        {
            achievementIdx = (unsigned int)it->second[i].m_achievementIdx;
            // ORIG 0x0828d111 `je +0x139` 直接落循环尾 addl（无岛）；另两个
            // continue 才各带独立 nop 岛（+0x135/+0x138）。平铺三 continue 在
            // 4.4 恒岛化三个 ⇒ 首条件改正向包裹，让 4.4 直落循环尾（P223）。
            if (achievementIdx != 0xffffffff)
            {
                const Quest* quest =
                    G_CDataManager()->find_quest((int)achievementIdx);
                if (quest == 0)
                {
                    continue;
                }
                if (!((quest->m_type == 7) && (quest->m_triggerType == 0xc) &&
                      (quest->m_triggerData.size() != 0)))
                {
                    continue;
                }
                clearQuestIdx = (unsigned int)quest->m_triggerData[0];
                // ORIG 0x0828d1ca：getCurCharacQuestR() + 4 为 WongWork::CQuestClear
                // 子对象（UserQuest 容器 +0 m_user / +4 CQuestClear），调 isClearedQuest。
                // [b28d-solo N95/N82 封顶] ORIG 实参先于 this 装载（`mov -0x2c(%ebp),%ebx;
                // call getCurCharacQuestR; add $0x4,%eax; mov %ebx,0x4(%esp)`）+ 槽基比
                // 我方浅 8（it -0x38 vs -0x40）；P277 具名 receiver 形实测更劣
                //（槽再深 4、内层 i 由 -0x14 漂到 -0x18），已回退。
                if (((WongWork::CQuestClear*)((unsigned int)m_user
                                                  ->getCurCharacQuestR() +
                                              4))
                        ->isClearedQuest(clearQuestIdx))
                {
                    _forceClear(achievementIdx);
                }
            }
        }
    }

    if (m_user->get_charac_level() <= 0x45)
    {
        const unsigned int resetIds[2] = { 0x1983, 0x198b };
        for (int i = 0; i <= 1; ++i)
        {
            _forceReset(resetIds[i]);
        }
    }
    else if (m_user->get_charac_level() > 0x45)
    {
        _forceClear(0x1983);
    }

    int missionA = 0x83;
    int missionB = 0x87;
    unsigned int clearId = 0x19c0;
    CMissionList_Charac* missionList = (CMissionList_Charac*)
        m_user->GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)8);
    if (missionList->isClearMission(missionA) ||
        missionList->isClearMission(missionB))
    {
        _forceClear(clearId);
    }
}

// [2026-09-09 复测] 真 bug 已修（find_quest 实参 questIdx→achievementIdx，
// ORIG 0x0828ca18 压 -0x18=m_achievementIdx）；titleIdx 内联进 putTitleOnly
// 实参位（P0，ORIG 结果留 %eax 直压）；补 -0x1c 死局部（P171）；声明序改
// questIdx/unused/achievementIdx/slot/category 后 168=168 且 6 条初始化与
// ORIG 槽值/发射序全等（questIdx@-0x20、死@-0x1c、achievementIdx@-0x18、
// slot@-0x24、category@-0x28、i@-0x14；&category/&slot 实参槽序经 0x0828cbb6
// 段核对）。余下残差仅 N7 寄存器角色（isClearedQuest/find_quest 的 ebx↔edx
// 缓存互换、movzwl 目标寄存器）+ N107 取址形。
#include "CAchievement/adjust.inc"
{
    // ORIG 0x828c97d：questIdx/achievementIdx 循环外预初始化（movl $0 序列）
    unsigned int questIdx = 0;
    // ORIG 0x828c984：-0x1c 死局部（P171 死写入照抄，仅复现初始化序列）。
    unsigned int unusedAdjust = 0;
    unsigned int achievementIdx = 0;
    // ORIG 0x0828cbb6..0x0828cbc0 实参槽序：&category 落 0x8(%esp)=槽 -0x28、
    // &slot 落 0xc(%esp)=槽 -0x24 ⇒ 声明序 slot 先于 category。
    int slot = 0;
    ENUM_TITLE_BOOK_CATEGORY category = (ENUM_TITLE_BOOK_CATEGORY)0;
    // ORIG 0x828c97d：不缓存 adjustList 局部引用（每次循环重新取），
    // isClearQuest 通过才进入（嵌套 if，非 continue 前置）
    for (unsigned int i = 0; i < GetAdjustList(G_CDataManager()).size(); ++i)
    {
        questIdx = GetAdjustList(G_CDataManager())[i].m_questIdx;
        achievementIdx =
            GetAdjustList(G_CDataManager())[i].m_achievementIdx;
        // ORIG 0x0828ca5x：getCurCharacQuestR() 结果 `add $0x4,%eax` 后直接调
        // WongWork::CQuestClear::isClearedQuest（bool 返回 ⇒ 调用点 test/je 无
        // setne；UserQuest::isClearQuest 包装是另一符号，p167 会记 MISSING）。
        // ORIG 0x0828ca5b：`mov -0x20(%ebp),%ebx` 在 receiver call **之前**物化
        // 并跨 call 保活（P52 callee-saved 独占形）。[2026-09-10 复测] 语句表达式
        // `({ questIdx; })` 形被 4.4 前端折叠（仍 `mov -0x20(%ebp),%edx`）⇒ 无效；
        // 残余 = N7 实参保活寄存器/时刻差。
        if (((WongWork::CQuestClear*)((unsigned int)m_user->getCurCharacQuestR() + 4))
                ->isClearedQuest(questIdx))
        {
            // [真 bug 修复] ORIG 0x0828ca18 压栈的是 -0x18(%ebp)=m_achievementIdx
            // （stAdjust +0x08），不是 isClearedQuest 用的 -0x20(%ebp)=m_questIdx
            // （+0x00）。旧译文用 questIdx 查找成就 quest = 查找键用错。
            Quest* quest =
                G_CDataManager()->find_quest((int)achievementIdx);
            if ((quest == 0) || (quest->m_type != 7))
            {
                continue;
            }
            // ORIG 0x828ca49：stAchievement 临时对象 ctor 清零，get_init 覆盖字段
            stAchievement ach;
            quest->get_init_achievement_trigger(ach.m_trigger0, ach.m_trigger1,
                                                ach.m_trigger2);
            _setTrigger(achievementIdx, ach.m_trigger0, ach.m_trigger1,
                        ach.m_trigger2);
            // ORIG 0x0828cab5..0x0828cacd：get_title_reward 结果留 %eax 直压
            // 0x4(%esp)（无具名 titleIdx 槽往返，P0 右值直传）。
            CTitleBook::putTitleOnly(m_user, quest->get_title_reward(), category,
                                     slot);
        }
    }

    if (m_user->get_charac_level() == 0x46)
    {
        Quest* quest = G_CDataManager()->find_quest(0x1983);
        if (quest == 0)
        {
            return;
        }
        // ORIG 0x828cb43：stAchievement 临时对象 ctor 清零，get_init 覆盖字段
        stAchievement ach;
        quest->get_init_achievement_trigger(ach.m_trigger0, ach.m_trigger1,
                                            ach.m_trigger2);
        _setTrigger(0x1983, ach.m_trigger0, ach.m_trigger1, ach.m_trigger2);
        // ORIG 0x0828cbab..0x0828cbc4：同上，结果直压实参槽。
        CTitleBook::putTitleOnly(m_user, quest->get_title_reward(), category,
                                 slot);
    }
    alter();
}

// ---- 发送 / 存档 ----

// 【N7 封顶】b26 lane19：虚调用 callee 寄存器 %edx↔%eax 互换（121 条逐条
// 长度/助记符全等仅寄存器名差），临时/引用/限定名不可翻。
#include "CAchievement/sendList.inc"
{
    stAchievement_DBData data;
    getData((char*)&data);

    PacketGuard packet;
    packet.put_header(0, 0x166);
    packet.put_int(data.m_count);
    for (int i = 0; i < data.m_count; ++i)
    {
        packet.put_int((int)data.m_entries[i].m_id);
        packet.put_short((int)data.m_entries[i].m_trigger0);
        packet.put_short((int)data.m_entries[i].m_trigger1);
        packet.put_short((int)data.m_entries[i].m_trigger2);
    }
    packet.finalize(true);
    m_user->Send(packet);
}

#include "CAchievement/_sendSetTrigger.inc"
{
    stAchievement trig = _getTrigger(id);

    PacketGuard packet;
    packet.put_header(1, 0x1a1);
    // ORIG 0x828c2a2：cmpb flag; je 跳到 flag==0 分支，flag!=0 分支为 fall-through
    if (flag != 0)
    {
        packet.put_byte(0);
        packet.put_byte((int)flag);
        packet.put_int((int)trig.m_id);
    }
    else
    {
        packet.put_byte(1);
        packet.put_int((int)trig.m_id);
        packet.put_short((int)trig.m_trigger0);
        packet.put_short((int)trig.m_trigger1);
        packet.put_short((int)trig.m_trigger2);
    }
    packet.finalize(true);
    m_user->Send(packet);
}

#include "CAchievement/_sendComplete.inc"
                                 ENUM_TITLE_BOOK_CATEGORY category, int slot)
{
    stAchievement trig = _getTrigger(id);

    PacketGuard packet;
    packet.put_header(0, 0x167);
    packet.put_int((int)trig.m_id);
    packet.put_int((int)category);
    packet.put_int(slot);
    packet.put_int(titleIdx);
    // ORIG 0x828c436：GetCharacExpandData 结果不缓存局部变量（edx 直接传 getTitleSlotR）
    Inven_Item titleSlot =
        ((CTitleBook*)m_user->GetCharacExpandData(
             (ENUM_CHARAC_EXPAND_TYPE)14))
            ->getTitleSlotR(category, slot);
    packet.put_short((int)titleSlot.m_fieldb);
    packet.finalize(true);
    m_user->Send(packet);
}

// [b28d-solo-CAchievement AE] 修法：①删 postal 具名局部（P0）②sectionIdx int 化
//（P218：size_t 转换中转触发 ORIG 的索引先求值 `mov -0x10(%ebp),%ebx` + add 形）。
// 帧 0x70、section/reward/字符串临时槽号全对齐；.o 口径 DIFF 为重定位伪差（P264）。
#include "CAchievement/_sendCompleteReward.inc"
                                       ENUM_TITLE_BOOK_CATEGORY category,
                                       int slot)
{
    // b28d-solo P218：ORIG 0x0828bf3x 先 `mov -0x10(%ebp),%ebx`（索引先求值）
    // 再 call G_CDataManager ⇒ 下标局部 int 化触发 size_t 转换中转（RHS 值先）。
    int sectionIdx = (int)_getSectionIdx(category, slot);
    if (sectionIdx == -1)
    {
        return;
    }
    // ORIG 0x828bf54：stTitleSection 复制构造到局部（非引用）
    stTitleSection section =
        GetTitleSections(G_CDataManager())[sectionIdx];
    stTitleRewardBasis* reward = _getPostalReward(&section);
    if (reward == 0)
    {
        return;
    }
    // 邮件字段位于 stRewardPostal（+0x0c..+0x1c）；基类仅承载 type 判定。
    // b28d-solo P0：ORIG 0x0828c0xx 无 postal 具名槽（每次 `mov -0xc(%ebp),%eax`
    // 重读 reward 后单 disp 取字段），删缓存局部以消多余 store 与 4B 帧。
    if (_getTabComplete(&section, category))
    {
        _sendPostal(static_cast<stRewardPostal*>(reward)->m_itemIdx,
                    static_cast<stRewardPostal*>(reward)->m_addInfo,
                    static_cast<stRewardPostal*>(reward)->m_sender,
                    static_cast<stRewardPostal*>(reward)->m_subject);
    }
}

#include "CAchievement/_sendEachAchieveReward.inc"
{
    // [b28d-solo-CAchievement AE] 修法：early-return 改正向包裹（P257）⇒ 消 4.4
    // EH 尾块多出的 jmp+nop（早返形尾部恒 +jmp+nop）。ORIG 0x0828c086 单 disp
    // `mov 0xa4(%eax),%eax` 直访 Quest::m_postalReward 具名成员。
    if (quest->m_postalReward.m_itemIdx != 0)
    {
        _sendPostal(quest->m_postalReward.m_itemIdx,
                    quest->m_postalReward.m_addInfo,
                    quest->m_postalReward.m_title,
                    quest->m_postalReward.m_contents);
    }
}

// [b28d-solo-CAchievement N7/N109 封顶] 三保活值 callee-saved 角色互换
//（ORIG GetServerGroup→edi、subject.c_str→esi、getCurCharacNo→ebx vs 4.4
//  →esi/→ebx/→edi；74 条长度/助记符全等，无虚调用，实参求值序相同）。
// P248 三档类型旋钮不适用（无 store 序/类型差）；本 TU 已路由 4.4.6 后端仍不翻。
// ORIG 0x0828c152。
#include "CAchievement/_sendPostal.inc"
                               std::string subject)
{
    Inven_Item item;
    CItem* pItem = G_CDataManager()->find_item(itemIdx);
    if (pItem == 0)
    {
        return;
    }
    item.m_addInfo = itemIdx;
    pItem->make_item(item);
    item.set_add_info(addInfo);
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        sender.c_str(), item, 0, (unsigned int)m_user->getCurCharacNo(),
        subject.c_str(), (int)subject.size(), 0, m_user->GetServerGroup(),
        false, false);
}

#include "CAchievement/_gm_init.inc"
{
    for (std::map<unsigned int, stAchievement>::iterator it =
             m_achievements.begin();
         it != m_achievements.end(); ++it)
    {
        Quest* quest = G_CDataManager()->find_quest((int)it->second.m_id);
        if (quest == 0)
        {
            break;
        }
        if (!quest->get_init_achievement_trigger(it->second.m_trigger0,
                                                 it->second.m_trigger1,
                                                 it->second.m_trigger2))
        {
            break;
        }
        _sendSetTrigger(it->second.m_id, 0);
    }
    alter();
}

// [2026-09-09 复测 AE] 旧 N78 判词作废：ORIG 0x0828d450 的两处 memset(0x400)
// 只有**循环前**那次被 4.1 内联为 rep stos（0x0828d467..0x47b），循环内那次
// 仍是 call memset（0x0828d70c）。修法（P328⑨）：把 `char buf[0x400];` +
// 显式 memset 改为声明即初始化 `char buf[0x400] = {0};`，并把声明放到
// `stAchievement ach;` **之后**以保持 ORIG 的「ach C1 → buf 清零」发射序；
// 循环内 memset 不动。改后 194=194，本件由 DIFF 转 AE（槽/寄存器全对齐）。
#include "CAchievement/_gm_report.inc"
{
    // ORIG 0x828d45c：stAchievement 临时对象循环外构造（get_init 直接写其 trigger 字段）
    stAchievement ach;
    // ORIG 0x828d467..0x828d47b：ach 构造之后 rep stos 清零 buf（P328⑨：
    // 初始化列表形才内联 rep stos；显式 memset 在 4.4 恒退化为 call）。
    // 循环内的 memset 在 ORIG 仍是 call（0x0828d70c），故只改首清。
    char buf[0x400] = { 0 };
    for (int idx = 0x1965; (int)idx <= 0x1bbb; ++idx)
    {
        // ORIG 0x828d489：QuestList::find_quest 死调用（结果丢弃）
        G_CDataManager()->m_questList->find_quest(idx);
        Quest* quest = G_CDataManager()->find_quest((int)idx);
        if ((quest == 0) || (quest->m_type != 7))
        {
            continue;
        }
        quest->get_init_achievement_trigger(ach.m_trigger0, ach.m_trigger1,
                                            ach.m_trigger2);
        std::map<unsigned int, stAchievement>::iterator it =
            m_achievements.find(idx);
        // b28d-solo P167：ORIG 调 _Rb_tree_iterator::ne（!=）；写 == 生 eq 且
        // p167 记 MISSING ne + EXTRA eq。改 != 并交换两臂。
        if (it != m_achievements.end())
        {
            std::sprintf(buf,
                         "%d\t\"%s\"\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
                         quest->m_index, quest->m_name.c_str(),
                         quest->get_title_reward(), (int)ach.m_trigger0,
                         (int)ach.m_trigger1, (int)ach.m_trigger2,
                         (int)it->second.m_trigger0,
                         (int)it->second.m_trigger1,
                         (int)it->second.m_trigger2);
        }
        else
        {
            std::sprintf(buf,
                         "%d\t\"%s\"\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
                         quest->m_index, quest->m_name.c_str(),
                         quest->get_title_reward(), (int)ach.m_trigger0,
                         (int)ach.m_trigger1, (int)ach.m_trigger2,
                         (int)ach.m_trigger0, (int)ach.m_trigger1,
                         (int)ach.m_trigger2);
        }
        out += buf;
        std::memset(buf, 0, sizeof(buf));
    }
}

#include "CAchievement/_gm_clearAll.inc"
{
    // ORIG 0x828d73f：stAchievement 临时对象循环外构造（get_init 直接写其 trigger 字段）
    stAchievement ach;
    for (int idx = 0x1965; (int)idx <= 0x1bbb; ++idx)
    {
        // ORIG 0x828d756：QuestList::find_quest 死调用（结果丢弃）
        G_CDataManager()->m_questList->find_quest(idx);
        Quest* quest = G_CDataManager()->find_quest((int)idx);
        if ((quest == 0) || (quest->m_type != 7))
        {
            continue;
        }
        quest->get_init_achievement_trigger(ach.m_trigger0, ach.m_trigger1,
                                            ach.m_trigger2);
        setTrigger(idx, ach.m_trigger0, ach.m_trigger1, ach.m_trigger2);
    }
}

#include "CAchievement/_reset.inc"
{
    m_user = 0;
    m_achievements.clear();
    m_field24 = 0;
    m_field25 = 0;
}

#include "CAchievement/_saveData.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("Achievement.cpp", 0x1d4), true);
    **guard << 0x286;
    **guard << (int)pUser->GetUID();
    stAchievement_DBSaveData* pData =
        guard->GetInBuffer<stAchievement_DBSaveData>();
    // b28c-L10：ORIG 0x828c805 `cmpl pData; jne <成功块>`——空值分支走 fall-through
    // (`mov $0,%ebx; jmp end`)，成功块在后跳走。`if(pData!=0){…return 1;}return 0;`
    // 被 4.4 反装为 `je <失败块>`；早期返回形 `if(pData==0)return 0; … return 1;`
    // 复现 ORIG 极性（P59/P16）。
    if (pData == 0)
        return 0;
    std::memset(pData, 0, sizeof(stAchievement_DBSaveData));
    getData((char*)&pData->m_data);
    pData->m_characNo = pUser->get_charac_no(-1);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 1;
}

// [b28d-solo-CAchievement AE] 真语义 bug 修复：`_verifyAchievement` 由 static
// 改非 static（CAchievement.h）后本函数调用点压 this + in->m_saveTime 两实参，
// 与 ORIG 0x0828c956 一致（旧 static 版把 m_saveTime 当 this 解引用 = UB）。
#include "CAchievement/loadData.inc"
{
    stAchievement_DBData* in = (stAchievement_DBData*)pData;
    for (int i = 0; i < in->m_count; ++i)
    {
        m_achievements.insert(
            std::make_pair(in->m_entries[i].m_id, in->m_entries[i]));
    }
    m_user = pUser;
    // [b24F] DONE：_verifyAchievement 参数 (unsigned int)this→in->m_saveTime
    //（ORIG 0x828c956 mov -0x10(%ebp);mov 0x1804(%eax),%eax 传 in->m_saveTime）。
    _verifyAchievement(in->m_saveTime);   // b28c-L10：m_saveTime 已 unsigned（cast 冗余）
    return 1;
}

#include "CAchievement/getData.inc"
{
    stAchievement_DBData* out = (stAchievement_DBData*)pData;
    int i = 0;
    for (std::map<unsigned int, stAchievement>::const_iterator it =
             m_achievements.begin();
         it != m_achievements.end(); ++it, ++i)
    {
        // ORIG 0x828c6e0：按 12 字节整块复制 value（3 个 dword），非逐字段
        out->m_entries[i] = it->second;
    }
    out->m_count = (int)m_achievements.size();
    // b28c-L10 判词（AE）：m_saveTime 声明 unsigned int（CAchievement.h）后，与
    // getCurSec() int 返回值异型，4.4 store 翻为 ORIG 的 RHS-first 形
    // `call; mov %eax,%edx; mov -0x10(%ebp),%eax; mov %edx,0x1804(%eax)`（P211②）。
    out->m_saveTime = GlobalData::s_systemTime_.getCurSec();
}

// ---- 章节查询 ----

#include "CAchievement/_getSectionIdx.inc"
                                          int slot)
{
    // ORIG 0x828bcc0：while(i<size)；section 内层块使析构先于 ++i；
    // 每次循环直接调用 GetTitleSections(G_CDataManager())，不缓存局部引用
    unsigned int i = 0;
    while (i < GetTitleSections(G_CDataManager()).size())
    {
        {
            stTitleSection section = GetTitleSections(G_CDataManager())[i];
            if ((section.m_category == (int)category) &&
                (section.m_startIdx <= slot) && (slot <= section.m_endIdx))
            {
                return i;
            }
        }
        ++i;
    }
    return 0xffffffff;
}

#include "CAchievement/_getPostalReward.inc"
{
    unsigned int unused = 0;
    unsigned int i = 0;   // ORIG i 槽 -0x10（死变量 -0x14 先声明占位）
    while (i < section->m_rewards.size())
    {
        stTitleRewardBasis* reward = section->m_rewards[i];
        if ((reward != 0) && (reward->m_type == 3))
        {
            return reward;
        }
        ++i;
    }
    return 0;
}

// [b28d-solo-CAchievement AE] 修法：ORIG 0x0828bdc6 result 无栈槽往返（ebx=循环完成标志、
// esi=返回值，尾部 `test %ebx; je; mov $1,%esi; mov %esi,%eax`）⇒ 原码是
// **两 return 形**（`return false` 在析构范围内 → 4.1 保存 esi/ebx 后跳
// cleanup），不是 result 局部 + break（后者 4.4 占槽 -0x10 + setne）。
#include "CAchievement/_getTabComplete.inc"
                                   ENUM_TITLE_BOOK_CATEGORY category)
{
    // ORIG 0x828bdd4：不缓存 elements 引用，find/end 各重新取一次
    std::map<int, std::vector<stTitleElement> >::iterator it =
        GetTitleElements(G_CDataManager()).find((int)category);
    if (it != GetTitleElements(G_CDataManager()).end())
    {
        // ORIG 0x828be47：vector 复制构造到局部（非引用），
        // 且每次成员访问都重新 operator[]（不缓存 elem 引用）
        std::vector<stTitleElement> list = it->second;
        for (unsigned int i = (unsigned int)section->m_startIdx;
             i < (unsigned int)section->m_endIdx; ++i)
        {
            // ORIG 0x828be6e/0x828be8d 两守卫各自 `cmp $0xffffffff; sete %al;
            // test %al,%al; jne <独立 nop 岛>`（P133/P225：否定式独立守卫 +
            // continue 才产 sete 物化与两枚独立岛；&& 短路被 4.4 折成直跳）。
            if (list[i].m_index == -1)
            {
                continue;
            }
            if (list[i].m_achievementIdx == -1)
            {
                continue;
            }
            if (!isClearedAchievement(
                    (unsigned int)list[i].m_achievementIdx))
            {
                return false;
            }
        }
    }
    return true;
}
