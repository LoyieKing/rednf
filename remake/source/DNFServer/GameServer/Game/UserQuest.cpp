// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 UserQuest/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "LogManager.h"
// ============================================================================
// df_game_r UserQuest（G2-5 批次）逐函数还原
// 语义依据：docs/class_func_reports/UserQuest.md（汇编为准）+ ORIG 反汇编直读。
// 布局（由 ORIG ctor/dtor 推导，总大小 0x761d pack(1)）：
//   +0x0000 CUser* m_user
//   +0x0004 WongWork::CQuestClear m_cleared（0x7530）
//   +0x7538 int m_quest[20]、+0x7588 int m_trigger[20]
//   +0x75d8 _Quest_Authen_Data（0x44：characNo/characClass/…/urgentQuest/authen[]）
//   +0x761c bool m_field761c（0x761d = CUser.h 的 m_pcRoomAuth 起点）
// 本 TU 定义 UserQuest 全部方法（构造/析构/23 个报告函数 + set_parent），
// 跨类调用均为真实类型 C++ 调用（无 asm 桥）。
// ============================================================================

#include <cstring>
#include <list>
#include <map>
#include <vector>

// ---- 权威头 ----
#include "CDataManager.h"      // CDataManager / QuestList / ENUM_QUEST_GRADE
#include "Quest.h"             // 全局 Quest（权威 0x150 布局；替代本 TU 旧局部视图）
#include "CUserCharacInfo.h"   // CUserCharacInfo（CUser 基类）权威声明
#include "CUser.h"              // 全局权威 CUser / UserQuest（本 TU 实现其方法）
#include "QuestClear.h"        // WongWork::CQuestClear / STClearedQuest
#include "InterfacePacketBuf.h"
#include "CTimeGate.h"
#include "GlobalData.h"      // s_timeGate_（get_quest_info 经 ORIG `mov 0x941f7f4` 读指针）
#include "CPowerManager.h"
#include "GameTypes.h"         // cMyTrace

// ============================================================================
// 本地最小视图（权威声明见 CUser.h / CInventory.h / CBattle_Field.h；因这些头
// 内置最小 UserQuest 与本 TU 完整 UserQuest 冲突，暂不 include —— 与
// CAchievement.cpp 同款模式；方法定义在权威 TU，mangled 与 ORIG 一致）
// ============================================================================




// ---- ORIG 域常量（nm：_ZN21UrgentQuestLevelLimitL…E，r 段）----
// gcc 4.4：namespace 内 static const 未使用也发射（内部链接，名含 L）。
namespace UrgentQuestLevelLimit
{
static const int kStartLvl = 27;   // 0x08ae5d64 (0x1b)
static const int kEndLvl = 70;     // 0x08ae5d68 (0x46)
}

// [Lane37CUser 2026-09-10 解环连带] 原 TU 局部 `class CInventory` 镜像已删：解环后
//   CUser.h 反向 include CInventory.h ⇒ 同 TU 内镜像与权威定义冲突
//   （UserQuest.cpp:50 redefinition vs CInventory.h）。改 include 权威头（方法签名
//   齐备：erase_repeated_item/CalNeedSlot/QuickEmptyCount/InvenEmptycount/
//   insert_event_items）。
#include "CInventory.h"
class CBattle_Field
{
public:
    void* getMaze();  // _ZN13CBattle_Field7getMazeEv（权威见 CBattle_Field.h，因含 CUser.h 未引入）
};

// getMaze() 返回迷宫对象 +0x50 处的 quest idx 字段视图（giveup_quest ORIG
// 单指令 mov 0x50(%eax) 折叠需要组件视图）
struct MazeQuestView
{
    char m_pad[0x50];  // +0x00
    int m_questIdx;    // +0x50
};

// stSelectQuestParam 前 11 个 int 的数组视角（P311/P183）：ORIG accept_quest
// 0x086ac234/0x086ac243 为单条 `mov -0xb0(%ebp,%eax,4),%eax`（基址+scaled），
// 用成员数组视图免 shl+add 且不缓存基址；偏移与权威成员逐一相同（m_level@+0）。
struct SelectQuestParamDataView
{
    int m_data[11];   // +0x00..+0x2b
};

