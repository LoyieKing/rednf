// ===========================================================================
// dispatch — dispatch.md @0x08416aac
// GetOutBuffer 判空返 0；SaveInven 失败返 0；m_saveFlag==0/2 时
// _saveCharacInvenExpand 失败返 0；否则 1。
// ===========================================================================
bool DB_SaveInven::dispatch(int userIdx, int slot, Stream* stream)
{
    bool r;
    SIG_SAVE_INVENTORY* sig = stream->GetOutBuffer<SIG_SAVE_INVENTORY>();
    if (sig == 0)
        return false;
    r = SaveInven(sig);
    if (r != 1)
        return false;
    if (sig->m_saveFlag == 0 || sig->m_saveFlag == 2)
    {
        r = _saveCharacInvenExpand(sig);
        if (r != 1)
            return false;
    }
    return true;
}