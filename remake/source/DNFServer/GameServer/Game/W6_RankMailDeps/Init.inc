// ORIG T 0x085519e2 (size 0x1c)：m_mailIndex=0 后调 ResetPackage()。
void WongWork::CMailBox::CMail::Init()
{
    m_mailIndex = 0;
    ResetPackage();
}