bool CeraShopPurcahseCountBonus::_findBonusItem(
    const std::vector<PurcahseBonus>& list, PurcahseBonus& out,
    unsigned short threshold)
{
    std::vector<PurcahseBonus>::const_iterator it = list.begin();
    std::vector<PurcahseBonus>::const_iterator best = list.end();
    for (; it != list.end(); ++it)
    {
        if ((unsigned short)it->m_flag <= threshold)
            best = it;
    }
    if (best == list.end())
        return false;

    out = *best;
    return true;
