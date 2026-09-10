void forceEmitBatch4()
{
    // CDungeonExpStatistic（头内联 ctor：map 默认构造）。ORIG 仅发 C1/C2
    // （W 0x082aa88a），无 D1/D2 ⇒ placement 构造触发 ctor ODR-use，对象不
    // 析构 ⇒ 不发射 dtor（栈对象会多发明 D1/D2）。
    char expStatStorage[sizeof(CDungeonExpStatistic)]
        __attribute__((aligned(8)));
    new (expStatStorage) CDungeonExpStatistic;

    // CDungeonSuccessNode（头内联拷贝构造：两 map 拷贝）
    CDungeonSuccessNode a1;
    CDungeonSuccessNode b1(a1);
    (void)b1;

    // ChangeStatusData（头内联拷贝构造 + operator=，P147 force 发射
    // _ZN16ChangeStatusDataC1ERKS_ / _ZN16ChangeStatusDataaSERKS_）
    ChangeStatusData c1;
    ChangeStatusData c2(c1);
    c1 = c2;
    (void)c1;

    // CharacterKind（头内联 ctor：两字节清零）
    CharacterKind d1;
    (void)d1;

    // DimensionCoinInfo（头内联 ctor：int+bool 清零）
    DimensionCoinInfo e1;
    (void)e1;

    // DimensionPartyCount（头内联 ctor：三字节赋值）
    DimensionPartyCount f1((char)1, (char)2, (char)3);
    (void)f1;

    // EffectAnimationInfo（头内联 C1/D1）
    EffectAnimationInfo g1;
    (void)g1;

    // EquipmentAuraAppendageData（头内联空析构）
    EquipmentParameterInfo::EquipmentAuraAppendageData h1;
    (void)h1;

    // ExpBonusPointData（头内联 ctor：调 clear）
    ExpBonusPointData i1;
    (void)i1;

    // GoldCardEventStatistic（头内联 ctor）
    GoldCardEventStatistic j1;
    (void)j1;

    // ItemAuraInfo（头内联 ctor）
    ItemAuraInfo k1;
    (void)k1;

    // ItemFarmingData（头内联 C1）
    ItemFarmingData l1;
    (void)l1;

    // LevelUpRewardItemData（头内联 ctor：调 clear）
    LevelUpRewardItemData m1;
    (void)m1;

    // ManagePointBonus（头内联 ctor：三 int 清零）
    ManagePointBonus n1;
    (void)n1;

    // MonsterKillItem（头内联 ctor：调 clear）
    MonsterKillItem o1;
    (void)o1;

    // Packet_Broadcast_Msg（头内联 ctor：PacketHeader(0x2711, 0x10e)）
    Packet_Broadcast_Msg p1;
    (void)p1;

    // DynamicPool<WongWork::Avatar_Item>（模板 ctor，ORIG W 0x082fa3fc）
    DynamicPool<WongWork::Avatar_Item> q1(16, 16);
    (void)q1;

    // CerashopAddRestrict::InfoDaily（头内联 ctor：7 参）
    CerashopAddRestrict::InfoDaily r1(CerashopAddRestrict::RestrictType::RestrictType_Account,
                                      0u, 0u, 0L, 0L, 0, 0);
    (void)r1;

    // Arad 两个 Data::operator==（头内联，std::find 语义）
    Arad_OnePlusCardEvent_ItemList::Data s1((unsigned short)1, 2ul);
    Arad_OnePlusCardEvent_ItemList::Data s2((unsigned short)1, 3ul);
    bool sb = (s1 == s2);
    (void)sb;
    Arad_VillageAttackRewardItem::Data t1(10, 20ul, 30);
    Arad_VillageAttackRewardItem::Data t2(10, 20ul, 30);
    bool tb = (t1 == t2);
    (void)tb;
