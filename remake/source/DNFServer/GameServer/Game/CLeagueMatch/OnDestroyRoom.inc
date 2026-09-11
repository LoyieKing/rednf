// 【N7 封顶】b26 lane19：虚调用 callee 寄存器 %ecx↔%eax 互换（ORIG
// mov (%eax),%ecx+call *%ecx vs 4.4 mov (%eax),%eax+call *%eax，16 条逐条
// 长度/助记符全等），临时/引用/限定名不可翻，语义同。
void CLeagueMatch::OnDestroyRoom(CUser* /*user*/)   // ORIG T 0x0855d168
{
    m_state = 0;
    pWaitingRoomList_->DelRoom(this);               // IRoomList vtable+0x04
}