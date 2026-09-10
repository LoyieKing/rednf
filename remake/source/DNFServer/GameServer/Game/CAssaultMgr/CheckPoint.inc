bool CExtraMatrix::CheckPoint(std::vector<MapArea>* areas,
                              Point2d& out)        // ORIG 0x82e5b4e
{
    // b28d-solo-CAssaultMgr: IDENTICAL_AE（清单 NEAR 2 → AE；路由口径复验）
    // b28d-L3: IDENTICAL_AE — b28d-L3：路由后 AE（ORIG 0x82e5b4e
    // b28d-solo-CAssaultMgr: P167 新判据（b28c C28L8）——ORIG 2×`deEv`(operator*)
    // 而旧写 `it->m_x/m_y` 产 2×`ptEv`(operator->)；改 `(*it).m_*` 归位。
    for (std::vector<Point2d>::iterator it = m_points.begin();
         it != m_points.end(); ++it)
    {
        if (IsIncludeVirtualMoveAreas(areas, (*it).m_x, (*it).m_y,
                                      out.m_x, out.m_y, 0))
        {
            return true;
        }
    }
    return false;
