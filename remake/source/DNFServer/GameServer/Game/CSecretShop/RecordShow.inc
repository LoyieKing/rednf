void CSecretShopStatistic::RecordShow(int npcIdx, int dungeonIdx, int price)
{
    SECRET_SHOP_STATISTIC_DATA* data =
        (SECRET_SHOP_STATISTIC_DATA*)GetDungeonData(npcIdx, dungeonIdx);
    data->m_data[1] += 1;
    data->m_data[2] += price;
}