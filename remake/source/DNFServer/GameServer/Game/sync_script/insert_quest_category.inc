bool CSyncScript::insert_quest_category(QuestScript* script)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0);
    // b28d-35：ORIG 帧 0x24c 且内联 rep stos 计数 0x40 dword = 0x100 字节
    //   （P116/P253）⇒ 两缓冲各 0x100 字节（原 0x40 使帧仅 0xcc 且 escaped
    //   在长名时有溢出风险）；`={0}` 复现内联清零（P214③）。
    char name[0x100] = {0};
    if (!CodePage::script2Database((char*)script->m_name.c_str(), name))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x6a4, 5)(
            "Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
            CodePage::database(), script->m_name.c_str(),
            __PRETTY_FUNCTION__, 0x6a4);
    }
    // b28d-35：ORIG 第二次 rep stos（-0x22c = escaped）位于 script2Database
    //   之后、escape_string 之前 ⇒ 声明位置后置以复现清零点。
    char escaped[0x100] = {0};
    db->escape_string(escaped, name);
    db->set_query("inSert into quest_category(quest_idx, quest_name) values(%d, '%s')",
                  script->m_index, escaped);
    // b28d-35：ORIG 尾部为 `call exec; xor $1,%eax; test %al; je; mov $0; jmp;
    //   mov $1` ⇒ P101 形（`return db->exec(true);` 会被 4.4 折叠掉取反段）。
    if (!db->exec(true))
        return false;
    return true;
