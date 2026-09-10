void forceCtorEmissions()
{
    // ---- 头内联值类 ctor（W）----
    WpBonusPointData wp;              (void)wp;
    _str_cost sc(0, 0);               (void)sc;
    _str_option_value_ratio ovr(0.0f, 0.0f, 0); (void)ovr;
    pieceQuestRewardStatus pqr;       (void)pqr;
    rewardSubKey rsk;                 (void)rsk;
    rewardKey rk;                     (void)rk;
    seriaRoomDecoRewardInfo srd;      (void)srd;
    stTempRewardInfo str;             (void)str;
    stAdjust sa;                      (void)sa;
    stLimitCubeResult slc;            (void)slc;
    stStackableBoosterElement_t sbe;  (void)sbe;
    stPvPGradeExp_t spe(0, 0);        (void)spe;
    stSetItemBackImageInfo ssb;       (void)ssb;
    upgrade_info_t ui;                (void)ui;
    stMonsterInfo_t smi;              (void)smi;
    stQuestShoptInitCost sqic;        (void)sqic;
    WriteLogSwitch wls;               (void)wls;
    stNPCRelationship_t snr;          (void)snr;
    tagCS_UPDATE_CHANNEL_INFO tcu;    (void)tcu;
    volatile int tsz = tcu.getSize(); (void)tsz;   // force _ZN15tagPacketHeader7getSizeEv
    stQuestTargetCharacter_t sqtc((ENUM_CHARACTERJOB)0, 0, 0); (void)sqtc;

    // ---- 析构（D1/D2）：new/delete 触发完整对象析构 ----
// b28d-solo-OnlyOrigBatch7_Force verdict=DIFF（N89 封顶）ORIG 0x08ad1320（W，
// 0x8d）orig/ours=50/27。ORIG 是 4.1 头内联的 std::string 析构：成员 +4 与
// +0 各自 `_M_p-0xc` sentinel 比较 + `lock xadd` 引用计数递减 + gthr 活跃检查
// + 引用计数归零时 call _ZNSs4_Rep10_M_destroyERKSaIcE。4.4 头集的
// basic_string::~basic_string 为 out-of-line（call _ZNSsD1Ev ×2），且对第二个
// 成员析构布 EH landing pad（_Unwind_Resume）。libstdc++ 头实现差异不可改
// （N89；`~stream_t() throw()` 探针实测不能消 landing pad）⇒ 语义等价封顶
// （两 string 成员按声明逆序析构，与 ORIG 相同）。
// [b29-R28 复核] p167：MISSING `_ZNSs4_Rep10_M_destroyERKSaIcE`×2（4.1 头内联
// `_M_dispose` 后只剩 out-of-line `_M_destroy`）vs EXTRA `_ZNSsD1Ev`×3 +
// `_Unwind_Resume`×1（4.4 头走 out-of-line `~basic_string` 且布 landing）
// = **libstdc++ 头实现代际差异，头文件不可改** ⇒ N89 封顶。
    // [b29-SymForceBatch] ORIG 仅发 stream_t D1/D2（W 0x08ad1320），无 C1/C2
    // （默认 ctor 从未 ODR-use）⇒ new/delete 改显式析构调用：只触发 dtor
    // 发射，不再发明 ctor 符号。force 函数不被调用，无运行时行为。
    CompiledDNFLex::stream_t* pst = 0;
    pst->~stream_t();
    stQuestTargetCharacter_t* psqtc =
        new stQuestTargetCharacter_t((ENUM_CHARACTERJOB)1, 0, 0);
    delete psqtc;

    // stPeriodRewardInfo（C1 定义 FavorableRelationDeps.cpp / D1 同 TU）
    stFavorableRelationShip_t::stPeriodRewardInfo spr;
    (void)spr;

    // stEVENTADDINFO D1/D2（ORIG W 0x08118b56 空体；new/delete 触发发射）
    stEVENTADDINFO* psea = new stEVENTADDINFO();
    delete psea;

    // ---- stBestRecord_t operator==/operator>（非 const 成员）----
    WongWork::CDeathTowerRanking::stBestRecord_t ba;
    WongWork::CDeathTowerRanking::stBestRecord_t bb;
    bool gt = ba > bb;
    bool eq = ba == bb;
    (void)gt; (void)eq;

    // ---- Zone::operator<（const 成员）----
    village_object::Zone za((unsigned char)0, (unsigned char)0);
    village_object::Zone zb((unsigned char)1, (unsigned char)1);
    bool zlt = za < zb;
    (void)zlt;
    village_attacked::Zone va((unsigned char)0, (unsigned char)0);
    village_attacked::Zone vb((unsigned char)1, (unsigned char)1);
    bool vlt = va < vb;
    (void)vlt;

    // ---- std::__atomic0::atomic_flag 成员（volatile）----
    std::__atomic0::atomic_flag af;
    bool tas = af.test_and_set(std::memory_order_seq_cst);
    af.clear(std::memory_order_seq_cst);
    (void)tas;
