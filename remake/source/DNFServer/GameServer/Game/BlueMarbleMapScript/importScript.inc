bool BlueMarbleMapScript::importScript(const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return 0;
    }
    // 局部声明顺序与 ORIG 栈布局一致（line/-0x10、value/-0x14、tile/-0xc、t/-0x18）
    std::string line;
    int value = 0;
    BlueMarbleTileScript* tile;
    BlueMarbleTileScript* t;
    for (;;)
    {
        if (!ScanType(line, true))
        {
            break;
        }
        if (line == "[tile]")
        {
            m_tiles.clear();
            for (;;)
            {
                if (!ScanInt(&value))
                {
                    break;
                }
                tile = BlueMarbleScriptManager::getInsance()->getTile(value);
                if (tile == 0)
                {
                    return 0;
                }
                if (!ScanInt(&value))
                {
                    break;
                }
                t = new BlueMarbleTileScript;
                struct BM_TileRaw
                {
                    int m_a;  // +0x00
                    int m_b;  // +0x04
                    int m_c;  // +0x08
                };
                *(BM_TileRaw*)t = *(BM_TileRaw*)tile;
                t->m_value = value;
                m_tiles.push_back(t);
            }
        }
    }
    return 1;
