short CUser::has_within_Mission() const
{
    CMissionList_Charac* mission =
        (CMissionList_Charac*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
    // ORIG 0x8662739 call 后直 leave;ret（无归一），调用点 0x865d433 族 `test %ax,%ax`（16 位判）⇒ short 直返
    return mission->getIndex_byKind(0x1b);
