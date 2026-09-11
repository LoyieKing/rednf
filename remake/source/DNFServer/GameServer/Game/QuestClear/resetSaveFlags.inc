void CQuestClear::resetSaveFlags()
{
    // ORIG 0x0808bb4e：清零 save-flag（movb $0,0x7530 单指令）。
    m_saveFlag = 0;
}