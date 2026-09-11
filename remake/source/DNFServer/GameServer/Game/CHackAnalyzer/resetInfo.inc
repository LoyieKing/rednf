void Secu_AccountHacking::resetInfo()
{
    // ORIG T 0x08278ab4（resetInfo.md）：memset(this,0,6)；+8/+0xc 清零。
    memset(&m_flag[0], 0, 6);
    m_sendCnt = 0;
    m_loginTime = 0;
}