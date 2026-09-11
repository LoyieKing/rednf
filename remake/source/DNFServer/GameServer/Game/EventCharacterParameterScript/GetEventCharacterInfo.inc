bool EventCharacterParameterScript::GetEventCharacterInfo(int job, int level,
                                                          STEventCharacterInfo& out) const
{
    if (level <= 0 || level >= 0x47) return false;
    std::map<int, STEventCharacterInfo_level>::const_iterator lit = m_levels.find(level);
    if (lit != m_levels.end()) {
        // b28c-36 AE（fresh ndiff 8→0，71=71）：ORIG 0x0895b799/0x0895b7b5 段
        // this(-0xc) 先载 %edx 再 lea key/sret。原 `const map& job_map = ...;
        // job_map.find(job)` 与指针形 `job_map->find` 两形均使 4.4 先算
        // key/sret 后载 this（残差 2 段/4 行）；改 **level 对象引用 +
        // 首成员访问** `lv.character_info.find(job)` 后 4.4 复现 ORIG 的
        // this-first 装载序（探针族 p2/p3 + 两形实测对照）。
        const STEventCharacterInfo_level& lv = (*lit).second;
        std::map<int, STEventCharacterInfo>::const_iterator jit =
            lv.character_info.find(job);
        if (jit != lv.character_info.end()) {
            out = (*jit).second;
            return 1;
        }
    }
    return 0;
}