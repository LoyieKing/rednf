bool DB_MailBox_Req_System_Multi_Mail::_InsertPostal(
    MySQL* db, SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL* sigRaw)
{
    std::string str;
    unsigned int i;

    str.clear();

    for (i = 0; i < ((SigSystemMultiMailView*)sigRaw)->m_count; ++i)
    {
        if (i != 0)
        {
            ((SigSystemMultiMailView*)sigRaw)->m_gold = 0;
        }

        unsigned char ampType = 0;
        unsigned short ampValue = 0;
        ((SigSystemMultiMailView*)sigRaw)->m_item[i].m_ag.m_amp.getValues(
            ampType, ampValue);

        char buf[0x400] = { 0 };

        sprintf(buf,
            "(from_unixtime(%d),%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s',%d,%d,%d,%d,'%s','%s',%d,%d)",
            ((SigSystemMultiMailView*)sigRaw)->m_occTime,
            ((SigSystemMultiMailView*)sigRaw)->m_sendCharacNo,
            ((SigSystemMultiMailView*)sigRaw)->m_recvCharacNo,
            ((SigSystemMultiMailView*)sigRaw)->m_item[i].m_sealFlag,
            ((SigSystemMultiMailView*)sigRaw)->m_item[i].m_itemId,
            ((SigSystemMultiMailView*)sigRaw)->m_item[i].m_addInfo,
            ((SigSystemMultiMailView*)sigRaw)->m_item[i].m_endurance,
            ((Inven_Item*)&((SigSystemMultiMailView*)sigRaw)->m_item[i])->GetItemAttr(),
            ampType, ampValue,
            ((SigSystemMultiMailView*)sigRaw)->m_gold, (const char*)sigRaw,
            ((Inven_Item*)&((SigSystemMultiMailView*)sigRaw)->m_item[i])->IsCreatureItemType(),
            ((Inven_Item*)&((SigSystemMultiMailView*)sigRaw)->m_item[i])->isAvatarItemType(),
            ((SigSystemMultiMailView*)sigRaw)->m_letterId,
            ((SigSystemMultiMailView*)sigRaw)->m_item[i].m_extendInfo,
            db->blob_to_str(0,
                (void*)&((SigSystemMultiMailView*)sigRaw)->m_item[i].m_ag.m_g.m_guidBlob[0], 10),
            db->blob_to_str(1,
                (void*)&((SigSystemMultiMailView*)sigRaw)->m_item[i].m_randomBlob[0], 0xe),
            ((UpgradeSeparateInfo*)&((SigSystemMultiMailView*)sigRaw)->m_item[i].m_seperateUpgrade)->GetUpgradeSeparate(),
            ((SigSystemMultiMailView*)sigRaw)->m_unlimitFlag);
        str.append(buf);

        if (i < ((SigSystemMultiMailView*)sigRaw)->m_count - 1)
        {
            str.append(",");
        }
    }

    db->set_query(
        "inSert into postal(occ_time,send_charac_no,receive_charac_no,seal_flag,item_id,add_info,endurance,upgrade ,amplify_option,amplify_value,gold,send_charac_name,creature_flag,avata_flag,letter_id,extend_info,item_guid,random_option,seperate_upgrade,unlimit_flag) values %s",
        str.c_str());

    if (!db->exec(true))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xa5b5, 5)(
            "Insert Postal Fail - inQuery(%s)", str.c_str());
        return 0;
    }
    return 1;
