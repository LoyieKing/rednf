// ORIG execute 0x084b35fc：user 有效时按子命令分流。
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
}