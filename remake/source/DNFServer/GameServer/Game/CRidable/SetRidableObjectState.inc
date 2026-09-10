bool CRidable::SetRidableObjectState(int idx, eRidableObjectState state)
{
    // ORIG 0x082feb5e 逐条：find == end → return 0；条件 (m_state==0||==4||==3||>4)
    // 成立 → `test %al,%al; je 赋值段; mov $0,%eax` **return 0**（真语义 bug：旧代码
    // 注释写"直接返回 true"并把 return true 放在条件外，返回码反了，P249 同族第 9 例）；
    // 条件不成立 → `m_state = state; mov $1,%eax` return 1。
    std::map<int, DungeonRidableObject>::iterator it = m_objects.find(idx);
    if (it != m_objects.end())
    {
        if (it->second.m_state == 0 || it->second.m_state == 4 ||
            it->second.m_state == 3 || it->second.m_state > 4)
            return false;
        it->second.m_state = (int)state;
        return true;
    }
    // ORIG 0x082fec25：it==end 的 return 0 落在函数末尾独立块（cond 成立的
    // return 0 在 0x082fec06 中间块）⇒ 用末尾 return 共享该块。
    return false;
