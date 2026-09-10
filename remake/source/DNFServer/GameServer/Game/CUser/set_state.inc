void CUser::set_state(ch_state state)
{
    // ORIG 0x867edb2-0x867ee27（probe s13 实证）：`if(cur==0) goto END` 产
    // `test;sete;test;jne` 物化岛；bFight 绑 asm("al")（call 结果原位 no-op）
    // 末用点 4.4 出 `movzbl %al,%ebx` 单条跨 call 持值（技巧 22b 新形）；
    // invenR/GetCreatureMgrW 全链式零槽（技巧 37）；尾 `call;jmp; nop; epilogue`。
    m_chState = state;
    if (m_chState == 5)
        ClearItemSellCount();
    if (getCurCharacR() == 0)
        goto ST_END;
    {
        // b28d-solo-CUser：ORIG `call is_fighting; movzbl %al,%ebx` 无 bool 槽（跨
        // getCurCharacInvenR/GetCreatureMgrW 保活 %ebx）。具名 bFight 落 -0x9 ⇒ 内联实参。
        ((CInventory*)getCurCharacInvenR())
            ->GetCreatureMgrW()
            ->TurnStomach(is_fighting());
    }
ST_END:;
