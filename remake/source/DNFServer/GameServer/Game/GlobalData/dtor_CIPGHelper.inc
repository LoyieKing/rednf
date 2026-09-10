WongWork::IPG::CIPGHelper::~CIPGHelper()
{
    // ORIG 0x082a71b6：仅设 vptr，不释放 m_mgr（非拥有指针）。
