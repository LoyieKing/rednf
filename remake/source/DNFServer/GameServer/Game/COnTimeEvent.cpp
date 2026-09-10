// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 COnTimeEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// COnTimeEvent.cpp —— 按报告 docs/class_func_reports/COnTimeEvent/*.md 忠实转写。
#include "COnTimeEvent.h"

#include <cstdio>

#include "GameTypes.h"          // cMyTrace
#include "CMyFileLog.h"
#include "CEnvironment.h"
#include "CUser.h"
#include "GameWorld.h"
#include "DB_InsertOnTimeRewardUserLog.h"

// ORIG 自由函数（权威声明 CInventory.h:408；按项目惯例局部声明）。
char* NumberToString(unsigned int value, int radix);

// ===================== 生命周期 =====================

// ORIG C1 @0x081bb826：基类 ctor → CRewardUserList ctor(+0x14) →
// +0x0c=0、+0x10=1 → clear()。
#include "COnTimeEvent/COnTimeEvent.inc"
{
    m_ontimeId = 0;
    m_rewardState = 1;
    m_rewardUsers.clear();
}

#include "COnTimeEvent/StartEvent.inc"
{
    SetEventFlag(true);
}

#include "COnTimeEvent/EndEvent.inc"
{
    // ORIG @0x081bb91a：SetEventFlag(false) 在 cMyTrace 构造前调用。
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x4d, 0);
    trace("COnTimeEvent Event End!");
}

#include "COnTimeEvent/StartEvent_1.inc"
{
    (void)param;
    // [b28c-lane17] 判词：22=22 助记符全等，唯一残差 = 下方虚调用的目标寄存器
    // （ORIG `mov (%eax),%edx; …; call *%edx` vs 4.4 恒 `call *%eax`）⇒ N7
    // （4.4 间接 call 目标恒 %eax，范式库负例表），无源形可翻。
    this->StartEvent();   // ORIG @0x081bb8ca：虚调 slot+0x8（本类 StartEvent）
    cMyTrace trace(__PRETTY_FUNCTION__, 0x47, 0);
    trace("COnTimeEvent Event Start!");
}

// ===================== getter/setter =====================

#include "COnTimeEvent/set_ontime_id.inc"
#include "COnTimeEvent/get_ontime_id.inc"
#include "COnTimeEvent/set_reward_state.inc"
#include "COnTimeEvent/get_reward_state.inc"

// ORIG @0x081bb994：*(time_t*)(this+0x54) = time(NULL) + param。
// ORIG 先把 time() 结果落 -0xc 栈槽再回读（time_t t = time(0); 两步）。
#include "COnTimeEvent/set_reward_end_time.inc"
{
    time_t t = time(0);
    m_rewardEndTime = t + span;
}

#include "COnTimeEvent/get_reward_end_time.inc"

#include "COnTimeEvent/set_ontime_reward_item.inc"
{
    m_itemNo = no;
    m_itemCount = count;
}
#include "COnTimeEvent/get_ontime_reward_item_no.inc"
#include "COnTimeEvent/get_ontime_reward_item_count.inc"

#include "COnTimeEvent/get_ontime_reward_user_count.inc"
{
    return m_rewardUsers.get_index_size();
}

// ===================== 业务流程 =====================

// ORIG @0x081bbce0（process_set_reward_start）。
#include "COnTimeEvent/process_set_reward_start.inc"
                                            int itemCount, int endSpan)
{
    set_reward_state(0);
    set_ontime_id(ontimeId);
    set_ontime_reward_item(itemNo, itemCount);
    set_reward_end_time(endSpan);
    m_rewardUsers.clear();
}

