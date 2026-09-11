// ============================================================================
// UseSealDoorItems —— ORIG T 0x085a8038（报告 CParty/UseSealDoorItems.md）
//
// [b28d-solo-CPartyDepsA2 判词·N 封顶] 185/185 条、助记符序列逐段同构；
// 已修：result/minLevel/itemId 具名局部删除（P0/P171）、非 const 迭代器
// （P96）、cMyTrace 全表达式临时链（P227）、slot 提到函数头、直接 return。
// 残余三族（语义等价）：
//  ① N95 槽基差 4：ORIG 槽基 local_14@-0x10/slot@-0x14/needCount@-0xc，
//     4.4 恒 ebp-0x18 起（帧顶多留 4B gap；double framePad 反致下移，实测）。
//  ② N88/P197④ 槽比 setne 物化：ORIG 0x085a8153 `cmpl $0xffffffff,-0x14;
//     setne %al; test %al,%al; je`（4.1 槽比零产物）；4.4-O0 对槽比恒出
//     `cmpl;je` 直跳——P197④ 探针实证源形不可复现（P225 while+break 形
//     要求单一条件+循环退出，本处 else 早返含 return false 无法单比较承载）。
//  ③ N7/N18b 寄存器角色差（slot 装载 eax↔edx）。
// ============================================================================
// [b28d-round5 §6.5 串盲区] ORIG .rodata 实串 =
//   `bool CParty::UseSealDoorItems(IntVector&)`（我方原为展开
//   `std::vector<int, std::allocator<int> >&`，串长差 25 字节 + 日志契约不符）。
//   全局 typedef 与 ARAD_Database.h:31 同型（C++ 允许同型重声明），
//   仅改 prett 拼写，不改 mangled/sizeof/布局/codegen。
typedef std::vector<int> IntVector;
bool CParty::UseSealDoorItems(IntVector& itemIds)
{
    // [b28d-solo-CPartyDepsA2] ORIG 0x085a8042 函数头 `movl $0x0,-0x14`
    // （slot 声明处初始化，4.1 函数级）→ 0x085a8049 tmpItem 构造 →
    // 0x085a8054 local_14=0；ORIG 无 bool result 局部（尾 `mov $0x1,%eax`
    // 直返常量，P171 照抄：不写 result）⇒ slot 提到函数头、直接 return。
    int slot = 0;
    Inven_Item tmpItem;   // ORIG 栈上构造后未使用，保持一致

    for (int local_14 = 0; local_14 <= 3; ++local_14)
    {
        if (!_checkValidUser(local_14))
            continue;
        if (CheckHellDungeonFreepassItemHaveAndDel(m_member[local_14].m_pUser,
                                                   true))
            continue;

        // [b28d-solo-CPartyDepsA2] ORIG 0x085a80b3/0x085a8275 段 begin/end/
        // operator!= 均非 const 实例 ⇒ P96/P270 非 const iterator。
        std::vector<int>::iterator it = itemIds.begin();
        while (it != itemIds.end())
        {
            // [b28d-solo-CPartyDepsA2] ORIG 0x085a80de `call; cmp $0x2c,%eax;
            // setle`（无 minLevel 槽）⇒ 内联表达式直用（P0）；ORIG 0x085a8108
            // 二次调用 get_min_level 作 needCount 实参，故两处各自内联。
            if (m_battleField.m_pDungeon->get_min_level() <= 0x2c)
            {
                m_battleField.m_sealDoorItemsOk = 0;
                return false;
            }

            int needCount = getHellPartyEntranceItemNeedCount(
                m_battleField.m_pDungeon->get_min_level());
            // [b28d-solo-CPartyDepsA2] ORIG 0x085a8123 `mov (%eax),%ebx`：
            // *it 值经 ebx 直传 check_item_exist，无 itemId 具名局部。
            slot = m_member[local_14].m_pUser->getCurCharacInvenR()
                       ->check_item_exist(*it);
            // [b28d-round5] ORIG 0x085a8153 `cmpl $0xffffffff,-0x14(%ebp);
            // setne %al; test %al,%al; je 085a8259`（槽比物化，4 条）。4.4 的
            // `if (slot != -1)` 直形恒折 `cmpl; je`（少 setne/test 两条；探针
            // /tmp/probe_party/p.cpp f1 vs f2 实证）；`while (slot != -1) {
            // body; goto OUT; }` 形逐条复现物化，且 goto 保住 ORIG 的分离跳靶
            // （je→slot==-1 块 0x085a8259；then 尾 jmp→0x085a826a）。
            while (slot != -1)
            {
                // [b28d-round5] 去具名 invW：ORIG 0x085a817c call 返回后 %eax
                // 直落 delete_item 的 this 实参位（0x085a81a7 `mov %eax,(%esp)`），
                // 具名指针多 1 条槽写 + 1 条槽重装（P0 槽-局部律，-2 条）。
                if (!m_member[local_14].m_pUser->getCurCharacInvenW()->delete_item(
                        (INVEN_TYPE)1, slot, needCount, (eItemDelReason)0x17, true))
                {
                    // [b28d-solo-CPartyDepsA2] ORIG 0x085a81d5：getCurCharacNo
                    // 在 cMyTrace ctor **之前**求值并入 %ebx 跨 ctor 保活 ⇒
                    // P227 全表达式临时链（具名 trace 反产 ctor 后求值 N80）。
                    cMyTrace(__PRETTY_FUNCTION__, 0x1d22, 5)(
                        "CParty::UseSealDoorItems, delete_item failed , "
                        "User ch =%d , %d %d",
                        m_member[local_14].m_pUser->getCurCharacNo(), slot,
                        needCount);
                }
                else
                {
                    m_member[local_14].m_pUser->SendUpdateItem(
                        (CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, slot);
                }
                goto sealDoorItemContinue;
            }
            m_battleField.m_sealDoorItemsOk = 0;
            return false;
        sealDoorItemContinue:
            ++it;
        }
    }

    m_battleField.m_sealDoorItemsOk = 1;
    return true;
}