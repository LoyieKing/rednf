void CGameManager::send_userinfos_to_cutoff_server()
{
    // [SweepCGameManager2 本轮] 槽序翻正：bucketIdx 提至函数头声明（P212），
    //   user(-0x14 浅)/bucketIdx(-0x18 深) 与 ORIG 逐槽一致；残余仅 `% 10`
    //   无符号取模展开——ORIG `shr $0x3,%edx` 商留寄存器 vs 4.4 商落槽
    //   （`mov %edx,%eax; shr; mov %eax,-0x18; mov -0x18,%edx`，+4 条）⇒ N。
    // b28d-solo-CGameManager 判词：DIFF·N82（ours 180 / orig 177，gap 3）。
    //   语义缺口已清零（p167 CLEAR 16/16）：删发明 placement-new 循环、accId 去具名、
    //   去 bucket 指针缓存（P183）、empty() 早返（P16）、字段写序照 ORIG。
    //   残余 = STTempUsers buckets[10] 对象组槽基差 + 第二循环表寻址 shl/lea 折叠
    //   （P311 族）——4.4 槽分区不可控。
    //   [b28d-solo 本波翻正] 写目标改 `buckets[bucketIdx].m_ids[bucket->m_count]`
    //   复现 ORIG 0x08295f87..0x08295f93 重算基址形
    //   （`imul $0x259,%esi,%edx; add %ebx,%edx; mov %eax,-0x6904(%ebp,%edx,4)`；
    //   原 `bucket->m_ids[...]` 产 `mov %edx,0x4(%eax,%ebx,4)`）；余差仅
    //   bucketIdx/bucket 槽号互换与 %esi 装载（N82）。
    // b28d-solo-CGameManager: ORIG 0x08295e84（178 insn）。修法（P167+P0+P183）：
    //   ①删发明 placement-new 循环（ORIG 无 _ZnwjPv/重复 ctor，P167 EXTRA）；
    //   ②accId 去具名 → get_acc_id() 两次表达式直读（ORIG 两处 call，P0）；
    //      m_count 索引不去具名 n（ORIG 经 %ebx 无槽）；
    //   ③第二循环去 bucket 指针缓存（ORIG 每访问重发 imul/lea/sub，P183）；
    //   ④empty() 改早返（ORIG call;test;jne <epilogue>，P16）；
    //   ⑤字段写序照 ORIG：packetSize→m_count→memcpy→m_connNo→SendPacket(,pkt.packetSize)。
    // [K9GameRestA.CGameMgr12Run 2026-09-11 复核] DIFF 178/182 残差唯一 = `% 10`
    //   展开（ORIG 0x08295f5f..0x08295f6d `shr $0x3,%edx` 商留寄存器 + `mov %ecx,%edx;
    //   sub %eax,%edx` 余数直落 -0x18）vs 4.4 商/余各经 -0x18 槽往返（+4 条）⇒
    //   N86b 封顶。p167 CLEAR 16=16；串已核（§6.5/§6.5.1）。
    struct STTempUsers
    {
        unsigned short m_count;
        int m_ids[600];
        STTempUsers() { m_count = 0; }
    };
    STTempUsers buckets[10];
    // P212：ORIG user(-0x14 浅) / bucketIdx(-0x18 深) ⇒ 全头部先声明 bucketIdx。
    unsigned int bucketIdx;
    Packet_CutOff_UDP_Reply_UserInfo pkt;
    pkt.m_channel = (unsigned char)((CEnvironment*)G_CEnvironment())->get_channel_no();
    pkt.m_group = (unsigned char)((CEnvironment*)G_CEnvironment())->m_serverEnvir.m_serverGroup;
    if (m_userByAccId.empty())
        return;
    std::map<unsigned int, CUser*>::iterator it = m_userByAccId.begin();
    for (; it != m_userByAccId.end(); ++it)
    {
        CUser* user = it->second;
        if (user->get_state() > 1)
        {
            bucketIdx = user->get_acc_id() % 10;
            STTempUsers* bucket = &buckets[bucketIdx];
            // ORIG 0x08295f87..0x08295f93 写目标用重算基址
            // （`imul $0x259,%esi,%edx; add %ebx,%edx; mov %eax,-0x6904(%ebp,%edx,4)`），
            // 仅 m_count 读走 bucket 指针（-0x10）⇒ 目标写成 buckets[bucketIdx]。
            buckets[bucketIdx].m_ids[bucket->m_count] = (int)user->get_acc_id();
            bucket->m_count = bucket->m_count + 1;
        }
    }
    for (int i = 0; i < 10; ++i)
    {
        if (buckets[i].m_count != 0)
        {
            pkt.packetSize = (unsigned short)(buckets[i].m_count * 4 + 0xe);
            pkt.m_count = buckets[i].m_count;
            memcpy(pkt.m_accIds, buckets[i].m_ids, (unsigned int)buckets[i].m_count * 4);
            pkt.m_connNo = buckets[i].m_ids[0];
            GlobalData::s_double_check_proxy->SendPacket((char*)&pkt, pkt.packetSize);
        }
    }
}