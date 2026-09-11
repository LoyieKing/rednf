// ORIG W _ZN15cUserHistoryLog11getItemXPosEv @0x08696000：
//   mov 0x8(%ebp),%eax; movzwl 0x40(%eax),%eax（单 disp 折叠）——具名成员
//   m_itemX（CUser.h:2153 +0x40）直读。
unsigned short cUserHistoryLog::getItemXPos()
{
    return m_itemX;
}