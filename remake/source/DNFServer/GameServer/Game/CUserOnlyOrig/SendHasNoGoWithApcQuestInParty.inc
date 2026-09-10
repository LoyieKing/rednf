void CUser::SendHasNoGoWithApcQuestInParty(Quest const* quest)
{
    if (get_state() == 5 ||
        (get_state() == 10 && quest->m_triggerType == 3 &&
         quest->m_triggerSubType == 0))
    {
        CParty* party = reinterpret_cast<CParty*>(GetParty());
        if (party == 0)
            return;
        // ORIG 0x086??：`test %al,%al; je <T>` 无 xor ⇒ 正向 bool 直落形
        if (party->HasNoQuestGoWithApcInParty(quest)) {
        PacketGuard packet;
        packet.put_header(0, 0xa8);
        packet.put_int(quest->m_index);
        packet.finalize(true);
        party->send_to_party(packet);
        }
    }
