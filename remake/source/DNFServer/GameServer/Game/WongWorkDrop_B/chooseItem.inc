// ---- CSecialIMonsterDropItemGenRateTable::chooseItem — ORIG T 0x08534fce ---
// type==2（random_option 槽）先试 m_optHandle.choose_item(rarity,level)，命中
// 即返；否则按 diff/ddt 选块（ORIG 槽址 = type*0x7110+rarity*0x12d8+level*24，
// type 调用方传 1..4 ⇒ m_slots[type-1]，m_slots 自块 +0x7110 起）：
// empty→-1；roll=m_rand(+0x211fb0)->randInt(maxKey)；
// lower_bound==end 回退 --it 返 second。
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N82/N93 + N-88 setne 族）→ 试=①守卫包裹形 `if(x==-1){}else{return x;}`（C1）读数不变；②bool 具名局部（C2）our 210→213 劣化回滚；③iterator 直接初始化（C3）our 210→204 且 p167 EXTRA `_Rb_tree_iteratorC1Ev` 消除，但 |gap| −2→+4、ndiff 81→80 ⇒ 回滚 ⇒ 维持（ORIG 0x08534fce+0x18 `cmpl;setne;test;je` 物化不可造；槽平移 0x14 独立存在）。
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
        // [M3S2WinDrop] ORIG 0x08535105 拷贝槽对（-0x24 → -0x40）：`mov (%eax),%eax;
        // mov %eax,-0x24; mov -0x24,%eax; mov %eax,-0x40`，第二个 ulong 局部取地址
        // 传给 randInt（引用参数）。
        unsigned long maxKeyRef = maxKey;
        // [M3S2WinDrop] ORIG 0x08535179 `lea -0x4c,%ecx; mov %ecx,0x8(%esp)`（直接传
        // roll 槽地址给 lower_bound）⇒ roll 类型 = map key_type（unsigned int），
        // 用 unsigned long 会多出一次截断临时（`mov S,%eax; mov %eax,S`）。
        unsigned int roll = m_rand->randInt(maxKeyRef);
        // ORIG 在 randInt 之后才 `movl $0xffffffff,-0x20`（result 槽）。
        result = -1;
        // [M3S2WinDrop] ORIG 0x0853518c 后置拷贝：lower_bound 返回先落临时槽(-0x5c)，
        // 再 `mov -0x5c,%eax; mov %eax,-0x48` ⇒ 先声明后赋值（非初始化式）。
        std::map<unsigned int, int>::iterator lb;
        lb = m_blocks[dungeonDiff][dropDungeonType][type][rarity][level].lower_bound(roll);
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
}