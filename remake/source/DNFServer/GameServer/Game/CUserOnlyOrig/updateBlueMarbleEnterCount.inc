// ---------------------------------------------------------------------------
// 24. CUser::updateBlueMarbleEnterCount @0x0869106c
// ---------------------------------------------------------------------------
void CUser::updateBlueMarbleEnterCount()
{
    // ORIG 0x0869107e 族：具名 count 槽使帧 0x24 且 movzbl 两段落 -0x9 再回读；
    // 免槽内联（call 结果 movzbl 直进 4(%esp) 实参）与 ORIG 帧 0x14/调用序一致
    //（2026-09-05 CUserC8 实测闭合 AE；此前 N18a 判读仅指 `movzbl %al,%ebx`
    // callee-saved 直落形，本形走栈实参路径不受该 N 约束）。
    DB_UpdateBlueMarbleEnterCount::makeRequest(get_charac_no(-1),
                                               (int)getBlueMarbleEnterCount());
}