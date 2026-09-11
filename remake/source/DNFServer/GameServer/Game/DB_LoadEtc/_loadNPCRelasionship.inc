// ============================================================================
// DB_LoadEtc::_loadNPCRelasionship @0x08407482 —— 返回 LineFunc 错误码（0 成功）。
// npc 数据区 SIG_LOAD_ETC +0x3e6c(cnt)/+0x3e70(binary, ≤600 字节)。
// 错误码 0x1c80/0x1c84/0x1c87/0x1c8a/0x1c8d/0x1c90（见汇编各失败出口）。
// ============================================================================
// b28d-31: N 封顶（N82/N95）——ORIG 0x08407482；槽号 -0x10/-0xc 整体平移
// （4.4 局部槽基策略），助记符序列等价。
// [b28d-solo-DB_LoadEtc] AE ndiff=0 (orig=86/ours=86)：具名 nRows 槽（P0 槽律）+ get_uint
// 直写 sig->m_npcCount（去 npcCnt 中间局部）+ 删 static_cast<int> 中转（P265 档②）。
int DB_LoadEtc::_loadNPCRelasionship(SIG_LOAD_ETC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect npc_cnt,npc_data from charac_npc where charac_no=%d",
                  sig->m_characNo);
    if (!db->exec(true))
    {
        return 0x1c80;
    }
    int nRows = db->get_n_rows();                            // ORIG -0xc 具名槽 + cmpl mem
    if (nRows != 1)
    {
        return 0x1c84;
    }
    if (!db->fetch())
    {
        return 0x1c87;
    }
    // ORIG 0x08407526：get_uint 目标直接是 sig+0x3e6c（无中间局部，P0 槽律）。
    if (!db->get_uint(0, sig->m_npcCount))
    {
        return 0x1c8a;
    }
    if (db->get_binary_length(1) != sig->m_npcCount * 12)
    {
        return 0x1c8d;
    }
    if (!db->get_binary(1, sig->m_npcData, 600))  // +0x3e70，cap 0x258
    {
        return 0x1c90;
    }
    return 0;
}