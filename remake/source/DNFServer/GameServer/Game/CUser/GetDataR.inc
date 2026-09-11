// ORIG T 0x0832acb2：`this[type*4]`（CData* 槽数组）；type > 0x13 返回 0。
// 与 update/ResetDaily 一致的 CData*[0x14] 槽解释；enum 底层无符号 → jbe。
CExpandEquipslot* charac_expand::CDataMgr::GetDataR(ENUM_CHARAC_EXPAND_TYPE type) const
{
    if (!((unsigned int)type <= 0x13))
        return 0;
    return (CExpandEquipslot*)m_pad[(int)type];
}