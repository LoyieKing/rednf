void forceEmitBatch12()
{
        char buf1024[1024];
        char buf256[256];
        char buf32[32];
        char buf512[512];
        char buf30[30];
        ss_sprintf(buf1024, "%d", 1);
        ss_sprintf(buf256, "%d", 1);
        ss_sprintf(buf32, "%d", 1);
        ss_sprintf(buf512, "%d", 1);
        ss_strcpy(buf30, "x");

        IntervalSet* a = 0;
        IntervalSet* b = 0;
        swap_value(a, b);

        stCeraShopBonusInfo_t ceraA;
        stCeraShopBonusInfo_t ceraB;
        ceraA = ceraB;

        WongWork::CGMAccounts::stGMInfo_t gm1; gm1.mid = 1; gm1.type = 2;
        WongWork::CGMAccounts::stGMInfo_t gm2; gm2.mid = 1; gm2.type = 2;
        volatile bool eq = (gm1 == gm2);

        stQuestTargetCharacter_t qt1(ENUM_CHARACTERJOB_0, 0, 0);
        stQuestTargetCharacter_t qt2(ENUM_CHARACTERJOB_0, 0, 0);
        volatile bool eq2 = (qt1 == qt2);

        stSeriaBlessingScript seria;
        stSeriaBlessingScript::Value v;
        volatile bool ok1 = seria.get_value(1, v);
        volatile bool ok2 = seria.is_possible_lv(1);

        stLimitItemUsageInfo lim;
        lim.Add(1, 2, 3);

        stNPCEventItemInfo npi;
        stNPCEventItemInfo npi2;
        npi = npi2;

        rewardSubKey rk1;
        rewardSubKey rk2;
        volatile bool lt = (rk1 < rk2);

        // [b29-SymForceBatch] tagRecommendNewbieOptionTable tnt 删除：ORIG 无
        // 该派生类的 C1/C2/D1/D2（仅 importRecommendNewbieOptionScript 以引用
        // 收参，从不构造/析构）；栈对象会发明四个 mangled。Table::clear（ORIG
        // W 0x0891ad8c）已由 importRecommendNewbieOptionScript 体内 out.clear()
        // （ORIG 0x89045b2 实证）自然 ODR-use，无需此处触发。

        (void)eq; (void)eq2; (void)ok1; (void)ok2; (void)lt;
