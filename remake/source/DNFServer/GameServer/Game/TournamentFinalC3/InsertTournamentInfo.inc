void CBattle_Field::InsertTournamentInfo(std::vector<int>& indexes,
    std::vector<TournamentInfo>& infos,
    std::map<short, TournamentInfo>& roundMap)
{
    std::vector<TournamentInfo>::iterator infoIt = infos.begin();
    std::vector<int>::iterator idxIt = indexes.begin();

    while (infoIt != infos.end() && idxIt != indexes.end())
    {
        infoIt->m_roundOrder = (short)*idxIt;
        roundMap.insert(std::make_pair(infoIt->m_roundOrder, *infoIt));

        infoIt = infos.erase(infoIt);
        idxIt = indexes.erase(idxIt);
    }
