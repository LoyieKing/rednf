// ORIG 反汇编对 get_state() 短路求值两次调用（==5 或 ==10 分支），不做缓存。
// [b28d-solo-CUserOnlyOrig AE] ORIG 0x0866d972：调用点/形态已对齐（本波修法见函数内注释）。
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
}