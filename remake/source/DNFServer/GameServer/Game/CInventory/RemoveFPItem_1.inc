int CInventory::RemoveFPItem(Inven_Item* inven, Inven_Item* equip)
{
    // b28d-solo-CInventory：73/77，N108/P223 负例确认（4.4 岛数恒=条件数或 0）封顶。
    // b28d-37 [N108/P223 负例，ORIG 0x850a2bc]：我方多 2 个 `jmp+nop` 岛（ORIG 双岛 je 落点 +0x314/+0x317，4.4 岛数恒=条件数或 0，探针 l2_probe9 R1-R4 四式全败）⇒ 封顶。73/77。
    // ORIG 0x850a2d7..+0x43：三条件各为 || 短路 je/jbe/ja → INC 岛（0x850a314/
    // 0x850a317 4.1 nop 落点）；COND setle i<=0x19 / i<=0x137（i 槽 -0x10/-0xc）。
    // [N 归因·4.1 pad 形] batch-13 P168a 五步亲验：非 xor 族；ORIG GCC 4.1 为空
    // continue 出口铺 nop+二次 jmp 落点 pad（je 直落+jbe/ja 双岛），4.4.7 对 ||链/
    // 正向嵌套/三层嵌套均直连 thread（实测 77/65 两形，无 nop 岛形可出）——源层
    // 不可翻，维持 `||`+continue 形（gap 4 = 2×nop+2×jmp pad 缺失，版本指令选择）。
    // b28c-L2 fresh 复算：本形式 ndiff 2（orig 69/our 71）；另测「负条件 goto 尾
    // 标签」形（探针 /tmp/l2_probe9.cpp R2/R3）与「正向 &&」/「空 else」（R1/R4）
    // 四式：R2/R3 出 3 岛（多 1 对 jmp+nop，同现形）、R1/R4 出 0 岛（缺 2 对）
    // ⇒ 4.4 无「1 直落 + 2 岛」组合形，维持现形 N 封顶 2。
    for (int i = 0; i <= 0x19; i++)
    {
        // b28d-solo-CInventory [P296+P223]：ORIG「条件1 直落 addl + 条件2/3 各带
        // nop 岛」（je +0x5c / jbe +0x58 / ja +0x5b）由「正向包裹 + 两条独立 continue」
        // 复现（探针 /tmp/probe_island2.cpp fP 逐条同形）；短路 || 形恒 3 岛（fO）。
        if (inven[i].m_addInfo != 0)
        {
            if (inven[i].m_addInfo <= 0x1963) continue;
            if (inven[i].m_addInfo > 0x1b57) continue;
            inven[i].reset();
        }
    }
    for (int i = 3; i <= 0x137; i++)
    {
        if (equip[i].m_addInfo != 0)
        {
            if (equip[i].m_addInfo <= 0x1963) continue;
            if (equip[i].m_addInfo > 0x1b57) continue;
            equip[i].reset();
        }
    }
}