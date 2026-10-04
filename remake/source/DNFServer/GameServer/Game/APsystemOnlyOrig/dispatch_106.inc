bool DB_WriteGeoReject::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                  E_SERVER_GROUP_NONE);
    SIG_SAVE_GEO_REJECT* sig = stream->GetOutBuffer<SIG_SAVE_GEO_REJECT>();
    db->set_query(
        "upDate geo_reject set rej_ip_count = rej_ip_count + 1, rej_src = 'g' where rej_ip = '%s'",
        sig);
    // ORIG 0x08437…：汇聚 island 在 %eax（mov $1/jmp/mov $0/test %al,je）——
    // gcc4.4 `if (!(exec==1 && rows!=0))` 直判形产生同岛（UpdateLogin 实证），
    // register bool 反而落 %bl（P78），故用直判形。
    if (!(db->exec(true) == 1 && db->getAffectedRowCount() != 0))
    {
        db->set_query(
            "inSert into geo_reject(rej_ip, rej_c_code, rej_ip_count, rej_src) values('%s', '%s', 1, 'g')",
            // ORIG 0x844bbf9-0x844bc06：`mov -0xc,%eax; lea 0x10(%eax),%edx;
            // mov -0xc,%eax; mov %edx,0xc(%esp); mov %eax,0x8(%esp)` —— 两实参
            // 均取视图数组 decay（+0 decay 零额外指令）；首实参传裸指针时 4.4
            // 对第二实参产 `add $0x10,%eax` 覆写形（探针 /tmp/w8lt_lea.o::g）。
            sig->m_rejIp,
            sig->m_rejCCode);
        db->exec(true);
    }
    return 1;
}