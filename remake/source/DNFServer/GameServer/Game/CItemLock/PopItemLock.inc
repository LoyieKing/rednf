void CItemLock::PopItemLock(CUser* user)
{
    if (m_lockTimer.GetTimerCount() == 0)
        return;

    int now = GlobalData::s_systemTime_.getCurSec();

    std::map<unsigned char, stItemLockInfo>::iterator it = m_lockMap.begin();
    while (it != m_lockMap.end())
    {
        // [b28d-solo-CItemLock] ORIG 0x8542bfa `mov $1,%eax; jmp; mov $0,%eax;
        // test %al,%al; je <body>` —— 正极性 `if (A && B)` + 体尾 continue
        // 复现该物化与 ORIG 的「体尾 jmp 条件、++it 块置体后」布局（旧 `if
        // (!(A&&B)) { ++it; continue; }` 形 4.4 产反向极性且 ++it 块前置）。
        if (now >= it->second.m_unlockSec + ITEM_UNLOCK_WAITING_TIME &&
            it->second.m_state == 2)
        {
            // [b28d-41] ORIG 0x8542c29 直接把 &space/-&slotVal 入参（lea
            // -0x20/-0x24 后即 call），无 `movl $0,-N` 预清 ⇒ 原码不初始化这两个
            // 出参（P131/P171）。
            ENUM_ITEMSPACE space;
            int slotVal;
            if (!((const CUserCharacInfo*)user)->FindItemLock(
                    it->first, space, slotVal, (CUser*)user))
                return;  // ORIG：定位失败立即退出（保留遍历位置）

            Inven_Item* item =
                user->GetCurCharacInventoryRef((unsigned short)space,
                                               (unsigned short)slotVal);
            if (item != 0)
            {
                item->m_amp.SetLock(0);
                UpdateItem(user, item);
            }

            // [b28d-41] ORIG 0x8542cc9 `mov -0x20(%ebp),%eax; cmp $3; jne;
            // movl $0x12,-0x20(%ebp)` —— 直接改 space 出参本身，无 sendSpace 局部
            // （具名副本多 2 条 + 一槽，P0/P171）。
            // [b28d-solo-CItemLock] P225 推广：ORIG 0x8542c55 `cmp $1,%al;
            // sete %al; test %al,%al; je` ⇒ && 拆嵌套 if（合并形 4.4 折直跳 +
            // 多 bool 物化块）。
            if (user->getCurChannelEquipslotSwitch() == 1)
            {
                if (slotVal > 9 && slotVal < 0x16)
                {
                    if (space == (ENUM_ITEMSPACE)3)
                        space = (ENUM_ITEMSPACE)0x12;
                    else if (space == (ENUM_ITEMSPACE)0x12)
                        space = (ENUM_ITEMSPACE)3;
                }
            }

            SendItemUnlock(user, (int)space, slotVal);
            m_lockStack.push(it->first);
            // [b28d-41] ORIG 0x8542d?? 用 ppEi（后置 it++ 带 sret 临时）⇒ 原码
            // erase(it++)（P28 后置增量），具名 next + 前置 ++next 形多一槽+一条调用。
            m_lockMap.erase(it++);

            m_lockTimer.DecreaseItemLockTimer();
            alter();

            // [b28d-41] ORIG 0x8542d71 直接读 item->m_addInfo2（无
            // `if (item != 0)` 守卫）⇒ 删除多余条件（P171）。
            // [b28d-solo-CItemLock] 内联 receiver（ORIG 0x8542d88 无 history 槽）。
            ((cUserHistoryLog*)((char*)user + 0x79700))
                ->ItemLockUsingLog(3, (int)space, slotVal,
                                   item->m_addInfo, item->m_addInfo2);
            // 体尾 continue：erase(it++) 已推进迭代器，跳过下方 ++it（ORIG 体尾
            // jmp 至条件块）。
            continue;
        }
        ++it;
    }

    m_lockTimer.NextItemLockTimer(user);