// ---- stQuestTriggerState_t（8 字节：3 个 9 位值；方法定义在 QuestTriggerState.cpp）----


// Quest 类型改用 Quest.h 全局权威类（0x150；本 TU 旧局部视图字段名映射：
//   m_levelLimit→m_levelMax、m_rewardItems→m_rewardSelectList、m_questType→m_triggerType、
//   m_questSubType→m_triggerSubType、m_params→m_triggerData、m_eventList→m_eventDialogs、
//   m_exceptions→m_exceptionList——偏移逐一相同，2026-08-30 统一）。

// ============================================================================
// 跨类调用（全部真实类型调用；asm 桥已删除）
// ============================================================================

#include "UserQuest/reset.inc"
{
    m_user = 0;
    m_cleared.clear();
    for (int i = 0; i <= 19; ++i)
    {
        m_quest[i] = 0;
        m_trigger[i] = 0;
    }
    m_authenData.m_urgentQuest = -1;
    memset(m_authenData.m_authen, 0, 0x28);
}

#include "UserQuest/checkAcceptableQuest.inc"
                          const WongWork::CQuestClear& cleared,
                          const stSelectQuestParam& param)
{
    if (quest->m_index == 0x3f8) return true;
    if (!quest->exposeQuest()) return false;
    if (!quest->check_possible(param)) return false;
    if (!quest->isRepeatableQuest())
    {
        if (!G_CDataManager()->m_questList->check_clear(
                quest->m_index, cleared))
        {
            return false;
        }
    }
    if (!G_CDataManager()->m_questList->check_ahead_quest(
            quest->m_index, cleared))
    {
        return false;
    }
    if (!G_CDataManager()->m_questList->check_anti_quest(
            quest->m_index, cleared))
    {
        return false;
    }
    return true;
}

// ============================================================================
// UserQuest（总大小 0x761d pack(1)，布局见文件头；与 CUser.h 同步）
// ============================================================================

#pragma pack(push, 1)
#pragma pack(pop)

// ============================================================================
// 构造 / 析构 / set_parent
// ============================================================================

#include "UserQuest/set_parent.inc"
{
    m_user = user;
}

// ============================================================================
// reset / resetClearQuest / isClearQuest
// ============================================================================

#include "UserQuest/resetClearQuest.inc"
{
    m_cleared.resetClearedQuests(questIdx);
}

// [b28c-L9] SIGFIX:UserQuest::isClearQuest —— ORIG 0x086ab920 体为
// `call _ZNK8WongWork11CQuestClear14isClearedQuestEj; leave; ret` 直返无归一化；
// callee 权威型 unsigned char（QuestClear.h:41）⇒ caller 同型化（P9/P174）。
#include "UserQuest/isClearQuest.inc"
{
    return m_cleared.isClearedQuest(questIdx);
}

// ============================================================================
// set_quest / get_quest
// ============================================================================

#include "UserQuest/set_quest.inc"
{
    for (int i = 0; i <= 19; ++i)
    {
        m_quest[i] = sig->m_quest[i];
        m_trigger[i] = sig->m_trigger[i];
        Quest* quest = G_CDataManager()->find_quest(m_quest[i]);
        if (quest == 0 && m_quest[i] != 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x42, 0)
                ("VERIFY_QUEST : ABNOMAL QUEST INDEX (%d) : CHARAC_NO (%u)",
                 m_quest[i], sig->m_characNo);
            m_quest[i] = 0;
            m_trigger[i] = 0;
        }
    }
    m_cleared.setClearedQuest(
        (const WongWork::STClearedQuest*)&sig->m_cleared[0]);
    m_authenData.m_urgentQuest = sig->m_urgentQuest;
    memcpy(m_authenData.m_authen, sig->m_authen, 0x28);
    return 1;
}

#include "UserQuest/get_quest.inc"
{
    sig->m_field04 = 0;
    for (int i = 0; i <= 19; ++i)
    {
        sig->m_quest[i] = m_quest[i];
        sig->m_trigger[i] = m_trigger[i];
    }
    memcpy(sig->m_cleared, m_cleared.getClearedQuest(), 0x7530);
    sig->m_urgentQuest = m_user->getCurCharacQuestR()->m_authenData.m_urgentQuest;
    memcpy(sig->m_authen, m_authenData.m_authen, 0x28);
    return 1;
}

