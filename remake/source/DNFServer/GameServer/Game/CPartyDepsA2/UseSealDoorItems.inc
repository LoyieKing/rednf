bool CParty::UseSealDoorItems(std::vector<int>& itemIds)
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
            if (slot != -1)
            {
                CInventory* invW = m_member[local_14].m_pUser->getCurCharacInvenW();
                if (!invW->delete_item((INVEN_TYPE)1, slot, needCount,
                                       (eItemDelReason)0x17, true))
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
            }
            else
            {
                m_battleField.m_sealDoorItemsOk = 0;
                return false;
            }
            ++it;
        }
    }

    m_battleField.m_sealDoorItemsOk = 1;
    return true;
