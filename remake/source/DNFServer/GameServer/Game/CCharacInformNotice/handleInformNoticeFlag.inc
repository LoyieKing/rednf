void CCharacInformNotice::handleInformNoticeFlag(unsigned int flag)
{
    unsigned int bit = flag & 7;
    if (bit > 7)
        goto L1;
    {
        unsigned int byteIdx = flag >> 3;
        if (byteIdx >= InformNotice::kInformNoticeIndex)
            goto L2;
        if (!checkInformNoticeFlag(bit, byteIdx))
            goto L3;
        m_flags[byteIdx] = (unsigned char)(m_flags[byteIdx] + (1u << bit));
        alter();
        goto END;
    }
L1:;
L2:;
L3:;
END:;
