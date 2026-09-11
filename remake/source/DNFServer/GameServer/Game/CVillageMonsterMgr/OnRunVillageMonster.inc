// ============================================================================
// CVillageMonster::OnRunVillageMonster —— ORIG T 0x86b310c
//   m_field10 清零；m_state!=0 或 idx%3!=m_field0c → 不动；否则随机 ±150 像素
//   试探新位置，用 IsIncludeVirtualMoveAreas 裁剪到虚拟移动区内；若距旧位置
//   的曼哈顿距离 > 0x31 保留新位置并置 m_field10=1，否则回退旧位置。
// ============================================================================
// [b28d-solo-CVillageMonsterMgr] AE（ORIG 0x086b310c）：去 newX/newY 局部，
// 改 `m_field04 = m_field04 + offset;` 两句复用同一 offset 槽（复现 ORIG
// call→sub→存槽→读 LHS 序）；IsInclude 实参直传字段；if/else 反写
// （`<=0x31` 为 then）复现 ORIG `jg` 块序（P47/P222）。p167 CLEAR。
void CVillageMonster::OnRunVillageMonster(std::vector<MapArea>* areas, int idx)
{
    m_field10 = 0;
    if (m_state != 0)
        return;
    if (idx % 3 != m_field0c)
        return;
    int oldX = m_field04;
    int oldY = m_field08;
    int offset = get_rand_int(MONSTER_MOVE_PIXEL) - 0x96;
    m_field04 = m_field04 + offset;
    offset = get_rand_int(MONSTER_MOVE_PIXEL) - 0x96;
    m_field08 = m_field08 + offset;
    pvp_assault::IsIncludeVirtualMoveAreas(areas, m_field04, m_field08,
                                           m_field04, m_field08, 0x14);
    if (abs(m_field04 - oldX) + abs(m_field08 - oldY) <= 0x31)
    {
        m_field04 = oldX;
        m_field08 = oldY;
    }
    else
        m_field10 = 1;
}