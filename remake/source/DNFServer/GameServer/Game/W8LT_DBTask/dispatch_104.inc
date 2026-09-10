bool DB_UpdateGMData::dispatch(int userIdx, int slot, Stream* stream)
{
    bool flag = false;
    *stream >> flag;
    GlobalData::gmList->LoadGmListFromDb();
    return 1;
