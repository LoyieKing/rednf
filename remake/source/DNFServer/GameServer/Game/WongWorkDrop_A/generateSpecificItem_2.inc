// ---- generateSpecificItem — ORIG T 0x08537070（虚槽 +0x0c）----
// [b28d-solo-WongWorkDrop_A] verdict=AE：ORIG 布局重写（difficulty>4 高难度块
// 直落、jle 跳后部低难度块；旧早返形相反）+ 低难度块具名 ctx 指针（-0x20，
// +0x38 find_monster / +0x3d 循环上界 / +0x3c 末次判定）+ m_field43/
// m_field44_byte bool 化（P248 档② xor $1 形）。真语义 bug：find_monster
// 旧码取 ref.m_field35，ORIG 取 +0x38（m_rankBonusIndex）。
// b21 同族：u8_at/i8_at 改成员直读；+0x43/+0x44 改为 ORIG 语义 (byte!=1)&&(byte!=1)（原 ==0 误）。
//   残余：字节比较 codegen（ORIG xor$0x1 vs cmpb，N-86b 全同）+ AE。
void CMonsterDrop::generateSpecificItem(stGenerateRefData_t const& ref,
                                        stGenerateResult_t& result)
{
    if (ref.m_field41 != 0)
    {
        // hell 旗标非零：地狱派对专用掉落
        m_hell.hellPartySpecificItem(ref, result);
        return;
    }

    // [b28d-solo-WongWorkDrop_A] ORIG 布局：`jle <低难度块>`（低难度块在
    // 函数后部 +0xfd），difficulty>4 高难度块直落；旧早返形（jg 跳高难度）
    // 与 ORIG 相反。
    if (static_cast<signed char>(ref.m_field3c_byte) > 4)
    {
        // 高难度：地狱派对普通 specific 循环 + AI Character 掉落
        for (int n = 0; n < static_cast<signed char>(ref.m_field3d); ++n)
            m_hell.generateSpecificItem(ref, result);

        // §4.3 具名化：dword@+0x38 上/index = m_rankBonusIndex（P0 内联实参，
        // ORIG 值留 %ebx 跨 G_CDataManager 调用，无 aiIdx 具名槽）
        CAICharacter* ai = G_CDataManager()->m_aiCharacterList->get(
            static_cast<unsigned int>(ref.m_rankBonusIndex));
        if (ai == 0)
            return;

        ai->generateIndependentItem(ref, result.m_dropItems);
        ai->generateDropAvatarItem(static_cast<unsigned char>(ref.m_rateSlot),
                                   ref.m_field3e_byte, result.m_dropItems);
        return;
    }

    // 低难度：ORIG 具名指针局部 ctx（-0x20）——find_monster/循环上界/
    // 末次 difficulty 判定均经 ctx 偏移读（+0x38/+0x3d/+0x3c）。
    StRefCtx2 const* ctx =
        reinterpret_cast<StRefCtx2 const*>(&ref.m_field34);
    CMonster* monster = static_cast<CMonster*>(
        G_CDataManager()->find_monster(ctx->m_rankBonusIndex));
    if (monster == 0)
        return;

    int unusedItemIdx = -1;        // ORIG movl $0xffffffff,-0x18（死局部，P171）
    (void)unusedItemIdx;

    // ORIG 无 script 具名槽：每次 getMonsterScript() 直链 empty()（P0/P183）
    if (!static_cast<STMonsterScript*>(monster->getMonsterScript())
             ->m_independentDrops.empty())
        monster->generateIndependentItem(ref, result.m_dropItems);

    monster->generateDropAvatarItem(static_cast<unsigned char>(ref.m_rateSlot),
                                    ref.m_field3e_byte, result.m_dropItems);

    if (!static_cast<STMonsterScript*>(monster->getMonsterScript())
             ->m_catchItems.empty() &&
        ref.m_field45 != 0)
        monster->generateCatchMonsterItem(result.m_dropItems);

    // ORIG 0x08537254 movzbl 0x43; xor $0x1; je → 两 bool 字段取反（P248 档②）
    if (!ref.m_field43 && !ref.m_field44_byte)
    {
        for (int c = 0; c < static_cast<signed char>(ctx->m_count); ++c)
            _generateMonseterSpecificItem(monster, 0, result);
    }
    if (ctx->m_difficulty != 0)
        _generateMonseterSpecificItem(monster,
                                      static_cast<char>(ctx->m_difficulty),
                                      result);
}