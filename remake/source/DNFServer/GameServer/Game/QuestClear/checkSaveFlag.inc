unsigned char CQuestClear::checkSaveFlag() const
{
    // ORIG 0x0808bb3e：movzbl 0x7530(%eax) 直返 save-flag 字节（P9）。
    return m_saveFlag;
