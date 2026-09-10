bool CUser::checkCharacLinkType(char linkPos, int slot)
{
    int curLinkType = static_cast<int>(getCharacLinkType());
    if (slot <= static_cast<int>(linkPos) || linkPos < 0) {
        return false;
    }
    if (curLinkType == 2) {
        if (static_cast<int>(linkPos) + 1 >= slot) {
            return false;
        }
        // ORIG `movzbl -0x1c,%eax; and $0x1; test %al` ⇒ linkPos 按 uchar
        // 做位测试（probe p2/f7：`% 2` 源形 4.4 产 movzbl;and$1;test %al，ORIG 同形）。
        if (linkPos % 2 != 0) {
            return false;
        }
        if (m_characList[linkPos + 1].m_characLinkType != 3) {
            return false;
        }
    }
    else if (curLinkType == 3) {
        if (static_cast<int>(linkPos) - 1 < 0) {
            return false;
        }
        if (linkPos % 2 == 0) {
            return false;
        }
        if (m_characList[linkPos - 1].m_characLinkType != 2) {
            return false;
        }
    }
    return true;
