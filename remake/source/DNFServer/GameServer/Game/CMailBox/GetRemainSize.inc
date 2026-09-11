// ORIG 0x08557922 (W)：StaticPool<CMail,20>::Remain @+0x10
int CMailBox::GetRemainSize()
{
    return m_mailPool.Remain();
}