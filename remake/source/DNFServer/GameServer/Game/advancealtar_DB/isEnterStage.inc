bool CharacAdvanceAltarManager::isEnterStage(int stageId) const
{
    // [b28d-solo-advancealtar_DB] P0/P183：ORIG 0x08131320/3336/1366 三处各自
    //   `mov 0x8(%ebp),%eax; mov (%eax),%eax` 重读 current（无具名局部槽）。
    //   P96/P220：ORIG 两次 const_iterator 转换构造 + `eq` 调用 ⇒ 具名
    //   const_iterator + `if (it == end()) return false; return true;`（P56 分支形）。
    if (m_currentAdvanceAltarData == 0)
        return false;
    std::map<int, _Stage>::const_iterator it =
        m_currentAdvanceAltarData->m_stages.find(stageId);
    if (it == m_currentAdvanceAltarData->m_stages.end())
        return false;
    return true;
