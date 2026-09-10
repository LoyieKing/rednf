unsigned int CDeathTowerRanking::makeDungeonIdx2TowerIdx(unsigned int dungeonIdx)
{
    // [b28d-solo-CDeathTower] verdict=AE（12/12，仅尾 nop 填充差异，§6.1 豁免）：
    // ORIG 08469d69 W 两段无符号边界 `cmpl $0x2af7; jbe NEG` / `cmpl $0x2afd;
    // jbe POS` 与 4.4 逐条同形；nop 填充已由 compare_tu 剥离。
    // ORIG 08469d69 W：dungeonIdx∈[0x2af8,0x2afd] → idx-0x2af8（塔序号 0..5），其余 -1。
    // ORIG 0x8469d6c..0x8469d88：cmp1 jbe→NEG@8469d7e；cmp2 jbe→POS@8469d85；
    // NEG 为 cmp2 fall-through，POS 殿后 —— && 条件 goto POS，-1 顺落（范式24 落点岛）。
    if (dungeonIdx > 0x2af7 && dungeonIdx <= 0x2afd)
        goto POS;
    return 0xffffffff;
POS:
    return dungeonIdx - 0x2af8;