// ============================================================================
// get_quest_info / get_mail_quest_info
// ============================================================================

#include "UserQuest/get_quest_info.inc"
{
    // b28d-40: N82 封顶——帧/槽平移（-0x54 vs -0x50 等），语义闭合。ORIG 0x086abba8。
    // 【b28e-M1S2 本波复核】136/136；残差逐 hunk 归类 = 实参装载序（ORIG 先装接收者
    //   %eax→(%esp) 后装 %edx→0x4(%esp)；4.4 反之）+ 寄存器中转选择（我方多
    //   `mov -0x10(%ebp),%eax; mov %eax,0x4(%esp)` vs ORIG `mov -0x10(%ebp),%edx;
    //   mov %edx,0x4(%esp)`）+ 数据地址重定位（0x941f7f4 = CTimeGate 单例，AE 盲区）。
    //   P329 核过：本件无中间具名局部（全内联表达式）⇒ 无该旋钮可用；P331 核过：
    //   -0x54 vs -0x50 为 4B 单段平移但该区含 std::list 类对象 ⇒ 明文不可翻 ⇒ N82。
    // [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N82）→ 试=①prett2 缺口 0、strall 串差 0
    //   （仅 base64 假阳性）、p167 本函数 CLEAR；②444 后端变体逐项相同；③本波复核
    //   5 块 13 行差**全为实参装载序 + 寄存器中转**：ORIG 0x086abba8 区段先写常量
    //   栈槽后载接收者（`movl $0x15,0x8(%esp); movl $0x0,0x4(%esp); mov -0x14(%ebp),
    //   %eax; mov %eax,(%esp)`），4.4 恒先载 this/pb（-0x14）再写常量；以及 ORIG
    //   `mov -0x10(%ebp),%edx; mov %edx,0x4(%esp)` 的 %edx 中转 vs 4.4 经 %eax。
    //   P329 已核全内联无具名局部可删 ⇒ 无新旋钮，维持（新=DIFF 136/136 gap=0,
    //   blocks=5）。
    stSelectQuestParam selectParam(m_user);

    InterfacePacketBuf* pb = reinterpret_cast<InterfacePacketBuf*>(buf);
    pb->put_header(0, 0x15);

    std::list<int> questList;
    G_CDataManager()->m_questList->select_quest(questList, selectParam, m_cleared);
    if (selectParam.m_questCount != -1)
    {
        questList.push_back(selectParam.m_questCount);
    }

    pb->put_byte(m_user->get_charac_level());
    int index = pb->get_index();
    int count = 0;
    pb->put_short(count);

    for (std::list<int>::iterator it = questList.begin(); it != questList.end(); it++)
    {
        int questIdx = *it;
        // ORIG get_quest_info 读取全局 s_timeGate_ 指针（mov 0x941f7f4），
        // 而非把指针变量的地址当作对象；改用 GlobalData::s_timeGate_。
        if (GlobalData::s_timeGate_->setBlindTimeGateStateQuest(questIdx))
        {
            // ORIG 0x086abcb5 `test %al,%al; jne 0x086abccf`：真值走空 then 岛
            // （P223 尾空标签复现 jmp+nop）。
            goto L_skip;
        }
        ++count;
        pb->put_short(questIdx);
    L_skip: ;
    }

    pb->put_short(index, count);
    pb->finalize(true);
}

#include "UserQuest/get_mail_quest_info.inc"
                                    stSelectQuestParam const& selectParam) const
{
    return G_CDataManager()->m_questList->select_mail_quest(questList, m_cleared, selectParam);
}
// ============================================================================
// set_authen_data
// ============================================================================

#include "UserQuest/set_authen_data.inc"
{
    switch (cond)
    {
    case QUEST_CONDITION_0:
        m_authenData.m_characNo = (short)v1;
        m_authenData.m_characClass = (short)v2;
        break;
    case QUEST_CONDITION_1:
        m_authenData.m_field4 = (short)v1;
        break;
    case QUEST_CONDITION_2:
        m_authenData.m_field8 = v1;
        break;
    case QUEST_CONDITION_3:
        m_authenData.m_fieldc = (char)(v1 != 0);
        break;
    case QUEST_CONDITION_4:
        m_authenData.m_field10 = v1;
        break;
    case QUEST_CONDITION_5:
        m_authenData.m_field14 = v1;
        break;
    }
}

