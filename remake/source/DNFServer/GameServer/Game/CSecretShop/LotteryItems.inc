void CSecretShop::LotteryItems(std::vector<SALE_INFO>& out, int dungeonIdx,
                               int level, int price)
{
    // 判词·AE（本轮修复）：ORIG 56 = 我们 56，gap 0。
    // P48 no-CSE：ORIG 每处 use 各自 fresh G_CDataManager()（GetItemByDungeonIdx /
    // GetItemByDungeonLev 各一处，共 2 次）。
    // m_pRand 即 PvPTypes.h 权威全局 CMTRand；ORIG 0x085fac85 event =
    // s_event_manager->GetRepeatEvent(0x3a)->IsEventing(0)（vtable slot 13，
    // CEventBase 权威定义经 CGuildServerProxy.h；ev 无槽往返 ⇒ P0 无具名局部，
    // 匿名链式虚调用复现 ORIG `mov (%eax),%edx; add $0x34,%edx; mov (%edx),%edx;
    // mov %eax,(%esp); call *%edx`）。
    bool event = GlobalData::s_event_manager->GetRepeatEvent(0x3a)->IsEventing(0);
    // [b28d-solo-CSecretShop] 尾块：ORIG 0x085facd6 `test %al,%al; jne <nop 岛>` +
    // 0x085fad14 `test %al,%al; jmp <epilogue>; nop` ⇒ 平铺双早返
    // `if (f() != 0) return;`（P63：散布 return 产 jmp/nop 中继岛；4.1 把第二处
    // 等价 jne 折成 jmp）。
    if (G_CDataManager()->m_secretShop.GetItemByDungeonIdx(m_pRand, out, dungeonIdx, level, event) != 0)
        return;
    if (G_CDataManager()->m_secretShop.GetItemByDungeonLev(m_pRand, out, dungeonIdx, price, event) != 0)
        return;
}