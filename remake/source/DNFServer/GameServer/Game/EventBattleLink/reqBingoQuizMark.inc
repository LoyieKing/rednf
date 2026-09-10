int BingoEvent::reqBingoQuizMark(CUser& user, int quizValue)
{
    // [b23S3C] ORIG 0x080ca262 逐 call 位实证的三处形差（本件真语义缺口 gap 20）：
    // ①m_bingo 是 boost::shared_ptr<CBingo>，ORIG 每用点 `add $0xc; call
    //   _ZNK5boost10shared_ptrI6CBingoEptEv`（W @0x080ce766）共 3 次 ⇒ 改真实成员
    //   `m_bingo->`（原镜像类 reinterpret 缓存 `bingo` 局部丢 2 次 call）；
    // ②user.getBingoData() ORIG 7 次重取（isSetBit/旧值/calNewValue 入参/新值比较/
    //   写 +6/读 +4/写 +4），无 `data` 具名缓存槽 ⇒ P48 每用点重取；
    // ③字段访问经 BingoData（CUser.h:1785）出单条 disp 形（`movb $0x1,0x6(%eax)` /
    //   `movzbl 0x4(%eax)`），原 `data[6]` 下标形多 1 条 add；
    // ④越界判 ORIG `cmpl $0x0;jns; cmpl $0x31;jle` = 死条件 `idx<0 && idx>0x31`
    //   （原 `||` 写成短路或在 idx<0 时误返 0x13，真语义 bug）⇒ 改 &&。
    if (m_bingo->getQuizValue() != quizValue)
    {
        return 0x17;
    }

    int idx = m_bingo->getQuizIndex();
    if (idx < 0 && idx > 0x31)
    {
        return 0x13;
    }

    if (isSetBit_Uint(static_cast<BingoData*>(user.getBingoData())->m_eventId, idx))
    {
        return 0x12;
    }

    unsigned int oldBoard = static_cast<BingoData*>(user.getBingoData())->m_eventId;
    int newLines = m_bingo->calNewValue(
        static_cast<BingoData*>(user.getBingoData())->m_eventId, 0);
    if (static_cast<BingoData*>(user.getBingoData())->m_eventId != oldBoard)
    {
        static_cast<BingoData*>(user.getBingoData())->m_needSave = true;
        int diff = newLines - static_cast<int>(static_cast<signed char>(
            static_cast<BingoData*>(user.getBingoData())->m_pad4));
        if (diff > 0)
        {
            if (diff > 3)
            {
                // [b28d-58] DIFF N（P85 + P238 反向）：ORIG 0x080ca376 段
                // `mov -0x10(%ebp),%eax; cmp $0x3,%eax; jle` + `mov $0x3,%eax;
                // mov %eax,-0x10(%ebp)`（局部载入比较 + 常量经 eax 中转 store）；
                // 4.4-O0 对 signed int 局部恒 `cmpl $0x3,mem` 直比 + `movl $3,mem`
                // 直存（`(int)3u` 异型化亦折）⇒ 2 条封顶，语义等价。
                diff = 3;
            }
            static_cast<BingoData*>(user.getBingoData())->m_pad4 =
                static_cast<char>(newLines);
            for (int i = 0; i < diff; ++i)
            {
                sendOneMatchLineReward(user);
            }
        }
    }
    return 0;
