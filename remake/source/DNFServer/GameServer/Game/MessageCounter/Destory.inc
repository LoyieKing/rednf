void CTimeInterval::Destory(CTimeInterval* p)
{
    // ORIG @0x0856f9b0：operator delete(param)，无析构链。
    ::operator delete(p);
