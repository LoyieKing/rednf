void WindowsDataFileFamily::SetCheckingOption(unsigned long minOff,
                                              unsigned long maxOff) // 0x0857511e
{
    m_randMin = minOff;
    m_randMax = maxOff;
}