// ORIG W 0x08142094（objdump）：return *(int*)(this+8)。
int CharacData::getGageLevel() const
{
    return m_gageLevel;
}