// ============================================================================
// accept_quest
// ============================================================================

#include "UserQuest/accept_quest.inc"
{
    // b28d-40: N82 封顶——帧 0xcc vs 0xc0 + 字符串地址重定位/槽平移，语义闭合。
    // ORIG 0x086abf38。
    // 【b28e-M1S2 本波复核】403/404（我方多 1）。P331 核过：quest（ORIG -0x3c / 我方
    // -0x30）与循环 i（ORIG -0x34 / 我方 -0x28）槽序**同向**（非倒置）⇒ 不属
    // P331 可翻域；平移恒 0xc = ORIG 独有 `push %edi`（帧 0xcc vs 0xc0）⇒ N82/N7。
    // [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N7/N78/N82）→ 试=①prett2 缺口 0、
    //   strall 串差 0、p167 新增复核（EXTRA memset x1 = 我方 call vs ORIG 4.1
    //   `rep stos` 内联 11 dwords，N78 在案；其余 MISSING 属 GI/undefined-weak
    //   假阳性族）；②444 后端变体逐项相同；③帧差 0xc = ORIG 独有 `push %edi`
    //   （4.1 三 callee-saved：edi/esi/ebx vs 4.4 两：esi/ebx）+ 槽平移，属寄存器
    //   分配不可择；P339 quest/i 槽序已核同向非倒置 ⇒ 无新旋钮，维持（新=DIFF
    //   403/404 gap=-1, blocks=15）。
    // P339 明文禁把非 POD（stSelectQuestParam / vector）提前 ⇒ 不适用。
    // 语义轴：logFormat/cMyTrace 串与 ORIG 0x8cf3a14 / 0x8cf47a0 / 0x8cf3a21 /
    // 0x8cf3a64 一致（已核）。
    if (m_user == 0)
    {
        LogManager::logFormat(1, "user_quest.cpp",
                                 __PRETTY_FUNCTION__, 0x1e8,
                                 "user is null");
        return 1;
    }
    if (m_user->getCurCharacR() == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x1ef, 5);
        trace("UserQuest::accept_quest user->m_selected is null");
        return 1;
    }

    Quest* quest = G_CDataManager()->find_quest(questIdx);
    if (quest == 0)
    {
        LogManager::logFormat(1, "user_quest.cpp",
                                 __PRETTY_FUNCTION__, 0x1f8,
                                 "G_CDataManager()->find_quest(%d)", questIdx);
        return 1;
    }

    for (int i = 0; i <= 19; ++i)
    {
        if (m_quest[i] == questIdx)
        {
            return 0x12;
        }
    }

    if (!m_user->IsGameMasterMode())
    {
        // P225：ORIG 0x086ac0c5/0x086ac0d7/0x086ac0e8 三守卫各自 je 同一出口
        // （无 setcc 物化）⇒ 源码必须是嵌套 if，禁 && 短路链（4.4 恒物化 bool）。
        if (!quest->isRepeatableQuest())
        {
            if (m_cleared.isClearedQuest(questIdx))
            {
                return 0x12;
            }
        }
    }

    stSelectQuestParam selectParam(m_user);
    if (!m_user->IsGameMasterMode())
    {
        UserQuest* cur = m_user->getCurCharacQuestR();
        if (!checkAcceptableQuest(quest, cur->m_cleared, selectParam))
        {
            m_user->getHackAnalyzer()->addServerHackCnt(
                m_user, (WongWork::ENUM_HACKTYPE)0x191, 1, 0, 0);
            return 1;
        }
    }

    std::vector<std::pair<int, int> > rewardItems;
    std::vector<std::pair<int, int> > erasedItems;
    rewardItems = quest->m_rewardSelectList;

    int erased = m_user->getCurCharacInvenW()->erase_repeated_item(
        rewardItems, erasedItems);
    if (erased > 0)
    {
        // N78：ORIG 0x086ac1b5..0x086ac1c9 为 4.1 内联 memset（rep stos 11 dwords，
        // `mov $0xb,%edx`）；4.4 恒发 call memset（结构性差异，语义等价）。
        memset(reinterpret_cast<SelectQuestParamDataView*>(&selectParam)->m_data,
               0, 0x2c);
        // P183 禁指针缓存：ORIG 0x086ac1d3/0x086ac1f9/0x086ac21d 每使用点各取一次
        // getCurCharacInvenR()（p167 差集实证 MISSING ×2）。
        m_user->getCurCharacInvenR()->CalNeedSlot(&selectParam.m_level, erasedItems);
        int quick = m_user->getCurCharacInvenR()->QuickEmptyCount();
        for (int i = 1; i <= 10; ++i)
        {
            int cnt = m_user->getCurCharacInvenR()->InvenEmptycount(
                (Inven_Item::ITEM_TYPE)i);
            if (reinterpret_cast<SelectQuestParamDataView*>(&selectParam)->m_data[i] > cnt)
            {
                quick += cnt -
                    reinterpret_cast<SelectQuestParamDataView*>(&selectParam)->m_data[i];
                if (quick < 0)
                {
                    return 0x11;
                }
            }
        }
    }

    trigger = quest->get_init_trigger();
    for (int i = 0; i < 20; ++i)
    {
        if (m_quest[i] == 0)
        {
            m_cleared.resetClearedQuests(questIdx);
            m_quest[i] = questIdx;
            m_trigger[i] = trigger;

            InterfacePacketBuf* pb = reinterpret_cast<InterfacePacketBuf*>(buf);
            pb->put_byte(1);
            pb->put_short(questIdx);
            pb->put_int(trigger);

            std::vector<int> itemIdx;
            int insertResult = m_user->getCurCharacInvenW()->insert_event_items(
                erasedItems, itemIdx,
                (eMoneyAddReason)3, (eItemAddReason)8, false, "", "");
            pb->put_byte(insertResult);

            for (std::vector<int>::iterator it = itemIdx.begin();
                 it != itemIdx.end(); it++)
            {
                pb->put_short(*it);
                it++;
                pb->put_int(*it);
                it++;
                pb->put_int(*it);
            }
            return 0;
        }
    }
    return 4;
}

