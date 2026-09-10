bool CMailBoxHelper::_DoGetPackageItem(CUser* user, CMailBox::CMail const* mail,
                                       std::vector<std::pair<int, int> >& items,
                                       bool bReset, ENUM_ERROR& err)
{
    // ORIG 0x08553936
    // ORIG 0x08553936：`lea -0xd9(%ebp),%eax; call Inven_Item 拷贝构造` ⇒
    // 直接初始化（引用局部 + 二次拷贝会多一枚指针槽并触发 4.4 逐字段展开）。
    Inven_Item pkg = mail->GetPackageItem();
    // P0：ORIG 无 itemIdx 具名局部（每处直读 `mov -0xd7(%ebp),%eax`）⇒
    // 全部内联为 pkg.m_addInfo。
    if (0x28d287 < (unsigned int)pkg.m_addInfo &&
        (unsigned int)pkg.m_addInfo < 0x28d29a)
        return user->DoGetGoldItem(pkg.m_addInfo, items, bReset, err);
    if (IsCeraPointItem((unsigned long)pkg.m_addInfo))
        return user->DoGetCeraPointItem(mail->GetPackageMoney(), bReset, err,
                                        (unsigned long)pkg.m_addInfo);

    WongWork::CCeraShop* shop = GlobalData::s_pCeraShop;
    // ORIG 0x08553936：bReset 与 !bReset 两分支各自一套
    // IsSpecialItem + find_item + _isUnusableSpecialItemInVillage 检查
    // （Ghidra C 0x08553a48 / 0x08553db7 两组，各 1 次调用）。
    if (bReset)
    {
        int specialA = 0;
        int specialB = 0;
        bool specialFlag = false;
        if (shop->IsSpecialItem(pkg.m_addInfo, specialA, specialB, specialFlag) &&
            _isUnusableSpecialItemInVillage(
                G_CDataManager()->find_item(pkg.m_addInfo)))
        {
            if (specialA == 1)
            {
                // [2026-09-10 D06 · 轴 B 试验] ORIG 0x08553a9f..0x08553aa4
                // `mov -0x4c(%ebp),%edx; cmp %edx,%eax; setge %al`（**有符号**，
                // = `GetCapacity() >= specialB`）；我方 `GetCapacity()` 返回
                // unsigned ⇒ `int <= unsigned` 隐式转无符号 ⇒ `setbe` + 提前
                // 物化（+4 条）。加 `(int)` cast 恢复有符号序。
                if (specialB <= (int)user->getCurCharacCargoR()->GetCapacity())
                {
                    err = (ENUM_ERROR)0xd;
                    return false;
                }
            }
            else if (specialA == 5)
            {
                // 背包镜像预演 event items 插入
                std::vector<std::pair<int, int> > evItems;
                CItem* proto = G_CDataManager()->find_item(pkg.m_addInfo);
                // P96/P270：ORIG 用 const 版 begin/end + const_iterator
                // （_ZNKSt6vectorI22STStackablePackageDataSaIS0_EE5beginEv）。
                const std::vector<STStackablePackageData>& vec =
                    static_cast<CStackableItem*>(proto)->m_pkg21c;  // +0x21c
                for (std::vector<STStackablePackageData>::const_iterator it = vec.begin();
                     it != vec.end(); ++it)
                    evItems.push_back(std::make_pair(it->m_field0, it->m_field4));

                CInventory mirror;
                InventoryMemory mem;
                mirror.SetInventoryMemory(&mem);
                mirror.setCopy(*user->getCurCharacInvenRefR());  // ORIG RefR
                int inserted = mirror.tryInsertEventItems(evItems);
                if (inserted != (int)evItems.size())
                {
                    err = (ENUM_ERROR)4;
                    return false;
                }
            }
            return true;
        }
        if (user->getCurCharacInvenR()->tryInsertItemIntoInventory(pkg) < 0)
        {
            err = (ENUM_ERROR)4;
            return false;
        }
        return true;
    }

    int specialA = 0;
    int specialB = 0;
    bool specialFlag = false;
    if (shop->IsSpecialItem(pkg.m_addInfo, specialA, specialB, specialFlag) &&
        _isUnusableSpecialItemInVillage(
            G_CDataManager()->find_item(pkg.m_addInfo)))
    {
        int restrictCode =
            G_CDataManager()->m_itemList->GetRestrictCode(pkg.m_addInfo);
        long curSec = GlobalData::s_systemTime_.getCurSec();
        std::vector<Inven_Item> outItems;
        CCeraShop::stCeraShopItemParam_t param;
        int outCnt = 0;
        // [本波 2026-09-09 真 bug 修复] ORIG 0x08553e94..0x08553eb0 压栈实证：
        // 第 3 参（count）= -0x10(restrictCode 值)、第 4 参（int& restrictCode）
        // = &(-0x78)(outCnt)。原写 `(…, 0, restrictCode, …)` 使 outCnt 恒为 0、
        // restrictCode 被当出参覆写 ⇒ 参数错位。全仓同族调用点
        // （Arad_ItemBonus/CConditionEventManager/W8_CU_A/Dispatcher_*）均为
        // 「第 3 参 restrictCode、第 4 参出参」。
        if (shop->ProcessSpecialItem(user, pkg.m_addInfo, restrictCode, outCnt, curSec, true,
                                     outItems, param, '\0', '\0', false, 1))
        {
            if (outCnt != 0)
                return false;
            if (restrictCode != 0 && !user->IsRestrictedGoods(restrictCode))
            {
                user->SetRestrictedGoods(restrictCode);
                shop->SaveFeaturedIdx(user, restrictCode, (int)curSec, 0);
                user->SendRestrictedGoods(true, restrictCode);
            }
            if (pkg.m_addInfo == 0xc1 &&
                !shop->ProcessSpecialItem(user, 0x10, 0xc352, outCnt, curSec,
                                          true, outItems, param, '\0', '\0', false, 1))
            {
                LogManager::logFormat(
                    1, "MailBox.cpp",
                    "static bool WongWork::CMailBoxHelper::_DoGetPackageItem(CUser*, "
                    "const WongWork::CMailBox::CMail*, "
                    "WongWork::CMailBoxHelper::IntPairVector&, bool, ENUM_ERROR&)",
                    0x34c, "AuctionVIP Error - Present to friend(add private-store)");
            }
        }
        return true;
    }

    int slot = user->getCurCharacInvenW()->insertItemIntoInventory(
        pkg, (eItemAddReason)0x15, true, false);
    if (slot < 0)
        return true; // ORIG：失败亦返回 1（不置错误码）
    items.push_back(std::make_pair((ENUM_ITEMSPACE)0, slot));
    return true;
