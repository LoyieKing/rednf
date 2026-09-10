unsigned int CUser::SellItemTimeChecker(unsigned int nowSec, unsigned int interval)
{
    // ORIG 0x08230256：条件 = `(nowSec - m_sellTimeChkBaseSec) > interval`
    // （nowSec→edx→ecx 中转 sub %eax,%ecx，cmp interval/ja）——重置块后置
    // （ja 直跳），++ 块 fall + jmp 公共出口；重置块内先 hitCount=0 后 base=nowSec。
    // b28d-35：反条件 if/else 形（++ 为 then 直落、reset 为 else 后置）复现 ORIG
    //   25 条无对齐 nop 形；goto 形在 4.4 于 ja 目标处多发一条对齐 nop
    //   （探针 /tmp/st_probe2.cpp fE 实证：25 条逐条全等）。
    if ((nowSec - m_mcap.m_sellTimeChkBaseSec) <= interval)
    {
        ++m_mcap.m_sellTimeHitCount;
    }
    else
    {
        // 超窗重置：基准秒=本次时间、计数清零
        m_mcap.m_sellTimeHitCount = 0;
        m_mcap.m_sellTimeChkBaseSec = nowSec;
    }
    return static_cast<unsigned int>(m_mcap.m_sellTimeHitCount);
