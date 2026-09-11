// ---- ORIG _ZN7IMethod10SetPaddingEi T @0x080b43aa（0x3b）：与 SetMode 同构（错误码
//   0x70000004 域 [0,2] 越界）。报告 docs/class_func_reports/IMethod/SetPadding.md：
//   `movzbl +0x4; xor $1; test; je` 判 m_bInit → 0x70000005；`cmpl $0 js / cmpl $2 jle`
//   有符号双检；ok 路径 `mov %edx,0x14(%eax)` 写 m_padding 后返 0x6fffffff。
int IMethod::SetPadding(int iPadding)
{
    if (!m_bInit)
        return 0x70000005;
    if (iPadding < 0 || iPadding > 2)
        return 0x70000004;
    m_padding = iPadding;
    return 0x6fffffff;
}