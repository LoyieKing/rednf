void PackSet::clear()   // @0x08ac48d2
{
    m_packFile = 0;
    m_revision = 0;
    m_headerSize = 0;
    m_type = PACK_LOAD_FULL;
    m_indexes.clear();
    m_addeds.clear();
    m_bUseEncryptedFile = false;
}