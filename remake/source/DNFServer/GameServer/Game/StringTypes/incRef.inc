void StringData::incRef()
{
    // ORIG: lock addl $0x1,(%eax)
    __sync_fetch_and_add(&m_refCount, 1);
}