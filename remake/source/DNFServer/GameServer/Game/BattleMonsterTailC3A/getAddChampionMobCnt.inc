int CBattle_Field::getAddChampionMobCnt()
{
    if (m_pMap == NULL)                              // +0x108
        return -1;

    // CMap+0x80：基础冠军怪数量。
    int championCnt = m_pMap->m_80;

    if (m_mapWidth == 0 && m_mapHeight == 0)         // +0x18c/+0x190
    {
        LogManager::logFormat(
            1, "battle_field.cpp",
            __PRETTY_FUNCTION__, 0x4ed,
            "[FPE] [CBattle_Field::ConsistMap] Dungeon[%d] Width[%d] Height[%d]",
            m_dungeonIndex, m_mapWidth, m_mapHeight);
    }

    // +0x70：SelectDungeon 自 CDungeon::m_6e0[diff] 复制的每图怪物基数；
    // 按地下城难度缩放（150%/250%/500%）。
    int baseCnt = m_mapMobCount;
    switch (get_dungeon_diff())
    {
    case 0:
        baseCnt = m_mapMobCount;
        break;
    case 1:
        baseCnt = m_mapMobCount * 150 / 100;
        break;
    case 2:
        baseCnt = m_mapMobCount * 250 / 100;
        break;
    case 3:
        baseCnt = m_mapMobCount * 500 / 100;
        break;
    case 4:
        baseCnt = m_mapMobCount;
        break;
    default:                                         // >4 保持切换前读数
        break;
    }

    // CMap+0x12c 非 1 时按密度掷点追加一只。
    // ORIG 0x08301xx 序：先测 m_12c（xor$1/test/je），后算 denser
    // （0x18c Width 先载 ecx、0x190 Height 后载 ebx、setg 物化）——嵌套 if 形。
    // N96 清绑：原 `register bool denser asm("al")` 已删。ORIG 的 denser 无槽
    // （cmp;setg %al;test %al,%al;je）⇒ 按 P226扩展/P316① 用语句表达式包住整个条件
    // （内不含具名变量）复现零槽短路物化。清绑前后 127=127、与 ORIG 逐条全等
    // （仅 .o 重定位地址差）⇒ 残差 0，无需 N96+N18a 封顶；具名 bool 局部形为 128（+1 槽物化）。
    if (m_pMap->m_12c == 0)
    {
        if (({ baseCnt * 100 / (m_mapWidth * m_mapHeight) > get_rand_int(100); }))
            championCnt += 1;
    }
    return championCnt;
