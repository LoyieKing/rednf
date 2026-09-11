// ---- CDBConnector ----
CDBConnector::CDBConnector(DBMgr* dbMgr)
    : m_dbMgr(dbMgr)   // ORIG C1 0x080ec5fc：先存 DBMgr*，再默认构造成员 STDBConnInfo。
{
}