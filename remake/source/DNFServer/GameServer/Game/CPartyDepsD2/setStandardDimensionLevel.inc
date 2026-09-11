// ---------------------------------------------------------------------------
// setStandardDimensionLevel —— ORIG T 0x0859f612
// 报告：docs/class_func_reports/CParty/setStandardDimensionLevel.md
// 仅 GM 有效成员参与平均：ORIG 中 isGMUser() 结果为 0 时 je 跳到循环尾
// （不累加等级/计数）；成员数为 0 时 ORIG idivl 除零同样崩溃，忠实保留。
// ---------------------------------------------------------------------------
// [b28d-solo-CPartyDepsD2] AE（62=62）：五槽照抄（char -0x19 / sum -0x18 /
// avg -0x14 / count -0x10 / i -0xc）+ P292 bool callee 裸 `if(!_checkValidUser)
// continue`（xor$1;test;jne）+ P223 岛形 + isGMUser 正极性（test;je 跳过 movb）
// + avg 具名 ⇒ 帧 0x38 全等。
bool CParty::setStandardDimensionLevel(CDungeon const* dungeon)
{
    (void)dungeon;
    // [b28d-solo-CPartyDepsD2] ORIG 0x0859f612 五槽照抄：char 死标志 -0x19
    // （isGMUser()!=0 时置 1，从不读取）；sum -0x18 / avg -0x14 / count -0x10 /
    // i -0x0c。ORIG `xor$1;test;jne 循环尾` ⇒ 源码为 `if(!_checkValidUser) continue;`
    // （P292 bool callee + P223 岛形）；isGMUser 正极性（test;je 跳过 movb）。
    char nonGM = 0;                                      // -0x19
    int levelSum = 0;                                    // -0x18
    int avg = 0;                                         // -0x14
    int validCount = 0;                                  // -0x10
    for (int i = 0; i < 4; ++i)                          // -0x0c
    {
        if (!_checkValidUser(i))
            continue;
        if (memberUser(this, i)->isGMUser())
            nonGM = 1;
        levelSum += memberUser(this, i)->get_charac_level();  // thunk 0x080da2b8
        ++validCount;
    }
    (void)nonGM;
    avg = levelSum / validCount;                         // cdq/idivl
    m_battleField.m_standardDimensionLevel = avg;
    return true;
}