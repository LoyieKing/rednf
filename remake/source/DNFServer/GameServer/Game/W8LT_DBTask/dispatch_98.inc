bool DB_SaveGamechannel::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_SAVE_LOGGAMECHANNEL* sig =
        stream->GetOutBuffer<SIG_SAVE_LOGGAMECHANNEL>();
    GlobalData::s_pLogGameChannel->DoSaveLog(sig);
    return 1;
