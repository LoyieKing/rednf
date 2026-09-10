int Inter_LastMsgResponse::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    // ORIG 0x084d0598：mgr 结果免槽直用（+0x08 mov %eax,(%esp)）；两计数跨 trace 调用缓存
    // %esi/%ebx（push esi/ebx，技巧2/24）；末比较 setle 物化（+0x8f）。
    GetInstanceShutdowManager()->IncrementLastMsgResponseCount();
    register unsigned int responseCnt =
        GetInstanceShutdowManager()->GetLastMsgResponseCount();
    register unsigned int msgCnt = GetInstanceShutdowManager()->GetLastMsgCount();
    cMyTrace(__PRETTY_FUNCTION__,
        0x2eb2, 0)("last msg (%d) response(%d)\n", msgCnt, responseCnt);
    // ORIG +0x7e 第 4 次 call 结果复用 %ebx，+0x8d cmp %eax,%ebx; setle %al; test; je（bool 物化）
    if ((int)GetInstanceShutdowManager()->GetLastMsgCount() <=
        (int)GetInstanceShutdowManager()->GetLastMsgResponseCount())
    {
        GetInstanceShutdowManager()->OnAllUserLogout();
    }
    return 0;
