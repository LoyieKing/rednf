void CUser::send_clear_quest_list()
{
    PacketGuard packet;
    packet.put_header(0, 0x163);
    packet.put_int(0x7530);
    // ORIG 0x08??：quest/cleared 均无槽——`call getCurCharacQuestR; add $0x4,%eax;
    // mov %eax,(%esp); call getClearedQuest` 全链式（技巧 37/36：具名指针中转 =
    // 槽膨胀主因，pg 基址 -0x1c 应回 -0x14）。
    {
        const WongWork::STClearedQuest* cq =
            getCurCharacQuestR()->m_cleared.getClearedQuest();
#if !remake_verify && defined(DNF_DIAG) && DNF_DIAG  // remake-only diag; off unless -DDNF_DIAG=1
        // [bughunt7] 计数循环仅供诊断：ORIG 0x0868b044 无此 0x7530 次遍历。
        int nz = 0;
        int first = -1;
        for (int i = 0; i < 0x7530; ++i) {
            if (cq[i].m_cleared) {
                ++nz;
                if (first < 0) first = i;
            }
        }
        cMyTrace(__PRETTY_FUNCTION__, 0, 5)(
            "[DNF-DIAG QCLEAR] nz=%d first=%d", nz, first);
#endif
        packet.put_binary((const char*)cq, 0x7530);
    }
    packet.finalize(true);
    Send(packet);
}