void CQuestClear::setClearedQuest(const STClearedQuest* quest)
{
    // ORIG 0x0808bb5e：memcpy(this, quest, 0x7530)，不含 save-flag。
    memcpy(m_cleared, quest, 0x7530);
