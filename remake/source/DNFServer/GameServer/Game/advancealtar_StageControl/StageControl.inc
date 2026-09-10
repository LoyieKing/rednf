StageControl::StageControl()
{
    m_index = -1;
    CharacData* characData = reinterpret_cast<CharacData*>(
        reinterpret_cast<char*>(this) + 0x98);
    new (reinterpret_cast<void*>(m_procStage)) ProcStage(*characData);
    new (characData) CharacData;
