void CBingoTest::getBingoRandomVaule()
{
    int v = 0;
    for (int i = 0; i < 100; ++i)
    {
        v = CBingo::getBingoRandomVaule(0xa);
    }
    for (int i = 0; i < 100; ++i)
    {
        v = CBingo::getBingoRandomVaule(0x14);
    }
    for (int i = 0; i < 100; ++i)
    {
        v = CBingo::getBingoRandomVaule(0x1e);
    }
    for (int i = 0; i < 100; ++i)
    {
        v = CBingo::getBingoRandomVaule(0x28);
    }
    for (int i = 0; i < 100; ++i)
    {
        v = CBingo::getBingoRandomVaule(0x32);
    }
}