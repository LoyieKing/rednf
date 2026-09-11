CExpandEquipslot* CUser::GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE type) const
{
    int frame_pad; (void)frame_pad;   // ORIG 0x80dd587 sub $0x18：调用点 2 outgoing 槽 + 16B 对齐（4.1 帧舍入），dead 局部无指令
    return m_characExpand.GetData(type);
}