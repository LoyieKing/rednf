// ORIG W 0x08142088（objdump）：return *(int*)(this+0xc)。
int CharacData::getGage() const
{
    return m_gage;
}