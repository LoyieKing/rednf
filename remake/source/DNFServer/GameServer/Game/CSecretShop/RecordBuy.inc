void CSecretShopStatistic::RecordBuy(int npcIdx, int dungeonIdx)
{
    SECRET_SHOP_STATISTIC_DATA* data =
        (SECRET_SHOP_STATISTIC_DATA*)GetDungeonData(npcIdx, dungeonIdx);
    data->m_data[3] += 1;
}