// ============================================================================
// giveup_quest
// ============================================================================

#include "UserQuest/giveup_quest.inc"
{
    for (int i = 0; i <= 19; ++i)
    {
        if (m_quest[i] == questIdx)
        {
            Quest* quest = G_CDataManager()->find_quest(questIdx);
            if (quest != 0)
            {
                if (quest->m_field120 == 1 || quest->m_field120 == 2)
                {
                    if (m_trigger[i] == 0)
                    {
                        return 0x12;
                    }
                    if (m_user->get_state() == 5)
                    {
                        void* party = m_user->GetParty();
                        if (party != 0)
                        {
                            void* maze = reinterpret_cast<CBattle_Field*>(static_cast<char*>(party) + 0xb24)->getMaze();
                            if (maze != 0 &&
                                reinterpret_cast<MazeQuestView*>(maze)->m_questIdx == questIdx)
                            {
                                return 0x14;
                            }
                        }
                    }
                }
                if (quest->m_type == 8)
                {
                    if (m_user->get_charac_level() > quest->m_levelMax)
                    {
                        m_authenData.m_urgentQuest = -1;
                    }
                    if (hasEpicQuest())
                    {
                        m_authenData.m_urgentQuest = -1;
                    }
                }
            }

            m_quest[i] = 0;
            m_trigger[i] = 0;
            return 0;
        }
    }
    return 0x13;
}

// ============================================================================
// set_trigger
// ============================================================================

#include "UserQuest/set_trigger.inc"
{
    for (int i = 0; i <= 19; ++i)
    {
        if (m_quest[i] == questIdx)
        {
            if (m_user->getGmQuestFlag())
            {
                m_trigger[i] = 0;
            }

            if (type == 1)
            {
                ++m_trigger[i];   // b25c：++ 形 4.4 单次装载 i（探针 probe_inc s3），ORIG 同构
            }
            else
            {
                if (!check_quest_condition(questIdx))
                {
                    return -1;
                }
                if (bIncrement)
                {
                    stQuestTriggerState_t state;
                    state = m_trigger[i];
                    state.incTirggerState(type);
                    m_trigger[i] = state.getTrigger();
                }
                else if (m_trigger[i] > 0)
                {
                    stQuestTriggerState_t state;
                    state = m_trigger[i];
                    state.subTriggerState(type);
                    m_trigger[i] = state.getTrigger();
                }
            }
            return m_trigger[i];
        }
    }
    return -1;
}

