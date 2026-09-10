InGameEventScript* InGameEventScriptLoader::importInGameEventScript(
    int recordType, const std::string& path)
{
    InGameEventScript* script = 0;
    bool loaded = true;

    switch (recordType)
    {
    case 0x194:  // 升级奖励（new 0x6c）
        script = new LevelUpRewardScript();
        loaded = importLevelUpRewardScript(
            static_cast<LevelUpRewardScript*>(script), path.c_str());
        break;
    case 0x65:   // 疲劳消耗赠物（new 100）
        script = new UsedFatigueGiveItemScript();
        loaded = importUsedFatigueGiveItemEventScript(
            static_cast<UsedFatigueGiveItemScript*>(script), path.c_str());
        break;
    case 0x56:   // 出勤签到（new 0x78）
        script = new AttendanceScript();
        loaded = importAttendanceScript(
            static_cast<AttendanceScript*>(script), path.c_str());
        break;
    case 0x5c:   // 网吧游戏时长（new 0x18）
        script = new PcBangPlayTimeScript();
        loaded = importPcBangPlayTimeScript(
            static_cast<PcBangPlayTimeScript*>(script), path.c_str());
        break;
    case 0x191:
    case 0x192:
    case 0x19c:  // 账号首次登录 / 每日变体共用 AccountFirstLoginScript（new 0x50）
        script = new AccountFirstLoginScript();
        loaded = importAccountFirstLoginEventScript(
            static_cast<AccountFirstLoginScript*>(script), path.c_str());
        break;
    case 0x193:  // 按条目计数 DB（new 0x24）
        script = new AddItemCountDBScript();
        loaded = importAddItemCountDBEventScript(
            static_cast<AddItemCountDBScript*>(script), path.c_str());
        break;
    case 0x196:  // 交换随机奖励（new 0x68）
        script = new ExchangeRandomItemRewardScript();
        loaded = importExchangeRandomItemRewardScript(
            static_cast<ExchangeRandomItemRewardScript*>(script), path.c_str());
        break;
    case 0x197:  // 头像分解随机奖励（new 100）
        script = new AvatarDisjointRandomRewardScript();
        loaded = importAvatarDisjointRandomRewardScript(
            static_cast<AvatarDisjointRandomRewardScript*>(script), path.c_str());
        break;
    case 0x199:  // 疲劳量（new 0x10）
        script = new FatigueQuantityScript();
        loaded = importFatigueQuantityEventScript(
            static_cast<FatigueQuantityScript*>(script), path.c_str());
        break;
    case 0x19a:  // 购买现金道具奖励（new 0x30）
        script = new PurchaseCashItemBonusScript();
        loaded = importPurchaseCashItemBonusEventScript(
            static_cast<PurchaseCashItemBonusScript*>(script), path.c_str());
        break;
    case 0x19b:  // 养成本体（new 0x34）
        script = new ObjectBringUpScript();
        loaded = importObjectBringUpEventScript(
            static_cast<ObjectBringUpScript*>(script), path.c_str());
        break;
    default:     // 未建模类型：ORIG 保持 ptr=0、ok=1，直接返回 0
        return 0;
    }

    if (!loaded)
    {
        // ORIG 失败路径：显式基类析构（D1 非虚直调）后 operator delete，两次判空。
        if (script != 0)
        {
            script->~InGameEventScript();
            ::operator delete(script);
        }
        script = 0;
    }
    return script;
