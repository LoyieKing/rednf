// [b28d-lane2] verdict=DIFF ndiff=293（ORIG 0x084cb726）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+5。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=225（ORIG 0x84cb726）：N22 cmp 操作数方向+N18a 单段 regmove+P30/N4 add-vs-lea。
// [b28d-SweepInterDispatcher2] verdict=DIFF gap 5→3（314 vs 311，TU 121/1331/5/133 不回退）：真修三处——
//   ① 成员过滤条件改正极性（P214①：`grade!=0 && grade!=0 && level>=200`，ORIG `jle` 假路直落）；
//   ② m_e/m_2d int→unsigned int（P211/P265 档③比较点中转，sizeof/偏移/mangled 零影响）；
//   ③ `(m_32>>1)&1` 补 `(unsigned char)` 包裹。残余=N6 位折叠（ORIG `shr %al` vs 4.4 `movzbl;sar`）+N95/N126 槽基平移 4（pPkt -0x20 vs -0x1c）。
int Inter_MonitorNoticeGuildInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    // ORIG 0x084****：state 岛后 pPkt 才物化 -0x1c（技巧13）。
    Inter_MonitorNoticeGuildInfoPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeGuildInfoPktView*>(data);
    if (user->get_charac_no(-1) !=
        pPkt->m_e)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2613,
            "Inter_MonitorNoticeGuildInfo::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    // ORIG 084cb8xx：这里的包基址是从 **pPkt 槽**读（`mov -0x1c(%ebp),%eax`），
    //   不是重读 data 形参（我方旧形 `mov 0x10(%ebp),%eax`）
    user->SetGuildDBInfo(                              // ORIG E 引用（头为 const&）
        *reinterpret_cast<STGuildDBInfoOnly*>(reinterpret_cast<char*>(pPkt) + 0x16));

    PacketGuard packet;                                // -0x28
    packet.put_header(0, 0x46);

    const int guildNameLen = (int)strlen(pPkt->m_16);   // -0x18
    packet.put_int(guildNameLen);
    packet.put_str(pPkt->m_16, guildNameLen);

    if (user->get_charac_no(-1) ==
        pPkt->m_2d)          // 会长标记
        packet.put_byte(1);
    else
        packet.put_byte(0);

    packet.put_byte((int)pPkt->m_31);
    packet.put_byte((int)((((unsigned char)pPkt->m_32) >> 1) & 1));
    packet.put_short((int)pPkt->m_38);
    packet.put_byte((int)(user->get_guild_create_right() & 0xff));
    packet.put_int(pPkt->m_3a);
    packet.put_byte((int)pPkt->m_3e);
    packet.put_int(pPkt->m_3f);
    packet.put_short((int)pPkt->m_58);

    int memberCountPos = packet.get_index();           // -0x2c（put_byte(int&,int) 回填需左值）
    int memberCount = 0;                               // -0x14
    packet.put_byte(0);
    // ORIG 循环体 `mov -0xSLOT(%ebp),%ecx; lea (%ecx,%eax,1),%eax`：基址走 pPkt 具名槽。

// [G2B-Monitor] 本件已做：`SetGuildDBInfo` 实参基址改走 pPkt 槽（ORIG `mov -0x1c(%ebp),%eax`；
//   我方旧形 `mov 0x10(%ebp),%eax` 重读 data 形参）。
//   残余留 N：① `(m_32>>1)&1` 的位折叠（ORIG `shr %al` vs 4.4 `movzbl %al,%eax; sar %eax`，
//   前轮已记 N6）；② 槽基平移 4（pPkt -0x20 vs -0x1c，N95/N126）；
//   ③ 循环体与尾段（成员环、put_byte 回填、公告/说明段）由聚合器 InterDispatcher.cpp 在
//   `#include` 之后续写 ⇒ 其中差异不在本任务写域 ⇒ 留 N。
#define pData (reinterpret_cast<char*>(pPkt))
    for (int i = 0;                                    // -0x10
         i < (int)(unsigned int)pPkt->m_5a;
         ++i)
    {
        if (user->getGuildMemberGrade() != 0 &&        // ORIG 双调用短路原样（正极性 P214①）
            user->getGuildMemberGrade() != 0 &&
            *reinterpret_cast<int*>(pData + i * 5 + 0x5b) >= 200)
        {
            packet.put_int(*reinterpret_cast<int*>(pData + i * 5 + 0x5b));
            packet.put_byte((int)*reinterpret_cast<signed char*>(pData + i * 5 + 0x5f));
            ++memberCount;
        }
    }
#undef pData
    packet.put_byte(memberCountPos, memberCount);      // ORIG put_byte(int&,int) 回填

    const int noticeLen = (int)strlen(pPkt->m_d4);      // 公告
    packet.put_int(noticeLen);
    packet.put_str(pPkt->m_d4, noticeLen);
    packet.put_byte((int)pPkt->m_ab);
    packet.put_byte((int)pPkt->m_b5);
    packet.put_int(pPkt->m_b6);
    const int introLen = (int)strlen(pPkt->m_ba);       // 说明
    packet.put_int(introLen);
    packet.put_str(pPkt->m_ba, introLen);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}