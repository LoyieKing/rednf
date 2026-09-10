void CItemLock::SendItemLockList(CUser* user)
{
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xfb);
    int countIndex = packet.get_index();
    int count = 0;
    // [b28d-41] ORIG 0x85428a6 `mov -0x20(%ebp),%eax; mov %eax,0x4(%esp)` ⇒
    // 实参来源是具名 count 变量（P97），不是字面量 0。
    packet.put_short(count);
    // [b28d-41] ORIG 在 begin() 之前预清 `movl $0,-0x1c`（state）与
    // `movl $0,-0x38`（remainSec）⇒ 两者声明于循环外；state 为 int
    // （`mov %eax,-0x1c` + `cmpl $0,-0x1c`），非 uchar。
    int state = 0;
    int remainSec = 0;

    for (std::vector<stItemLockRef>::iterator it = m_lockList.begin();
         it != m_lockList.end(); ++it)
    {
        stItemLockRef& ref = *it;
        // [b28d-solo-CItemLock] ORIG 槽序：ref -0x18、outSpace -0x11、item -0x10
        // （4.1 按声明序，先声明占深槽）⇒ 两局部提至循环体头部、outSpace 先声明。
        unsigned char outSpace;
        Inven_Item* item;
        state = GetItemLockInfo((unsigned char)ref.m_lock, remainSec);
        // [b28d-solo-CItemLock] P225/P197④：ORIG 0x854285b `cmpl $0,-0x1c;
        // sete %al; test %al,%al; je` 的槽比零 setcc 物化用 while+goto 复现
        // （4.4 直写 `if (state==0)` 恒折 `cmpl;jne`；while 条件保留 setcc 段）。
        while (state == 0)
        {
            // 已解锁：清除物品上的锁位并同步（ORIG 经 GetCurCharacInventoryRef
            // 全分派：含 cargo(space=2)/title(space=0x13..0x16)）。
            // [b28d-41] ORIG 0x85428ce `movzbl 0x1(%eax),%eax; movzbl %al,%eax`
            // 双段零扩展 ⇒ 实参写 (unsigned char)（char 直传生 cbtw 符号扩展）。
            item = user->GetCurCharacInventoryRef(
                (unsigned char)ref.m_space, ref.m_slot);
            if (item != 0)
            {
                item->m_amp.SetLock(0);
                UpdateItem(user, item);
            }
            goto send_list_next;   // 跳过 else 路径（ORIG A 路尾 jmp）
        }
        {
            outSpace = (unsigned char)ref.m_space;
            // [b28d-solo-CItemLock] P225 推广：ORIG 0x85428de `cmp $1,%al;
            // sete %al; test %al,%al; je` ⇒ && 必须拆嵌套 if（合并形 4.4 折直跳）。
            // 内层 slot 比较为无符号（ORIG 0x85428f3 `ja`），原码恒真短路照抄。
            if (user->getCurChannelEquipslotSwitch() == 1)
            {
                if ((unsigned short)ref.m_slot > 9 ||
                    (unsigned short)ref.m_slot < 0x16)
                {
                    if (ref.m_space == 3)
                        outSpace = 0x12;
                    else if (ref.m_space == 0x12)
                        outSpace = 3;
                }
            }
            packet.put_byte(outSpace);
            // [b28d-solo-CItemLock] ORIG 0x8542936 `movzwl 0x2(%eax),%eax;
            // movzwl %ax,%eax` 双段零扩展 ⇒ 实参 (unsigned short)（signed 生 cwtl）。
            packet.put_short((unsigned short)ref.m_slot);
            packet.put_byte(state);
            if (state == 2)
            {
                int remain = remainSec + ITEM_UNLOCK_WAITING_TIME -
                             GlobalData::s_systemTime_.getCurSec();
                if (remain < 0)
                    remain = 0;
                packet.put_int(remain);
            }
            ++count;
        }
    send_list_next:;
    }
    packet.put_byte(countIndex, count);
    packet.finalize(true);
    user->Send(packet);
    m_lockList.clear();
