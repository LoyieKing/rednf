bool CSyncScript::insert_random_option_ref()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    // ORIG 0x08625ea2：外层 prefix 为 byte，初值 1，==0xff 终止（cmpb $0xff/setne，
    // 即使用域 [1,0xfe]）；内层 grade 为 byte 0..0x64（cmpb $0x64/setbe ≤）。
    std::string name;   // ORIG：循环外构造一次，循环内 operator=
    for (unsigned char prefix = 1; prefix != 0xff; ++prefix)
    {
        for (unsigned char grade = 0; grade <= 0x64; ++grade)
        {
            name = G_CDataManager()->m_randomOption.getPrefix(prefix, grade, false);
            if (name.length() != 0)
            {
                char converted[0x400] = {0};
                // script2Database 失败仅记日志（ORIG 行 0xb31），继续入库；
                // ORIG 无 escape_string，转码结果直接入 SQL。
                if (!CodePage::script2Database((char*)name.c_str(), converted))
                {
                    cMyTrace(__PRETTY_FUNCTION__, 0xb31, 5)(
                        "Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
                        CodePage::database(), name.c_str(), __PRETTY_FUNCTION__,
                        0xb31);
                }
                db->set_query("inSert into random_option_ref(random_option_index, random_option_value, random_option_name) values(%d,%d, '%s')",
                              prefix, grade, converted);
                if (!db->exec(true))
                {
                    return false;
                }
            }
        }
    }
    return true;
