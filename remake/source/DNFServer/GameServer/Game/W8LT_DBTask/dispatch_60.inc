bool DB_SaveBloodDungeon::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_BLOOD_DUNGEON* sig = stream->GetOutBuffer<SIG_BLOOD_DUNGEON>();
    SaveBloodDungeonReward(sig);
    return 1;
