bool DB_LoadEtc::getComboSkill(MySQL* db, unsigned int accId, int count,
                               comoboSkillData* out)
{
    if (db == 0 || out == 0)
    {
        return false;
    }
    for (int row = 0; row < count && row < 6; ++row)
    {
        if (!db->fetch())
        {
            cMyTrace("bool DB_LoadEtc::getComboSkill(MySQL*, unsigned int,"
                     " int, comoboSkillData*)", 0xbb01, 5)(
                "DB_LoadEtc::getComboSkill fetch, exec() ERROR charac_no=%u",
                accId);
            return false;
        }
        // P183 地址重物化：ORIG 每次重发 row*0xe 全链（无 slot 具名局部缓存）。
        int col = 0;                                          // ORIG -0x10 起 0，取后自增
        if (!db->get_short(col++, out[row].m_comboSkillIndex))
        {
            cMyTrace("bool DB_LoadEtc::getComboSkill(MySQL*, unsigned int,"
                     " int, comoboSkillData*)", 0xbb0a, 5)(
                "DB_LoadEtc::getComboSkill get(%d) ERROR charac_no=%u", col,
                accId);
            return false;
        }
        for (int k = 0; k <= 5; ++k)
        {
            if (!db->get_short(col++, out[row].m_comboSkillList[k]))
            {
                cMyTrace("bool DB_LoadEtc::getComboSkill(MySQL*,"
                         " unsigned int, int, comoboSkillData*)", 0xbb14, 5)(
                    "DB_LoadEtc::getComboSkill get(%d) ERROR charac_no=%u",
                    col, accId);
                return false;
            }
        }
    }
    return true;
