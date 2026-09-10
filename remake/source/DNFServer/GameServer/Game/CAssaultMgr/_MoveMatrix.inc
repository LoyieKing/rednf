void CStaticDisposition::_MoveMatrix(const Point2d& delta)   // ORIG 0x82e6498
{
    for (int j = 0; j <= 1; ++j)
        m_mats[j].MovePoint(delta);
    m_iconPos += delta;
