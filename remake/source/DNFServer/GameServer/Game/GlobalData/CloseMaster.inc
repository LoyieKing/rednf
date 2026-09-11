void CDBConnector::CloseMaster()
{
    // ORIG 0x080ec6bc：GlobalData::s_db_mgr->Close((ENUM_DB_HANDLE_IDX)0)。
    GlobalData::s_db_mgr->Close((ENUM_DB_HANDLE_IDX)0);
}