// ============================================================================
// finish_quest
// ============================================================================

#include "UserQuest/finish_quest.inc"
{
    for (int i = 0; i <= 19; ++i)
    {
        if (m_quest[i] == questIdx)
        {
            if (m_trigger[i] == 0)
            {
                m_quest[i] = 0;
                m_trigger[i] = 0;

                Quest* quest = G_CDataManager()->find_quest(questIdx);
                if (quest != 0 && !quest->isRepeatableQuest())
                {
                    m_cleared.setClearedQuest(questIdx);
                }
                if (quest->m_type == 8)
                {
                    m_authenData.m_urgentQuest = -1;
                }
                return 0;
            }
            return 0x16;
        }
    }
    return 0x13;
}

// ============================================================================
// item_reward_quest / get_rescue_questmap / check_cond_clear
// ============================================================================

// b28d-40: P223 双守卫分落——外层正向 if 包裹 + 内层 continue 复现
// ORIG 第一守卫直落循环尾、第二守卫落 nop 岛；0x086ac954，AE。
#include "UserQuest/item_reward_quest.inc"
{
    for (int i = 0; i <= 19; ++i)
    {
        if (m_quest[i] != 0)
        {
            if (m_trigger[i] == 0)
            {
                continue;
            }
            Quest* quest = G_CDataManager()->find_quest(m_quest[i]);
            if (quest == 0)
            {
                return 0;
            }
            if (quest->check_clear_item(mapIdx, (int)m_authenData.m_characClass))
            {
                return m_quest[i];
            }
        }
    }
    return 0;
}

// b28d-40: P223 双守卫分落（同 item_reward_quest 形）；0x086aca16，AE。
#include "UserQuest/get_rescue_questmap.inc"
{
    for (int i = 0; i <= 19; ++i)
    {
        if (m_quest[i] > 0)
        {
            if (m_trigger[i] == 0)
            {
                continue;
            }
            Quest* quest = G_CDataManager()->find_quest(m_quest[i]);
            if (quest == 0)
            {
                return 0;
            }
            int map = quest->get_appearmap(mapIdx, (int)m_authenData.m_characClass);
            if (map > 0)
            {
                return map;
            }
        }
    }
    return 0;
}

#include "UserQuest/check_cond_clear.inc"
{
    for (int i = 0; i <= 19; ++i)
    {
        if (m_user->getGmQuestFlag())
        {
            if (m_quest[i] == questIdx)
            {
                return 1;
            }
        }
        else if (m_quest[i] == questIdx && m_trigger[i] == 0)
        {
            return 1;
        }
    }
    return 0;
}

// ============================================================================
// check_quest_condition / check_quest_type_condition /
// check_quest_subtype_condition
// ============================================================================

#include "UserQuest/check_quest_condition.inc"
{
    Quest* quest = G_CDataManager()->m_questList->find_quest(questIdx);
    if (quest == 0)
    {
        LogManager::logFormat(
            1, "user_quest.cpp", __PRETTY_FUNCTION__, 0x403,
            "G_CDataManager()->m_pQuestList->find_quest(%d) fail", questIdx);
        return 0;
    }
    return check_quest_type_condition(quest);
}

#include "UserQuest/check_quest_type_condition.inc"
{
    switch (quest->m_triggerType)
    {
    case 0:
    case 1:
    case 2:
    case 4:
    case 6:
    case 0xf:
    case 0x19:
        return 1;
    case 5:
        if (quest->m_triggerData.at(0) <=
            m_user->getPVPResultR()->m_pvpGrade)
        {
            return 1;
        }
        return 0;
    case 3:
        if (quest->m_triggerData.at(0) == (int)(short)m_authenData.m_field4)
        {
            return 1;
        }
        return 0;
    case 8:
        if (quest->m_triggerData.at(0) <= (int)(short)m_user->GetUserPowerWarPoint())
        {
            return 1;
        }
        return 0;
    case 7:
        if (GlobalData::s_power_manager->GetWinnerSide() ==
            m_user->getPowerSide())
        {
            return 1;
        }
        return 0;
    case 0x10:
    case 0x13:
        return 1;
    default:
        return check_quest_subtype_condition(quest);
    }
}

