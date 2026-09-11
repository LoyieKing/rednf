// ============================================================================
// DB_LoadQuest::GetUserQuest @0x0840e39a
// 列序：charac_no(0)/clear_quest(1)/play_1..play_20(2..39 交替 quest/trigger)/
// urgentQuestIndex(40)/quest_notify(41)。clear_quest blob 解压进 +0xb0；
// 40 个 int 依序 get_int 写 m_quest[i]/m_trigger[i]；+0xac urgent、
// +0x75e0 notify(0x28)。任一步失败 throw int（DBThread.cpp 行号），
// catch 后 logFormat 0x2a81 并返 0；成功返 1。
// ============================================================================
// [SweepDBLoadEtcMain 本波复核] 1030/1029（gap=1）、p167 CLEAR（156/156，仅 cMyTrace
// C1/C2 同址别名）、帧 `sub $0x8d00` 与 push 集全等；残余 3 处全部封顶：
//   ① memset size 经 `mov $0x8cac,%edx; mov %edx,0x8(%esp)` 中转（ORIG 0840e3e7）
//      —— 范式库「待解」条目；4.4 对字面量/cast/`'\0'`/sizeof 四形恒 `movl $imm,
//      0x8(%esp)`（探针 /tmp/memsetp.cpp f1..f4）⇒ N；
//   ② ORIG 0840e634 `nop`（if-join 对齐填充，§6.1 豁免）；
//   ③ 成功块 `mov $0x1,%ebx` 位置：ORIG 0840f47d 在 catch 块之后（`je` 直达尾块），
//      4.4 置于 try 末尾 `mov $0x1,%ebx; jmp`（0840f3d4 对应处）⇒ N104 EH 块相对序；
//   ④ 槽号恒定平移 4：ORIG db -0x18/col -0x14/buf -0x8cdc vs 我方 -0x1c/-0x18/
//      -0x8ce0（大数组+标量混合区，P331 声明序不可翻）⇒ N82/N95。
bool DB_LoadQuest::GetUserQuest(SIG_LOAD_QUEST* sig)
{
    char buf[0x8cac];
    unsigned int outLen;
    unsigned int col0;
    MySQL* db;
    int idx;
    unsigned int binLen;

    db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                           (ENUM_SERVER_GROUP)0);
    db->set_query("seLect charac_no, clear_quest,play_1, play_1_trigger, play_2,"
                  " play_2_trigger, play_3, play_3_trigger, play_4,"
                  " play_4_trigger, play_5, play_5_trigger,play_6,"
                  " play_6_trigger, play_7, play_7_trigger, play_8,"
                  " play_8_trigger, play_9, play_9_trigger, play_10,"
                  " play_10_trigger,play_11, play_11_trigger, play_12,"
                  " play_12_trigger, play_13, play_13_trigger, play_14,"
                  " play_14_trigger, play_15, play_15_trigger,play_16,"
                  " play_16_trigger, play_17, play_17_trigger, play_18,"
                  " play_18_trigger, play_19, play_19_trigger, play_20,"
                  " play_20_trigger,urgentQuestIndex,quest_notify from"
                  " new_charac_quest  where charac_no=%u", sig->m_characNo);
    memset(buf, 0, 0x8cac);

    outLen = 0x8cac;
    idx = 0;

    try
    {
        if (!db->exec(true))
        {
            throw 0x2a30;
        }
        if (db->get_n_rows() == 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x2a33,
                     5)("QUEST_EXPEND_LOG : NO QUEST DATA EXIST!! :"
                        " CHARAC_NO(%u)", sig->m_characNo);
            throw 0x2a34;
        }
        if (!db->fetch())
        {
            throw 0x2a36;
        }
        col0 = 0;
        if (!db->get_int(idx++, col0))
        {
            throw 0x2a39;
        }
        unsigned int binLen = db->get_binary_length(idx);
        if (!db->get_binary(idx++, buf, outLen))
        {
            throw 0x2a3c;
        }
        if (!uncompress_zip((char*)sig->m_cleared, outLen, buf, binLen))
        {
            throw 0x2a3d;
        }
        if (!db->get_int(idx++, sig->m_quest[0]))
        {
            throw 0x2a40;
        }
        if (!db->get_int(idx++, sig->m_trigger[0]))
        {
            throw 0x2a41;
        }
        if (!db->get_int(idx++, sig->m_quest[1]))
        {
            throw 0x2a42;
        }
        if (!db->get_int(idx++, sig->m_trigger[1]))
        {
            throw 0x2a43;
        }
        if (!db->get_int(idx++, sig->m_quest[2]))
        {
            throw 0x2a44;
        }
        if (!db->get_int(idx++, sig->m_trigger[2]))
        {
            throw 0x2a45;
        }
        if (!db->get_int(idx++, sig->m_quest[3]))
        {
            throw 0x2a46;
        }
        if (!db->get_int(idx++, sig->m_trigger[3]))
        {
            throw 0x2a47;
        }
        if (!db->get_int(idx++, sig->m_quest[4]))
        {
            throw 0x2a48;
        }
        if (!db->get_int(idx++, sig->m_trigger[4]))
        {
            throw 0x2a49;
        }
        if (!db->get_int(idx++, sig->m_quest[5]))
        {
            throw 0x2a4a;
        }
        if (!db->get_int(idx++, sig->m_trigger[5]))
        {
            throw 0x2a4b;
        }
        if (!db->get_int(idx++, sig->m_quest[6]))
        {
            throw 0x2a4c;
        }
        if (!db->get_int(idx++, sig->m_trigger[6]))
        {
            throw 0x2a4d;
        }
        if (!db->get_int(idx++, sig->m_quest[7]))
        {
            throw 0x2a4e;
        }
        if (!db->get_int(idx++, sig->m_trigger[7]))
        {
            throw 0x2a4f;
        }
        if (!db->get_int(idx++, sig->m_quest[8]))
        {
            throw 0x2a50;
        }
        if (!db->get_int(idx++, sig->m_trigger[8]))
        {
            throw 0x2a51;
        }
        if (!db->get_int(idx++, sig->m_quest[9]))
        {
            throw 0x2a52;
        }
        if (!db->get_int(idx++, sig->m_trigger[9]))
        {
            throw 0x2a53;
        }
        if (!db->get_int(idx++, sig->m_quest[10]))
        {
            throw 0x2a55;
        }
        if (!db->get_int(idx++, sig->m_trigger[10]))
        {
            throw 0x2a56;
        }
        if (!db->get_int(idx++, sig->m_quest[11]))
        {
            throw 0x2a57;
        }
        if (!db->get_int(idx++, sig->m_trigger[11]))
        {
            throw 0x2a58;
        }
        if (!db->get_int(idx++, sig->m_quest[12]))
        {
            throw 0x2a59;
        }
        if (!db->get_int(idx++, sig->m_trigger[12]))
        {
            throw 0x2a5a;
        }
        if (!db->get_int(idx++, sig->m_quest[13]))
        {
            throw 0x2a5b;
        }
        if (!db->get_int(idx++, sig->m_trigger[13]))
        {
            throw 0x2a5c;
        }
        if (!db->get_int(idx++, sig->m_quest[14]))
        {
            throw 0x2a5d;
        }
        if (!db->get_int(idx++, sig->m_trigger[14]))
        {
            throw 0x2a5e;
        }
        if (!db->get_int(idx++, sig->m_quest[15]))
        {
            throw 0x2a5f;
        }
        if (!db->get_int(idx++, sig->m_trigger[15]))
        {
            throw 0x2a60;
        }
        if (!db->get_int(idx++, sig->m_quest[16]))
        {
            throw 0x2a61;
        }
        if (!db->get_int(idx++, sig->m_trigger[16]))
        {
            throw 0x2a62;
        }
        if (!db->get_int(idx++, sig->m_quest[17]))
        {
            throw 0x2a63;
        }
        if (!db->get_int(idx++, sig->m_trigger[17]))
        {
            throw 0x2a64;
        }
        if (!db->get_int(idx++, sig->m_quest[18]))
        {
            throw 0x2a65;
        }
        if (!db->get_int(idx++, sig->m_trigger[18]))
        {
            throw 0x2a66;
        }
        if (!db->get_int(idx++, sig->m_quest[19]))
        {
            throw 0x2a67;
        }
        if (!db->get_int(idx++, sig->m_trigger[19]))
        {
            throw 0x2a68;
        }
        if (!db->get_int(idx++, sig->m_urgentQuest))
        {
            throw 0x2a6f;
        }
        if (!db->get_binary(idx++, sig->m_authen, 0x28))
        {
            throw 0x2a7c;
        }
        return 1;
    }
    catch (int err)
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x2a81,
                              "3RD_EXPEND : DB_LoadQuest::GetUserQuest line(%d)"
                              " charac_no(%u)", err, sig->m_characNo);
        return 0;
    }
}