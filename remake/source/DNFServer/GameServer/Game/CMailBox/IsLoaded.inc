bool CMailBox::IsLoaded() { return m_bInit; }

// ORIG 0x08557950 (W)：写加载状态；置 true 时同时写 lastLoadIdx ← nowSec
