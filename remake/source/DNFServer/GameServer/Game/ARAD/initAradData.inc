// b28d-solo-ARAD verdict=IDENTICAL/AE：P118/P139 第一参用 __FUNCTION__（ORIG 局部静态符号
// _ZZN4ARAD16Arad_DataManager12initAradDataEvE12__FUNCTION__）；P221 全部
// cMyTrace 改「构造即调用」全表达式临时链复现 ORIG 递减槽序（-0x100…-0x30）；
// P197③ ep/openResult 用「声明=0 + 独立赋值」两句式（ORIG 0x08188304 -0x20、
// 0x08188691 -0x1c 各一条 movl $0）；单基类指针变量 script 复用 -0x20 槽（P211 档③
// 派生→基类赋值中转）；P225 5 处 open 检查改 `while(openResult != 0){…return}` 复现
// `cmpl $0,槽; setne; test; je`；trace2(0x59) 标志位按 ORIG 为 (true,false)。
// 345=345 指令全等，clean 归一化 0 残差。
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
        // [R11fixE5] ORIG 0x0818873c 串=0x8b91114，raw 逐字节带 '\t- ' 前缀。
        "\t- AradSpecialUseItems script load fail. (line:%d)", openResult);
    return false; }
    openResult = m_purchaseLimitItem.open("Nexon/AradSpecialUseItems.txt");
    while (openResult != 0) { cMyTrace(__FUNCTION__, 0x9d, 9, true, true)(
        "\t- AradSpecialUseItems script load fail. (line:%d)", openResult);
    return false; }
    openResult = m_bonusItem.open("Nexon/AradSpecialUseItems.txt");
    while (openResult != 0) { cMyTrace(__FUNCTION__, 0xa3, 9, true, true)(
        "\t- AradSpecialUseItems script load fail. (line:%d)", openResult);
    return false; }
    openResult = m_mileageData.open("Nexon/CeraShopMileage.txt");
    while (openResult != 0) { cMyTrace(__FUNCTION__, 0xb2, 9, true, true)(
        // [R11fixE5] ORIG 0x081888ad 串=0x8b91164，raw 逐字节带 '\t- ' 前缀。
        "\t- CeraShopMileage script load fail. (line:%d)", openResult);
    return false; }
    openResult = m_villageAttackReward.open("Nexon/VillageAttackRewardItem.txt");
    while (openResult != 0) { cMyTrace(__FUNCTION__, 0xbb, 9, true, true)(
        // [R11fixE5] ORIG 0x08188927 串=0x8b911b8，raw 逐字节带 '\t- ' 前缀。
        "\t- VillageAttackRewardItem script load fail. (line:%d)", openResult);
    return false; }
    return true;
}