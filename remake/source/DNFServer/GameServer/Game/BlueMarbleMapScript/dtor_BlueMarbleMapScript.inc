BlueMarbleMapScript::~BlueMarbleMapScript()
{
    unsigned int i = 0;
    while (i < m_tiles.size())
    {
        if (m_tiles[i] != 0)
        {
            delete m_tiles[i];
        }
        m_tiles[i] = 0;
        ++i;
    }
    m_tiles.clear();
}