STSkillCommandData::STCommand* CUser::get_skill_command_buf(STSkillCommandData::STCommand* out) const
{
    std::map<int, STSkillCommandData>* pMap =
        reinterpret_cast<std::map<int, STSkillCommandData>*>(const_cast<char*>(getSkillCommand()));
    if (pMap == 0)
        return out;
    int idx = 0;
    for (std::map<int, STSkillCommandData>::iterator it = pMap->begin();
         it != pMap->end() && idx <= 0x31; ++it, ++idx)
    {
        // b28d-solo-CUser p167：ORIG 0x0865459a/0x08654768 每字段各发一次
        //   `_Rb_tree_iterator::operator->()`（共 6 次，我方缓存引用 v 缺 6 次）⇒
        //   去掉具名引用，逐字段 `it->second.m_xxx`（P167 判据/P183 反向）。
        STSkillCommandData::STCommand cmd;
        cmd.m_command = it->second.m_command;   // bits 0-13（& 0x3fff）
        cmd.m_count   = it->second.m_count;     // bits 29-31
        cmd.m_key0    = it->second.m_key0;      // bits 14-16
        cmd.m_key1    = it->second.m_key1;      // bits 17-19
        cmd.m_key2    = it->second.m_key2;      // bits 20-22
        cmd.m_key3    = it->second.m_key3;      // bits 23-25
        cmd.m_key4    = it->second.m_key4;      // bits 26-28
        out[idx] = cmd;
    }
    return out;
