bool WindowsDataFileFamily::IsScanningAddressReset()   // 0x08575444
{
    if (m_scanOffset == 0)
        return true;
    return false;
}