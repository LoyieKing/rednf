CExpandEquipslot* CUser::GetCharacExpandDataR(ENUM_CHARAC_EXPAND_TYPE type) const
{
    int frame_pad; (void)frame_pad;   // 值版重载无 ORIG 对应符号；与引用版（0x828b5de sub $0x18）保持一致
    return m_characExpand.GetDataR(type);