#include "UserQuest/check_quest_subtype_condition.inc"
{
    switch (quest->m_triggerSubType)
    {
    case 0:
    {
        int at0 = quest->m_triggerData.at(0);
        int at1 = quest->m_triggerData.at(1);
        int at2 = quest->m_triggerData.at(2);
        if (at1 != -1 && (int)m_authenData.m_characClass != at1)
        {
            return 0;
        }
        if ((int)m_authenData.m_characNo != at0)
        {
            return 0;
        }
        if (at2 * 1000 < m_authenData.m_field8)
        {
            return 0;
        }
        return 1;
    }
    case 1:
    case 2:
    case 3:
    case 6:
        return 1;
    case 4:
    {
        int at0 = quest->m_triggerData.at(0);
        int at1 = quest->m_triggerData.at(1);
        if (at1 != -1 && (int)m_authenData.m_characClass != at1)
        {
            return 0;
        }
        if ((int)m_authenData.m_characNo != at0)
        {
            return 0;
        }
        if (m_authenData.m_fieldc != 0)
        {
            return 0;
        }
        return 1;
    }
    case 5:
    {
        int at0 = quest->m_triggerData.at(0);
        int at1 = quest->m_triggerData.at(1);
        int at2 = quest->m_triggerData.at(2);
        if (at1 != -1 && (int)m_authenData.m_characClass < at1)
        {
            return 0;
        }
        if ((int)m_authenData.m_characNo != at0)
        {
            return 0;
        }
        if (at2 == 1 && m_authenData.m_field10 != 1)
        {
            return 0;
        }
        if (m_authenData.m_field10 < at2)
        {
            return 0;
        }
        return 1;
    }
    case 7:
    {
        int at0 = quest->m_triggerData.at(0);
        int at1 = quest->m_triggerData.at(1);
        int at2 = quest->m_triggerData.at(2);
        if (at1 != -1 && (int)m_authenData.m_characClass != at1)
        {
            return 0;
        }
        if ((int)m_authenData.m_characNo != at0)
        {
            return 0;
        }
        if (m_authenData.m_field14 < at2)
        {
            return 0;
        }
    }
    default:
        return 0;
    }
}

// ============================================================================
// IsDoingQuest / CheckDungeonEventStoryPause / isDoingJobChangeQuest
// ============================================================================

#include "UserQuest/IsDoingQuest.inc"
{
    for (int i = 0; i <= 19; ++i)
    {
        if (m_quest[i] == questIdx && m_trigger[i] != 0)
        {
            return 1;
        }
    }
    return 0;
}

#include "UserQuest/CheckDungeonEventStoryPause.inc"
{
    // b28d-40: 两个 continue 改嵌套正向 if 包裹（消 4.4 同层多 continue 的 nop+jmp
    // 中继岛，ORIG 0x086ad079/0x086ad0a7 两守卫直接落外层循环尾 0x086ad159）。
    for (int i = 0; i <= 19; ++i)
    {
        if (m_quest[i] != 0)
        {
            // P167：ORIG 0x086ad0xx 三处调 const 版
            // `_ZNKSt6vectorI24dungeonEventHanaseStructSaIS0_EEixEj` ⇒ 原码 quest 为 const Quest*。
            const Quest* quest = G_CDataManager()->find_quest(m_quest[i]);
            if (quest != 0)
            {
                for (unsigned int j = 0; j < quest->m_eventDialogs.size(); ++j)
                {
                    switch (quest->m_eventDialogs[j].m_field0)
                    {
                    case 0:
                        if (quest->m_eventDialogs[j].m_field8 == fieldNo)
                        {
                            return 1;
                        }
                        break;
                    case 1:
                        if (quest->m_eventDialogs[j].m_field8 == dungeonNo)
                        {
                            return 1;
                        }
                        break;
                    }
                }
            }
        }
    }
    return 0;
}