// ORIG @0x081bb9dc。双日志 + 已领过判定。
// 汇编要点：① 两个 256 数组为 { '\0' } 初始化（rep stos 内联，非 call memset）；
// ② 比较 setg（get_ontime_id() > last_recv_idx()，先求左操作数）；
// ③ insert 实参右→左内联求值 get_ontime_id→get_charac_no→get_acc_id；
// ④ insert 后先 set_ontime_last_recv_idx(get_ontime_id()) 再 set_update；
// ⑤ else 分支求值序 get_charac_no→get_acc_id→NumberToString→last_recv→ontime_id。
// [b28c-lane17] fresh：158=158 助记符全等 ⇒ NEAR；残差为互斥块内同类 cMyTrace/
// char[0x100] 临时槽深方向与 callee-saved 分配差（N82附/N86 族），
// 调用点差集与槽位集无缺项 ⇒ 语义完成、byte 层 N 封顶。
#include "COnTimeEvent/insert_reward_user.inc"
{
    if (user == 0)
        return false;

    char logDir[256] = { '\0' };
    sprintf(logDir, "./log/%s/OnTimeEvent", G_CEnvironment()->get_file_name());

    register unsigned int characNo = user->get_charac_no(-1);
    register char* accStr = NumberToString(user->get_acc_id(), 0);
    // [b28d-51] P221/P227：互斥 sibling 块同类日志对象槽深方向 —— 具名对象 4.4 反
    // （首块 -0x20/次块 -0x28），改全表达式临时链复现 ORIG（首块 -0x28/次块 -0x20）。
    CMyFileLog(__PRETTY_FUNCTION__, 0x75)(
        logDir, "On Time Event:On Request Reward m_id(%s) charac_no(%u)",
        accStr, characNo);

    if (get_ontime_id() > user->get_ontime_last_recv_idx())
    {
        m_rewardUsers.insert(user->get_acc_id(), user->get_charac_no(-1),
                             get_ontime_id());
        user->set_ontime_last_recv_idx(get_ontime_id());
        user->set_update_ontime_last_recv_idx();
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x120, 0);
        return true;
    }
    else
    {
        char errDir[256] = { '\0' };
        sprintf(errDir, "./log/%s/OnTimeEvent",
                G_CEnvironment()->get_file_name());

        register unsigned int characNo2 = user->get_charac_no(-1);
        register char* accStr2 = NumberToString(user->get_acc_id(), 0);
        register unsigned int lastIdx = user->get_ontime_last_recv_idx();
        register unsigned int ontimeId = get_ontime_id();
        CMyFileLog(__PRETTY_FUNCTION__, 0x89)(
            errDir,
            "On Time Event:Reward(no.%u user-%u)error m_id(%s)charac_no(%u)",
            ontimeId, lastIdx, accStr2, characNo2);
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x120, 0x12);
        return false;
    }
}

// ORIG @0x081bbc64。
#include "COnTimeEvent/delete_reward_user.inc"
{
    if (user == 0)
        return false;

    if (get_ontime_id() == user->get_ontime_last_recv_idx())
    {
        m_rewardUsers.delete_mark(user->get_acc_id());
        return true;
    }
    return false;
}

// ORIG @0x081bbd46：遍历 key 队列（上限 0x33 个/次），逐个投递奖励。
// 汇编要点：① while (!m_index.empty()) 前置 jmp 循环头（P25 while 形态）；
// ② 循环尾 ++processed 先、pop_front 后、cmpl $0x32; jg break；
// ③ iterator 函数头默认构造（P28 声明序 it→user→processed）；
// ④ 条件嵌套 if（两个独立 je，非 && 汇聚）；⑤ 比较用 it->second.a（charac_no）。
#include "COnTimeEvent/process_send_reward_to_user.inc"
{
    std::map<unsigned int, RewardUserData>::const_iterator it;
    CUser* user = 0;
    int processed = 0;

    while (!m_rewardUsers.m_index.empty())
    {
        it = m_rewardUsers.m_map.find(m_rewardUsers.m_index.front());

        if (it != m_rewardUsers.m_map.end())
        {
            if (it->second.mark != 1)
            {
                if ((user = G_GameWorld()->find_user_from_world_byaccid(
                         it->first)) != 0)
                {
                    if (user->get_charac_no(-1) == it->second.a &&
                        user->get_state() > 2)
                    {
                        user->InsertOnTimeEventReward(
                            get_ontime_reward_item_no(),
                            get_ontime_reward_item_count());
                        DB_InsertOnTimeRewardUserLog::makeRequest(
                            (int)user->GetUID(), it->first, get_ontime_id());
                    }
                }
            }
        }
        ++processed;
        m_rewardUsers.m_index.pop_front();
        if (processed > 0x32)
            break;
    }
    return true;
}
