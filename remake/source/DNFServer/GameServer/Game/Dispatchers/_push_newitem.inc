int Dispatcher_Compound_ExtreamItem::_push_newitem()
{
    // ORIG 0x08225b54：inven=getCurCharacInvenW(m_user) → insertItemIntoInventory(
    // m_resultItem, 0x30, 1, 1) → m_newSlot=返回值；返回 m_newSlot > -1
    // [G1Frest 2026-09-10] ORIG 0x08225b6b `mov %eax,%edx`（callee-saved 保活，无栈槽）
    //   ⇒ 原码是链式调用；旧码具名 `CInventory* inven`(-0xc) 多 store/load 且帧 +0x10。
    m_newSlot = (short)m_user->getCurCharacInvenW()->insertItemIntoInventory(m_resultItem,
                                                      (eItemAddReason)0x30, true, true);
    return m_newSlot > -1 ? 1 : 0;