#include "UserQuest/isDoingJobChangeQuest.inc"
{
    for (int i = 0; i <= 19; ++i)
    {
        if (m_quest[i] != 0)
        {
            Quest* quest = G_CDataManager()->find_quest(m_quest[i]);
            if (quest != 0 && (quest->m_jobChangeQuest == 1 || quest->m_jobChangeQuest == 2))
            {
                return true;
            }
        }
    }
    return false;
}

// ============================================================================
// ResetUrgentQuestWaitingList / hasEpicQuest
// ============================================================================

#include "UserQuest/ResetUrgentQuestWaitingList.inc"
{
    // [b28c-L9] fresh 复验 ndiff 11→1：唯一残差 = 4.4 把 quest2!=0 臂尾的
    // 「写 -1 + jmp end」与 else 臂尾的 jmp 合并（共享尾块少一枚 jmp）。
    Quest* quest2;
    for (int i = 0; i <= 19; ++i)
    {
        Quest* quest = G_CDataManager()->find_quest(m_quest[i]);
        if (quest != 0 && quest->m_type == 8)
        {
            return;
        }
    }

    if (hasEpicQuest())
    {
        m_authenData.m_urgentQuest = -1;
    }
    else
    {
        quest2 = G_CDataManager()->find_quest(m_authenData.m_urgentQuest);
        // b25c 翻案：ORIG 4.1 把 quest2==0 的块外置在后（je 落点岛），
        // 且槽序 quest2/-0x18 i/-0x14 quest/-0x10 levelLimit/-0xc。
        // b28d-40：两臂 return 改自然结束（ORIG 0x086ad24e/0x086ad25d 各一条 jmp，
        // return 形多一条 nop+jmp 中继）。
        if (quest2 != 0)
        {
            int levelLimit = quest2->m_levelMax;
            if (levelLimit > UrgentQuestLevelLimit::kEndLvl)
            {
                levelLimit = UrgentQuestLevelLimit::kEndLvl;
            }
            if (m_user->get_charac_level() + 1 > levelLimit)
            {
                m_authenData.m_urgentQuest = -1;
            }
        }
        else
        {
            m_authenData.m_urgentQuest = -1;
        }
    }
}

#include "UserQuest/hasEpicQuest.inc"
{
    // b28d-40: N4 封顶——`lea 0x4(%edx)` vs 4.4 `add $0x4,%eax`（add-vs-lea），
    // 语义闭合。ORIG 0x086ad266。
    if (m_user == 0)
    {
        return false;
    }

    std::list<int> questList;
    stSelectQuestParam selectParam(m_user);

    WongWork::CQuestClear cleared(m_user->getCurCharacQuestR()->m_cleared);

    G_CDataManager()->m_questList->allowable_questlist_as_npc(
        questList,
        *G_CDataManager()->m_questList->getQuestNPCList(ENUM_QUEST_GRADE_NONE),
        selectParam,
        cleared, false);

    if (!questList.empty())
    {
        char deadLocal = 0;   // ORIG 0x086ad334 死写（P171 照抄）
        (void)deadLocal;
        // ORIG 0x086ad3f4 增量段调 `_ZNSt14_List_iteratorIiEppEv`（前缀，无 sret）⇒ 写 ++it。
        for (std::list<int>::iterator it = questList.begin(); it != questList.end(); ++it)
        {
            // P167：ORIG 0x086ad399 调 const 版 `_ZNKSt6vectorI25ENUM_QUEST_EXCEPTION_TYPESaIS0_EEixEj`
            // ⇒ 原码 quest 为 const Quest*。
            const Quest* quest = G_CDataManager()->find_quest(*it);
            if (quest == 0)
            {
                goto L_quest0;   // P223 多岛：L1 岛（nop; jmp）
            }
            {
                char bException = 0;
                for (unsigned int j = 0; j < quest->m_exceptionList.size(); ++j)
                {
                    if (quest->m_exceptionList[j] == ENUM_QUEST_EXCEPTION_TYPE_0)
                    {
                        bException = 1;
                        break;
                    }
                }
                if (bException != 0)
                {
                    goto L_bexc;   // P223 多岛：L2 岛（nop）
                }
                if (selectParam.m_level < quest->m_levelMin + 0xc)
                {
                    return true;
                }
            }
        L_quest0: ;
        L_bexc: ;
        }
    }
    return false;
}
