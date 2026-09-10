// ============================================================================
// GetOntimeUserRecvIdx（ORIG T 0x08414f5c）
// ============================================================================

// [S216_DB_Login2] verdict=AE ndiff=0 gap=0（0x08414f5c 104=104；下列 N18a/N82 判词作废）
// 序列 4.4 中途 store/load 回读 mod 槽（ORIG 全程寄存器算完再存 -0xc）；P0 去 mid
// 缓存后 32→6）。
// S216 收口（原 N18a/N82 判词作废）：`int mod = (int)(sig->m_accId % 10u);`
// （P217/P310 类型旋钮）⇒ 4.4 除法 magic 中间值不再 spill 到 -0xc，逐条 = ORIG。
int DB_Login::GetOntimeUserRecvIdx(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)DB_HANDLE_D,
                                                  E_SERVER_GROUP_NONE);
    // [b28d-solo-DB_Login] P0/P48：ORIG 槽 -0xc 存 `m_accId % 10` 结果（具名局部），
    // 且 NumberToString 前**再读一次** m_accId（无 mid 缓存）。
    sig->m_ontimeLastRecvIdx = 0;                        // +0x3fa4
    int mod = (int)(sig->m_accId % 10u);
    db->set_query("seLect last_no from event_1112_ontime_user_%u where m_id=%s",
                  mod, NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        return 0;
    }
    if (db->get_n_rows() == 0)
    {
        sig->m_ontimeLastRecvIdx = 1;
        db->set_query("inSert into event_1112_ontime_user_%u(m_id,last_no) values (%s,%u )",
                      mod, NumberToString(sig->m_accId, 0),
                      sig->m_ontimeLastRecvIdx);
        if (!db->exec(true))
        {
            return 0;
        }
    }
    else
    {
        if (!db->fetch())
        {
            return 0;
        }
        if (!db->get_int(0, sig->m_ontimeLastRecvIdx))
        {
            return 0;
        }
    }
    return 1;
}
