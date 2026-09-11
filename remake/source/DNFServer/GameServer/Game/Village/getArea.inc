Area* Village::getArea(int area)
{
    if (area < 0 || area >= m_areaCount)
    {
        // [b28c-35] ORIG 0x086c3bba：m_iAreaCount 在 cMyTrace::C1 调用**之前**读入
        // %ebx 并跨 ctor 保活（push %ebx; sub $0x24），具名对象形 4.4 会改在 ctor 后
        // 重读（无 push ebx，帧 0x28）。全表达式临时链（P227/P87）复现 ORIG 形。
        cMyTrace(__PRETTY_FUNCTION__, 0x356, 5)(
            "area(%d) < 0 || area >= m_iAreaCount(%d)",
            area, m_areaCount);
        return 0;
    }
    return &m_pAreas[area];
}