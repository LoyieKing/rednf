// ORIG resetData 0x084b363e：清 bingo 数据、置脏并发送全量数据。
void CBingoGmTest::resetData()
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }
    static_cast<BingoData*>(user->getBingoData())->clear();
    static_cast<BingoData*>(user->getBingoData())->m_needSave = true;
    user->sendBingoData();
}