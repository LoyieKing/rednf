void CEventBase::SetEventFlag(bool flag)
{
    // ORIG 0x080c84e2：写 +0x08 的 flag 字节。
    m_isEventing = flag;
}