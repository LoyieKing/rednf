bool DB_LoadEtc::LoadTitleBook(SIG_LOAD_ETC* sig)
{
    // ORIG 槽形：ok=-0x19 保存、srcLen=-0x18/-0x14/-0x10/-0xc（各列命名槽）、
    // outLen 各块独立槽 -0x34/-0x38/-0x3c/-0x40。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    bool ok;
    db->set_query("seLect general_section, specific_section, despair, event from"
                  " charac_titlebook where charac_no=%u", sig->m_characNo);
    ok = db->exec(true);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x19c0, 5)(
            "LoadTitleBook(), exec() ERROR charac_no=%u", sig->m_characNo);
        return false;
    }
    if (db->get_n_rows() == 0)
    {
        return false;
    }
    if (!db->fetch())                 // ORIG 直测 call 结果（无 ok 槽保存）
    {
        return false;
    }

    // ORIG 四列各自函数级 static const compressed_buff_size（0x1410/_0 0x393c/
    // _1 0x225c/_2 0x1134），展开为四个独立作用域块保持符号一一对应。
    {
        static const int compressed_buff_size = 0x1410;
        // [b28d-solo-DB_LoadEtc] P214③：`={0}` 触发 4.4 内联 rep stos（替 memset call，N78）。
        char blob[0x1410] = { 0 };
        if (!db->get_binary(0, blob, compressed_buff_size))
        {
            return false;
        }
        unsigned int srcLen1 = db->get_binary_length(0);
        unsigned int outLen1 = 0x10ae;   // ORIG 初始化在 call 之后
        if (!uncompress_zip(sig->m_titlebookGeneral, outLen1, blob, srcLen1))
        {
            return false;
        }
    }
    {
        static const int compressed_buff_size = 0x393c;
        char blob[0x393c] = { 0 };
        if (!db->get_binary(1, blob, compressed_buff_size))
        {
            return false;
        }
        unsigned int srcLen2 = db->get_binary_length(1);
        unsigned int outLen2 = 0x2fa8;   // ORIG 初始化在 call 之后
        if (!uncompress_zip(sig->m_titlebookSpecific, outLen2, blob, srcLen2))
        {
            return false;
        }
    }
    {
        static const int compressed_buff_size = 0x225c;
        char blob[0x225c] = { 0 };
        if (!db->get_binary(2, blob, compressed_buff_size))
        {
            return false;
        }
        unsigned int srcLen3 = db->get_binary_length(2);
        unsigned int outLen3 = 0x1c98;   // ORIG 初始化在 call 之后
        if (!uncompress_zip(sig->m_titlebookDespair, outLen3, blob, srcLen3))
        {
            return false;
        }
    }
    {
        static const int compressed_buff_size = 0x1134;
        char blob[0x1134] = { 0 };
        if (!db->get_binary(3, blob, compressed_buff_size))
        {
            return false;
        }
        unsigned int srcLen4 = db->get_binary_length(3);
        unsigned int outLen4 = 0x0e4c;   // ORIG 初始化在 call 之后
        if (!uncompress_zip(sig->m_titlebookEvent, outLen4, blob, srcLen4))
        {
            return false;
        }
    }
    return true;
