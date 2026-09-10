int CUser::GetItemTimeChecker(unsigned int nowSec, unsigned short val1,
                              unsigned short val2, unsigned int rangeSec)
{
    // b28d-solo-W8_CU_A：IDENTICAL 复原（原 b28d-35 N82/P182 判词作废）。
    //   ORIG @0x082302ac 帧 0x8、条件 `ja` 落 reset 块（窗口内逻辑 fall-through）。
    //   三处修法：① 条件极性反写（`<= rangeSec` 窗口在前 / reset 后置）复现
    //   `cmp 0x18(%ebp),%eax; ja <reset>`；② 删具名 int a/b 局部（帧 0x18→0x8）
    //   改为「先写 diff 字段（16 位 sub）再对字段本身 abs 写回」——
    //   ORIG 0x08230398/0x08230409 两段 `movzwl 字段;cwtl;sar;xor;sub` 即此形；
    //   ③ 真语义 bug 修正：原「MaxB 用 min 判据 / MinB 用 max 判据」两条互换了
    //   字段（B 通道 min/max 语义反转）；按 ORIG 0x0823031a/0x08230344 恢复为
    //   MinA,MinB,MaxA,MaxB 顺序（reset 块同序，ORIG 0x08230459..0x0823048a）。
    if ((nowSec - m_mcap.m_timeChkBaseSec) <= rangeSec) {
        ++m_mcap.m_timeChkCount;
        if ((static_cast<int>(m_mcap.m_timeChkMinA) - (short)val1) > 0) {
            m_mcap.m_timeChkMinA_u = val1;
        }
        if ((static_cast<int>(m_mcap.m_timeChkMinB) - (short)val2) > 0) {
            m_mcap.m_timeChkMinB_u = val2;
        }
        if ((static_cast<int>(m_mcap.m_timeChkMaxA) - (short)val1) < 0) {
            m_mcap.m_timeChkMaxA_u = val1;
        }
        if ((static_cast<int>(m_mcap.m_timeChkMaxB) - (short)val2) < 0) {
            m_mcap.m_timeChkMaxB_u = val2;
        }

        m_mcap.m_timeChkDiffA = static_cast<short>(
            static_cast<short>(m_mcap.m_timeChkMaxA_u) -
            static_cast<short>(m_mcap.m_timeChkMinA_u));
        m_mcap.m_timeChkDiffB = static_cast<short>(
            static_cast<short>(m_mcap.m_timeChkMaxB_u) -
            static_cast<short>(m_mcap.m_timeChkMinB_u));
        m_mcap.m_timeChkDiffA = static_cast<short>(
            m_mcap.m_timeChkDiffA < 0 ? -m_mcap.m_timeChkDiffA
                                      : m_mcap.m_timeChkDiffA);
        m_mcap.m_timeChkDiffB = static_cast<short>(
            m_mcap.m_timeChkDiffB < 0 ? -m_mcap.m_timeChkDiffB
                                      : m_mcap.m_timeChkDiffB);
    } else {
        // 重置分支（ORIG 写序：diff 清零 → 计数清零 → 基准秒与四端点）
        m_mcap.m_timeChkDiffA = 0;
        m_mcap.m_timeChkDiffB = 0;
        m_mcap.m_timeChkCount = 0;
        m_mcap.m_timeChkBaseSec = nowSec;
        m_mcap.m_timeChkMinA_u = val1;
        m_mcap.m_timeChkMinB_u = val2;
        m_mcap.m_timeChkMaxA_u = val1;
        m_mcap.m_timeChkMaxB_u = val2;
    }
    return m_mcap.m_timeChkCount;
