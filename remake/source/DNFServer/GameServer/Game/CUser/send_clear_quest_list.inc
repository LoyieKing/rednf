void CUser::send_clear_quest_list()
{
    PacketGuard packet;
    packet.put_header(0, 0x163);
    packet.put_int(0x7530);
    // ORIG 0x08??：quest/cleared 均无槽——`call getCurCharacQuestR; add $0x4,%eax;
    // mov %eax,(%esp); call getClearedQuest` 全链式（技巧 37/36：具名指针中转 =
    // 槽膨胀主因，pg 基址 -0x1c 应回 -0x14）。
    packet.put_binary(
        (const char*)reinterpret_cast<WongWork::CQuestClear*>(
            (char*)getCurCharacQuestR() + 4)->getClearedQuest(),
        0x7530);
    packet.finalize(true);
    Send(packet);
