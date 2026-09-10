int CSecialIMonsterDropItemGenRateTable::chooseItem(
    eItemDropType_t type, ENUM_RARITY rarity, int level,
    ENUM_DUNGEON_DIFFICULTY dungeonDiff,
    ENUM_ITEM_DROP_DUNGEON_TYPE dropDungeonType)
{
    // [b28d-solo-WongWorkDrop_B] 语义完成 DIFF（208/204；p167 CLEAR 18/18）。
    // 修法 = result 声明/赋值位置照 ORIG（randInt 后 `movl $-1,-0x20`、正常/catch
    // 两路共享尾返回）+ randomOptionIdx 两句式预清（P198①）+ __PRETTY_FUNCTION__（P139）。
    // 残差 N 封顶：①ORIG `cmpl $-1;setne;test;je` 物化 vs 4.4 直 je（N-88 族）；
    // ②迭代器/临时槽号整体平移 0x10（N82/N93）。
    // ORIG 0x08534fce：type==2 先试 random_option 槽；否则按 diff/ddt 选块；
    // 整段 try 包裹，catch(...) 走 cMyTrace（pretty/0x2e5/5）+ 日志后返回 -1。
    // result 声明在 try 外、赋值在 randInt 之后（ORIG `movl $0xffffffff,-0x20`
    // 位置实证），正常/catch 两路共享尾部 `mov -0x20,%eax` 返回。
    int result;
    try
    {
        if (type == eItemDropType_2)
        {
            // ORIG 先 `movl $0xffffffff,-0x1c` 再 call（P198① 两句式预清）。
            int randomOptionIdx = -1;
            randomOptionIdx =
                static_cast<int>(m_optHandle.choose_item(rarity, level));
            if (randomOptionIdx != -1)
                return randomOptionIdx;
        }
        // [b28d-38] P167：ORIG 调 map::size()（无 empty）；P28 增量侧：ORIG 用
        // 后置 it--（mmEi，带临时槽）；P50 复活：ORIG 有 iterator::C1Ev ⇒ 先声明后赋值。
        if (m_blocks[dungeonDiff][dropDungeonType][type][rarity][level].size() == 0)
            return -1;
        std::map<unsigned int, int>::iterator lastIter;
        lastIter = m_blocks[dungeonDiff][dropDungeonType][type][rarity][level].end();
        lastIter--;
        unsigned long maxKey = lastIter->first;
        unsigned long roll = static_cast<unsigned long>(
            m_rand->randInt(maxKey));
        // ORIG 在 randInt 之后才 `movl $0xffffffff,-0x20`（result 槽）。
        result = -1;
        std::map<unsigned int, int>::iterator lb =
            m_blocks[dungeonDiff][dropDungeonType][type][rarity][level].lower_bound(roll);
        if (lb == m_blocks[dungeonDiff][dropDungeonType][type][rarity][level].end())
            lb--;
        result = lb->second;
    }
    catch (...)
    {
        // P139：ORIG 传 __PRETTY_FUNCTION__ 局部静态（_ZZ...E19__PRETTY_FUNCTION__）。
        cMyTrace t(__PRETTY_FUNCTION__, 0x2e5, 5);
        t("[%s][%d]", __PRETTY_FUNCTION__, 0x2e5);
    }
    return result;
