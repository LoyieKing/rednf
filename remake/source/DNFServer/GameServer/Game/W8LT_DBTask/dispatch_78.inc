// ============================================================================
// 虚覆盖（ORIG vtable @0x8c5eec0 槽序 [dispatch][D1][D0]）。
// GetOutBuffer<SIG_UPDATE_AVATAR_JEWEL_SOCKET> → DB_HANDLE_3 →
// blob_to_str(+8 m_emblem, 0x1e)；upDate user_items set jewel_socket='%s'
// where ui_id=%d and charac_no=%d（ui_id=m_avatarKey, charac_no=m_characNo）；
// exec(true)。Ghidra 反编译标 void：函数尾无 movzbl/置值——ORIG 源码为 bool
// 覆盖且体末无显式 return（eax 残留 exec 返回值）。故此处不写 return。
bool DB_UpdateAvatarJewelSlot::dispatch(int userIdx, int slot, Stream* stream)
{
    static const int handleIdx = 3;
    SIG_UPDATE_AVATAR_JEWEL_SOCKET* sig =
        stream->GetOutBuffer<SIG_UPDATE_AVATAR_JEWEL_SOCKET>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate user_items set jewel_socket='%s' where ui_id=%d and charac_no=%d",
        db->blob_to_str(0, sig->m_emblem, 0x1e),
        sig->m_avatarKey, sig->m_characNo);
    db->exec(true);
#if !remake_verify
    return 1;   // DNF-FIX: 非 void 函数落底 —— g++ 13 在此发 ud2（ORIG 的 GCC 4.x 只是返回残留 eax）
#endif
}