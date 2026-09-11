// ORIG 0x08427dfc：局部 idx=1 → GetDBHandle(idx, 0)（DB_HANDLE_1）。
MySQL* DB_SaveUserHandicap::_getDBHandle()
{
    // enum 型局部直传（无调用点强转）：复现 ORIG「先载 this 再重读 idx」入栈序
    // （证据：docs/class_func_reports/DB_SaveUserHandicap/_getDBHandle.md 0x08427e02+）。
    ENUM_DB_HANDLE_IDX idx = (ENUM_DB_HANDLE_IDX)1;
    return GlobalData::s_db_mgr->GetDBHandle(idx, (ENUM_SERVER_GROUP)0);
}