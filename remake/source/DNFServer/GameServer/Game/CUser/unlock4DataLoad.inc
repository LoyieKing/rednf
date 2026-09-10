bool CUser::unlock4DataLoad()
{
    // ORIG 0x864a9dd：xor $0x1,%eax; test; je WRITE（!bool 取反形）; WRITE: movb $0; mov $1
    if (!isLocked4DataLoad())
        return 0;
    m_field8d0f0 = 0;
    return 1;
