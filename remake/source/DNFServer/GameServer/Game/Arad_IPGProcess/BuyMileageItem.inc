// ---------------------------------------------------------------------------
// BuyMileageItem —— ORIG T 0x0819dd4e（报告 BuyMileageItem.md 全文转写）。
// ---------------------------------------------------------------------------
// [b28d-solo-Arad_IPGProcess] NEAR（318=318，助记符序列全等）+ 三处真语义 bug：
// ①BillingRecord 第 7 实参按 ORIG 0x0819e18d 是 `(int)stack`（原写 0）；
// ②`CItem* item = 0;` 先于 localItem ctor（ORIG 0x0819dd9d 先于 0x0819ddaa）；
// ③第一次 GetSubGroupIndex 判定内联（ORIG `cmp $1; setne; test; je`）、case 0 receiver
// 链式（P52）、characNo 实参内联（P0）、case 3 两 getter 内联（%ebx 跨 call 保活）。
// 残差：len 槽 ORIG -0x98（4.1 与形参 spill 区 -0x8c..-0x94 连续分配）vs 4.4 具名区
// -0x30，连带 localItem(-0x7d→-0x81)/items/outCodeA/outCodeB 四处平移 4 ⇒ N82/N93 封顶。
// [S3/M3FreeNext15B 2026-09-11 复扫] ①§6.5 串审计 CLEAR（2 kinds/2 refs：
//   game_server_msg_104 @我方 .rodata+0、mileage item @+0x4d，与 ORIG 0x08ce.. 逐字节
//   相等）；②P331 试验：`int len;` 由函数头后移至 `int subGroup` 之后 ⇒ 指令数
//   318 不变（四数不变）但槽引用计数由 -0x24:4/-0x28:7/-0x2c:4 变为 2/4/7
//   （ORIG 基准为 4/7/4）⇒ 与 ORIG 更远，已回滚；③P339 方向不成立（ORIG 槽深序
//   len>localItem>items>outCodeB>outCodeA>goods>item>subGroup 与源码执行序冲突，
//   重排会改变 G_CDataManager 查询/构造序）。⇒ 维持 N82/N93。
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
}