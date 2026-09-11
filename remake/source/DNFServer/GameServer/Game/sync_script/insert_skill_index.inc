bool CSyncScript::insert_skill_index(STSkillScript* script, int job)
{
    // [b28d-solo-sync_script] 判词：**AE/IDENTICAL**（check_tu routed；126=126）。
    //   归位要点：`char name[0xff] = {0}`（0xff 是数组大小，旧 memset 0x100 系误读
    //   展开计数）+ `char escaped[0x2fd] = {0}`（ORIG 765 字节、基址 -0x428 对齐）。
    //   ORIG @0x0862262c。
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)3, (ENUM_SERVER_GROUP)0);
    // ORIG 0x0862266f：name 基址 -0x12b、`mov $0xff,%ebx` 三段内联清零 ⇒
    // `char name[0xff] = {0}`（0xff 是**数组大小**，非 memset 0x100；旧转写把
    // 展开计数误当 memset 长度）。ORIG 0x086226ca：escaped 基址 -0x428、
    // `rep stos $0xbf` + 尾 1 字节 = 765 = 0x2fd 字节 ⇒ `char escaped[0x2fd]`；
    // 4.4 对 `= {0}` 同样三段展开（显式 memset 恒 call）。
    char name[0xff] = {0};
    char escaped[0x2fd] = {0};
    if (!CodePage::script2Database((char*)script->m_str4.c_str(), name))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x6e0, 5)(
            "Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
            CodePage::database(), name, __PRETTY_FUNCTION__, 0x6e0);
    }
    db->escape_string(escaped, name);
    db->set_query("inSert into skill_index(job, skill_idx, skill_name) values(%d, %d, '%s')",
                  job, script->m_field0, escaped);
    // ORIG 0x086227d1：exec 结果保 bool 比较控制流（P101）。
    if (!db->exec(true))
        return false;
    return true;
}