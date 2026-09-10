void CUser::deleteGuildExpBook()
{
    std::vector<std::pair<int, int> > requestItems;
    std::vector<std::pair<int, int> > deletedItems;

    // 保持 enum 实参：实测改成 `(int)` 会让 4.4 走另一条内联展开路径，
    // ndiff 4→27（gap +5，多出 mov/lea/call），TU 合计 90→87 连带回退三分，
    // 已回退。pair 的 first/second 写入序差（ORIG 先 first 后 second）
    // 属 4.4 对 `pair<ENUM,int>` → `pair<int,int>` 转换的固定形态，标 N。
    // b28d-45 复测：右值引用具名化（static_cast<T&&> 控声明序）反劣 117→121、
    // NEAR→DIFF，确认 pair 临时槽序为 4.4 固定形态，维持 N 封顶（ORIG
    // 0x0867e8b6 first 临时=-0x10 / second=-0xc）。
    requestItems.push_back(
        std::make_pair((ENUM_HARDCODE_ITEM)ENUM_HARDCODE_ITEM_0, 0x4b0));

    deleteSpecificItem(requestItems, deletedItems);

    // P248 档① 定论（P168a X1 不成立）：ORIG 0x0867e930 `call send_itemspace;
    // xor $0x1,%eax; test %al,%al; je` 的唯一复现形 = **callee 返回 bool +
    // 调用点裸 `if (!f())`**（P292 表：bool 返回的 `!f()` 直产 xor/test/je）。
    // 本仓 send_itemspace 早已 bool 化（CUser.h:3007 / CUser.cpp:8255），旧写
    // `^ 1` 恒被 4.4 折成 `cmp $1; setne; test` 三段 ⇒ 是配套漏改，非 N 封顶。
    if (!send_itemspace((int)ENUM_ITEMSPACE_0))
    {
        LogManager::logFormat(
            1, "user.cpp", __PRETTY_FUNCTION__, 0x7c57,
            "deleteGuildExpBook, pUser(%s)->send_itemspace( ENUM_ITEMSPACE_INVENTORY ) return FAILED",
            NumberToString(get_acc_id(), 0));
        return;
    }
    if (!send_itemspace(2))  // ENUM_ITEMSPACE_CARGO（P248 档① 同形）
    {
        LogManager::logFormat(
            1, "user.cpp", __PRETTY_FUNCTION__, 0x7c5c,
            "deleteGuildExpBook, pUser(%s)->send_itemspace( ENUM_ITEMSPACE_CARGO ) return FAILED",
            NumberToString(get_acc_id(), 0));
        return;
    }
    send_RedeemInfo();
