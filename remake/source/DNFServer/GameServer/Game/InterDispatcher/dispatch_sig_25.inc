int Inter_ReloadIngameAdvertisement::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    // ORIG：InitADExposeInfo 前 local_14=param_3（-0x10）；循环体每迭代重调 Instance
    // （不缓存局部，asm +0x4c call）；-1 检查在循环体顶部（break）、计数器 setle 在底部。
    // [b28c-34] ORIG 0x084c8d17 `mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)` = 具名
    // recv 局部（P0 槽往返）；循环内两处经该槽装载（lea/add 形）。
    stInGameADExposeCondition* cond =
        reinterpret_cast<stInGameADExposeCondition*>(data);
    InGameADManagerInstance()->InitADExposeInfo();
    int count;
    for (count = 0; count < 200; ++count)
    {
        // [b28d-PoolInterDisp] int 加法链形（probe_g 实证，10→4）：POINTER_PLUS 形被
        // 4.4 fold 成 base+(prod+const)（恒残 10）；纯 int PLUS 不折 ⇒ 复现
        // imul/lea/add $0x210 三段。字段访问形 cond[count].m_sentinel 实测折成
        // `add MEM+mov 0x210(,)`（gap+2、ndiff 6，劣于本形）。残 4 = imul 目标寄存器
        // 与 base 装载先后对调（LRA 形，N18a 族）。
        if (*reinterpret_cast<int*>(
                (reinterpret_cast<int>(cond) + count * 0x84c) + 0x210) == -1)
            break;                                       // ORIG +0x3c je（顶部门槛）
        InGameADManagerInstance()->InsertADExposeInfo(
            cond[count]);
    }
    cMyTrace(__PRETTY_FUNCTION__,
        0x2290, 0)("In Game Advertisement condition Reload OK, count(%d)", count);
    return 0;
