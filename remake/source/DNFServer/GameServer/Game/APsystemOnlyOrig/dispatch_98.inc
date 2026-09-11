// ============================================================================
// DB_SaveGamechannel::dispatch —— ORIG 0x0841c428（报告 dispatch.md 全文）
// ============================================================================
// ORIG 0x841c428：GetOutBuffer<SIG_SAVE_LOGGAMECHANNEL>() →
// WongWork::CLogGameChannel::DoSaveLog(GlobalData::s_pLogGameChannel, sig)
// （CLogGameChannel::GetLogTargetDB 取 DB_HANDLE_4 落 log_game_channel 表）
// → return 1。
bool DB_SaveGamechannel::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_SAVE_LOGGAMECHANNEL* sig =
        stream->GetOutBuffer<SIG_SAVE_LOGGAMECHANNEL>();
    GlobalData::s_pLogGameChannel->DoSaveLog(sig);
    return 1;
}