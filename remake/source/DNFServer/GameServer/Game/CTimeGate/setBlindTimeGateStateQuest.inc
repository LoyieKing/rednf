bool CTimeGate::setBlindTimeGateStateQuest(int questIdx)
{
    // b28d-35：ORIG 0x08234e46 短路物化在 %al（`mov $1/%al; test %al,%al`，
    //   零槽，帧 0x18）。实测：register bool → %ebx+push（+5）；普通 bool →
    //   落槽 movb/cmpb（+2）。按 P226 扩展用语句表达式包条件（内无具名变量）
    //   复现短路物化岛且零槽。
    if (({ getQuestIndex() == questIdx && isOpen(); }))
    {
        return true;
    }
    if (({ getOpenedQuestIndex() == questIdx && !isOpen(); }))
    {
        return true;
    }
    return false;
