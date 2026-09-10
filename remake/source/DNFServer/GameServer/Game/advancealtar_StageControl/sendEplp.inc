void StageControl::sendEplp(StageEndType::T endType)
{
    CUser* user = reinterpret_cast<CharacData*>(m_characData)->getUser();
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
