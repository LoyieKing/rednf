int PvP_Room::SelectRandomMap()
{
    std::vector<int> local_1c;
    GetCandidateMapIndexList(local_1c);
    int local_10 = DrawRandomMapIndex(local_1c);
    ChangePvPMode(local_10);
    return local_10;
}