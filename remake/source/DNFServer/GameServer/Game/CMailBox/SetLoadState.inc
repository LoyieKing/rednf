void CMailBox::SetLoadState(bool loaded, long nowSec)
{
    m_bInit = loaded;                              // ORIG mov %dl,(%eax) 直写
    if (loaded)
        m_lastLoadIdx = (unsigned int)nowSec;      // ORIG mov %edx,0x4(%eax)
