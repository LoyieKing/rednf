// ClearAction —— 报告全文转写。ORIG T 0x08121dcc。
// [b28d-27] DIFF N82：ORIG 0x08121dcc；p167 CLEAR（3/3），残差为帧/槽分配，
// [b28d-solo-APSystemOnlyOrig] 判词：**N18b/P265 封顶**（残差 = ①字段 vs enum 形参比较 4.4 恒载寄存器 1 条；②enum 实参 int→unsigned-enum 转换两段中转 ×2；p167 CLEAR 3/3；帧 0x34/槽 -0xd/-0xc/汇聚变量全等）。
// [F2d-C05② 甄别] 比较对象为 enum 形参（非「两具名局部」）⇒ F2 不适用；A/B 实测删
//   `(int)group` cast 两态逐条同形（show_func_diff orig=107 our=111 gap=-4 ndiff=12，
//   OURS-only={mov×3,jmp×1}）⇒ 回滚维持原形，判 N18b。
// [S5-2026-09-11 复攻·负结论] OURS-only 4 条逐处定位：(a)+(b) 两个 SendActionClear 调
//   用点 `mov 0x17(%eax,%ecx,1),%eax; mov %eax,%ecx` vs ORIG 直落 `…,%ecx`（4.4 目标
//   寄存器选择：ORIG 用 ecx 承接 groupIndex，4.4 用 eax 再中转）；(c) `mov …,%edx;
//   mov 0x10(%ebp),%eax; cmp %eax,%edx` vs ORIG `mov …,%eax; cmp 0x10(%ebp),%eax`
//   （enum 形参→int 载入中转）；(d) 末 then 块尾 4.4 多发 `jmp +1; nop` 空岛（ORIG
//   直接 fall-through 到循环尾 addl），源码无对应构造（无 else/无多余语句）⇒ 不可消。
//   均为 4.1↔4.4 后端分配差（N18b/N107），维持封顶。
// 语义闭合。
bool CActionPointManager::ClearAction(CUser& user, _ActionGroupIndex group,
                                      int itemIdx, ENUM_PACKETCLASS packetClass)
{
    // [b28d-solo-APSystemOnlyOrig] ORIG 0x08121dd3：bool 汇聚变量 -0xd（初值 0，
    // 成功置 1，尾部 movzbl 返回）+ 行号 -0xc；**无 groupOut 具名局部**（ORIG 每
    // 次调用点重算地址并直装 ecx）。早返 `return true` 会改块布局并多槽。
    bool result = false;
    for (int row = 0; row <= 299; ++row)
    {
        if (m_sig.m_actionPoints[row].m_groupIndex == 0)
        {
            break;
        }
        if (m_sig.m_actionPoints[row].m_groupIndex != (int)group)
        {
            continue;
        }
        if (ClearActionOne(m_sig.m_actionPoints[row], itemIdx))
        {
            result = true;
            CUserProc::SendActionClear(user, m_sig.m_actionPoints[row].m_actionNo,
                                       (_ActionGroupIndex)m_sig.m_actionPoints[row].m_groupIndex,
                                       true, packetClass);
            break;
        }
        if (packetClass == 1)
        {
            CUserProc::SendActionClear(user, m_sig.m_actionPoints[row].m_actionNo,
                                       (_ActionGroupIndex)m_sig.m_actionPoints[row].m_groupIndex,
                                       false,
                                       static_cast<ENUM_PACKETCLASS>(1));
        }
    }
    return result;
}