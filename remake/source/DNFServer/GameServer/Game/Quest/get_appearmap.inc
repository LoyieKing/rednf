// b28d-solo-Quest AE：ORIG 0x083536f2。`while (m_percent >= get_rand_int(100))
// { return m_appearMap; }` 形同时复现 4.1 的 `cmp %eax,%ebx; setge %al;
// test %al,%al; je` 条件物化与 m_percent 升格 %ebx 跨 call 保活（两处分支各一）
// ⇒ 47=47（b28d-20 N7/P197④「无源码形态」判词作废；P225/P226 边界）。
int Quest::get_appearmap(int mapA, int mapB) const
{
    // b28d-solo-Quest [P225/P226 边界]：while(cond){return X;} 形复现 ORIG
    // 0x08353729 的 `cmp %eax,%ebx; setge %al; test %al,%al; je`（%ebx 跨
    // get_rand_int 保活）。
    if (m_appearMap.m_mapB == -1)
    {
        if (m_appearMap.m_mapA == mapA)
        {
            while (m_appearMap.m_percent >= get_rand_int(100))
            {
                return m_appearMap.m_appearMap;
            }
        }
    }
    else if ((m_appearMap.m_mapB == mapA) &&
             (m_appearMap.m_mapB == mapB))
    {
        while (m_appearMap.m_percent >= get_rand_int(100))
        {
            return m_appearMap.m_appearMap;
        }
    }
    return 0;
}