// ---- VerifyPrivateStoreItem（ORIG T 0x85c874c，报告 VerifyPrivateStoreItem.md）----
// ISeller 槽 +0x1c / IBuyer 槽 +0x04。买家购买前校验并回发 0x195 详情包：
// 计价/交易限制/每日金额校验通过后，按条目类型序列化（avatar 附剩余时间、
// 宝石槽 30B 与扩展信息 4B），put_packet 后发买家。
// [b28d-24] DIFF 残47: 帧/槽分配（P112/N82）。ORIG 0x085c874c
// [b28d-solo-CPrivateStoreMgr] 残 76 行：P225 while 形已用于
// getCurCharacR()==0 / find_item==0 两处；余为 CErrorHandler::Do 调用序 +
// put_binary 段顺序 + jmp 偏移差（帧/槽 N82）。
// [L14] 本轮复核：456/456 助记符序列全等；残 9 行 = `lea -0x28(%ebp),%eax;
// mov %eax,0x18(%esp)`（makeRequest 实参装载时刻）与 ORIG 0x085c874c 段序相反 ⇒ P75 封顶。
bool CPrivateStore::VerifyPrivateStoreItem(int slot, int count)
{
    m_errorHandler.Set(m_pBuyer, 1, 0x195);

    if (m_pBuyer == 0)
        return false;
    if (m_pParent == 0)
    {
        m_errorHandler.Do(0x3a);
        return false;
    }
    if (!IsExistBuyerList(m_pBuyer))
    {
        m_errorHandler.Do(0x15);
        return false;
    }
    // [b28d-CPrivateStoreMgr] P225：ORIG 0x085c8804 `test %eax,%eax; sete %al;
    // test %al,%al; je` 四段物化 ⇒ while 形（body 内 return，语义等价）。
    while (m_pBuyer->getCurCharacR() == 0)
    {
        m_errorHandler.Do(1);
        return false;
    }
    if (slot < 0 || 0xd < slot)
    {
        m_errorHandler.Do(0x39);
        return false;
    }
    // 卖家侧角色必须有效。
    if (m_pSeller == 0 || m_pSeller->getCurCharacR() == 0)
    {
        m_errorHandler.Do(0x3a);
        return false;
    }

    // [b23S3C] P48 两次独立 operator[]：ORIG 0x169 首次结果 eax 直读 +0x3d（无
    // 槽），0x1ee 二次 call 结果才落 -0x18 槽（后续全经 -0x18）⇒ 状态判定用完整
    // 下标表达式，绑定延后到 itemInfo 预清（0x1d7 movl $0,-0x1c 两段式）之后。
    // [b23S3C] ORIG 0x16e movzbl 0x3d(%eax) 直读字段（-O0 下 GetState call 是
    // 本仓 out-of-line 化的产物；ORIG 此处无 GetState call，p167 EXTRA 实证）。
    if ((int)m_items[slot].m_flag3d != 3)
    {
        m_errorHandler.Do(1);
        return false;
    }
    if (m_pParent->IsBusyPrivateStore(m_pBuyer))
    {
        m_errorHandler.Do(0x3c);
        return false;
    }

    CItem* itemInfo = 0;
    PrivateStoreItem& storeItem = m_items[slot];
    itemInfo = G_CDataManager()->find_item((int)storeItem.m_item.m_addInfo);
    // P225：ORIG 0x085c895e `cmpl $0,-0x1c; sete %al; test %al,%al; je` 四段物化。
    while (itemInfo == 0)
    {
        m_errorHandler.Do(0x11);
        return false;
    }

    int attachType = itemInfo->GetAttachType();
    if (attachType != 0)
    {
        if (attachType == 3)
        {
            if ((int)storeItem.m_item.m_seal != 1)
            {
                m_errorHandler.Do(0x11);
                return false;
            }
        }
        else
        {
            // [b28d-CPrivateStoreMgr] ORIG 0x085c89d0 `cmpl $7; jne <Do(0x11)块>`
            // ⇒ 原码是**正条件嵌套**（==7 then 直落 / else Do 块），负条件早返
            // 在 4.4 产 `je <跳过>` 反极性（P59/P222）。
            if (attachType == 7)
            {
                if (!storeItem.m_item.GetTradeLimitCount())
                    return false;   // ORIG：绑定类无次数静默拒绝
            }
            else
            {
                m_errorHandler.Do(0x11);
                return false;
            }
        }
    }
    if (IsCreatureItemType((unsigned int)(unsigned char)storeItem.m_item.m_itemType))
    {
        m_errorHandler.Do(0x11);
        return false;
    }

    // [b28d-CPrivateStoreMgr] P0/P88：ORIG 0x085c8a4a 直接 lea 三个出参槽
    // （无前置 movl $0x0）⇒ 原码是裸声明（出参由 GetItemPrice 写）。
    // [b28d-G11 P331] 声明序 = 槽序（同尺寸标量区按声明序从高到低）。ORIG 直证：
    // 0x085c8a51 `lea -0x20 → 0x14(%esp)`（a5=dealGoldA）+ 0x085c8a90
    // `mov -0x20 → 0x8(%esp)`（IsPossibleDeal a2=dealGoldA）⇒ dealGoldA=-0x20 先声明；
    // 0x085c8a4a `lea -0x24 → 0x18(%esp)`（a6=dealGoldB）+ 0x085c8a96 读 -0x24→0xc
    // ⇒ dealGoldB=-0x24；0x085c8a58 `lea -0x28 → 0x10(%esp)`（a4=price）⇒ price=-0x28。
    unsigned int dealGoldA;
    unsigned int dealGoldB;
    int price;
    if (!GetItemPrice(itemInfo, storeItem, count, price, dealGoldA, dealGoldB))
        return false;
    if (!IsPossibleDeal(price, dealGoldA, dealGoldB))
        return false;
    // [BigStore REAL-BUG] ORIG 0x085c8ae8 实参 = GetItemPrice out1 槽 -0x28（=price，
    // 0x085c8a58 lea -0x28 → esp+0x10 实证），旧写 dealGoldA（out2 恒 0）⇒ 校验被短路。
    // [b28d-CPrivateStoreMgr] P133/P225 推广：ORIG 0x085c8acf/0x085c8aed 两条件
    // **各自** `xor $1; test %al,%al; je <succ>`（独立守卫、共享 succ 出口）⇒ 源码是
    // 嵌套 if 而非 `&&` 短路链（合并形在 4.4 折成单段 jne，少 xor/je 各一条）。
    if (!m_pBuyer->GetReliablePerson())
    {
        if (!m_pBuyer->IsAvailableCurCharacTradeGoldDaily((unsigned int)price))
        {
            m_errorHandler.Do(0x46);
            return false;
        }
    }

    PacketGuard packet;
    packet.put_header(1, 0x195);
    packet.put_byte(1);
    packet.put_short(slot);
    packet.put_int(storeItem.m_field40);                       // +0x40 售价
    packet.put_int((int)storeItem.m_item.m_addInfo);           // +0x02 物品索引
    int nowTick = OS_API::GetDateTimeTick();
    if ((int)storeItem.m_item.m_itemType == Inven_Item::ITEM_TYPE_AVATAR)
    {
        packet.put_int(
            m_pSeller->getCurCharacInvenR()->GetAvatarItemMgrR()->GetRemainDate(
                (int)storeItem.m_item.m_addInfo2, nowTick));
    }
    else
    {
        packet.put_int(count);
    }
    packet.put_byte((int)storeItem.m_item.GetItemAttr());
    packet.put_short((int)storeItem.m_item.m_endurance);          // +0x0b 耐久
    packet.put_int((int)storeItem.m_item.m_extendInfo);            // +0x0d
    packet.put_byte((int)storeItem.m_item.m_amp.getAbilityType());
    packet.put_short((int)storeItem.m_item.m_amp.getAbilityValue());
    if ((int)storeItem.m_item.m_itemType == Inven_Item::ITEM_TYPE_AVATAR)
    {
        packet.put_int(0x1e);
        packet.put_binary(
            (char const*)m_pSeller->getCurCharacInvenR()->GetAvatarItemMgrR()->getJewelSocketData(
                (int)storeItem.m_item.m_addInfo2),
            0x1e);
        void* expansion =
            m_pSeller->getCurCharacInvenR()->GetAvatarItemMgrR()->GetExpansionInfo(
                (int)storeItem.m_item.m_addInfo2);
        packet.put_int(4);
        // [b28d-solo-L43] ORIG 0x085cae0x 段布局：`cmpl $0,-0xc; je L` 后 then 为
        // put_binary(expansion)（直落）、else 为 defaultExpansion 块（je 目标在后）
        // ⇒ 条件取反正向 + 两臂对调（P59/P222），4.4 才产 ORIG 的 je→后块布局。
        if (expansion != 0)
        {
            packet.put_binary((char const*)expansion, 4);
        }
        else
        {
            stAvatarExpansionInfo_t defaultExpansion;
            defaultExpansion.init();
            packet.put_binary((char const*)&defaultExpansion, 4);
        }
    }
    packet.put_packet(storeItem.m_item);
    packet.finalize(true);
    m_pBuyer->Send(packet);
    return true;
}