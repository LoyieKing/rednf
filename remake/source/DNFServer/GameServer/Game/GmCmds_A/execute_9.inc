void CBingoGmTest::execute()
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }
    if (m_subCmd == 0)
    {
        resetData();
    }
    else
    {
        reqValue();
    }
