void CGameManager::WarRoomCountAdjustByChannelInfo()
{
    channel_script_t* script = (channel_script_t*)G_CDataManager()->GetChannelScript();
    // ORIG get_channel_no 与 getChannelInfo 各独立 G_CEnvironment()（P183 勿缓存 env）。
    // b28d-solo-CGameManager 判词：N85①+N82 封顶——ORIG `flds 0x8(%eax,%edx,4);
    // fstps -0x34`（具名 float 经 x87 栈槽往返）vs 4.4 `mov mem,%eax; mov %eax,-0x10`
    // 直搬（P252/N85① 跨版本）；另 ORIG push edi/esi/ebx 缓存 a/b/dmW（P52），4.4 槽化。
    // [A12-2026-09-10 · C07 新形复查·实测零效果已回滚] 该处具名局部由
    // `unsigned int` 改 `int`（S1 一次性旋钮）后 TU 四数 729/1894/0/16 **逐值不变**
    // ⇒ 不命中 C07 形（本例升格点消费者是「被调方按值实参」而非「跨 call 复用」，
    // 与 Main ③ 的 A14 反例边界一致）⇒ 维持 N7/N82 判词。
    // [J9-2026-09-11 · P0 槽-局部双向对应律] ORIG 0x08298961-0x08298966 为
    // `call get_channel_no; mov %eax,%ebx`（值升格 callee-saved、**无槽往返**）⇒ 原码
    // 无 channelNo 具名局部，值是 getChannelInfo 第 3 实参的**纯表达式**；我方旧形多
    // `mov %eax,-0x28(%ebp)` + 尾部 `mov -0x28(%ebp),%edx; mov %edx,0x8(%esp)` 两条，
    // 并把其后全部槽顶深 0x10（帧 0x4c vs ORIG 0x3c）。删具名局部后槽集/帧逐值归位。
    channel_info_t* info = script->getChannelInfo(
        (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup,
        G_CEnvironment()->get_channel_no());
    for (int a = 0; a < 10; ++a)
    {
        for (int b = 0; b < 0x18; ++b)
        {
            // ORIG 内层每轮 **两次** G_CDataManager：dmW(写表基, 先)→f→第二处**内联**
            // 在乘法表达式里（ORIG 无 dmR 槽：0x8298a0a `call G_CDataManager` 结果
            // 直接 `mov -0x1c(%ebp),%edx; add $0x21ec,%edx; mov 0x4(%eax,%edx,4),%eax`
            // ——P0/P183 表达式直传）。
            // [b29x P52/P183/P243 复检] ORIG 0x082989a1..0x082989f2 的**求值序**：
            //   ①`call G_CDataManager → mov %eax,%esi`（LHS receiver, callee-saved 独占）
            //   ②`mov b→%ebx / mov a→%edi`（LHS 索引）
            //   ③`flds 0x8(%eax,%edx,4); fstps -0x34`（RHS 左操作数 f 先求值并跨下一次
            //     call 存槽）
            //   ④`call G_CDataManager → fildl -0x2c; fmuls -0x34`（RHS 右操作数 X 后求值）
            // ⇒ 原码是**链式 LHS receiver + RHS 写 `f * X`**（不是 `X * f`：后者 4.1 会
            // 先求 X ⇒ call 在 flds 之前，与 ORIG 相反；P243 x87 书写序判据）。
            // 我方旧形（具名 dmW + 具名 f + `X * f`）读数 74/74/31：dmW 落 -0x10 槽 +
            //   尾部 reload ⇒ 帧 0x40 vs ORIG 0x3c。本次按上式重写。
            // 写表：G_CDataManager()+8+(b*10+a+0x223c)*4 = m_warRoomCount[b][a]
            // （ORIG 0x082989f2..0x08298a02 单条 scaled store）。
            ((CDataManager*)G_CDataManager())
                ->m_warAreaCounter.m_warRoomCount[b][a] =
                (int)(info->m_gridPercent[a] *
                      ((CDataManager*)G_CDataManager())
                          ->m_warAreaCounter.m_counterTable[b]);
        }
    }
}