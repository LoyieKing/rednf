// ============================================================================
// DB_InsertAutoPunishFirstUser::dispatch —— ORIG 0x0842d6ce（报告 dispatch.md
// 全文）
// ============================================================================
// 段1：DB_HANDLE_6 —— auto_punish_first_user 先 inSert（8 列）；失败 →
// upDate 同键计数（再失败 cMyTrace 0x70fb "upDate auto_punish_first_user
// Fail"）。段2：DB_HANDLE_1 —— punish_flag∈{2,7,8} 直接返 1；否则
// dwPunishDay=etc/10、dwApplyFlag=etc%10，punish_type=3（etc==10 → 0xc）；
// member_punish_info upDate（end_time 按天 adddate），exec!=1 或 affected==0
// → inSert 同值；inSert 失败返 0。段3：sprintf 邮件原因 → strftime("%Y") 后缀
// 表 member_punish_info_history_%s 逐日 inSert（6 惩罚值）→ 失败返 0；type∈
// {3} 或 {4 且 flag==0} → member_punish_info_ars_mail inSert 失败返 0；恒返 1。
// [b28d-4] DIFF：P222：块序/极性（ORIG je 落点 vs 4.4 jne）；ORIG 0842d6ce
bool DB_InsertAutoPunishFirstUser::dispatch(int /*userIdx*/, int /*slot*/,
                                            Stream* stream)
{
    // ORIG 0x0842d6da 函数首句 movl $0x0,-0x30（P198① 预清：声明=0 + 独立赋值）
    int punishType = 0;                 // ORIG local_34 @ -0x30
    DB_InsertAutoPunishFirstUserView* sig =
        reinterpret_cast<DB_InsertAutoPunishFirstUserView*>(
            stream->GetOutBuffer<SIG_INSERT_AUTO_PUNISH_FIRST_USER>());
    MySQL* db6 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                   E_SERVER_GROUP_NONE);
    db6->set_query(
        "inSert into auto_punish_first_user(m_id,occ_time,ip,hack_type,cnt,hack_sub_type,hack_sub_cnt,punish_flag) values(%s,now(),'%s',%d,%d,%d,%d,%d)",
        NumberToString(sig->m_memberId, 0), sig->m_ip, sig->m_hackType,
        sig->m_cnt, sig->m_hackSubType, sig->m_hackSubCnt, sig->m_punishFlag);
    if (db6->exec(true) != 1)
    {
        db6->set_query(
            "upDate auto_punish_first_user set occ_time=now(),ip='%s',cnt=%d, hack_sub_cnt=%d, punish_flag=%d where m_id=%s and hack_type=%d and hack_sub_type=%d",
            sig->m_ip, sig->m_cnt, sig->m_hackSubCnt, sig->m_punishFlag,
            NumberToString(sig->m_memberId, 0), sig->m_hackType,
            sig->m_hackSubType);
        if (db6->exec(true) != 1)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x70fb, 0);
            trace("upDate auto_punish_first_user Fail");
        }
    }
    // [b28d-TakeW8LT 声明位复原] ORIG 0x0842d8ab 才调 GetDBHandle(1,0)
    // （段1 之后、punishFlag 判断之前，结果落 -0x24）；旧形在函数开头提前调用，
    // 多发一次 GetDBHandle 并整体错帧。
    MySQL* db1 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                   E_SERVER_GROUP_NONE);
    // ORIG 0x0842d8ba..0x0842d8f3 三个独立 if-return（各 mov $1; jmp 独立岛），
    // 非短路合并链（P45 反向：合并形会产 je 共落点）
    if (sig->m_punishFlag == 2)
        return 1;
    if (sig->m_punishFlag == 7)
        return 1;
    if (sig->m_punishFlag == 8)
        return 1;
    punishType = 3;                     // ORIG 0x0842d8f8 movl $0x3,-0x30
    if (sig->m_punishFlag == 10)
        punishType = 0xc;
    unsigned int dwPunishDay = sig->m_etcValue / 10;  // ORIG local_24
    unsigned int dwApplyFlag = sig->m_etcValue % 10;  // ORIG local_20
    printf("[ON!!] request->etcValue_ : %d , dwPunishDay(%d), dwApplyFlag(%d)\n",
           sig->m_etcValue, dwPunishDay, dwApplyFlag);
    if (dwPunishDay == 0)
    {
        db1->set_query(
            "upDate member_punish_info set occ_time=now(), start_time=now(), end_time='9999-12-31 23:59:59', apply_flag=%d where m_id=%s and punish_type=%d",
            dwApplyFlag != 0 ? 1 : 2, NumberToString(sig->m_memberId, 0),
            punishType);
    }
    else
    {
        db1->set_query(
            "upDate member_punish_info set occ_time=now(), start_time=now(), end_time= adddate(now(),interval %d day), apply_flag=%d where m_id=%s and punish_type=%d",
            dwPunishDay, dwApplyFlag != 0 ? 1 : 2,
            NumberToString(sig->m_memberId, 0), punishType);
    }
    // ORIG 0x0842da37..0x0842da62 无槽往返（xor $1;test;jne; call;or;test;jne;
    // mov$1/jmp/mov$0; test %al,%al; je）⇒ 谓词整体内联，禁具名 bool（P198②）
    if (!(db1->exec(true) == 1 && db1->getAffectedRowCount() != 0))
    {
        if (dwPunishDay == 0)
        {
            db1->set_query(
                "inSert into member_punish_info(m_id, punish_type, occ_time, start_time, end_time, punish_value, apply_flag) values(%s, %d, now(), now(), '9999-12-31 23:59:59', 0, %d)",
                NumberToString(sig->m_memberId, 0), punishType,
                dwApplyFlag != 0 ? 1 : 2);
        }
        else
        {
            db1->set_query(
                "inSert into member_punish_info(m_id, punish_type, occ_time, start_time, end_time, punish_value, apply_flag) values(%s, %d, now(), now(), adddate(now(),interval %d day), 0, %d)",
                NumberToString(sig->m_memberId, 0), punishType, dwPunishDay,
                dwApplyFlag != 0 ? 1 : 2);
        }
        if (db1->exec(true) != 1)
            return 0;
    }
    // ORIG 0x0842db2d mov $0xff,%ebx（待清字节数 255）+ 起始槽 -0x17b 非 4 对齐
    // ⇒ 0xff 字节数组（P312/P253；旧 0x100 使 4.4 折 0x40 dwords 简单 rep stos）
    char reason[0xff] = { '\0' };
    sprintf(reason, "{Auto 1st Information-Mail}|*|ARS 1st Punish / %d / %d",
            sig->m_hackType, sig->m_hackSubType);
    char year[10] = { '\0' };
    time_t t = GlobalData::s_systemTime_.getCurSec();
    struct tm tmBuf;
    localtime_r(&t, &tmBuf);
    strftime(year, 10, "%Y", &tmBuf);
    if (dwPunishDay == 0)
    {
        db1->set_query(
            "inSert into member_punish_info_history_%s(m_id, punish_type, occ_time, punish_value, start_time, end_time, admin_id, apply_flag, reason) values(%s, %d, now(), 6, now(), '9999-12-31 23:59:59', 'ARS', %d, '%s')",
            year, NumberToString(sig->m_memberId, 0), punishType,
            dwApplyFlag != 0 ? 1 : 2, reason);
    }
    else
    {
        db1->set_query(
            "inSert into member_punish_info_history_%s(m_id, punish_type, occ_time, punish_value, start_time, end_time, admin_id, apply_flag, reason) values(%s, %d, now(), 6, now(), adddate(now(),interval %d day), 'ARS', %d, '%s')",
            year, NumberToString(sig->m_memberId, 0), punishType, dwPunishDay,
            dwApplyFlag != 0 ? 1 : 2, reason);
    }
    if (db1->exec(true) != 1)
        return 0;
    if (punishType == 3 || (punishType == 4 && dwApplyFlag == 0))
    {
        db1->set_query(
            "inSert into member_punish_info_ars_mail (m_id, punish_type, apply_flag, occ_time) values(%s, %d, %d, now())",
            NumberToString(sig->m_memberId, 0), punishType,
            dwApplyFlag != 0 ? 1 : 2);
        if (db1->exec(true) != 1)
            return 0;
    }
    return 1;
}