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
