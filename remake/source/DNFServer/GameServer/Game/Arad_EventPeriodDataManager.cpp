// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Arad_EventPeriodDataManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ ARAD::Arad_EventPeriodDataManager 实现
//（批3：从 GameStubs.cpp 迁移；批4：补 Arad_InterfaceGameScript 基类生命周期）。
//
// ORIG ctor 0x81953fa：Arad_InterfaceGameScript("Arad_EventPeriodDataManager") 基类 +
// vector<SEventPeriodData>(+0x0c) / vector(+0x18) / Mutex(+0x24) / list(+0x3c)，+0x08=0。
// Load() 0x8195c9a：清理 + 装载事件周期脚本（loadScriptData）。
// isApplied 0x81956b0：在 +0x0c vector 按事件码找周期项，判定 [m_beginTime, m_endTime)。
#include "Arad_EventPeriodDataManager.h"

#include <algorithm>

#include "CUser.h"       // get_acc_id / get_charac_no
#include "CGuildServerProxy.h"   // WongWork::CGMAccounts::isGM 权威声明
#include "GlobalData.h"  // s_pGMAccounts_
#include "LogManager.h"
#include "MoreTypes.h"   // arad::SigLoadRewardEventItem 完整定义

namespace ARAD
{

#include "Arad_EventPeriodDataManager/Arad_EventPeriodDataManager.inc"
    : Arad_InterfaceGameScript("Arad_EventPeriodDataManager")
{
    m_field8 = 0;
}

#include "Arad_EventPeriodDataManager/dtor_Arad_EventPeriodDataManager.inc"
{
    clearScriptData();    // ORIG 0x0819554c：D1 写 vptr 后调 clearScriptData()
}

#include "Arad_EventPeriodDataManager/loadScriptData.inc"
{
    // ORIG 0x08195650：直接调 importAradEventScript(m_eventPeriodList,
    // m_rewardPeriodList)（+0xc/+0x18 两 vector），无 return（eax 残留）。
    ARAD::SCRIPT::importAradEventScript(m_eventPeriodList, m_rewardPeriodList);
}

#include "Arad_EventPeriodDataManager/clearScriptData.inc"
{
    // ORIG 0x8195670：仅清空 +0x0c 的 vector<SEventPeriodData>（奖励容器不清）。
    m_eventPeriodList.clear();
}

// [b28d-28] DIFF | ORIG 0x081956b0 | N86/P28：find_if sret 临时槽序 + 谓词实参装载序。
// [L4fEventAvatar 2026-09-11] P0/P16 复原：ORIG 0x08195749 `mov 0x10(%ebp),%eax;
//   mov %eax,-0xc(%ebp)` ⇒ 具名 time 局部占 -0xc（其余槽随之下移 4）；外层
//   `if (it != end)` 直接 test（0x08195745，无 xor 取反），内层 `if (A && B)`
//   由 ORIG 物化为 `mov $1 / mov $0 + test + je`（0x08195775/0x0819577c/0x08195781）。
#include "Arad_EventPeriodDataManager/isApplied.inc"
{
    // P167：ORIG 用 SearchEventPeriod 谓词 + find_if（非 const iterator）
    SearchEventPeriod pred(eventCode);
    std::vector<ARAD::SCRIPT::SEventPeriodData>::iterator it =
        std::find_if(m_eventPeriodList.begin(), m_eventPeriodList.end(), pred);
    if (it != m_eventPeriodList.end())
    {
        unsigned int t = time;   // ORIG 0x08195749 赋值点（if 体内）
        if (t >= it->m_beginTime && t < it->m_endTime)
            return true;
    }
    return false;
}

#include "Arad_EventPeriodDataManager/isRunning.inc"
{
    // ORIG isRunning@0x08195686：以 s_systemTime_（0x941f714）的当前 tick 作为
    // time 转发给 isApplied（getCurTickCount@0x081458ac 返回 m_curTickCount）。
    return isApplied(eventCode, GlobalData::s_systemTime_.getCurTickCount());
}

// ORIG 0x08195994：遍历 SigLoadRewardEventItem 条目（count=+0x04 short），
// 每条以 {事件码@+0x06+4i, accId, 角色号@+0x2e+4i} 构造 EventRewardItemArad；
// 未奖励过则 addUserRewardData 记录，否则 logFormat 打印 already reward。
#include "Arad_EventPeriodDataManager/addUserRewardData.inc"
    CUser* user, arad::SigLoadRewardEventItem& sig)
{
    for (int i = 0; i < (int)sig.m_count; ++i)
    {
        EventRewardItemArad item(sig.m_eventCodes[i], user->get_acc_id(),
                                 sig.m_characNos[i]);
        if (isRewarded(item))
        {
            LogManager::logFormat(
                1, "localjapan/Arad_EventPeriodDataManager.cpp",
                "void ARAD::Arad_EventPeriodDataManager::addUserRewardData(CUser*, "
                "arad::SigLoadRewardEventItem&)",
                0xe3,
                "[FnReward] already reward item. (mid:%u, charac:%u, event:%d)",
                user->get_acc_id(), user->get_charac_no(-1), sig.m_eventCodes[i]);
        }
        else
        {
            addUserRewardData(user, item);
        }
    }
}

