// ============================================================================
// getCharacterJobToEnumValue（ORIG 0x89bbc0f，_Z26getCharacterJobToEnumValueRSs）
// ----------------------------------------------------------------------------
// 被 importPvPReferenceTable（"[pvp record order]" 键）与
// EquipmentParameterInfo.cpp 引用。ORIG 反汇编（objdump -d df_game_r）为 12 次
// std::string==const char* 比较链（_ZSteqIcSt11char_traitsIcESaIcEEbRKSbIT_T0_T1_EPKS3_
// @0x80809a1）；字符串表 .rodata 0x8e198be..0x8e19923 逐字节核对：
//   "swordman"→0 "fighter"→1 "gunner"→2 "mage"→3 "priest"→4 "at gunner"→5
//   "thief"→6 "at fighter"→7 "at mage"→8 "demonic swordman"→9
//   "creator mage"→10 "common"→11，全不中→-1（0xffffffff）
// ============================================================================
int getCharacterJobToEnumValue(std::string& job)
{
    if (job == "swordman")
        return 0;
    if (job == "fighter")
        return 1;
    if (job == "gunner")
        return 2;
    if (job == "mage")
        return 3;
    if (job == "priest")
        return 4;
    if (job == "at gunner")
        return 5;
    if (job == "thief")
        return 6;
    if (job == "at fighter")
        return 7;
    if (job == "at mage")
        return 8;
    if (job == "demonic swordman")
        return 9;
    if (job == "creator mage")
        return 10;
    if (job == "common")
        return 11;
    return -1;
}