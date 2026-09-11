// [b28d-11b] N：push ebx + 帧差（N82/P210）@0x086c4814
// [b28d-solo-GameWorld] _ZNK9GameWorld18is_available_pointEiiii verdict=AE 112=112
bool GameWorld::is_available_point(int vill, int area, int x, int y) const
{
    // [b28d-solo-GameWorld] _ZNK9GameWorld18is_available_pointEiiii：ORIG
    // 0x086c484c/0x086c4882/0x086c48b2 三次独立 getArea（P183/P157 逐访问点重取），
    // 第 1 检查 empty() 直判（无 NULL 守卫，P171 照抄）、第 2 检查 NULL、
    // 第 3 检查 `pAreas = &getArea(area)->m_mapAreas` 具名 -0x10 + `!pAreas->empty()`
    // xor 物化；循环 = 0x086c48e8 `jmp 条件块` + 尾 `!=` 回跳（P283），
    // `MapArea& ma = *it` 具名引用 -0xc。
    // ORIG 0x086c481a `cmpl $0,0xc; js L_ret0`（vill<0 有符号）+ 0x086c4828
    // `jl L_next`（vill<MAX）双跳共享 L_ret0 ⇒ 短路 OR 共享出口形。
    if (vill < 0 || vill >= MAX_VILLAGE_NUM)
    {
        return false;
    }
    if (m_pVill[vill].getArea(area)->m_mapAreas.empty())
    {
        return false;
    }
    if (m_pVill[vill].getArea(area) == NULL)
    {
        return false;
    }
    std::vector<MapArea>* pAreas = &m_pVill[vill].getArea(area)->m_mapAreas;
    if (!pAreas->empty())
    {
        // ORIG 条件块 0x086c4946 每轮重调 end()（非缓存；临时槽 -0x14，it=-0x18）
        // ⇒ 不声明 e，条件处内联 `pAreas->end()`。
        std::vector<MapArea>::iterator it = pAreas->begin();
        goto COND;
    LOOP:
        {
            MapArea& ma = *it;
            if (ma.m_x <= x && x <= ma.m_x + ma.m_w &&
                ma.m_y <= y && y <= ma.m_y + ma.m_h)
            {
                return true;
            }
        }
        ++it;
    COND:
        if (it != pAreas->end())
        {
            goto LOOP;
        }
    }
    return false;
}