// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 StackablePackage/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "StackablePackage.h"

#include <algorithm>

#include "CDataManager.h"
#include "CItem.h"

// [A31-GI · P338/P327] 补 <iostream> 恢复 __ioinit；setupEventProcessors 前移为本 TU
// 首个强外部符号 ⇒ GI 键名
// `_GLOBAL__I__ZN16StackablePackage19EventProcessManager20setupEventProcessorsEv`（ORIG 0x8281bde）。
#include <iostream>

namespace StackablePackage
{
struct EventProcessManager::EventProcessManagerImpl
{
    std::map<EVENT_TYPE, std::map<int, EventProcessor*> > processors;
};

#include "StackablePackage/setupEventProcessors.inc"
{
}

#include "StackablePackage/EventProcessManager.inc"
    : m_impl(new EventProcessManagerImpl)
{
    setupEventProcessors();
}

#include "StackablePackage/dtor_EventProcessManager.inc"
{
    destroy();
}

// ORIG 0x08281a2a (0x70)：无 null 检查。GetEventId 先求值存 -0x10，
// pair<const int,EventProcessor*> 直接双参构造（0x08281ffa C1IiRS3_EEOT_OT0_），
// 再 GetEventType 求值 -0xc，operator[] 取内层 map，insert(pair)。
#include "StackablePackage/registerEventProcessor.inc"
{
    m_impl->processors[processor->GetEventType()].insert(
        std::pair<int const, EventProcessor*>(processor->GetEventId(), processor));
}

// ORIG 0x08281a9a (0x104)：双层迭代 delete；无 clear()（map 由 Impl D1 清空）。
// b28d-lane25：P167 差集实证 ORIG 走 `_Rb_tree_iterator<...>::deEv`（operator*）
// 而非 ptEv（operator->）⇒ 源码用 (*it).second 形。
#include "StackablePackage/destroy.inc"
{
    for (std::map<EVENT_TYPE, std::map<int, EventProcessor*> >::iterator group =
             m_impl->processors.begin(); group != m_impl->processors.end(); ++group)
    {
        std::map<int, EventProcessor*>& inner = (*group).second;
        for (std::map<int, EventProcessor*>::iterator it = inner.begin();
             it != inner.end(); ++it)
        {
            EventProcessor* p = (*it).second;
            if (p != 0)
            {
                delete p;
                p = 0;
            }
        }
    }
}

// ORIG 0x082819c6 (0x63)：early-return 结构（P16）：item==0 → false、
// !is_stackable → false、最后 GetItemType()==0x26（sete %al 直返）。
// [b28c-11] NEAR 封顶（34/34 条）：残余仅 call/jmp 目标显示差（N7 族寄存器分配）。
#include "StackablePackage/isStackablePackageItem.inc"
{
    CItem* item = G_CDataManager()->find_item(itemId);
    if (item == 0)
        return false;
    if (!item->is_stackable())
        return false;
    CItem* it = item;
    return it->GetItemType() == 0x26;
}

// ORIG 0x0828186c (0x15a)：isStackablePackageItem 失败 → 0；外层 find/end 比较；
// 内层 std::find_if(begin, end, FindControllableEventProcessor(itemId))；
// == end → 0；否则 (*it).second->Process(user, itemId)（vtable+0xc 虚调用）。
// b28d-lane25：P167 差集实证 ORIG 走 deEv（operator*），源码全部改 (*it).second 形。
#include "StackablePackage/Process.inc"
{
    if (!isStackablePackageItem(itemId))
        return 0;
    std::map<EVENT_TYPE, std::map<int, EventProcessor*> >::iterator group =
        m_impl->processors.find(type);
    if (m_impl->processors.end() == group)
        return 0;
    std::map<int, EventProcessor*>& inner = (*group).second;
    std::map<int, EventProcessor*>::iterator it =
        std::find_if(inner.begin(), inner.end(),
                     FindControllableEventProcessor(itemId));
    if (inner.end() == it)
        return 0;
    return (*it).second->Process(user, itemId);
}

static EventProcessManager& G_EventProcessor()
{
    static EventProcessManager instance;
    return instance;
}
}
