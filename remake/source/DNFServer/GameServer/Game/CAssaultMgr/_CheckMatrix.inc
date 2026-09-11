bool CStaticDisposition::_CheckMatrix(std::vector<MapArea>* areas,
                                      Point2d& out)          // ORIG 0x82e64f2
{
    for (int j = 0; j <= 1; ++j)
    {
        if (!m_mats[j].CheckPoint(areas, out))
            return false;
    }
    return IsIncludeVirtualMoveAreas(areas, m_iconPos.m_x, m_iconPos.m_y,
                                     out.m_x, out.m_y, 0);
}