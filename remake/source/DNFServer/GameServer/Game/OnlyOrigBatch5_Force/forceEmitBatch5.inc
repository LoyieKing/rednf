void forceEmitBatch5()
{
    // ---- Packet_* ctor（shared/packet/include 头内联，PacketHeader + 字段清零）----
    Packet_DeathTower_Statistic_Value p1;
    (void)p1;
    Packet_Forbid_Chat_By_Monitor p2;
    (void)p2;
    Packet_GameServer2Statisctics2DBServer p3;
    (void)p3;
    Packet_Guild_Check_Guild_Cargo_Push_Item p4;
    (void)p4;
    Packet_Guild_Guild_Cargo_Move_Item p5;
    (void)p5;
    Packet_Guild_Guild_Cargo_Pop_Item p6;
    (void)p6;
    Packet_HellParty_Statistic_Item p7;
    (void)p7;
    Packet_Monitor_Guild_Chat p8;
    (void)p8;
    Packet_Monitor_MegaPhone p9;
    (void)p9;
    Packet_Monitor_Member_Chat p10;
    (void)p10;
    Packet_Monitor_Notice_Message p11;
    (void)p11;
    Packet_Monitor_Other_Channel_Chat_Hyper_Link p12;
    (void)p12;
    Packet_P2P_Statistics p13;
    (void)p13;
    Packet_Pcroom_Response p14;
    (void)p14;
    Packet_PvPResult p15;
    (void)p15;
    Packet_User_Ting_TimeCheck_Statistic_Add p16;
    (void)p16;
    Packet_Value_Statistic p17;
    (void)p17;
    Packet_VillageMonsterFightResult p18;
    (void)p18;

    // ---- PassiveObjGenerator* 拷贝构造（隐式生成，force C1ERKS）----
    PassiveObjGeneratorObject o1;
    PassiveObjGeneratorObject o2(o1);
    (void)o2;
    PassiveObjGeneratorPattern n1;
    PassiveObjGeneratorPattern n2(n1);
    (void)n2;
    PassiveObjGeneratorSequence q1;
    PassiveObjGeneratorSequence q2(q1);
    (void)q2;

    // ---- PostalReward 拷贝构造（隐式生成，2 string + 2 int）----
    PostalReward r1;
    PostalReward r2(r1);
    (void)r2;

    // ---- PvPSkillTreeKey 3 参 ctor（Ehhh）----
    PvPSkillTreeKey k1((unsigned char)1, (unsigned char)2, (unsigned char)3);
    (void)k1;

    // ---- RandomApplicationKey 默认 ctor（两字节清零）----
    RandomApplicationKey a1;
    (void)a1;

    // ---- QuantityRatio / ResetValue / PartSelectonField 空析构（D1/D2）----
    {
        QuantityRatio t1;
        (void)t1;
        ResetValue t2;
        (void)t2;
        PartSelectonField t3;
        (void)t3;
    }

    // ---- SIG_* ctor ----
    SIG_ATTENDANCE_EVENT s1;
    (void)s1;
    SIG_BEST_CLEAR_TIME s2;
    (void)s2;
    SIG_BEST_CLEAR_TIME_DATA s3;
    (void)s3;
    SIG_BOOSTER_GAGE_DATA s4;
    (void)s4;
    SIG_BREAK_AWAY_PREVENT_REWARD_MANAGER s5;
    (void)s5;
    SIG_BREAK_AWAY_PREVENT_SYSTEM_MANAGER s6;
    (void)s6;
    SIG_CERASHOP_ERROR_LOG s7;
    (void)s7;
    SIG_COMBO_SKILL s8;
    (void)s8;

    // ---- OnlyOrigBatch14：头内联 operator=/clear 的 W 符号发射 ----
    // PassiveIObjSniperInfo::operator= W 0x08a3e76a
    PassiveIObjSniperInfo sniperA;
    PassiveIObjSniperInfo sniperB;
    sniperB = sniperA;
    // PassiveObjGeneratorObject/Pattern/Sequence::operator= W
    PassiveObjGeneratorObject o1b;
    PassiveObjGeneratorObject o2b;
    o2b = o1b;
    PassiveObjGeneratorPattern n1b;
    PassiveObjGeneratorPattern n2b;
    n2b = n1b;
    PassiveObjGeneratorSequence q1b;
    PassiveObjGeneratorSequence q2b;
    q2b = q1b;
    // LevelUpRewardItemTableKey::clear W 0x0891bad4
    LevelUpRewardItemTableKey key1;
    key1.clear();
    (void)key1;
