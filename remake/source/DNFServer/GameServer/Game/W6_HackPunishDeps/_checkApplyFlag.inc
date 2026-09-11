// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// _checkApplyFlag —— ORIG W 0x0869550c。
// 依据 objdump 逐分支恢复：
//   m_value3 == 1/5  → 拷贝出参直接放行；
//   m_value3 == 2/10 → 在 lookOutsA/lookOutsB 观察名单中命中 {m_type1,m_type2}
//                      即拒绝；m_value3 == 3 → 在 ipCounters 名单中命中即拒绝；
//   m_value3 == 4/9  → _findIPCount 超阈值记日志并拒绝，否则记日志继续；
//   m_value3 == 0xb/0xc → _findFullIPCount 同型处理；
//   其余 → 拷贝出参放行。
// ----------------------------------------------------------------------------
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N18b/N7）→ 试=新口径 n1_v 324=324 gap=+0；唯一残差 prologue `push %esi; sub $0x40` vs 我方 `sub $0x44`（ORIG 以 %esi 跨 4 次 call 缓存第 9 实参 ip：索引 199/223/277/301 `mov 0x24(%ebp),%esi; mov %esi,0x1c(%esp)`）+ P209 禁改（无符号 jbe 语义）⇒ 维持。
bool CAutoPunishRuleHackTypeMgr::_checkApplyFlag(
    const HackTypeInfo_t& info, CHackAnalyzer* analyzer, HackTypeInfo_t& outInfo,
    std::vector<stLookOuts>& lookOutsA, std::vector<tyIPCounters>& ipCounters,
    std::vector<stLookOuts>& lookOutsB, char* ip)
{
    // b28d-solo-_checkApplyFlag: N 封顶（寄存器分配/帧差）。已修：①cnt 两句式
    // 预清（P198①）；②补回 ORIG 0x8695825 的冗余 `szC = ipCounters.size()` 求值
    // ⇒ 324=324、p167 CLEAR 18/18。残差：ORIG 用 %esi 缓存 ip、%ebx 缓存 cnt，
    // 4.4 两值共用 %ebx（帧 0x44 vs 0x40、缺 push %esi）+ `m_value4 > (unsigned
    // int)cnt` 4.4 恒载 cnt 入寄存器而 ORIG mem 直比（P209）。**jcc 逐位交叉
    // 验证：37/37 MATCH**（控制流与 ORIG 等价，残差纯为寄存器分配/装载序，
    // 属 P75/N7 跨版本结构性差异）。ORIG 0x0869550c。
    // [S2W6Deps 复核·负结论] 重新逐条核对（reloc 归一口径）：唯一残差 = prologue
    // `push %esi` + `sub $0x40` vs 我方 `sub $0x44`，ORIG 用 %esi 跨 4 次 call 缓存
    // 第 9 实参（索引 199/223/277/301：`mov 0x24(%ebp),%esi; mov %esi,0x1c(%esp)`），
    // 我方 gcc11 每次从参数区重载入 %eax（**指令数完全相同**，仅寄存器角色差）。
    // 该缓存由 gcc 分配器决定（同源多 call 跨活跃），源码改写（具名局部/指针）只会
    // 引入新槽 ⇒ 维持 N7 封顶，不动。
    // [F2a-C05② 复测·不适用] 件级 F2 旋钮（具名局部 unsigned→int 求比较内存直比）
    //   逐条核对报告 5 处 ORIG `cmp -0xNN(%ebp),%eax`：其中 3 处（`setl` 族，
    //   ORIG 0x869550c+0x?? 的 cnt 循环尾测）我方已为同形 mem 直比（仅槽号差）；
    //   其余 2 处 = `m_value4 > (unsigned int)cnt`（ORIG `cmp -0x10/-0xc(%ebp),%eax;
    //   jbe`）——mem 操作数 cnt 是**跨调用返回值**（_findIPCount 结果局部），
    //   且 jcc 为**无符号序 jbe**（P196 红线：局部改 int 即翻有符号序，属真语义
    //   bug 家族）⇒ 禁改，维持 P209/N18b 封顶。无改动、无回滚。
    if (info.m_value3 == 1 || info.m_value3 == 5)
    {
        outInfo.CopyStruct(info);
        return true;
    }

    int szA = (int)lookOutsA.size();
    if (info.m_value3 == 2 && szA > 0)
    {
        for (int i = 0; i < szA; ++i)
        {
            if (info.m_type1 == lookOutsA[i].m_a &&
                info.m_type2 == lookOutsA[i].m_b)
            {
                return false;
            }
        }
    }

    int szB = (int)lookOutsB.size();
    if (info.m_value3 == 10 && szB > 0)
    {
        for (int i = 0; i < szB; ++i)
        {
            if (info.m_type1 == lookOutsB[i].m_a &&
                info.m_type2 == lookOutsB[i].m_b)
            {
                return false;
            }
        }
    }

    int szC = (int)ipCounters.size();
    if (info.m_value3 == 3 && szC > 0)
    {
        for (int i = 0; i < szC; ++i)
        {
            if (info.m_type1 == ipCounters[i].m_a &&
                info.m_type2 == ipCounters[i].m_b)
            {
                return false;
            }
        }
    }

    if (info.m_value3 == 4 || info.m_value3 == 9)
    {
        // P198①：ORIG 在 call 前有 `movl $0x0,-0x10(%ebp)` 预清。
        int cnt = 0;
        cnt = _findIPCount(info.m_type1, info.m_type2, ip);
        if (info.m_value4 > (unsigned int)cnt)
        {
            WriteIPCounterLog(analyzer, 0, info.m_type1, info.m_type2,
                              info.m_value4, cnt, ip);
            return false;
        }
        WriteIPCounterLog(analyzer, 1, info.m_type1, info.m_type2,
                          info.m_value4, cnt, ip);
    }

    // ORIG 0x8695825 在 4/9 段与 11/12 段之间重新求值 ipCounters.size() 并复用
    // szC 槽（-0x20）；P167 差集 MISSING 1×tyIPCounters::size()。
    szC = (int)ipCounters.size();

    if (info.m_value3 == 11 || info.m_value3 == 12)
    {
        // P198①：ORIG 在 call 前有 `movl $0x0,-0xc(%ebp)` 预清。
        int cnt = 0;
        cnt = _findFullIPCount(info.m_type1, info.m_type2, ip);
        if (info.m_value4 > (unsigned int)cnt)
        {
            WriteFullIPCounterLog(analyzer, 0, info.m_type1, info.m_type2,
                                  info.m_value4, cnt, ip);
            return false;
        }
        WriteFullIPCounterLog(analyzer, 1, info.m_type1, info.m_type2,
                              info.m_value4, cnt, ip);
    }

    outInfo.CopyStruct(info);
    return true;
}