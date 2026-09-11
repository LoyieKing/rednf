// ============================================================================
// DB_UpdateGMData::dispatch —— ORIG 0x0843990a
// ============================================================================
// ORIG 0x843990a：Stream>>bool（消费首字节，值未用）→
// Gm_List::LoadGmListFromDb(this=GlobalData::gmList) → return 1。
// 声明见 W8LT_DBTask.h（Main 批准补 virtual bool dispatch）。
bool DB_UpdateGMData::dispatch(int userIdx, int slot, Stream* stream)
{
    bool flag = false;
    *stream >> flag;
    GlobalData::gmList->LoadGmListFromDb();
    return 1;
}