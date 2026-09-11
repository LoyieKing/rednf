char charac_expand::CDataMgr::update(CUser* pUser, ENUM_CHARAC_EXPAND_TYPE type)
{
    // [b28c-11] AE（25/25）：同上（bool isSave 直判，无 setne 物化）。
    char result = 1;
    if (m_pad[(int)type]->isSave() != 0)
    {
        if (m_pad[(int)type]->saveData(pUser) != 1)
            result = 0;
    }
    return result;
}