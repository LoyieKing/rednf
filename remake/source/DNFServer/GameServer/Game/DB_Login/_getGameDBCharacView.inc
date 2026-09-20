// [S216_DB_Login2] verdict=AE ndiff=0 gap=0（0x084149ce 167=167）。
// 收口改法 = P235 数组视角：SIG_LOGIN_DATA 增匿名 union 同址视角 m_characView
// （SigTypes_D.h，SIG_CHARAC_SLOT_VIEW/SLOTS_VIEW），`sig->m_characView.m_slots[i]
// .m_characNo` 令 4.4 产 `mov i,%eax; mov sig,%edx; imul $0x94,%eax,%eax;
// lea (%edx,%eax,1),%eax; add $0xe0,%eax` 逐条 = ORIG（探针 /tmp/s216_probe1.cpp
// fA=reinterpret_cast 形恒 base 先 vs fD=视角形 i 先）。p167 CLEAR。
DB_Login::RV_GetGameDBCharacInfo DB_Login::_getGameDBCharacView(SIG_LOGIN_DATA* sig)
{
    MySQL* db = _getGameDB(DB_HANDLE_2);
    db->set_query(
        "seLect info,hash_key,password(info),slot_effect_count,charac_slot_limit from charac_view where m_id=%s",
        NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        return -1;
    }
    if (db->get_n_rows() == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x73a, 5)("[DNF-DIAG CHARVIEW] no row m_id=%d", sig->m_accId);
        return 1;
    }
    if (!db->fetch())
    {
        return -1;
    }
    if (!get_compressed_blob_data(db, 0, SGBC(sig, OFF_CHARAC_VIEW),
                                  0x14d0, 0x94))
    {
        // P87/P227：全表达式临时链 ⇒ m_accId 入 %ebx 跨 ctor 保活。
        cMyTrace(__PRETTY_FUNCTION__, 0x3beb, 5)(
            "DB_Login::_getGameDBCharacView uncompress fail, m_id(%d)",
            sig->m_accId);
        return -1;
    }

    // [b28d-solo-DB_Login] P253/P214③：ORIG 0x08414b00..0x08414b40 对三个 0x84
    // 缓冲各发 `rep stos` 0x21 dwords 清零（-0xa4 缓冲清零后未使用，P171 照抄）。
    char deadBuf[0x84] = {0};
    char hashKey[0x84] = {0};
    char pwdHash[0x84] = {0};
    (void)deadBuf;
    if (!db->get_binary(1, hashKey, 0x84))    // ORIG：get_binary 全量覆盖，无预清零
    {
        return -1;
    }
    if (!db->get_binary(2, pwdHash, 0x84))
    {
        return -1;
    }
    if (!db->get_ubyte(3, sig->m_characSlotCurCount))    // +0x3fa0
    {
        return -1;
    }
    if (!db->get_ubyte(4, sig->m_characSlotMaxCount))    // +0x3fa1
    {
        return -1;
    }

    sig->m_characCount = 0;                              // +0xdc
    // [b28d-solo-DB_Login] P311 具名 Slot 数组视图（CharacDataView，见文件头
    // 匿名 namespace）：ORIG 地址链 `mov -0xc(%ebp),%eax; mov 0xc(%ebp),%edx;
    // imul $0x94,%eax,%eax; lea (%edx,%eax,1),%eax; add $0xe0,%eax; mov (%eax),%eax`
    // （i 先算、基址后并、0xe0 最后加）⇒ 完整类型成员直访问逐条复现（探针 f4）；
    // 旧 `sig->m_characData + i*0x94`（char 数组 + 指针算术）恒把 add $0xe0 提前。
    // ORIG：36 个槽位（i<=0x23；blob 解压 0x14d0 = 36×0x94），非 37
    for (int i = 0; i <= 0x23; ++i)
    {
        // [S216/P235] 改具名数组视角（SigTypes_D.h 匿名 union）⇒ 4.4 产
        // `mov i,%eax; mov sig,%edx; imul; lea; add $0xe0; mov (%eax),%eax` 逐条 = ORIG。
        if (sig->m_characView.m_slots[i].m_characNo != 0)
        {
            sig->m_characCount += 1;
        }
    }
    // [DNF-DIAG CHARVIEW] login char view load result
    cMyTrace(__PRETTY_FUNCTION__, 0x73a, 5)(
        "[DNF-DIAG CHARVIEW] m_id=%d count=%d slot0_no=%d slot0_lv=%u cur=%u max=%u",
        sig->m_accId, sig->m_characCount,
        sig->m_characView.m_slots[0].m_characNo,
        (unsigned)sig->m_characView.m_slots[0].m_level,
        (unsigned)sig->m_characSlotCurCount,
        (unsigned)sig->m_characSlotMaxCount);
    return 0;
}