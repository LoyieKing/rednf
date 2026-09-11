// ORIG 0x08891c18。P0/P335 复核（2026-09-09）：ORIG 入口 0834e1c20..0834e1c35 死存
// 4 个常量局部（-0x1c=0x78、-0x18=0x6a、-0x14=0x8c、-0x10=0x14），全函数从未读取
// ⇒ 原码有 4 个具名局部（赋值后未用）；且各默认值分支为 `movl $imm,disp(%eax)`
// 直接落字段、then 分支亦直接落字段（0834e070/0834e08b），非三元物化
// ⇒ 源码是 if/else 语句而非 `?:`（原重建用 ?: 使 8 处字段赋值退化为 eax 物化 + join store）。
void makePathArea(MapArea* areas, const int area1[][2], const int area2[][2])
{
    int defW01 = 0x78;      // ORIG -0x1c（死存；未读取）
    int defH01 = 0x6a;      // ORIG -0x18（死存）
    int defW23 = 0x8c;      // ORIG -0x14（死存）
    int defH23Half = 0x14;  // ORIG -0x10（死存；= 0x28/2）
    (void)defW01;
    (void)defH01;
    (void)defW23;
    (void)defH23Half;
    for (int i = 0; i < 4; ++i)
    {
        if (i == 0 || i == 1)
        {
            if (area2[i][0] > 0)
                areas[i].m_w = area2[i][0];
            else
                areas[i].m_w = 0x78;
            if (area2[i][1] > 0)
                areas[i].m_h = area2[i][1];
            else
                areas[i].m_h = 0x6a;
            if (i == 0)
                areas[i].m_x = area1[i][0];
            else
                areas[i].m_x = area1[i][0] - areas[i].m_w;
            areas[i].m_y = area1[i][1] - areas[i].m_h / 2;
        }
        else
        {
            if (area2[i][0] > 0)
                areas[i].m_w = area2[i][0];
            else
                areas[i].m_w = 0x8c;
            if (area2[i][1] > 0)
                areas[i].m_h = area2[i][1];
            else
                areas[i].m_h = 0x28;
            areas[i].m_x = area1[i][0] - areas[i].m_w / 2;
            areas[i].m_y = area1[i][1] - areas[i].m_h / 2;
        }
    }
}