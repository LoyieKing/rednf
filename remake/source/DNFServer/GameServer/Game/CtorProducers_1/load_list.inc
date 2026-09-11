bool CWorldMapList::load_list()
{
    // [b28d-solo-CtorProducers_1 判词·P304+P50+P197③] ORIG 0x08364506：旧「N82 帧差
    // 封顶」判词作废。ORIG 逐条：①`movl $0x0,-0x20` 预清 name 在 script C1 **之前**、
    // `movl $0x0,-0x1c` 预清 worldMap 在 iterator C1Ev 之前（P197③ 声明=0 + 循环内
    // 独立赋值两段式）；②iterator 默认构造 + `it = begin()` 经 sret -0x8c 拷入
    // （P50 先声明后赋值）；③begin/end 的 this 直接 `movl $0x9501000,0x4(%esp)`
    // —— 旧 `reinterpret_cast<STFileListMapMirror*>(&g_worldMapScriptFileList)`
    // 使 4.4 物化寄存器（`mov $0,%eax; mov %eax,%edx`，P304）；STScriptFileList
    // 的 m_fileMap@+0 是权威同型成员，直接成员访问即消 cast。
    const char* name = 0;
    WorldMapScript script;
    CWorldMap* worldMap = 0;
    std::map<int, std::string>::iterator it;
    it = g_worldMapScriptFileList.m_fileMap.begin();
    for (; it != g_worldMapScriptFileList.m_fileMap.end(); it++)
    {
        name = it->second.c_str();
        if (name == 0)
            break;
        if (!importWorldMapScript(&script, name))
        {
            printf("World Map Script Error: %s\n", name);
            return false;
        }
        worldMap = new CWorldMap;
        worldMap->set_world_map(&script);
        insert_world_map(worldMap);
    }
    return true;
}