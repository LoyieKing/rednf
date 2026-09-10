unsigned char CUser::getInformNoticeFlag(int idx)
{
    return m_informNoticeFlags[idx];   // ORIG 0x0868fa76 单 movzbl 0x8ebb1(%edx,%eax,1)
