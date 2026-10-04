// ORIG 0x841a0a6：GetDBHandle(2,0) → upDate member_dungeon set dungeon='%s'
// where m_id=%s；exec 失败或 affected==0 时补 inSert into member_dungeon。
bool DB_SaveDungeonClear::SaveDungeonClear(SIG_SAVE_DUNGEON_CLEAR* sig)
{
    // ORIG 每字段重读参数（P86），不缓存视图局部。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("upDate member_dungeon set dungeon='%s' where m_id=%s",
                  sig->m_dungeon,
                  // [Lane31W8LTDBTaskView] CUser.h→CInventory.h 链入
                  // `NumberToString(unsigned long long,int)` 后，int 实参对
                  // (unsigned int)/(unsigned long long) 两候选歧义 ⇒ 显式同型化
                  // （m_id 语义即 uint，ORIG 无转换指令）。
                  NumberToString((unsigned int)sig->m_mId, 0));
    if (!(db->exec(true) == 1 && db->getAffectedRowCount() != 0)) {
        db->set_query("inSert into member_dungeon (m_id,dungeon) values (%s,'%s')",
                      NumberToString((unsigned int)sig->m_mId, 0),
                      sig->m_dungeon);
        // ORIG 尾仅一个 `mov $1,%eax` 汇聚块 ⇒ inSert 失败直返 0，成功
        // fall-through 共享尾部 return 1。
        if (!db->exec(true))
            return 0;
    }
    return 1;
}