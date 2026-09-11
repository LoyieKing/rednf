void CSecretShopStatistic::RecordPrice(int npcIdx, int dungeonIdx, int price)
{
    SECRET_SHOP_STATISTIC_DATA* data =
        (SECRET_SHOP_STATISTIC_DATA*)GetDungeonData(npcIdx, dungeonIdx);
    data->m_data[4] += price;
}