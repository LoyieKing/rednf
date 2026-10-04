// ---------------------------------------------------------------------------
// 22) CUser::alonePlay_Mode(MSG_REQ_TO_PEER const&)
//     报告：alonePlay_Mode_0868dcbc.md。ORIG 0x0868dcbc。
// ---------------------------------------------------------------------------
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0868dcbc：N 封顶——同 alonePlay_Mode()：ORIG
// `movswl -0xa(%ebp),%eax; cmp $1,%eax` vs 4.4 `cmpw $1,mem`（short 比较 lowering 跨版本差）。
bool CUser::alonePlay_Mode(const MSG_REQ_TO_PEER& req)
{
    // ORIG 0x0868dcbc 序列：opt(-0x10)→get_etc_option→short 结果存 -0xa（mov %ax）
    // →再读 req+0xf（movzbl 0xf(%eax);movsbl 双扩展，MSG_REQ_TO_PEER 无布局符号，
    // §4.3 例外视图，同 get_SoloPlay_msgType 模板）→`(unsigned)>3 return false`
    // →`movswl -0xa; cmp $1` 具名装载比较（P77，直用恒融 cmpw mem）。
    // const 标量被 4.4 单点使用内联下移（call 落入尾块）；非 const 才保 ORIG
    // 0x0868dcd0-0x0868dce3 前置声明序物化槽 -0x10/-0xa。
    CGameOption* opt = GetGameOptionRef();
    short etc = opt->get_etc_option((ENUM_OPTION_ETC)0x1b);
    // P1 组件视图把 `add $0xf; movzbl (%eax)` 折叠为 ORIG 0x0868dcea 单条
    // `movzbl 0xf(%eax),%eax`（MSG_REQ_TO_PEER 无布局符号，§4.3 例外具名视图）。
    // 免具名 reqType 槽（ORIG `add $0,%eax` 即 4.1 无槽物化；槽版挤占 opt/-0xa）。
    // ORIG 0x0868dcea-0x0868dd00：`add $0x0,%eax; cmp $0x3,%eax; jbe` 后随
    // mov$0 false 岛 = switch 连续 0..3 区间 lower-bound 减法形（4.1 range
    // lowering）⇒ 用 switch 全 case break + default return false 复现。
    // [b28d-solo-CUserOnlyOrig P279/P0] 具名 t 局部生 -0x10 槽 + 回读（ORIG 无槽）
    // ⇒ switch 直接吃表达式（call/load 结果寄存器直比）。
    switch (static_cast<signed char>(
        req.m_reqType)) {
    case 0: case 1: case 2: case 3:
        break;
    default:
        return false;
    }
    // ORIG 0x0868dd01 `movswl -0xa(%ebp),%eax; cmp $1,%eax`（4.1 32 位化）；
    // 删具名 v 槽后 opt/etc 槽位与 ORIG 一致（-0x10/-0xa）。
    // [Lane6 2026-09-10 P286 二试] 同 21 号：`if (etc == 1)` 4.4 恒融 `cmpw $1,mem`，
    // 单 case switch 复现「载入 + 寄存器比较」（21 号实证 DIFF→AE）。
    switch (etc) {
    case 1:
        return true;
    default:
        return false;
    }
}