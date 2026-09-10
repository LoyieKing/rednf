void emitStStatusLevelInfoWeakSymbols()
{
    // C1（构造）+ D1/D2（delete 完整对象析构）。
    stStatusLevelInfo* info = new stStatusLevelInfo();
    delete info;
