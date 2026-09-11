// ---------------------------------------------------------------------------
// CUser::deleteGuildExpBook()
// ORIG T 0x0867e898 sz 0x1c4，报告 docs/class_func_reports/CUser/deleteGuildExpBook.md。
// 流程：deleteSpecificItem({make_pair((ENUM_HARDCODE_ITEM)0, 0x4b0)}, deleted)
// → send_itemspace(0) 失败则 logFormat(0x7c57) 返回 → send_itemspace(2) 失败则
// logFormat(0x7c5c) 返回 → send_RedeemInfo()。ORIG 尾部不设置返回寄存器（void）。
// ---------------------------------------------------------------------------
// [Q8 2026-09-11 旧N判词重扫] 旧判=NEAR → n1_v 新口径 AE（gap=+0，blocks=0）⇒ 旧判词作废。
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
    // [b28e-S4 语义修复] ORIG 0x0867e8b6 `movl $0x0,-0x10` / 0x0867e8bd
    // `movl $0x4b0,-0xc`：make_pair 的 A&&/B&& 绑定 (&(-0xc), &(-0x10))
    // ⇒ ORIG pair 的 first(ENUM)=0x4b0、second(int)=0。原 (ENUM)0/0x4b0 把
    // item id 落成 0（_isMatchedItem 以 it->first==itemIdx 匹配，
    // CUser.cpp:12783；_deleteSpecificItemInven 消费）⇒ 运行期删不到公会经验书。
    requestItems.push_back(
        std::make_pair((ENUM_HARDCODE_ITEM)0x4b0, 0));

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
}