// ----------------------------------------------------------------------------
// ORIG T 0x081306be（sendEplp.md 全文）：getUser 判空直接返回；
//   available = user->m_advanceAltar.isAvailableTciket()（CUser+0x8df60）；
//   char isAvailableTicket; pad[3]}（8B，权威定义 NOTIPacketStruct.h）；
//   char isAvailableTicket; pad[3]}（8B，权威定义 advancealtar.h）；
//   put_header(0,0x20e) + put_binary(8) + finalize(true) → Send。
// ----------------------------------------------------------------------------
// [b28d-solo-StageControl] P183/P0：ORIG 直接 `this+0x98` 调 CharacData::getUser
//   （无 getCharacData thunk call）；isAvailableTciket 结果先落具名 bool 槽 -0x9
//   （`mov %al,-0x9` 直存、body 字段处 `movzbl -0x9; mov %al` 读回），且声明在 body
//   之前 ⇒ 调用序在 body ctor 之前；旧 `? 1 : 0` 三目被物化成 test/je/mov$1/jmp/mov$0。
void StageControl::sendEplp(StageEndType::T endType)
{
    CUser* user = m_characData.getUser();   // [BigP3D 成员化] 真实成员直调（等价 this+0x98）
    if (user == 0)
    {
        return;
    }

    const bool isAvailableTicket = user->m_advanceAltar.isAvailableTciket();
    NOTIPacketStruct::_NotiAdvanceAltarEplpState body;
    body.m_eplpType = endType;
    body.m_isAvailableTicket = isAvailableTicket;

    PacketGuard guard;
    guard.put_header(0, 0x20e);
    guard.put_binary((char*)&body, 8);
    guard.finalize(true);
    user->Send(guard);
}