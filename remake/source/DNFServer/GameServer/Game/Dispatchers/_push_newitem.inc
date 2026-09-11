// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08225b54 orig/ours=58/59 ndiff-blocks=6
//   帧差 sub $58 vs $68 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x58,%esp | mov    %eax,%edx | mov    0x90(%eax),%ecx
int Dispatcher_Compound_ExtreamItem::_push_newitem()
{
    // ORIG 0x08225b54：inven=getCurCharacInvenW(m_user) → insertItemIntoInventory(
    // m_resultItem, 0x30, 1, 1) → m_newSlot=返回值；返回 m_newSlot > -1
    // [G1Frest 2026-09-10] ORIG 0x08225b6b `mov %eax,%edx`（callee-saved 保活，无栈槽）
    //   ⇒ 原码是链式调用；旧码具名 `CInventory* inven`(-0xc) 多 store/load 且帧 +0x10。
    m_newSlot = (short)m_user->getCurCharacInvenW()->insertItemIntoInventory(m_resultItem,
                                                      (eItemAddReason)0x30, true, true);
    // [g6 2026-09-11] 尾块序：ORIG 0x08225b54 尾 `test %ax,%ax; jns <mov $1>`+
    //   fall `mov $0;jmp`（then=return 0 才是 4.4 的 fall-through 侧）。
    //   `> -1 ? 1 : 0` 产反向 `js <mov $0>`（探针 /tmp/k1sbx/g6/probe/p2.cpp f1/f3
    //   与 f4 对照：`if (x<0) return 0; return 1;` 才是 jns+mov$1 形）。
    if (m_newSlot < 0)
    {
        return 0;
    }
    return 1;
}