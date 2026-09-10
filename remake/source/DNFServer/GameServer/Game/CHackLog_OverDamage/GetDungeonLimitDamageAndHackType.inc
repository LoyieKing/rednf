void CHackLog_OverDamage::GetDungeonLimitDamageAndHackType(
    CUser* user, unsigned int& limit, WongWork::ENUM_HACKTYPE& type)
{
    // [b28d-46 判词·N/N82] ORIG 0x08286790：149 vs 147；帧 0x54 vs 0x58 与
    // 局部预清槽（movb $0,-0x31 等）平移（N82）⇒ 封顶。
    // ORIG 0x0828679e-0x082868a0：4 条件单出口 bool 汇聚（3 guard 各自
    // `je <mov$0,eax>`，通过则 `mov$1,eax`，尾 `test%al; je ret`）；
    // 每使用点重 call GetParty()+mov 0xcac（P160 去缓存）。
    bool bOk = false;
    int level = 0;
    if (user != 0)
    {
        if (user->GetParty() != 0)
        {
            if (static_cast<CParty*>(user->GetParty())->m_battleField.m_pDungeon != 0)
            {
                level = user->get_charac_level();
                if (level >= 1)
                    bOk = true;
            }
        }
    }
    if (!bOk)
        return;

    int sectionIdx = 6;                       // ORIG local_30 初值
    double ratio = 1.0;                       // ORIG local_2c 初值
    int lvDelta = level - static_cast<CParty*>(user->GetParty())
                           ->m_battleField.m_pDungeon->get_min_level();   // ORIG local_20
    if (lvDelta < 0)
        lvDelta = 0;
    double divisor = 1.0;                     // ORIG local_1c 初值

    int dungeonIndex = static_cast<CParty*>(user->GetParty())
                               ->m_battleField.m_pDungeon->get_index();
    // ORIG 0x0828684d-0x082868a2：switch(dungeonIndex) 二分树
    // （cmp 0x2af8 双试 je/jg + cmp 0x64/0x2af9/0x2afe），**所有 case 汇聚
    // 同一 LAB_0828692e 尾计算块**（旧形把 limit/type 算块复制了两份）。
    switch (dungeonIndex)
    {
    case 100:
        ratio = _S_KILL_MONSTER_OVER_DAMAGE_DUNGEON_RATIO[
            reinterpret_cast<CBattle_Field*>(
                &static_cast<CParty*>(user->GetParty())->m_battleField)->get_dungeon_diff()];
        break;                              // → 尾（ORIG +0xeb..+0x0d jmp 19e）
    case 11000:
    case 0x2af9:
    case 0x2afe:
        break;                              // ratio/divisor 保持初值直达尾
    default:
        divisor = static_cast<double>(lvDelta / 20);
        if (divisor < 1.0)
            divisor = 1.0;
        int lvKey = level - 1;
        if (lvKey < 0)                      // movzbl/not/and 形态 = max(0)
            lvKey = 0;
        sectionIdx = LEVEL_SECTION_TABLE[lvKey];
        ratio = _S_KILL_MONSTER_OVER_DAMAGE_DUNGEON_RATIO[
            reinterpret_cast<CBattle_Field*>(
                &static_cast<CParty*>(user->GetParty())->m_battleField)->get_dungeon_diff()];
        break;
    }

    limit = static_cast<unsigned int>(
        static_cast<long long>(
            static_cast<double>(_S_KILL_MONSTER_OVER_DAMAGE_CONST[sectionIdx]) *
            ratio * divisor));
    type = static_cast<WongWork::ENUM_HACKTYPE>(
        _S_KILL_MONSTER_OVER_DAMAGE_HACK_TYPE[sectionIdx]);
