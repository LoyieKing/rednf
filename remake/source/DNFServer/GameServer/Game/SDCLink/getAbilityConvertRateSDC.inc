// ============================================================================
// 1) _Z24getAbilityConvertRateSDCPv23ENUM_CONVERT_TABLE_TYPE @0x080950fb (0x13)
//    sub $0x4,%esp; mov $0x3f800000,-0x4; flds -0x4; leave; ret
//    → 两参均未读，恒返 1.0f（ORIG 即纯常量，非桩）。
// ============================================================================
float getAbilityConvertRateSDC(void* /*table*/, ENUM_CONVERT_TABLE_TYPE /*type*/)
{
    return 1.0f;
}