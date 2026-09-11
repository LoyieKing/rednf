// [b28d-L1] N82 封顶：ORIG 0x088d589c；443 vs 449 条、帧 0x60 vs 0x70。P167 残差 1 处（ScanInt(Pi)/ScanInt(Pb) 计数，疑区间假象；.o 内两类重定位均存在）。残余为槽位/帧形态差（N82/P232），语义等价。
bool BlueMarbleInfoScript::importScript(const char* dir, const char* path)
{
    if (!loadRDARScriptFile(dir, path))
    {
        return 0;
    }
    // 局部声明顺序与 ORIG 栈布局一致（d4/-0x20、d3/-0x2c、ri/-0x34、
    // rw/-0x3c、tp/-0x44、d2/-0x4c、d1/-0x58、line/-0x10、flag/-0x14）
    BlueMarbleRandomDungeonInfo d4;
    BlueMarbleRandomDungeonInfo d3;
    BlueMarbleRandomItemInfo ri;
    BlueMarbleRewardInfo rw;
    BlueMarbleTilePos tp;
    BlueMarbleDungeonDifficulty d2;
    BlueMarbleRandomDungeonInfo d1;
    std::string line;
    int loopFlag = 0;
    (void)loopFlag;

    while (ScanType(line, true))
    {
        if (line == "[dungeon list]")
        {
            m_randomDungeonList.clear();
            while (ScanInt(&d4.m_minLevel) && ScanInt(&d4.m_maxLevel) &&
                   ScanInt(&d4.m_dungeonIndex))
            {
                m_randomDungeonList.push_back(d4);
            }
        }
        else if (line == "[boss dungeon list]")
        {
            m_bossDungeonList.clear();
            while (ScanInt(&d3.m_minLevel) && ScanInt(&d3.m_maxLevel) &&
                   ScanInt(&d3.m_dungeonIndex))
            {
                m_bossDungeonList.push_back(d3);
            }
        }
        else if (line == "[item list]")
        {
            m_randomItemList.clear();
            while (ScanInt(&ri.m_itemIndex) && ScanInt(&ri.m_weight))
            {
                m_itemWeightTotal += ri.m_weight;
                m_randomItemList.push_back(ri);
            }
        }
        else if (line == "[player max]")
        {
            // [P6AuditOldN.P6S2 2026-09-11 · 类型轴⑦] p167fix 同点对：ORIG
            //   MISSING `_Z7ScanIntPi`×6 / 我方 EXTRA `_Z7ScanIntPb`×6。按 ORIG
            //   调用序逐点对齐（ORIG 的 `_Z7ScanIntPb` 全函数仅 3 处 =
            //   m_dungeonDifficulty/m_superArmorTime/m_equipmentUpgradeMax，
            //   其余 6 处含本文 5 连发 + [buff max value] 均为 `_Z7ScanIntPi`）。
            //   ORIG 0x088d5a9a 形：`mov 0x8(%ebp),%eax; mov %eax,(%esp);
            //   call ScanIntPi; jmp` = 传 `&m_playerMax`（offset 0 ⇒ 直接 this）
            //   并**丢弃返回值** ⇒ 原码是传址调用而非赋值形（我方旧形传 null +
            //   存返回值，多 movzbl 且 miss Pi）。
            ScanInt(&m_playerMax);
        }
        else if (line == "[enter level]")
        {
            ScanInt(&m_enterLevel);
        }
        else if (line == "[enter max level]")
        {
            ScanInt(&m_enterMaxLevel);
        }
        else if (line == "[enter count]")
        {
            ScanInt(&m_enterCount);
        }
        else if (line == "[die count]")
        {
            ScanInt(&m_dieCount);
        }
        else if (line == "[dice value]")
        {
            ScanInt(&m_diceMin);
            ScanInt(&m_diceMax);
        }
        else if (line == "[reward item]")
        {
            m_rewardList.clear();
            while (ScanInt(&rw.m_a) && ScanInt(&rw.m_b))
            {
                m_rewardList.push_back(rw);
            }
        }
        else if (line == "[tile pos]")
        {
            m_tilePosList.clear();
            while (ScanInt(&tp.m_a) && ScanInt(&tp.m_b))
            {
                m_tilePosList.push_back(tp);
            }
        }
        else if (line == "[dungeon difficulty]")
        {
            m_dungeonDifficulty = ScanInt((bool*)0);
        }
        else if (line == "[dungeon difficulty list]")
        {
            m_dungeonDifficultyList.clear();
            while (ScanInt(&d2.m_dungeon) && ScanInt(&d2.m_difficulty))
            {
                m_dungeonDifficultyList.push_back(d2);
            }
        }
        else if (line == "[super armor time]")
        {
            m_superArmorTime = ScanInt((bool*)0);
        }
        else if (line == "[unique dungeon list]")
        {
            m_uniqueDungeonList.clear();
            while (ScanInt(&d1.m_minLevel) && ScanInt(&d1.m_maxLevel) &&
                   ScanInt(&d1.m_dungeonIndex))
            {
                m_uniqueDungeonList.push_back(d1);
            }
        }
        else if (line == "[buff max value]")
        {
            ScanInt(&m_buffMaxValue);
        }
        else if (line == "[gold base]")
        {
            m_goldBase = ScanFloat((float*)0);
        }
        else if (line == "[gold multi]")
        {
            m_goldMulti = ScanFloat((float*)0);
        }
        else if (line == "[equipment upgrade max]")
        {
            m_equipmentUpgradeMax = ScanInt((bool*)0);
        }
    }
    return 1;
}