// ORIG T 0x0832ac94：`this[type*4]`（CData* 槽数组）；type > 0x13 返回 0。
// 与 GetDataR 同形（ORIG 两函数字节一致）；enum 底层无符号 → jbe。
CExpandEquipslot* charac_expand::CDataMgr::GetData(ENUM_CHARAC_EXPAND_TYPE type) const
{
    if (!((unsigned int)type <= 0x13))
        return 0;
    return (CExpandEquipslot*)m_pad[(int)type];
}