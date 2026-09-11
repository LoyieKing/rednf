// ---- _generateMonseterSpecificItem — ORIG T 0x08536fce ----
void CMonsterDrop::_generateMonseterSpecificItem(CMonster const* monster,
                                                 char idxType,
                                                 stGenerateResult_t& result)
{
    // ORIG 0x08536fce：itemIdx 两段式预置 -2（movl $-2,-0xc 后重赋）；
    // roll 无槽——generateNumber 结果 %eax 直传 0x8(%esp)（P0）；
    // pushCreatedItem helper 遮蔽 ORIG 内联体（Wide21B makeMemberLoginTable
    // 同案），本点展开：Inven_Item C1 → m_addInfo 槽回读 → itemList
    // create_item(1, inv, 1) → vector push_back(inv)。
    int itemIdx = -2;
    itemIdx = monster->drop_item_index(
        static_cast<unsigned int>(idxType), m_random.generateNumber(2, 10000));
    // ORIG cmpl -0xc;setne %al;test %al,%al;je 统一出口（无 bool 栈槽）——
    // P225 while+break 物化形（P0/N96：原 register asm("eax") 绑定已清除）。
    while (itemIdx != -2)
    {
        Inven_Item inv;
        inv.m_addInfo = itemIdx;
        G_CDataManager()->m_itemList->create_item(
            static_cast<ENUM_ITEM_CREATE_TYPE>(1), inv, 1);
        result.m_dropItems.push_back(inv);
        break;
    }
}