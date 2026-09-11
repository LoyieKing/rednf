// ---- ORIG T 0x08364df6 _ZNK12StageMapList7isExistEi ----
bool StageMapList::isExist(int stageIdx) const
{
    // [b28d-D33] ORIG 0x08364e40：find 直构 -0x10（this）、end 临时 -0xc（rhs），
    // 调 operator==（eq）后 test/je/mov$0/mov$1 分支形（P56 + P47）。
    std::map<int, _StageMapSummary>::const_iterator it = m_map.find(stageIdx);
    if (it == m_map.end())
    {
        return false;
    }
    return true;
}