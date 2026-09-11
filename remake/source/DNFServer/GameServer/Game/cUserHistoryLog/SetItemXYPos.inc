// ORIG W _ZN15cUserHistoryLog12SetItemXYPosEtt @0x0822f084：short 形参 spill
// 槽（-0x4/-0x8）+ movzwl 回读 + `mov %dx,0x40(%eax)` disp 折叠写。
void cUserHistoryLog::SetItemXYPos(unsigned short x, unsigned short y)
{
    m_itemX = x;
    m_itemY = y;
}