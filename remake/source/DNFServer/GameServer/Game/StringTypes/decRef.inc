void StringData::decRef()
{
    // ORIG: lock xadd -1；新值 <=0 则 free(this)
    if (__sync_sub_and_fetch(&m_refCount, 1) <= 0)
        ::free(this);
