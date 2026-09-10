bool DB_CreateCharac::_checkCharacName(MySQL* db, char const* dbTable,
                                       SIG_CREATE_CHARAC* sig)
{
    db->set_query("seLect delete_flag from %s.charac_info where charac_name='%s'",
                  dbTable, sig->m_name);
    if (!db->exec(true))
    {
        sig->m_resultCode = 2;
        return false;
    }

    int deleted = 0;
    // P220/P222 正极性包裹：ORIG `test; setne; test; je <return-true 岛>`。
    if (db->get_n_rows() != 0)
    {
        sig->m_resultCode = 2;
        // [b23S3C] P48 去缓存：无 nRows/badFetch 具名槽（ORIG rows 直判 + log 点
        // 二次 call；短路双 xor 直判无 al 落槽）；deleted!=0 反形（ORIG `test;je
        // gc块`、rc3 作 body，P191）；rowCnt/qs 在 ctor 前求值（ORIG +0x10c/
        // +0x17d call 位于 ctor 之前），值跨 ctor 4.1 落 %ebx/%esi 寄存器缓存
        // （P227 全表达式临时链复现，4.4 槽差已消）。
        if (!db->fetch() || !db->get_int(0, deleted))
            return false;
        if (deleted != 0)
        {
            sig->m_resultCode = 3;
        }
        else
        {
            if (G_CEnvironment()->get_gc_no() == 0x3f2)
            {
                // P227 全表达式临时链：ORIG rowCnt 于 ctor 前求值入 %ebx、
                // sig+4 入 %esi 跨 ctor 保活（084012eb/084012f0 段）。
                cMyTrace(__PRETTY_FUNCTION__, 0xd6a, 5)(
                    "already charac_name(%s) row(%d)", sig->m_name,
                    db->get_n_rows());
                // DAT_08c42c04 = "사이토"（ORIG 字符串常量）
                static const char BASELINE_NAME[] = "\xec\x82\xac\xec\x9d\xb4\xed\x86\xa0";
                if (strcmp(sig->m_name, BASELINE_NAME) == 0)
                {
                    // P227：GetQueryString 结果于 ctor 前入 %ebx（08401356 段）。
                    cMyTrace(__PRETTY_FUNCTION__, 0xd6d, 5)(
                        "already charac_name query(%s)", db->GetQueryString());
                }
            }
            sig->m_resultCode = 1;
        }
        return false;
    }
    return true;
