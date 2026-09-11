// [b28d-lane2] verdict=DIFF ndiff=67（ORIG 0x084d0466）：槽序/帧差(N82/P212/P221)+混合残差；gap+3。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=55（ORIG 0x84d0466）：N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_MonitorForbidChat::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    // ORIG 084d4xxx：全程 `mov 0x10(%ebp),%eax` 重读 data 形参 ⇒ **无 pPkt 具名槽**
    //   （我方旧形把 data 存槽，等于用 4B 栈槽顶替 ORIG 的 `push %ebx`：帧 0x68 vs 0x64）；
    //   `secs` 跨调用值落 %ebx（ORIG `mov 0xa(%eax),%ebx`），亦无具名局部（P0）。
    cMyTrace(__PRETTY_FUNCTION__, 0x2e96, 0)("Inter_MonitorForbidChat::dispatch_sig recv\n");

    const size_t nameLen =
        reinterpret_cast<Inter_MonitorForbidChatPktView*>(data)->m_e;
    char name[30];                                              // ORIG call memset 清零
    memset(name, 0, sizeof(name));
    strncpy(name,
            reinterpret_cast<Inter_MonitorForbidChatPktView*>(data)->m_12, nameLen);

    if (!G_GameWorld()->IsForbiddenChat(name))
    {
        G_GameWorld()->InsertUserToIDMap(name, 0);
        const int secs =
            reinterpret_cast<Inter_MonitorForbidChatPktView*>(data)->m_a;
        G_GameWorld()->DisableChat(name, secs);
        cMyTrace(__PRETTY_FUNCTION__, 0x2ea2, 0)("Inter_MonitorForbidChat::dispatch_sig %s for %d secs\n", name, secs);
    }
    return 0;

// [G2B-Monitor] 本件已做：删 pPkt 具名槽（ORIG 全程 `mov 0x10(%ebp),%eax` 重读 data，
//   且 `secs` 落 %ebx；我方旧形用 4B 栈槽顶替 ORIG 的 `push %ebx`，帧 0x68 vs 0x64）。
//   残余留 N：`&pPkt->m_12` 的 ORIG `mov <S>,%edx; add $0x12,%edx` vs 我方 `lea 0x12(%eax),%edx`
//   —— add-vs-lea 基址折叠族（N4）；尾部 ORIG 显式 `add $0x64,%esp; pop %ebx; pop %ebp`
//   vs 我方 `leave`（同一 epilogue 的两种编码）⇒ 留 N。
}