// ORIG 0x811a4d4（0x2d0 字节）：为空则直接返回 1；否则遍历 ids 中各事件表索引，
// 用事件表 name/explain 字段构造 "(%d,%d,'%s','%s')" 插入串（explain 经
// g_scriptStringManager_.findString(4, ...) 解析），一次性 INSERT 到
// dnf_event_info，exec 成功返回 1，出错码 0x426（诱骗重复）或失败返回 0。
bool CEventTable::InsertEventTable(std::vector<int>& ids)
{
    // b28d-solo-CEventManager: [AE] ORIG 0x0811a4d4 实证——buf[0xff]={0} 声明在
    // ids.empty() 之后；while + `++it; if (it != ids.end()) sql += ",";`
    // （无 operator+，ORIG 两次 end()/ne() 调用）；events[*it].field 逐次重读
    // （P157/P183，无 idx/e 引用缓存）；set_query 单格式串 "…values%s"
    // （ORIG .rodata 0x8b50858 实读）；bool ok 局部 + getDBError()==0x426 判定。
    // P214① 正向包裹 `if (!ids.empty()){…} return true;` 复现 `call empty; xor $1;
    // test; je <成功尾>` ⇒ 208=208；逐条归一化仅剩重定位伪差（P264）。
    // ORIG 0x0811a4d4 实证：`call empty; xor $1; test; je <成功尾>` ⇒ 正向包裹
    // `if (!ids.empty()) { … } return true;`（P214①/P168a），非 `if(empty)return true`。
    if (!ids.empty())
    {
        char buf[0xff] = {0};
        std::string sql;
        std::vector<int>::iterator it = ids.begin();
        while (it != ids.end())
        {
            std::sprintf(buf, "(%d,%d,'%s','%s')", events[*it].id,
                         events[*it].apply_type, events[*it].name,
                         g_scriptStringManager_.findString(4, events[*it].explain, 0));
            sql += buf;
            ++it;
            if (it != ids.end())                           // ORIG 末位元素不加逗号
                sql += ",";
        }

        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1, E_SERVER_GROUP_NONE);
        bool ok = false;
        db->set_query(
            "inSert into dnf_event_info(event_id, apply_type, event_name, event_explain) values%s",
            sql.c_str());
        ok = db->exec(true);
        if (!ok)
        {
            if (db->getDBError() == 0x426)
                return false;
            return false;
        }
    }
    return true;
}