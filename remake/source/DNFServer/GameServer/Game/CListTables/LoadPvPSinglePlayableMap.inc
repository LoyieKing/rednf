void CMapList::LoadPvPSinglePlayableMap(char* path)
{
    char buf[512];
    CNRDScriptData data;
    data.import(path);
    for (std::map<int, PvPMapInfo>::iterator it = mapPvpMapInfo_.begin();
         it != mapPvpMapInfo_.end(); ++it)
    {
        // [b28d-solo-CListTables] ORIG 0x0834f4ba：ptEv 一次后把 &it->second 存具名
        // 指针槽 -0xc（mov %eax,-0xc(%ebp) / 复用回读），sprintf 与 m_playable 写
        // 均经该槽（P0 有槽往返 ⇒ 原码有具名局部）。
        PvPMapInfo* p = &it->second;
        sprintf(buf, "pvp deathmatch single play %d", p->m_mapIdx);
        p->m_playable = data.getInt(buf) == 1;
    }
