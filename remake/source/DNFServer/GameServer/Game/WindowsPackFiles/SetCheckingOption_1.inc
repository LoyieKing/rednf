void WindowsExeFileFamily::SetCheckingOption(unsigned long minOff,
                                             unsigned long maxOff)  // 0x08574620
{
    m_randMin = minOff;
    m_randMax = maxOff;
}