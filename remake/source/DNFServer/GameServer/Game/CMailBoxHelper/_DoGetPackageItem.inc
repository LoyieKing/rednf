// [b28d-L8] N82 | 超大函数帧差 0xca30 vs 0xca60 + 结构槽序（ORIG 0x08557xxx）
// [b28d-solo-CMailBoxHelper] 修法 + N 封顶（§6.2，p167 CLEAR 63/63）：
// ①`Inven_Item const& pkgRef` + 二次拷贝 → 直接初始化
//   `Inven_Item pkg = mail->GetPackageItem()`（ORIG 0x08553936 单次拷贝构造；
//   旧形多指针槽 + 4.4 逐字段展开，565→530）；
// ②删 `int itemIdx` 具名局部改内联 pkg.m_addInfo（P0：ORIG 每处直读
//   -0xd7(%ebp)；保留具名版 530、内联版 533，取 P0 合规形）。
// [b28d-solo-CMailBoxHelper 本波实测] 两项旋钮均反劣已回退：①删 `shop` 局部
// 改 `GlobalData::s_pCeraShop->` 每处直读（P0/P183 方向）⇒ 533→534 且短路块
// 结构重排；②补 `int framePad` 撑帧（P31/P190）⇒ 槽 -0xd5→-0xdd 过头 4。
// 残余 = sret/迭代器槽序与帧平移（N82/P112，523 vs 533）。
// [本波 2026-09-09 真 bug] ProcessSpecialItem 两处实参错位已修（第 3 参 count
// ← restrictCode 值、第 4 参 int& restrictCode ← outCnt；ORIG
// 0x08553e94..0x08553eb0 / 0x08553fa3..0x08553fc4 压栈序实证 + 全仓同族
// 调用点一致）⇒ ndiff 334→277，523 vs 534（+1 条为参数序寄存器分配）。
// [2026-09-11 本轮] gap −11 → **−1**（我方 525 / ORIG 524）。修复：
//  ①`&&` 短路 → 两独立守卫（P222/P133，ORIG 08553a4d/08553a7c 同落 08553caa；
//    4.4 合并形多 `mov $1/jmp/mov $0/test` 5 条 ×2）；
//  ②find_item 结果具名局部（P0，ORIG 08553a6e `mov %eax,-0x20`）；
//  ③slot 两句式预清（P198①，ORIG 0855405c `movl $0x0,-0x74`）+ logFormat
//    第二守卫拆独立 if（ORIG 08553f55 cmp/jne + 08553fc9 xor/test/je）；
//  ④logFormat 第 3 参手写签名串 → __PRETTY_FUNCTION__（P139 符号一致）；
//  ⑤删 `CCeraShop* shop` 具名局部（P0，ORIG 每次 `mov &s_pCeraShop,%eax` 直读）。
// 残余（N 封顶）：①帧槽整体浅 4（pkg sret −0xdd vs ORIG −0xd9，帧同为
// sub $0xca30）⇒ N82/P112 聚合对象分区；②slot 判负 ORIG `mov slot,%eax;
// shr $0x1f,%eax; test %al,%al; je`（4 条，08554132..）vs 4.4 `test %eax,%eax;
// jns`（2 条）⇒ N22；已试回滚：具名 `unsigned slotSign = (unsigned)slot>>31`
// 使帧 0xca30→0xca40、pkg→−0xdf、Inven_Item 压栈错位（gap −1→−3）。
bool CMailBoxHelper::_DoGetPackageItem(CUser* user, CMailBox::CMail const* mail,
                                       // [Q6HeaderTypeAxis 2026-09-11 类型轴] 定义处拼写
                                       // 决定 __PRETTY_FUNCTION__ 内容（实测）⇒ 用类内
                                       // typedef（CMailBoxHelper.h:224）对齐 ORIG prett
                                       // `…CMailBoxHelper::IntPairVector&…`（size 0x9f）。
                                       IntPairVector& items,
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

    // ORIG 0x08553936：bReset 与 !bReset 两分支各自一套
    // IsSpecialItem + find_item + _isUnusableSpecialItemInVillage 检查
    // （Ghidra C 0x08553a48 / 0x08553db7 两组，各 1 次调用）。
    if (bReset)
    {
        int specialA = 0;
        int specialB = 0;
        bool specialFlag = false;
        // [2026-09-11] P222/P133：ORIG 08553a4d/08553a7c 两守卫各自
        // `call; test %al,%al; je 08553caa`（正向 if 直落 fallback）；
        // `&&` 短路物化多 5 条 ×2，`if(!g()) goto` 反产 xor 物化（+2 ×2）。
        // P0：ORIG 08553a6e `mov %eax,-0x20(%ebp)` ⇒ find_item 结果具名局部。
        if (GlobalData::s_pCeraShop->IsSpecialItem(pkg.m_addInfo, specialA,
                                                   specialB, specialFlag))
        {
            CItem* specItem = G_CDataManager()->find_item(pkg.m_addInfo);
            if (_isUnusableSpecialItemInVillage(specItem))
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
                    // [R16fixCF P13] ORIG 0x08553b0d begin → 0x08553b22
                    // `call vector<STStackablePackageData>::end` → 0x08553b2a
                    // `jmp 0x08553b8c`（条件头）⇒ 两迭代器同列 for-init（begin 先、
                    // end 后，均在回边之外）。
                    for (std::vector<STStackablePackageData>::const_iterator
                             it = vec.begin(),
                             itEnd = vec.end();
                         it != itEnd; ++it)
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
    // [2026-09-11] 同 bReset 分支：P133/P272 独立守卫（ORIG 08553dbc/08553ded
    // 两 `test %al,%al; je 0855405c`）+ P0 具名 find_item 局部（ORIG 08553ddd
    // `mov %eax,-0x14(%ebp)`）。
    if (GlobalData::s_pCeraShop->IsSpecialItem(pkg.m_addInfo, specialA,
                                               specialB, specialFlag))
    {
        CItem* specItem = G_CDataManager()->find_item(pkg.m_addInfo);
        if (_isUnusableSpecialItemInVillage(specItem))
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
        if (GlobalData::s_pCeraShop->ProcessSpecialItem(user, pkg.m_addInfo, restrictCode,
                                                        outCnt, curSec, true, outItems, param,
                                                        '\0', '\0', false, 1))
        {
            if (outCnt != 0)
                return false;
            if (restrictCode != 0 && !user->IsRestrictedGoods(restrictCode))
            {
                user->SetRestrictedGoods(restrictCode);
                GlobalData::s_pCeraShop->SaveFeaturedIdx(user, restrictCode, (int)curSec, 0);
                user->SendRestrictedGoods(true, restrictCode);
            }
            // [2026-09-11] P222：ORIG 08553f55 `cmp $0xc1; jne 08553ffc` 与
            // 08553fc9 `xor $0x1; test %al,%al; je 08553ffc` 是两条独立守卫
            // （同落 logFormat 之后），`&&` 短路物化多 5 条。
            if (pkg.m_addInfo == 0xc1)
            {
                if (!GlobalData::s_pCeraShop->ProcessSpecialItem(
                        user, 0x10, 0xc352, outCnt, curSec, true, outItems, param,
                        '\0', '\0', false, 1))
                {
                    LogManager::logFormat(
                        1, "MailBox.cpp", __PRETTY_FUNCTION__, 0x34c,
                        "AuctionVIP Error - Present to friend(add private-store)");
                }
            }
        }
        return true;
        }
    }
    // [2026-09-11] P198①：ORIG 0855405c `movl $0x0,-0x74(%ebp)` 预清，
    // 0855412f 才写 call 结果 ⇒ 两句式；判负为 `mov slot,%eax; shr $0x1f,%eax;
    // test %al,%al; je`（4 条）⇒ `(unsigned)slot >> 31` 取符号位形。
    int slot = 0;
    slot = user->getCurCharacInvenW()->insertItemIntoInventory(
        pkg, (eItemAddReason)0x15, true, false);
    if ((unsigned int)slot >> 31)
        return true; // ORIG：失败亦返回 1（不置错误码）
    items.push_back(std::make_pair((ENUM_ITEMSPACE)0, slot));
    return true;
}