// ORIG 0x08195aa4：Guard<Mutex>(this+0x24) 后 push_back 到 +0x3c list。
#include "Arad_EventPeriodDataManager/addUserRewardData_1.inc"
    CUser* user, EventRewardItemArad& item)
{
    (void)user;
    Guard<Mutex> guard(&m_mutex);
    m_rewardItemList.push_back(item);
}

// ORIG 0x081958b6：Guard<Mutex> 后 std::find（operator== 三字段比较），命中即 true。
// [b28d-28] DIFF | ORIG 0x081958b6 | N86/P28：std::find 迭代器槽序 + EH 块序。
// [L4fEventAvatar 2026-09-11] P0：ORIG 0x081958c8..0x08195936 的调用序为
//   end(-0x14) → begin(-0x10) → find(-0x1c) → end2(-0xc)，即 find 结果先落具名
//   迭代器槽（sret 直达、无额外拷贝槽）再与 end() 比较；直写 `find(...) != end()`
//   会让 4.4 先求值比较右侧 end2（-0x18）打乱槽序。
#include "Arad_EventPeriodDataManager/isRewarded.inc"
{
    Guard<Mutex> guard(&m_mutex);
    std::list<EventRewardItemArad>::iterator it =
        std::find(m_rewardItemList.begin(), m_rewardItemList.end(), item);
    // ORIG 0x08195950 `test %al,%al; je <mov $0,%ebx>` ⇒ 分支物化形
    // （4.1 对 bool 返回表达式 + 尾部 Guard 析构的产物），非 4.4 的 `mov %eax,%ebx`。
    // ORIG 0x0819593e `lea -0x1c; mov 0x4(%esp)` + `lea -0xc; mov (%esp)`
    //   ⇒ 实参位 (a=-0xc=end2, b=-0x1c=it)：源码写 `end() != it`。
    if (m_rewardItemList.end() != it)
        return true;
    return false;
}

