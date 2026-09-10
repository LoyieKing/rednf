bool CCreature::RenameCreature(const char* name)
{
    strncpy(m_name, name, 0xc);
    return true;
