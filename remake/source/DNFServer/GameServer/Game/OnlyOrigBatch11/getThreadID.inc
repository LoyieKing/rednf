// ORIG W 0x082a4218（非 const）：读 +8（m_tidCopy）。
unsigned long ThreadInterface::getThreadID()
{
    return m_tidCopy;
}