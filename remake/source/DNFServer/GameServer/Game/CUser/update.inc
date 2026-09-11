// ORIG T 0x0832acd0：对 20 个 CData* 槽（m_pad）isSave→saveData 全链；任一失败返回 0。
char charac_expand::CDataMgr::update(CUser* pUser)
{
    // [b28c-11] AE（32/32）：`isSave() != 0` + bool 字段/返回（CBoosterGage.h
    // m_bIsSave/isSave bool 化）复现 ORIG `call isSave; je` 直判（char 形 4.4
    // 恒物化 setne %al; test %al，P73）。
    char result = 1;
    for (int i = 0; i < 0x14; ++i)
    {
        if (m_pad[i]->isSave() != 0)
        {
            if (m_pad[i]->saveData(pUser) != 1)
                result = 0;
        }
    }
    return result;
}