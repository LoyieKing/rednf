bool DB_UpdateQuest::_updateQuests(SIG_UPDATE_QUEST* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    // ORIG 0x8418c48 `movl $0x7530,-0x20(%ebp)` 与 0x8418c75/78
    // `mov -0x28(%ebp),%eax; mov %eax,-0x1c(%ebp)`：两个写后未读的死局部
    // （全函数再无 -0x20/-0x1c 引用，objdump 实证）——P171/P232 照抄，
    // 同时把帧 0x8ed8→0x8edc 与 db(-0x24)/zipLen(-0x28) 槽位对齐 ORIG。
    int maxSrcLen = 30000;
    char zipBuf[0x8cac] = { 0 };
    unsigned int zipLen = 0x8cac;
    unsigned int origZipLen = zipLen;   // ORIG 0x8418c75/78 写后未读死槽
    (void)maxSrcLen;
    (void)origZipLen;
    if (!compress_zip(zipBuf, zipLen,
                      reinterpret_cast<const char*>(((DB_UpdateQuestView*)sig)->m_clearData),
                      30000))
    {
        LogManager::logFormat(
            1, "DBThread.cpp",
            __PRETTY_FUNCTION__, 0x4a5e,
            "3RD_EXPEND : DB_UpdateQuest::DB ERROR!! charac_no(%u)",
            sig->m_characNo);
        return false;
    }
    // ORIG 0x8418cea/0x8418d07/0x8418d34 等处一律 `mov 0xc(%ebp),%eax;
    // add $0xa4,%eax` 重读 sig 参数（无视图指针局部）⇒ P183 去缓存，
    // 逐点内联；原 `DB_UpdateQuestView* v` 局部会多占 -0x1c 槽并挤出 Y 死槽。
    // ORIG 0x8418cf6 `test %al,%al; je <flag==0 块>`（0x8419132 在函数后段）
    // ⇒ 源码块序 = flag!=0 块在前、flag==0 块在后（4.4 对 then 恒前置）。
    if (((DB_UpdateQuestView*)sig)->m_clearFlag != 0)
    {
        db->set_query(
            "upDate new_charac_quest set clear_quest='%s', play_1=%d, play_1_trigger=%d, play_2=%d, play_2_trigger=%d, "
            "play_3=%d, play_3_trigger=%d, play_4=%d, play_4_trigger=%d, play_5=%d, play_5_trigger=%d,"
            "play_6=%d, play_6_trigger=%d, play_7=%d, play_7_trigger=%d, play_8=%d, play_8_trigger=%d, "
            "play_9=%d, play_9_trigger=%d, play_10=%d, play_10_trigger=%d, play_11=%d, play_11_trigger=%d, "
            "play_12=%d, play_12_trigger=%d, play_13=%d, play_13_trigger=%d, play_14=%d, play_14_trigger=%d, "
            "play_15=%d, play_15_trigger=%d,play_16=%d, play_16_trigger=%d, play_17=%d, play_17_trigger=%d, "
            "play_18=%d, play_18_trigger=%d, play_19=%d, play_19_trigger=%d, play_20=%d, play_20_trigger=%d,"
            "urgentQuestIndex=%d,quest_notify='%s' where charac_no=%u",
            db->blob_to_str(0, zipBuf, zipLen),
            ((DB_UpdateQuestView*)sig)->m_play[0], ((DB_UpdateQuestView*)sig)->m_playTrigger[0], ((DB_UpdateQuestView*)sig)->m_play[1], ((DB_UpdateQuestView*)sig)->m_playTrigger[1],
            ((DB_UpdateQuestView*)sig)->m_play[2], ((DB_UpdateQuestView*)sig)->m_playTrigger[2], ((DB_UpdateQuestView*)sig)->m_play[3], ((DB_UpdateQuestView*)sig)->m_playTrigger[3],
            ((DB_UpdateQuestView*)sig)->m_play[4], ((DB_UpdateQuestView*)sig)->m_playTrigger[4], ((DB_UpdateQuestView*)sig)->m_play[5], ((DB_UpdateQuestView*)sig)->m_playTrigger[5],
            ((DB_UpdateQuestView*)sig)->m_play[6], ((DB_UpdateQuestView*)sig)->m_playTrigger[6], ((DB_UpdateQuestView*)sig)->m_play[7], ((DB_UpdateQuestView*)sig)->m_playTrigger[7],
            ((DB_UpdateQuestView*)sig)->m_play[8], ((DB_UpdateQuestView*)sig)->m_playTrigger[8], ((DB_UpdateQuestView*)sig)->m_play[9], ((DB_UpdateQuestView*)sig)->m_playTrigger[9],
            ((DB_UpdateQuestView*)sig)->m_play[10], ((DB_UpdateQuestView*)sig)->m_playTrigger[10], ((DB_UpdateQuestView*)sig)->m_play[11], ((DB_UpdateQuestView*)sig)->m_playTrigger[11],
            ((DB_UpdateQuestView*)sig)->m_play[12], ((DB_UpdateQuestView*)sig)->m_playTrigger[12], ((DB_UpdateQuestView*)sig)->m_play[13], ((DB_UpdateQuestView*)sig)->m_playTrigger[13],
            ((DB_UpdateQuestView*)sig)->m_play[14], ((DB_UpdateQuestView*)sig)->m_playTrigger[14], ((DB_UpdateQuestView*)sig)->m_play[15], ((DB_UpdateQuestView*)sig)->m_playTrigger[15],
            ((DB_UpdateQuestView*)sig)->m_play[16], ((DB_UpdateQuestView*)sig)->m_playTrigger[16], ((DB_UpdateQuestView*)sig)->m_play[17], ((DB_UpdateQuestView*)sig)->m_playTrigger[17],
            ((DB_UpdateQuestView*)sig)->m_play[18], ((DB_UpdateQuestView*)sig)->m_playTrigger[18], ((DB_UpdateQuestView*)sig)->m_play[19], ((DB_UpdateQuestView*)sig)->m_playTrigger[19],
            ((DB_UpdateQuestView*)sig)->m_urgentIndex,
            db->blob_to_str(1, ((DB_UpdateQuestView*)sig)->m_questNotify, 0x28),
            ((DB_UpdateQuestView*)sig)->m_characNo);
    }
    else
    {
        db->set_query(
            "upDate new_charac_quest set play_1=%d, play_1_trigger=%d, play_2=%d, play_2_trigger=%d, "
            "play_3=%d, play_3_trigger=%d, play_4=%d, play_4_trigger=%d, play_5=%d, play_5_trigger=%d,"
            "play_6=%d, play_6_trigger=%d, play_7=%d, play_7_trigger=%d, play_8=%d, play_8_trigger=%d, "
            "play_9=%d, play_9_trigger=%d, play_10=%d, play_10_trigger=%d, play_11=%d, play_11_trigger=%d, "
            "play_12=%d, play_12_trigger=%d, play_13=%d, play_13_trigger=%d, play_14=%d, play_14_trigger=%d, "
            "play_15=%d, play_15_trigger=%d,play_16=%d, play_16_trigger=%d, play_17=%d, play_17_trigger=%d, "
            "play_18=%d, play_18_trigger=%d, play_19=%d, play_19_trigger=%d, play_20=%d, play_20_trigger=%d,"
            "urgentQuestIndex=%d,quest_notify='%s' where charac_no=%u",
            ((DB_UpdateQuestView*)sig)->m_play[0], ((DB_UpdateQuestView*)sig)->m_playTrigger[0], ((DB_UpdateQuestView*)sig)->m_play[1], ((DB_UpdateQuestView*)sig)->m_playTrigger[1],
            ((DB_UpdateQuestView*)sig)->m_play[2], ((DB_UpdateQuestView*)sig)->m_playTrigger[2], ((DB_UpdateQuestView*)sig)->m_play[3], ((DB_UpdateQuestView*)sig)->m_playTrigger[3],
            ((DB_UpdateQuestView*)sig)->m_play[4], ((DB_UpdateQuestView*)sig)->m_playTrigger[4], ((DB_UpdateQuestView*)sig)->m_play[5], ((DB_UpdateQuestView*)sig)->m_playTrigger[5],
            ((DB_UpdateQuestView*)sig)->m_play[6], ((DB_UpdateQuestView*)sig)->m_playTrigger[6], ((DB_UpdateQuestView*)sig)->m_play[7], ((DB_UpdateQuestView*)sig)->m_playTrigger[7],
            ((DB_UpdateQuestView*)sig)->m_play[8], ((DB_UpdateQuestView*)sig)->m_playTrigger[8], ((DB_UpdateQuestView*)sig)->m_play[9], ((DB_UpdateQuestView*)sig)->m_playTrigger[9],
            ((DB_UpdateQuestView*)sig)->m_play[10], ((DB_UpdateQuestView*)sig)->m_playTrigger[10], ((DB_UpdateQuestView*)sig)->m_play[11], ((DB_UpdateQuestView*)sig)->m_playTrigger[11],
            ((DB_UpdateQuestView*)sig)->m_play[12], ((DB_UpdateQuestView*)sig)->m_playTrigger[12], ((DB_UpdateQuestView*)sig)->m_play[13], ((DB_UpdateQuestView*)sig)->m_playTrigger[13],
            ((DB_UpdateQuestView*)sig)->m_play[14], ((DB_UpdateQuestView*)sig)->m_playTrigger[14], ((DB_UpdateQuestView*)sig)->m_play[15], ((DB_UpdateQuestView*)sig)->m_playTrigger[15],
            ((DB_UpdateQuestView*)sig)->m_play[16], ((DB_UpdateQuestView*)sig)->m_playTrigger[16], ((DB_UpdateQuestView*)sig)->m_play[17], ((DB_UpdateQuestView*)sig)->m_playTrigger[17],
            ((DB_UpdateQuestView*)sig)->m_play[18], ((DB_UpdateQuestView*)sig)->m_playTrigger[18], ((DB_UpdateQuestView*)sig)->m_play[19], ((DB_UpdateQuestView*)sig)->m_playTrigger[19],
            ((DB_UpdateQuestView*)sig)->m_urgentIndex,
            db->blob_to_str(1, ((DB_UpdateQuestView*)sig)->m_questNotify, 0x28),
            ((DB_UpdateQuestView*)sig)->m_characNo);
    }
    if (db->exec(true) != 1 || db->getAffectedRowCount() == 0)
        return false;
    return true;
