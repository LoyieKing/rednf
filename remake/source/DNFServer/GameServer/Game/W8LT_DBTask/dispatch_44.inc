bool DB_SaveExpandEquipslot::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_SAVE_EXPAND_CHARAC* sig = stream->GetOutBuffer<SIG_SAVE_EXPAND_CHARAC>();
    if (!_SaveExpandEquipslot((SIG_SAVE_EXPAND_EQUIPSLOT*)sig))
        return 0;
    if (!_SaveCharacStat(((SIG_SAVE_EXPAND_EQUIPSLOT*)sig)->m_characNo,
                         &((DB_SaveExpandCharacView*)sig)->m_stat))
        return 0;
    return 1;
}