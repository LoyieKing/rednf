DBThread::DBThread() : m_bLogDB(false)
{
    // ORIG 0x083ff5f2：先置 +0x0c=false，再隐式构造 CQueryCounter@+0x10。
