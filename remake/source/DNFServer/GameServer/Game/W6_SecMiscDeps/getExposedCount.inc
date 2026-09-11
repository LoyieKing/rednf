unsigned int WongWork::CMCAPManager::getExposedCount() const
{
    // ORIG W 0x0869504c (size 0xb)：return *(unsigned int*)(this + 0x10)。
    return m_field10;
}