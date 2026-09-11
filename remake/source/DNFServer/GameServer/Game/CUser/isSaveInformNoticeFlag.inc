bool CUser::isSaveInformNoticeFlag()
{
    return m_saveInformNoticeFlag;   // ORIG movzbl 0x8ebc1 直返（m_informNoticeFlags 起 +0x10）
}