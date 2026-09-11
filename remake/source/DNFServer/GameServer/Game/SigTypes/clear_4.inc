void TAG_CHARAC_ETC_DATA::clear()     // ORIG W 0x822edf4
{
    memset(this, 0, sizeof(TAG_CHARAC_ETC_DATA));   // 0x330 字节清零（tagSkillSlot+tagSkillCombo）
}