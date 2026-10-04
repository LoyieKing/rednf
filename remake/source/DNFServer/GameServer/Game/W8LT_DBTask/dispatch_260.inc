// ============================================================================
// DB_GoblinPadUpdateReward::dispatch —— ORIG 0x0842413c
// GetDBHandle(DB_HANDLE_6,0) → GetOutBuffer<SIG_GOBLIN_UPDATE_REWARD> →
// seLect mousepass from member_mousepass where m_id=%s；exec&&有行&&fetch
// 失败返 0；get_str(0,buf,0x24) 失败返 0。m_field4==2 或 m_field5==0 →
// modify_type=0xf（仅 INSERT history）；否则 modify_type=0x10（先 UPDATE
// reward_time=unix_timestamp(now()) 且 exec&&受影响行数非 0，失败返 0，再
// INSERT history）。INSERT history（表名 %Y%m）exec 失败或受影响行数==0
// 返 0；成功返 1。（gap=0；残差为 field5 分支 je/jne 极性 + 物理块序 ——
// N 类布局码，见 report dispatch.md）
// ============================================================================
// [b28d-4] AE：P222/P258 德摩根块序交换：ORIG 0x0842413c cmp$2;je<then> + test;je<then> 两跳同落 then ⇒ if(field4!=2&&field5!=0){else}else{then}；探针 /tmp/l4_probe_pol.cpp f2/f3/f4
bool DB_GoblinPadUpdateReward::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    // ORIG 用 GetOutBuffer<SIG_GOBLIN_UPDATE_REWARD>（模板实例符号一致）；
    // View 仅作字段访问视图（reinterpret_cast 保留，布局 +0x18 相同）。
    SIG_GOBLIN_UPDATE_REWARD* sig =
        /*SIG_GOBLIN_UPDATE_REWARD*/(
            stream->GetOutBuffer<SIG_GOBLIN_UPDATE_REWARD>());
    db->set_query(
        "seLect mousepass from member_mousepass where m_id=%s",
        NumberToString(sig->m_mId, 0));
    if (!(db->exec(true) && db->get_n_rows() != 0 && db->fetch()))
        return 0;
    char mousepass[0x24];
    if (!db->get_str(0, mousepass, 0x24))
        return 0;
    // ORIG 0x0842413c：getCurSec/localtime_r/strftime/NumberToString 在
    // m_field4==2||m_field5==0 与否则两分支内各自重复（独立 ym 缓冲
    // local_2e/local_1e，见 report dispatch.md），非合并共享 —— 逐分支保留。
    // ORIG 0x0842413c 段 `cmp $0x2,%al; je <then>` + `test %al,%al; je <then>`
    // 两跳同落 then（then 在函数后段、else 直落）⇒ 源码形 = 德摩根
    // `if (field4 != 2 && field5 != 0) { else } else { then }`
    // （探针 /tmp/l4_probe_pol.cpp f2/f3/f4 三形同产物；正极性 || 形 f1 反产
    //  `jne <else>` 块序 DIFF）。
    if (sig->m_field4 != 2 && sig->m_field5 != 0)
    {
        char ym[10] = { 0 };
        time_t curSec = (time_t)GlobalData::s_systemTime_.getCurSec();
        struct tm tmBuf;
        localtime_r(&curSec, &tmBuf);
        strftime(ym, 10, "%Y%m", &tmBuf);
        db->set_query(
            "upDate member_mousepass set reward_time = unix_timestamp(now()) where m_id=%s and enable_flag = '1'",
            NumberToString(sig->m_mId, 0));
        if (!(db->exec(true) && db->getAffectedRowCount() != 0))
            return 0;
        db->set_query(
            "inSert into member_mousepass_history_%s (occ_time, m_id, pre_mousepass, modify_type, ip_info, port_info) values (now(),%s,'%s',%d,'%s','')",
            ym, NumberToString(sig->m_mId, 0), mousepass, 0x10, sig->m_ip);
    }
    else
    {
        char ym[10] = { 0 };
        time_t curSec = (time_t)GlobalData::s_systemTime_.getCurSec();
        struct tm tmBuf;
        localtime_r(&curSec, &tmBuf);
        strftime(ym, 10, "%Y%m", &tmBuf);
        db->set_query(
            "inSert into member_mousepass_history_%s (occ_time, m_id, pre_mousepass, modify_type, ip_info, port_info) values (now(),%s,'%s',%d,'%s','')",
            ym, NumberToString(sig->m_mId, 0), mousepass, 0xf, sig->m_ip);
    }
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
        return 0;
    return 1;
}