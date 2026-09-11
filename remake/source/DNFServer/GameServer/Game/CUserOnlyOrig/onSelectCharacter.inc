// ----------------------------------------------------------------------------
// ORIG 0x086800c6 CUser::onSelectCharacter() void
// [b28d-solo-CUserOnlyOrig AE] ORIG 0x086800c6：调用点/形态已对齐（check_tu_routed 口径
// AE，p167 CLEAR）。
// 掉落穿透(MCAP)衰减：+0x8e40c(=m_mcap.m_field1c) 初值 100，
// 0..9 格扫背包计数金币道具 -> 扣 40 与 10*n；
// 第 10..21 格(跳过 11)数剩余格 -> >8 再扣 20；pvpGrade>9 再扣 20。
void CUser::onSelectCharacter()
{
    m_mcap.m_field1c = 100;
    m_mcap.resetIncRate();

    int richCount = 0;                                     // -0x20
    int rareCount = 0;                                     // -0x1c
    int spareCount;                                        // -0x18（ORIG 声明在前、中部赋值）
    int i = 0;                                             // -0x14
    for (; i <= 9; ++i)   // ORIG cmpl $9; setle; test; jne 尾条件
    {
        // ORIG：GetInvenSlot 按值返回（sret 直写 -0x5d），m_addInfo@+2
        Inven_Item tmp = getCurCharacInvenR()->GetInvenSlot(0, i);
        if (tmp.m_addInfo != 0)
        {
            ++richCount;
            register int addInfo = tmp.m_addInfo;   // W8A 范式：register+同型 → mov slot,%eax; mov %eax,%ebx
            CItem* def = G_CDataManager()->find_item(addInfo);
            if (def != 0 && def->get_rarity() == 2)
                ++rareCount;
        }
    }

    if (richCount > 0) {
        m_mcap.m_field1c -= 0x28;
        m_mcap.m_field1c += richCount * -10;
    }
    m_mcap.decIncRate(rareCount * 100);

    spareCount = 0;
    for (int j = 10; j <= 0x15; ++j) {                     // P2 有符号
        if (j != 0xb) {
            Inven_Item t2 = getCurCharacInvenR()->GetInvenSlot(0, j);   // ORIG sret 直写
            // ORIG 0x08680230: addl $1,-0x18 后 `jmp +0x3` 到 8680237（inc 前有 nop 8680236）——技巧24 goto 落点岛
            if (t2.m_addInfo == 0)
                goto SKIP_INC;
            ++spareCount;
SKIP_INC:;
        }
    }
    if (spareCount > 8) m_mcap.m_field1c -= 0x14;
    if (get_pvp_grade() > 9) m_mcap.m_field1c -= 0x14;
    if (m_mcap.m_field1c < 0) m_mcap.m_field1c = 0;
}