// ---- ORIG T 0x08364e58 _ZNK12StageMapList12getNextStageEiRb ----
int StageMapList::getNextStage(int stageIdx, bool& outFlags) const
{
    std::map<int, _StageMapSummary>::const_iterator it = m_map.find(stageIdx);
    if (it == m_map.end())
    {
        return 0;
    }
    std::map<int, _StageMapSummary>::const_iterator next =
        m_map.find(it->second.m_nextId);
    if (next != m_map.end())
    {
        // [b28c-21] ORIG `movzbl 0x10(%eax),%edx; mov %dl,(%eax)` 无归一化；
        // 4.4 对 uchar→bool 赋值恒插 test/setne（跨版本 bool ABI，N 族）；
        // 字节视图形（引用/指针）实测均致 push %ebx + 帧 0x28→0x24，已回退。
        outFlags = next->second.m_flags;
    }
    return it->second.m_nextId;
}