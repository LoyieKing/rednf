// ORIG 0x8418c1c：GetDBHandle(2,0) → zipBuf[0x8cac] 清零 → compress_zip(
// zipBuf, zipLen, sig+0xa4, 30000) 失败 logFormat(0x4a5e) 返 false；
// +0x75d4 标志为 0 走无 clear_quest SQL，否则压缩串作 clear_quest='%s' 首占位；
// 40 个 play/trigger 字段 + urgentQuestIndex(+0x75d5) + quest_notify blob
// (blob_to_str(1, sig+0x75d9, 0x28)) + charac_no；exec 成功且 affected!=0 返 true。
// §6.2 巨件核销（b20 首判，b25 复验）：P167 11/11 [0缺0多]；gap 1→1；
//   残差归因 N82（帧槽平移）+ 40 个 play/trigger 字段 set_query 实参栈槽序
//   跨版本排布（详见 docs/asm_translation_patterns.md 负例表）。
//   语义与 ORIG 一致（upDate new_charac_quest 两分支 SQL + blob_to_str 载荷）
//   ⇒ 语义完成、byte 层 N 封顶。
// [b28d-4] AE：P183 去缓存（删 DB_UpdateQuestView* v 局部、85 处内联 sig 重读）+ P171 补两死局部(maxSrcLen=30000@-0x20 / origZipLen@-0x1c, ORIG 0x8418c48/0x8418c75)+ P222 交换 clear_quest 块序（ORIG 0x8418cf6 je 落 flag==0 块）
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
                      reinterpret_cast<const char*>(sig->m_clearedQuest),
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
    if (sig->m_clearedFlag != 0)
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
            sig->m_quest[0], sig->m_trigger[0], sig->m_quest[1], sig->m_trigger[1],
            sig->m_quest[2], sig->m_trigger[2], sig->m_quest[3], sig->m_trigger[3],
            sig->m_quest[4], sig->m_trigger[4], sig->m_quest[5], sig->m_trigger[5],
            sig->m_quest[6], sig->m_trigger[6], sig->m_quest[7], sig->m_trigger[7],
            sig->m_quest[8], sig->m_trigger[8], sig->m_quest[9], sig->m_trigger[9],
            sig->m_quest[10], sig->m_trigger[10], sig->m_quest[11], sig->m_trigger[11],
            sig->m_quest[12], sig->m_trigger[12], sig->m_quest[13], sig->m_trigger[13],
            sig->m_quest[14], sig->m_trigger[14], sig->m_quest[15], sig->m_trigger[15],
            sig->m_quest[16], sig->m_trigger[16], sig->m_quest[17], sig->m_trigger[17],
            sig->m_quest[18], sig->m_trigger[18], sig->m_quest[19], sig->m_trigger[19],
            sig->m_urgentQuest,
            db->blob_to_str(1, sig->m_authen, 0x28),
            sig->m_characNo);
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
            sig->m_quest[0], sig->m_trigger[0], sig->m_quest[1], sig->m_trigger[1],
            sig->m_quest[2], sig->m_trigger[2], sig->m_quest[3], sig->m_trigger[3],
            sig->m_quest[4], sig->m_trigger[4], sig->m_quest[5], sig->m_trigger[5],
            sig->m_quest[6], sig->m_trigger[6], sig->m_quest[7], sig->m_trigger[7],
            sig->m_quest[8], sig->m_trigger[8], sig->m_quest[9], sig->m_trigger[9],
            sig->m_quest[10], sig->m_trigger[10], sig->m_quest[11], sig->m_trigger[11],
            sig->m_quest[12], sig->m_trigger[12], sig->m_quest[13], sig->m_trigger[13],
            sig->m_quest[14], sig->m_trigger[14], sig->m_quest[15], sig->m_trigger[15],
            sig->m_quest[16], sig->m_trigger[16], sig->m_quest[17], sig->m_trigger[17],
            sig->m_quest[18], sig->m_trigger[18], sig->m_quest[19], sig->m_trigger[19],
            sig->m_urgentQuest,
            db->blob_to_str(1, sig->m_authen, 0x28),
            sig->m_characNo);
    }
    if (db->exec(true) != 1 || db->getAffectedRowCount() == 0)
        return false;
    return true;
}