bool DB_LoadEtc::LoadAchievement(SIG_LOAD_ETC* sig)
{
    static const int compressed_buff_size = 0x1cd8;  // ORIG _ZZ…LoadAchievement…E20compressed_buff_size
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect achievement, unix_timestamp(last_update_time) from"
                  " charac_achievement where charac_no=%u", sig->m_characNo);
    bool ok = db->exec(true);                                // ORIG -0x15 具名槽
    if (!ok)
    {
        return false;
    }
    if (db->get_n_rows() == 0)
    {
        return true;
    }
    if (!db->fetch())
    {
        return false;
    }

    // ORIG 0x084064cf：rep stos edx=0x736 **dword** 计数 = 0x1cd8 字节（P253 全清），
    // `={0}` 触发 4.1/4.4 的 rep stos 展开（P214③）。
    char blob[0x1cd8] = { 0 };
    if (!db->get_binary(0, blob, compressed_buff_size))
    {
        return false;
    }
    unsigned int srcLen = db->get_binary_length(0);
    unsigned int outLen = 0x1800;      // ORIG 0x08406531：srcLen 之后才初始化
    if (!uncompress_zip(sig->m_expandDataF, outLen, blob, srcLen))
    {
        return false;
    }

    int count = 0;
    // ORIG 0x0840658c：`mov sig,%ecx; i*12; lea (%ecx,%eax,1),%eax; add $0xb1a0,%eax;
    //   mov 0x4(%eax),%eax`——基址偏移 0xb1a0 非 12 倍数 ⇒ 4.4 保留独立 add（不折 disp）。
    struct AchievementEntry
    {
        int m_field0;
        int m_flag;      // +4（ORIG 读点）
        int m_field8;
    };
    struct AchievementView
    {
        char m_pad[0xb1a0];
        AchievementEntry m_entries[0x200];
    };
    for (int i = 0; i <= 0x1ff; ++i)                       // i <= 0x1ff（cmpl 0x1ff setle）
    {
        if (((AchievementView*)sig)->m_entries[i].m_flag == 0)
        {
            break;                                          // 计数停在首个零条目
        }
        ++count;
    }
    sig->m_achievementCount = count;             // +0xc9a4（movl）

    // ORIG 0x084065c4：get_uint 目标直接是 sig+0xc9a8（无 lastUpdate 局部，P0 槽律）。
    if (!db->get_uint(1, sig->m_achievementLastUpdate))
    {
        return false;
    }
    return true;
