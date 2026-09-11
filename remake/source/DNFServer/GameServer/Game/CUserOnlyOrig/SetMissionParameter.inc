// ---------------------------------------------------------------------------
// 2. CUser::SetMissionParameter @0x08662558
// ---------------------------------------------------------------------------
void CUser::SetMissionParameter(int kind, int value)
{
    // ORIG 0x08662558：`test %al,%al; je <T>` 无 xor ⇒ 正向 if(isGMUser()) 直落形
    if (isGMUser()) {
    CMissionList_Charac* missions =
        reinterpret_cast<CMissionList_Charac*>(
            GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8));
    missions->GM_SetMissionValue(kind, value);
    }
}