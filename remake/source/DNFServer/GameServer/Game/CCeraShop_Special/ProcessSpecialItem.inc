// ============================================================================
// ProcessSpecialItem —— ORIG 0x0832587e (T)
// ============================================================================
// b28d-D42 verdict=DIFF [N82/N89] ORIG 0x0832587e orig/ours=125/108 ndiff=108。
// 帧/槽号平移 + STL 内联差；语义按报告逐条对齐。
// b28d-solo-CCeraShop_Special verdict=NEAR [N93] 125/125 等长，map end/find 迭代器槽方向相反（end -0x18 vs -0x24）
// [b28d-poolcera P326 翻案命中] end 迭代器不具名、右起内联 `== m.end()` ⇒ 临时槽
// 阶梯 end -0x18 / find -0x14 与 ORIG 逐槽一致 ⇒ **IDENTICAL 125/125 ndiff=0**。
// 旧 N93「槽方向无源码解」作废（P326 三级阶梯形，本件实证并列）。
bool CCeraShop::ProcessSpecialItem(CUser* user, int itemIdx, int count,
                                   int& restrictCode, long curSec, bool flag,
                                   std::vector<Inven_Item>& items,
                                   stCeraShopItemParam_t& param,
                                   char c1, char c2, bool flag2, int extra)
{
    if (user->getCurCharacR() == 0)
        return false;

    int type = 0;
    int value = 0;
    bool f = false;

    // ORIG 0x0832587e：先 end() 后 find(itemIdx) 再 operator==（P293 求值序），
    // 命中（迭代器不相等）时 type=2 直落 else 块，未命中才走 IsSpecialItem。
    // [b28d-poolcera P326 重试] end 不具名（具名槽落 -0x24 深），右操作数右起
    // 先求值 ⇒ end 临时 -0x18 / find 临时 -0x14，与 ORIG 逐槽一致。
    if (g_map_premium_item_data.find((unsigned long)itemIdx) ==
        g_map_premium_item_data.end())
    {
        if (!IsSpecialItem(itemIdx, type, value, f))
            return false;
    }
    else
    {
        type = 2;
    }

    if (count == 0 && (itemIdx == 0x10 || itemIdx == 0x28d171))
        count = 50000;

    // ORIG 死存储（-0xc 槽）：结果未被使用但 O0 保留读取。
    CPremiumLetheManager* lethe = G_CGameManager()->GetPremiumLetheManager();
    (void)lethe;

    STSpecailItem_Param p(items, param);
    // ORIG 字段写入序：value → user → itemIdx → count → time → flag → c1/c2/flag2 → extra。
    p.m_value = value;
    p.m_user = user;
    p.m_itemIdx = itemIdx;
    p.m_count = count;
    p.m_time = curSec;
    p.m_flag = flag;
    p.m_char1d = (unsigned char)c1;
    p.m_char1e = (unsigned char)c2;
    p.m_char1f = flag2;
    p.m_extra = extra;

    // m_handler：CCeraShop +0x00 的 SpecialItemHandler*（权威具名成员，ctor new 填充）。
    STSpecailItem_Result result;
    if (!m_handler->execute(type, p, result))
        return false;

    restrictCode = result.m_value;
    return true;
}

