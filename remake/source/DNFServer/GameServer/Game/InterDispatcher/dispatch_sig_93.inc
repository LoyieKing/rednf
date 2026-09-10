int Inter_MonitorNoticeBlackList::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;
    // ORIG 0x084d0b5b：recv 在 state 检查后才物化 -0x10；count 每轮 cond 重读（movzbl 双扩展 + cmp/setg）
    // [b28d-lane2] 实测三形：①两独立视图 cast=NEAR 2（装载序反）；②union 视图+char* recv=NEAR 2；
    //   ③union 视图+类型化 recv=DIFF 3（装载序正但 `lea 0x4(%eax),%edx` vs ORIG `add $0x4,%edx`
    //   + i 落 %eax vs %edx）。取②（残差最小）。add-vs-lea 与寄存器名属 P30/N4 结构性伪影。
    Inter_MonitorNoticeBlackListPktView* recv =
        reinterpret_cast<Inter_MonitorNoticeBlackListPktView*>(data);
    for (int i = 0; i < (int)(unsigned char)recv->m_hdr.m_12; ++i)
    {   // ORIG 0x084d0b73 mov 0x3(%eax,%edx,4),%eax：具名数组视图 [i+4] 保 add $4+disp3 折叠形
        user->registerToBlackList(recv->m_ids[1][i]);
    }
    return 0;
