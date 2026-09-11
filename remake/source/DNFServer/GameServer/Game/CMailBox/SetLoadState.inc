// ORIG 0x08557950 (W)：写加载状态；置 true 时同时写 lastLoadIdx ← nowSec
void CMailBox::SetLoadState(bool loaded, long nowSec)
{
    m_bInit = loaded;                              // ORIG mov %dl,(%eax) 直写
    if (loaded)
        m_lastLoadIdx = (unsigned int)nowSec;      // ORIG mov %edx,0x4(%eax)
}