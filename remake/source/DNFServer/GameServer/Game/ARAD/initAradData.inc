bool Arad_DataManager::initAradData()
{
    // b28d-solo-ARAD：ORIG 0x08188304 单槽 -0x20 初始化一次，ep/st 两段复用同一
    // 基类指针变量（Ghidra local_24 被两派生类型共用；虚调用 vptr+8=槽2=loadScriptData）。
    // 派生*→基类* 的赋值另出 ORIG `mov %ebx,%eax; mov %eax,-0x20` 中转（P211 档③）。
    Arad_InterfaceGameScript* script = 0;
    // 各段起始/失败/成功均按 ORIG 记 cMyTrace（line/flag/bC/bD 逐点核对）：
    // 段①起(0x48,9,1,0)、Fail(0x4c/0x52,9,1,1)、Success(0x55,9,0,1)；
    // 段②起(0x59,9,1,0)、Fail(0x5d/0x63,9,1,1)、Success(0x66,9,0,1)；
    // 段③起(0x94,9,1,0)、open 失败(0x97/0x9d/0xa3/0xb2/0xbb,9,1,1) 带
    // "…script load fail. (line:%d)" 参数为 open 返回值。
    cMyTrace(__FUNCTION__, 0x48, 9, true, false)("\t- Loading Event Period Script - ");
    script = new Arad_EventPeriodDataManager();
    if (script == 0 || !script->loadScriptData())
    {
        cMyTrace(__FUNCTION__, 0x4c, 9, true, true)("Fail");
        return false;
    }
    if (!__insertGameScript(script))
    {
        cMyTrace(__FUNCTION__, 0x52, 9, true, true)("Fail");
        return false;
    }
    cMyTrace(__FUNCTION__, 0x55, 9, false, true)("Success");

    cMyTrace(__FUNCTION__, 0x59, 9, true, false)("\t- Loading Statistics Item List - ");
    script = new Arad_StatisticsEventItemList();
    if (script == 0 || !script->loadScriptData())
    {
        cMyTrace(__FUNCTION__, 0x5d, 9, true, true)("Fail");
        return false;
    }
    if (!__insertGameScript(script))
    {
        cMyTrace(__FUNCTION__, 0x63, 9, true, true)("Fail");
        return false;
    }
    cMyTrace(__FUNCTION__, 0x66, 9, false, true)("Success");

    int openResult = 0;
    cMyTrace(__FUNCTION__, 0x94, 9, true, false)("\t- Loading NexonFolder Script - ");
    openResult = m_exceptionItem.open("Nexon/AradSpecialUseItems.txt");
    while (openResult != 0) { cMyTrace(__FUNCTION__, 0x97, 9, true, true)(
        "AradSpecialUseItems script load fail. (line:%d)", openResult);
    return false; }
    openResult = m_purchaseLimitItem.open("Nexon/AradSpecialUseItems.txt");
    while (openResult != 0) { cMyTrace(__FUNCTION__, 0x9d, 9, true, true)(
        "AradSpecialUseItems script load fail. (line:%d)", openResult);
    return false; }
    openResult = m_bonusItem.open("Nexon/AradSpecialUseItems.txt");
    while (openResult != 0) { cMyTrace(__FUNCTION__, 0xa3, 9, true, true)(
        "AradSpecialUseItems script load fail. (line:%d)", openResult);
    return false; }
    openResult = m_mileageData.open("Nexon/CeraShopMileage.txt");
    while (openResult != 0) { cMyTrace(__FUNCTION__, 0xb2, 9, true, true)(
        "CeraShopMileage script load fail. (line:%d)", openResult);
    return false; }
    openResult = m_villageAttackReward.open("Nexon/VillageAttackRewardItem.txt");
    while (openResult != 0) { cMyTrace(__FUNCTION__, 0xbb, 9, true, true)(
        "VillageAttackRewardItem script load fail. (line:%d)", openResult);
    return false; }
    return true;
