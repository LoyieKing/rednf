void CCharacBloodDungeon::IncPoint(int delta)       // ORIG 0x85bfed2 (W)
{
    // b28d-lane6 修复（AE）：ORIG 0x85bfee6 为「mov 0x8(%eax),%edx; mov 0xc(%ebp),%eax;
    // add %eax,%edx」= 字段 unsigned + 形参 int 的 4.4 形态（P211 类型旋钮）。
    // 旧 b26「N18a 反向封顶」判词作废：探针 /tmp/l6p/p1.cpp B 形实证。
    charac_expand::CData::alter();
    m_point += delta;
