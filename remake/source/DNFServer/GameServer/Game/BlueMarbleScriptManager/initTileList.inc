bool BlueMarbleScriptManager::initTileList(const char* dir, const char* list)
{
    if (!loadRDARScriptListFile(&m_tileFileList, dir, list, 0))
    {
        return false;
    }

    m_tiles.clear();
    // b28d-96/270：ORIG 0x088d626e 用非 const _Rb_tree_iterator（ptEv/ppEv/ne），
    // const_iterator 会多出转换构造 + const ptEv ⇒ 改非 const。
    for (std::map<int, std::string>::iterator it = m_tileFileList.m_fileMap.begin();
         it != m_tileFileList.m_fileMap.end(); ++it)
    {
        BlueMarbleTileScript* tile = new BlueMarbleTileScript;
        // b28d-183 反例（ORIG 有槽）：ORIG 0x088d62f3 缓存 it->first 到 -0x1c 后再用。
        const int key = it->first;
        std::string path(dir);
        path += it->second;
        // b28d-59/168a①：ORIG 失败分支直落（`xor $1; test; je <成功>`）⇒ 否定早返形。
        if (!tile->importScript(path.c_str()))
        {
            // [b29 R20] ORIG 0x088d638d `cmpl $0x0,-0xc(%ebp); je`：4.1 的 delete 内联判空。
            // 4.4 的 -fdelete-null-pointer-checks 对「来自 new 的非空指针」省略该判空
            // ⇒ 用显式 if 复现（实测净 1 重判空，与 ORIG 同）。
            if (tile != 0)
                delete tile;
            tile = 0;
        }
        else
        {
            m_tiles[key] = tile;
        }
    }
    return true;
