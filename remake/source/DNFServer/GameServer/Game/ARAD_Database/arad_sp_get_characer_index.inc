// ORIG 0x0818ad8b：取 @@identity 作为新 charac_no 写 +0x5348。
int CREATE_QUERY::arad_sp_get_characer_index(MySQL* db, SIG_CREATE_CHARAC* sig)
{
    // ORIG 0x0818ad91：函数头 `movl $0x0,-0xc(%ebp)` 死槽写（原码遗留未用局部，
    //   P197③/ P171 照抄）；加死局部复现。
    int index = 0;
    (void)index;
    if (db->select("seLect @@identity") != 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x47, "%s", "select @@identity error");
        return -1;
    }
    if (!db->get_uint(0, sig->m_characNo))
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x4c, "%s", "Parsing Error of Selected Field");
        return -1;
    }
    if (sig->m_characNo == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x51, "%s", "charac_no error");
        return -1;
    }
    return 0;
}