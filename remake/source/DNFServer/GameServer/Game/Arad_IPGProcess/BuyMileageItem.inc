int Arad_MileageProcess::BuyMileageItem(CUser* user, int param2, unsigned long itemIdx,
                                        unsigned short stack, int count, long seq,
                                        int param7, char c1, char c2, int price,
                                        int a11, int a12, char const* str,
                                        WongWork::CCeraShop::stCeraShopItemParam_t& param)
{
    int len;   // ORIG -0x98（形参 spill 区 -0x8c..-0x94 之后）
    CCeraShopGoods* goods = reinterpret_cast<CCeraShopGoods*>(G_CDataManager()->FindGoods(param2));
    if (goods == 0)
        return 0x15;

    // ORIG 0x0819dd9d：`movl $0x0,-0x28`（item）先于 localItem ctor（0x0819ddaa）。
    CItem* item = 0;
    Inven_Item localItem;

    // ORIG 0x0819ddba：`cmp $0x1,%eax; setne %al; test; je` ⇒ 判定内联（无 subGroup 槽）。
    if (goods->GetSubGroupIndex() != 1)
    {
        item = G_CDataManager()->find_item((int)itemIdx);
        if (item == 0)
            return 0x15;
        item->make_item(localItem);
        localItem.m_addInfo = (int)itemIdx;
        localItem.m_addInfo2 = (int)stack;
    }

    if (user->GetMileage() < price)
        return 0x3ed;

    int outCodeA = 0;  // local_30（creature 结果）
    int outCodeB = 0;  // local_34（错误码）
    std::vector<Inven_Item> items;

    // ORIG 0x0819de57：第二次调用结果落 -0x24 供 switch 复用。
    int subGroup = goods->GetSubGroupIndex();
    switch (subGroup)
    {
    case 0:
    {
        // P52：receiver 链式（ORIG c1/c2/itemIdx 先物化 edi/esi/ebx 跨 getter 保活）。
        int slot = user->getCurCharacInvenW()->AddAvatarItem(
            (int)itemIdx, param7, false, c1, c2, str, (eAvatarItemAddReason)0, false, 0);
        if (slot < 0)
        {
            const char* msg = g_scriptStringManager_.findString(4, "game_server_msg_104", (bool*)0);
            len = (int)strlen(msg);
            // P0：ORIG 无 characNo 槽 ⇒ 实参内联。
            WongWork::CMailBoxHelper::ReqDBSendNewAvatarMailCashShop(
                user, user->getCurCharacNo(), (int)itemIdx, param7, c1, c2, str, msg, len);
        }
        else
        {
            user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)1, slot);
        }
        break;
    }
    case 1:
        // esp: shop / user / (uint)stack / 1（同 CashShopModeCoin 非 gift 分支）
        reinterpret_cast<WongWork::CCeraShop*>(m_vm)->_processCoin(user, stack, true);
        break;
    case 2:
    case 7:
    case 9:
        reinterpret_cast<WongWork::CCeraShop*>(m_vm)->_processItem(
            user, localItem, count, outCodeB, seq, false, true, items, param);
        break;
    case 3:
    {
        // ORIG 0x0819e080：getExpirationDate 先 call（%ebx 保活）→ getUsablePeriod（P0 内联）。
        reinterpret_cast<WongWork::CCeraShop*>(m_vm)->_processCreature(
            user, localItem, outCodeA, outCodeB, true, item->getUsablePeriod(),
            item->getExpirationDate());
        break;
    }
    default:
        break;
    }

    if (outCodeB == 0)
    {
        BuySuccess(user, param2, (int)stack, outCodeA, price, a11, a12, items);
        // ORIG 0x0819e18d：`movzwl -0x8c(%ebp),%edx; mov %edx,0x1c(%esp)` ⇒ 第 7 实参
        // 是 (int)stack（原写 0 为真语义 bug）；0x20..0x28 = 0ULL 的低/高 + 0。
        m_vm->BillingRecord(user, param2, subGroup, price, str, "mileage item",
                            (int)stack, 0ULL, 0);
    }

    return outCodeB;
