#include "StackablePackage.h"

#include <algorithm>

#include "CDataManager.h"
#include "CItem.h"

namespace StackablePackage
{
struct EventProcessManager::EventProcessManagerImpl
{
    std::map<EVENT_TYPE, std::map<int, EventProcessor*> > processors;
};

EventProcessManager::EventProcessManager()
    : m_impl(new EventProcessManagerImpl)
{
    setupEventProcessors();
}

EventProcessManager::~EventProcessManager()
{
    destroy();
}

void EventProcessManager::setupEventProcessors()
{
}

// ORIG 0x08281a2a (0x70)：无 null 检查。GetEventId 先求值存 -0x10，
// pair<const int,EventProcessor*> 直接双参构造（0x08281ffa C1IiRS3_EEOT_OT0_），
// 再 GetEventType 求值 -0xc，operator[] 取内层 map，insert(pair)。
void EventProcessManager::registerEventProcessor(EventProcessor* processor)
{
    m_impl->processors[processor->GetEventType()].insert(
        std::pair<int const, EventProcessor*>(processor->GetEventId(), processor));
}

// ORIG 0x08281a9a (0x104)：双层迭代 delete；无 clear()（map 由 Impl D1 清空）。
// b28d-lane25：P167 差集实证 ORIG 走 `_Rb_tree_iterator<...>::deEv`（operator*）
// 而非 ptEv（operator->）⇒ 源码用 (*it).second 形。
void EventProcessManager::destroy()
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
bool EventProcessManager::isStackablePackageItem(int itemId) const
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
int EventProcessManager::Process(EVENT_TYPE type, CUser& user, int itemId)
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
