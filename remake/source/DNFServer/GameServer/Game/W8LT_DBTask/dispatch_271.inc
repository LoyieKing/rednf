bool DB_MailBoxSendNewMailWithDBWork::dispatch(int userIdx, int slot,
                                               Stream* stream)
{
    static const int handleIdx = 3;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    DB_MailBoxSendNewMailView* sig =
        reinterpret_cast<DB_MailBoxSendNewMailView*>(
            stream->GetOutBuffer<SIG_MAILBOX_SEND_NEW_MAIL_WITH_DBWORK>());
    char mailBuf[0x168];
    memset(mailBuf, 0, sizeof(mailBuf));
    DB_MailSendDataView* buf = reinterpret_cast<DB_MailSendDataView*>(mailBuf);
    strncpy(buf->m_sender, sig->m_sender, 0x1d);
    buf->m_field22 = sig->m_field24;
    buf->m_field1e = sig->m_field28;
    buf->m_field26 = 0;
    buf->m_field57 = sig->m_field1a2;
    if (sig->m_titleLen != 0)
    {
        buf->m_field59 = sig->m_titleLen;
        strncpy(buf->m_title, sig->m_title, sig->m_titleLen);
    }
    buf->m_field2a = 0;
    buf->m_field33 = 0;
    try
    {
        if (sig->m_type == 0)
        {
            if (!DB_AvatarItemInsert::InsertAvatarItem(
                    reinterpret_cast<SIG_AVATAR_INSERT*>(sig->m_attach)))
                throw (unsigned int)0x6871;
            unsigned int identity = WongWork::DBCommon::GetIdentity(db);
            if (identity == 0)
                throw (unsigned int)0x6874;
            buf->m_field2b = sig->m_field30;
            buf->m_field34 = (unsigned short)sig->m_field35;
            buf->m_identity = identity;
            buf->m_field36 = sig->m_field54;
            buf->m_field56 = 1;
        }
        else if (sig->m_type == 1)
        {
            if (!DB_InsertCreatureItem::insertCreatureItem(
                    db, reinterpret_cast<SIG_CREATURE_ITEM*>(sig->m_attach)))
                throw (unsigned int)0x6883;
            unsigned int identity = WongWork::DBCommon::GetIdentity(db);
            if (identity == 0)
                throw (unsigned int)0x6886;
            buf->m_field2b = sig->m_field34;
            buf->m_field34 = sig->m_field40;
            buf->m_identity = identity;
            buf->m_field58 = 1;
        }
        if (!DB_MailBox_Req_Send::insertMailData(
                db, reinterpret_cast<const SIG_MAILBOX_SEND_NEW_MAIL*>(mailBuf)))
            throw (unsigned int)0x6891;
        if (!DB_MailBox_Req_Send::sendResult(slot, 0, sig->m_field24,
                                             (ENUM_SERVER_GROUP)0))
            throw (unsigned int)0x6894;   // ORIG xor$1/test%al vs 4.4 sete 物化，跨版本残差
    }
    catch (unsigned int e)
    {
        // ORIG 0x0842761c：cMyTrace(__PRETTY_FUNCTION__, 0x6899, 5) + trace("%d", e)。
        cMyTrace trace(__PRETTY_FUNCTION__, 0x6899, 5);
        trace("%d", e);
    }
    return 1;
