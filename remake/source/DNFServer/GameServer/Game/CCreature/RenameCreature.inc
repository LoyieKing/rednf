// ORIG _ZN13user_creature9CCreature14RenameCreatureEPKc T 0x83373aa：
// strncpy(m_name, name, 0xc) 后恒返 1。
bool CCreature::RenameCreature(const char* name)
{
    strncpy(m_name, name, 0xc);
    return true;
}