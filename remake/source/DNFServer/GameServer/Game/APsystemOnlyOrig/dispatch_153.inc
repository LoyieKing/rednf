// ============================================================================
// DB_SavePvPMissionList::dispatch —— ORIG 0x0843f8fa（报告 dispatch.md 全文）
// ============================================================================
// ORIG 0x843f8fa：GetDBHandle(DB_HANDLE_3,0) → GetOutBuffer<
// SIG_REQUEST_PVP_MISSION_LIST> → 0x198 字节缓冲清零 → zipLen=0x198 →
// compress_zip(buf,&zipLen,sig,0x14a) 失败返 0；blob_to_str(0,buf,zipLen)
// → set_query("upDate fair_pvp_score set pvp_mission_info='%s' where
// charac_no=%u", str, sig->m_characNo) → exec 失败返 0；
// getAffectedRowCount()==0 → blob_to_str 再编 →
// inSert fair_pvp_score(charac_no,pvp_mission_info) values(%u,'%s')
// （m_characNo, str）→ exec 失败返 0；全绿返 1。
// ============================================================================
// [b28d-4] DIFF：N18a/P211：4.4 多 {'mov': 4} 中转 mov（单段直落/同型化待验）；ORIG 0843f8fa
// [r3] 消具名 `char* str`（两次 blob_to_str 内联进 set_query 实参）：帧不再
//   多 4B，槽序对齐 ORIG（db -0x10 / sig -0xc / zipBuf -0x1ac / zipLen -0x14）
//   ⇒ ndiff 52→0 完全一致（orig=our=99、gap=0、差集空）。
bool DB_SavePvPMissionList::dispatch(int /*userIdx*/, int /*slot*/,
                                     Stream* stream)
{
    static const int handleIdx = 3;
    MySQL* db;                                  // -0x10
    SIG_REQUEST_PVP_MISSION_LIST* sig;          // -0xc
    db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                           E_SERVER_GROUP_NONE);
    sig = stream->GetOutBuffer<SIG_REQUEST_PVP_MISSION_LIST>();
    // [Lane4] 声明序按 ORIG 代码序：ORIG 0x843f8fa 的 rep stos（zipBuf 清零）位于
    // GetDBHandle/GetOutBuffer 两调用**之后**、zipLen 初始化**之前**（实测 0x843f933
    // 起 rep stos，0x843f949 才 movl $0x198,-0x14）。旧序把 zipBuf 提到函数首 ⇒
    // 我方前面多整整一段 rep stos（gap −4/ndiff 62）。
    char zipBuf[0x198] = { '\0' };              // -0x1ac（ORIG rep stos 清零）
    unsigned int zipLen = 0x198;                // -0x14（ORIG rep stos 之后初始化）
    if (!compress_zip(zipBuf, zipLen, (const char*)sig, 0x14a))
        return 0;
    db->set_query(
        "upDate fair_pvp_score set pvp_mission_info='%s' where charac_no=%u",
        db->blob_to_str(0, zipBuf, (int)zipLen), sig->m_characNo);
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into fair_pvp_score(charac_no, pvp_mission_info) values(%u, '%s')",
            sig->m_characNo, db->blob_to_str(0, zipBuf, (int)zipLen));
        if (!db->exec(true))
            return 0;
    }
    return 1;
}