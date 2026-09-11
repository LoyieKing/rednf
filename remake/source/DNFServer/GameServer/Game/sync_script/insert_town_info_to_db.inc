bool CSyncScript::insert_town_info_to_db(const TownScript& script)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    /* w2 源形（4.4 双 je 同目标汇聚块 = ORIG 形；探针 /tmp/l5_cnd3.cpp w2）。
     * ORIG escaped 清零 rep stos（N78），显式 {0} 初始化复现。 */
    if (script.m_field0 != 0 && script.m_str4.length() != 0)
    {
        static char town_name[0xf0];
        memset(town_name, 0, sizeof(town_name));
        char escaped[0x400] = { 0 };
        // ORIG：script2Database 输出到 escaped（栈），escape_string 目标与 set_query 用 static 缓冲。
        if (!CodePage::script2Database((char*)script.m_str4.c_str(), escaped))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xbc2, 5)(
                "Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
                CodePage::database(), script.m_str4.c_str(),
                __PRETTY_FUNCTION__, 0xbc2);
        }
        db->escape_string(town_name, escaped);
        db->set_query("inSert into dnf_town_info(idx, town_name_kr) values(%d, '%s')",
                      script.m_field0, town_name);
        /* t2 形（探针 /tmp/l5_ret.cpp）：ORIG exec 后 xor $1/test 物化；
         * exec 成功 fall-through 与块外 return true 共享单一尾块（ORIG L_end 合一）。 */
        if (!db->exec(true))
            return false;
    }
    return true;
}