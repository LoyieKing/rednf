bool DB_MailBox_Req_Send::insertMailData(MySQL* db, const SIG_MAILBOX_SEND_NEW_MAIL* sigRaw)
{
    static int last_letter_id = 0;   // ORIG _ZZN19DB_MailBox_Req_Send14insertMailDataEP5MySQLPK25SIG_MAILBOX_SEND_NEW_MAILE14last_letter_id 0x0943c230
    struct tm tmBuf;
    time_t now;
    struct tm* tmPtr;
    time_t t;
    int letterId;

    now = GlobalData::s_systemTime_.getCurSec();
    tmPtr = localtime_r(&now, &tmBuf);
    tmPtr->tm_hour += 1;
    tmPtr->tm_min = 0;
    tmPtr->tm_sec = 0;
    t = mktime(tmPtr);

    if (((const SigMailboxSendNewMailView*)sigRaw)->m_flag != 0)
    {
        t = (int)((const SigMailboxSendNewMailView*)sigRaw)->m_nB - 0x13c680;
    }
    else if (((const SigMailboxSendNewMailView*)sigRaw)->m_nB <= 0xe)
    {
        // b28d: P211 档③——LHS 与 RHS 异型（unsigned int）⇒ 4.4 出 ORIG 的
        // `mov t,%edx; imul; mov %edx,%ecx; sub %eax,%ecx; mov %ecx,%eax; mov %eax,t`
        // （原 (int) 同型形恒折 `sub %eax,-0x1c(%ebp)`，ORIG 0x0841f389..0x841f3a1）。
        t = (time_t)((unsigned int)t - ((const SigMailboxSendNewMailView*)sigRaw)->m_nB * 0x15180u);
    }
    else if (((const SigMailboxSendNewMailView*)sigRaw)->m_nB > 0xf)
    {
        // b28d: 同上——`(unsigned int)t` 触发 lea 合并形
        // （ORIG 0x0841f3bd..0x841f3ce `imul→%edx; mov t,%eax; lea (%edx,%eax,1),%eax`）。
        t = (time_t)((int)(((const SigMailboxSendNewMailView*)sigRaw)->m_nB * 0x15180u) + (unsigned int)t - 0x13c680);
    }

    if (((const SigMailboxSendNewMailView*)sigRaw)->m_nType != 0 &&
        ((const SigMailboxSendNewMailView*)sigRaw)->m_nType != last_letter_id)
    {
        db->set_query("upDate letter set stat =%d where letter_id = %d",
                      1, ((const SigMailboxSendNewMailView*)sigRaw)->m_nType);
        db->exec(true);
    }
    last_letter_id = ((const SigMailboxSendNewMailView*)sigRaw)->m_nType;
    letterId = ((const SigMailboxSendNewMailView*)sigRaw)->m_nType;

    if (((const SigMailboxSendNewMailView*)sigRaw)->m_titleLen != 0 && letterId == 0)
    {
        if (!InsertLetter(db, ((const SigMailboxSendNewMailView*)sigRaw)->m_amount,
                          ((const SigMailboxSendNewMailView*)sigRaw)->m_senderCharacNo,
                          (const char*)sigRaw, ((const SigMailboxSendNewMailView*)sigRaw)->m_title,
                          letterId, t, (ENUM_LETTER_STAT)1))
        {
            LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0x550c,
                                  "InsertLetter() error");
        }
        if (((const SigMailboxSendNewMailView*)sigRaw)->m_addInfo == 0 &&
            ((const SigMailboxSendNewMailView*)sigRaw)->m_destCharacNo == 0)
        {
            return 1;
        }
    }

    char nameBuf[0x57] = { 0 };
    strcpy(nameBuf, (const char*)sigRaw);
    db->set_query(
        "inSert into postal(occ_time,send_charac_no,receive_charac_no,seal_flag,item_id,add_info,endurance,upgrade,amplify_option,amplify_value,gold,send_charac_name,creature_flag,avata_flag,letter_id,extend_info,item_guid,random_option,seperate_upgrade,unlimit_flag) values(from_unixtime(%d),%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s',%d,%d,%d,%d,'%s','%s',%d,%d)",
        t, ((const SigMailboxSendNewMailView*)sigRaw)->m_senderCharacNo,
        ((const SigMailboxSendNewMailView*)sigRaw)->m_amount,
        ((const SigMailboxSendNewMailView*)sigRaw)->m_itemField0,
        ((const SigMailboxSendNewMailView*)sigRaw)->m_addInfo,
        ((const SigMailboxSendNewMailView*)sigRaw)->m_addInfo2,
        ((const SigMailboxSendNewMailView*)sigRaw)->m_fieldb,
        ((const SigMailboxSendNewMailView*)sigRaw)->m_itemAttr,
        ((const SigMailboxSendNewMailView*)sigRaw)->m_ampType,
        ((const SigMailboxSendNewMailView*)sigRaw)->m_ampIdx,
        ((const SigMailboxSendNewMailView*)sigRaw)->m_destCharacNo, nameBuf,
        ((const SigMailboxSendNewMailView*)sigRaw)->m_isCreature,
        ((const SigMailboxSendNewMailView*)sigRaw)->m_isAvatarSend, letterId,
        ((const SigMailboxSendNewMailView*)sigRaw)->m_fieldd,
        // blob_to_str 按 ORIG +0x3a/+0x47 裸偏移（byte-blob 读 10/0xe 字节，非自然宽度
        // 字段；&view->m_field15/25 的 int 类型与读长不配会产生地址物化差异）。
        db->blob_to_str(0, (void*)((char*)sigRaw + 0x3a), 10),
        db->blob_to_str(1, (void*)((char*)sigRaw + 0x47), 0xe),
        ((const SigMailboxSendNewMailView*)sigRaw)->m_upgradeSep,
        ((const SigMailboxSendNewMailView*)sigRaw)->m_cashShopFlag);
    return db->exec(true);
