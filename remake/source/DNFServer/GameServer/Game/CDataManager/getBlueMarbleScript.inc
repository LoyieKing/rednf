void* CDataManager::getBlueMarbleScript()
{
    // ORIG 0x08365eea T：return *(void**)(this+0xb4a8)
    //   = m_choiceItem.m_blueMarbleScriptMgr（ChoiceItemInfo +0x08，已类型化）。
    return (void*)m_choiceItem.m_blueMarbleScriptMgr;
