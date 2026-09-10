bool PvP_MissionSystem::Load_list()
{
    // b28d-61 按 ORIG 0x085e1dc6 逐段还原：
    // ① path 函数级声明先清零（P198 两句式，ORIG `movl $0x0,-0xc` 在函数头）；
    // ② 非 const iterator（ORIG begin 为非 const 版，无转换构造）；
    // ③ insert(make_pair(idx,mission))（ORIG 085e7567 make_pair + 085e75de
    //    map::insert(pair const&)，非 operator[]）；
    // ④ m_kindMissionList[kind] 无边界防御（P140，ORIG 直接下标）；
    // ⑤ grade/kind 每次内联调用 getter（无具名 int 槽，ORIG sete/test/je 形）；
    // ⑥ `m_dailyMissionKind[kind] = true`（ORIG bitset::operator[] + reference::op=，
    //    非 set()&0x1f）。
    const char* path = 0;
    // ⑦ 声明序 path→it→script：ORIG 发射序为 path 清零、begin、script 构造
    //    （script 槽 -0xc4 最深），故 it 的初始化必须写在 script 声明之前。
    std::map<int, std::string>::iterator it =
        g_PvpMissionScriptFileList.m_fileMap.begin();
    MissionScript script;
    for (; it != g_PvpMissionScriptFileList.m_fileMap.end(); ++it)
    {
        path = it->second.c_str();
        if (path == 0)
            break;
        if (!importPvpMissionScript(&script, path))
        {
            printf("Mission Script Error: %s\n", path);
            return false;
        }
        CMission* mission = Factory(script);
        if (mission == 0)
            return false;
        m_missionMap.insert(std::make_pair(mission->GetMissionIndex(), mission));
        m_kindMissionList[mission->GetMissionKind()].push_back(mission);
        if (mission->GetMissionGrade() == 2)
            m_dailyMissionKind[mission->GetMissionKind()] = true;
    }
    sort_list();
    return true;
