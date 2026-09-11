// ---------------------------------------------------------------------------
// hellPartySpecificItem — ORIG T 0x08535b08
// b21 同族：如 generateSpecificItem 注，成员直读替换 u8_at/i8_at（+0x3d 有符号 movsbl）。
// ---------------------------------------------------------------------------
void CMonsterDrop_Hell::hellPartySpecificItem(stGenerateRefData_t const& ref,
                                              stGenerateResult_t& result)
{
    // [b28d-solo-WongWorkDrop_B] AE：149=149 逐条同形（残差仅重定位标签）。
    // 修法 = ctx 视图替裸偏移 + aiIdx/list 内联实参 + IsHellMonster 桥 bool 化
    // （P292 `call;test%al;je` 判据）+ script 去具名局部 + hasCatch 短路内联。
    int extraCount = 0;

    // ORIG 0x08535b08：aiIdx 经 %ebx 跨 G_CDataManager() 保活、list 直取，
    // 无具名局部（P0/P24 内联实参）；m_aiCharacterList 在 +0x869c。
    CAICharacter* ai = G_CDataManager()->m_aiCharacterList->get(
        static_cast<unsigned int>(ref.m_rankBonusIndex));
    if (ai != 0)
    {
        // ORIG je/jmp 分支形（三目会改块布局）
        if (ai->IsHellMonster() != 0)
            extraCount = 0;
        else
            extraCount =
                static_cast<int>(static_cast<signed char>(ref.m_field3d));
        ai->generateIndependentItem(ref, result.m_dropItems);
        ai->generateDropAvatarItem(static_cast<unsigned char>(ref.m_rateSlot),
                                   ref.m_field3e_byte, result.m_dropItems);
    }

    // ORIG 此处才绑定 ctx 指针局部（-0x14），并以其 +4 作 find_monster 实参。
    stGenerateRefCtx_t const& ctx =
        *reinterpret_cast<stGenerateRefCtx_t const*>(&ref.m_field34);
    CMonster* mon = static_cast<CMonster*>(
        G_CDataManager()->find_monster(ctx.m_rankBonusIndex));
    if (mon != 0)
    {
        if (mon->IsHellMonster() != 0)
            extraCount = 0;
        else
            extraCount =
                static_cast<int>(static_cast<signed char>(ref.m_field3d));

        // ORIG 无 script 具名局部（getMonsterScript 结果直接 +0x3ac 后调 empty）
        if (!static_cast<STMonsterScript*>(mon->getMonsterScript())
                 ->m_independentDrops.empty())     // +0x3ac
            mon->generateIndependentItem(ref, result.m_dropItems);

        mon->generateDropAvatarItem(static_cast<unsigned char>(ref.m_rateSlot),
                                    ref.m_field3e_byte, result.m_dropItems);

        // ORIG 短路物化无槽（mov $1/$0 岛 + 尾 test），具名 bool 会多槽+回读。
        if (!static_cast<STMonsterScript*>(mon->getMonsterScript())
                 ->m_catchItems.empty() &&
            ref.m_field45 != 0)                 // +0x70
            mon->generateCatchMonsterItem(result.m_dropItems);
    }

    // ORIG 0x08535b08 以 movzbl 读 +0x40 仅测低字节（非 dword 非空判据）。
    if (ref.m_field40_byte != 0)
    {
        for (int i = 0; i < extraCount; ++i)
            generateSpecificItem(ref, result);         // ORIG vtable 槽 +0xc 直呼
    }
}