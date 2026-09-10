BlueMarbleTileScript* BlueMarbleMapScript::getTile(unsigned int zone)
{
    if (m_tiles.size() < zone || zone == 0)
    {
        return 0;
    }
    return m_tiles[zone - 1];
