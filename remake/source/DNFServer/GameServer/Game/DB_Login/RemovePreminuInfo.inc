// ============================================================================
// RemovePreminuInfo（ORIG T 0x084128c4，成员）
// ============================================================================

void DB_Login::RemovePreminuInfo(SIG_LOGIN_DATA* sig, std::vector<int>& ids)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    // [b28d-solo-DB_Login] P232/P28：ORIG 有 `movl $0x0,-0xc(%ebp)` dead int 槽
    // （db=-0x10），且 begin() 先于 end() 求值（-0x18/-0x14 临时 → -0x1c/-0x20 转换）。
    int slotPad = 0;
    // [b28d-38] P167：ORIG 两端均 const_iterator（end() 亦转换构造）；混合比较
    // 会实例化 operator!=<PK,P> 变体（我们原形）。
    std::vector<int>::const_iterator it = ids.begin();
    std::vector<int>::const_iterator endIt = ids.end();
    for (; it != endIt; ++it)
    {
        db->set_query("deLete from dnf_event_entry where m_id=%s and event_id=%d",
                      NumberToString(sig->m_accId, 0), *it);
        // ORIG 0x08412996..0x084129a9：`xor $1; test; jne <nop 岛>` + dead 读
        // sig+0x2938（共享头 pad 区，原码字段未具名；P171 死读照抄 + P223 goto 尾标签）。
        if (!db->exec(true))
        {
            goto nextEntry;
        }
        // 共享头 pad 区（m_pad28d8 内 +0x60）：具名成员下标 ⇒ 常量折叠单条 disp 读
        // （SGFC 的 (char*)sig+off 会多一条 add，N107）。
        if ((unsigned char)sig->m_pad28d8[0x2938 - 0x28d8])
        {
            continue;
        }
    nextEntry:
        ;
    }
    (void)slotPad;
}
