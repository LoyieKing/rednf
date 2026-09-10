bool APSystem::CSyncScript::TruncateDescTable()
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0);
    db->set_query("trUncate table charac_action_point_desc");
    // ORIG 0x08122676：exec 结果 `xor $1,%eax; test %al,%al; je L; mov $0,%eax; jmp end;
    // L: mov $1,%eax`（b28c-L4 探针 f1 逐条全等）。`return !db->exec(true);` 被 4.4 折叠成
    // xor;leave;ret（少 4 条分支），须写成早期返回形保留分支（P101/P84）。
    if (!db->exec(true))
        return false;
    return true;
