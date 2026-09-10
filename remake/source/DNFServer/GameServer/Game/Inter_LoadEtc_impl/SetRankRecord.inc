bool Inter_LoadEtc::SetRankRecord(CUser* user, SIG_LOAD_ETC* sig)
{
    // ORIG 0x84c2a98：无 records 局部槽，sig->m_records[i] 每轮直算（P0/P183）。
    for (int i = 0; i < sig->m_recordCount; ++i)
    {
        sig->m_records[i].m_field0 = 1;
        user->update_record(sig->m_records[i]);     // ORIG T 0x08676876
    }
    return true;