// ORIG 0x08195794：SearchEventPeriod 谓词 find_if +0x0c vector；
// 命中后起点 = GM ? m_field8 * (-86400) + 0x5005D250 : 0x5005D250（32 位回绕，
// -0x15180 = -86400 秒/天）；begin <= time < 条目 m_endTime(+0x08) 返回 true。
// 注意：ORIG 此函数不加锁。
// [b28d-28] DIFF | ORIG 0x08195794 | N86/P28：同 isApplied（find_if 槽序 +
// 谓词实参序）+ isGM 分支块序；语义等价。
#include "Arad_EventPeriodDataManager/checkCreateTime.inc"
    ENUM_REPEAT_EVENT_CODE eventCode, unsigned int time, unsigned int gmId)
{
    SearchEventPeriod pred(eventCode);
    std::vector<SCRIPT::SEventPeriodData>::iterator it =
        std::find_if(m_eventPeriodList.begin(), m_eventPeriodList.end(), pred);
    // P167：ORIG 生成 operator!=（0x08195829 `test %al,%al; je` 直落成功路径，
    //   [L4fEventAvatar 2026-09-11] 无 xor 取反 ⇒ 原码是 if(命中){...} 而非
    //   `if(!(it != end)) return false;`）。
    if (it != m_eventPeriodList.end())
    {
        // [L23-r2 判据⑫ widthscan] `clr_b orig=1 our=0`：ORIG 有 `movb $0x0,-N(%ebp)`
        //   预清（call 前）⇒ P198① 两句式「声明带初值 + 独立赋值」；合写会丢预清行，
        //   且该 bool 局部同时是槽集合差 4 的来源（我方槽整体浅 4）。
        bool isGM = false;
        // ORIG 0x08195845 `test %al,%al; je <跳过>` + 0x08195849 `movb $0x1,-0x11(%ebp)`
        //   ⇒ 条件赋值（if 形），非 `isGM = 调用()`（后者为 `mov %al,-0x11`）。
        if (GlobalData::s_pGMAccounts_->isGM(gmId))
            isGM = true;
        // [L4fEventAvatar 2026-09-11] ORIG 0x0819584d..0x0819586b 是
        //   `cmpb $0,-0x11; je <mov $0x5005d250>` 的双臂选择（非 `+=`）⇒
        //   源码为 if/else 整体赋值；0x0819586e 再把 it->m_endTime 读进具名槽
        //   -0xc（我方原先直读进比较，缺该槽 ⇒ 槽整体浅 4）。
        // ORIG 0x08195864 `jmp 0x0819586b`（真支算完跳汇合）+ 0x08195866
        //   `mov $0x5005d250,%eax` + 0x0819586b 唯一 `mov %eax,-0x10(%ebp)`
        //   ⇒ 三目整体赋值（if/else 会各自落槽，多一对 store）。
        unsigned int beginTime = isGM
            ? ((unsigned int)(m_field8 * -86400) + 0x5005D250u)
            : 0x5005D250u;
        unsigned int endTime = it->m_endTime;
        // [L4fEventAvatar 2026-09-11] 负结论①：内层改「失败早退 + 成功直落」
        //   （`if(!(A&&B)) return false; return true;`）⇒ 4.4 生 `jb <早退>; mov $0;
        //   jmp`，比 ORIG 的 `ja/jae → 尾 mov $0` 更差（实测 82 vs 80），已回滚。
        // 现形：ORIG 0x08195885/0x0819588d 两条跳转与 0x0819582b（外层未命中）
        //   共享尾部唯一 `mov $0`（0x08195896），4.4 对 `return (A&&B);` + 后置
        //   `return false;` 会给内层 0 出口另生 `jmp <epilogue>` 中继（多 2 条）
        //   ⇒ 用 goto 把内层失败并入函数尾唯一 false 出口。
        if (!((beginTime <= time) && (time < endTime)))
            goto L_false;
        return true;
    }
L_false:
    return false;
}

// ORIG 0x08195b06：Guard<Mutex> 后删除 +0x3c list 中 +0x04 == get_acc_id(user) 的记录。
// [b28d-28] DIFF | ORIG 0x08195b06 | N86/P28：list erase 迭代器槽序
// （ORIG -0x18/-0x1c vs 4.4 -0x1c/-0x18）+ 比较方向（P220）；语义等价。
#include "Arad_EventPeriodDataManager/delUserRewardData.inc"
{
    Guard<Mutex> guard(&m_mutex);
    std::list<EventRewardItemArad>::iterator it = m_rewardItemList.begin();
    while (it != m_rewardItemList.end())
    {
        // [L23-r2] ORIG 0x08195b48 每轮 `mov 0x4(%eax),%ebx; call get_acc_id;
        //   cmp %eax,%ebx`（无 accId 槽、无 CSE）⇒ P0/P48 去具名局部、实参位内联；
        //   if 体 ORIG 0x08195b5f `mov -0x18,%eax; mov %eax,-0x1c; call ppEv;
        //   call erase(旧值)` = 4.1 内联 `erase(it++)` 的拷贝+前置自增形
        //   （非 `it = erase(it)`，后者多 sret 回写 2 条）。
        if (it->m_accId == user->get_acc_id())
        {
            // ORIG 0x08195b5f..b87：`mov -0x18,%eax; mov %eax,-0x1c`（旧值落
            // 具名槽）+ 同一符号前置 operator++（0x819613c，else 分支同址调用）
            // + erase(旧值) ⇒ 原码是 tmp/++it/erase(tmp) 三句（`it++` 形会让
            // 4.4 发 out-of-line ppEi + dummy 实参多 2 条）。
            std::list<EventRewardItemArad>::iterator old = it;
            ++it;
            m_rewardItemList.erase(old);
        }
        else
        {
            ++it;
        }
    }
}

// ORIG 0x0819589e：param >= 0 时写入 +0x08。
// [b28 BM11] P223：ORIG js→空块 nop 岛 + body 尾 jmp；4.4 对 if{} 恒
// jump-thread（少 jmp+nop），复现形=「负条件 goto 尾空标签」。
#include "Arad_EventPeriodDataManager/setCreatePeriod_GM.inc"
{
    if (days < 0) goto L_skip;
    m_field8 = days;
L_skip:
    ;
}

}  // namespace ARAD
