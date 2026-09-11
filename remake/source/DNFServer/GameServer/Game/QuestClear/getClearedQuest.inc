const STClearedQuest* CQuestClear::getClearedQuest() const
{
    // ORIG（0x0808bb80）返回 this：m_cleared 位于 +0x00，get_quest 据此 memcpy 0x7530 字节。
    return reinterpret_cast<const STClearedQuest*>(this);
}