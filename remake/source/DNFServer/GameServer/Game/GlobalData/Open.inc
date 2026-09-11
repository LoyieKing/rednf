bool CDBConnector::Open(ENUM_DB_HANDLE_IDX idx, ENUM_SERVER_GROUP group,
                        STDBConnInfo& info)
{
    // ORIG 0x080ec6da：info 首字节为 0（m_host 空）→ 直接成功；否则调 DBMgr::Open。
    // [b28c-lane17] 实参地址序按 ORIG 压栈槽逐条对齐：p2=+0x00(m_host)、
    // p3=*(+0x14)(m_port)、p4=+0x42(m_db)、p5=+0x18(m_user)、p6=+0x2d(m_pass)
    // （原实现把 m_pass 传成 p2、m_host 传成 p6 —— 真语义 bug，此处修正）。
    if (info.m_host[0] == 0)
    {
        return true;
    }
    if (!GlobalData::s_db_mgr->Open(idx, info.m_host, (unsigned int)info.m_port,
                                    info.m_db, info.m_user, info.m_pass, group))
    {
        return false;
    }
    return true;
}