// ---------------------------------------------------------------------------
// §2 CraneMinigameManager / CraneMinigameScript（ORIG 0x080ebd44/0x080ebce0/
// 0x080ebd7a/0x088e38d6）。ORIG 中 Manager 与 Script 同一对象布局：
// +0x04 vector<CraneItemInfo*>、+0x18 内嵌 Action_SendMail。
// ---------------------------------------------------------------------------
// getItem @ 0x088e38d6。
CraneItemInfo* CraneMinigameScript::getItem(short idx)
{
    if (static_cast<unsigned int>(idx) < m_items.size())
    {
        return m_items.at(idx);   // ORIG 用 at（含界检）
    }
    return 0;
}