StageControl::~StageControl()
{
    reinterpret_cast<CharacData*>(reinterpret_cast<char*>(this) + 0x98)->~CharacData();
    reinterpret_cast<ProcStage*>(m_procStage)->~ProcStage();
