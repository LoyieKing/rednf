ARAD::DISPATCHER::Arad_InternalDispatcher::Arad_InternalDispatcher()
{
    // ORIG 0x081984d4：map C1 后跟一次显式 map::clear。
    m_dispatchers.clear();
