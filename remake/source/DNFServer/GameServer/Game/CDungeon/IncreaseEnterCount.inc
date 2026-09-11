// ORIG T 0x0864388a（报告 TOD_UserState/IncreaseEnterCount.md）：enterCount(+0x10)
// 与 lastEnterLayer(+0x16) 各自 ++、lastEnterTime(+0x18)=getCurSec；当日层槽
// (+0x14) 按 ORIG 是内嵌 TOD_Layer——判 0 走真实 call TOD_Layer::GetLayer
// (W 0x0822ec40, this+0x14)，非字段直读。（Sec 批：TOD_UserState +0x8 段已内嵌
// SIG_REQUEST_TOD_USER_STATE，m_todayLayer 本体即 TOD_Layer，直调免 cast。）
// [b22判词·跨版本] 残余 1 insn：ORIG 4.1 对 ushort 回写生成 `add $0x1,%eax;
// mov %eax,%edx`，4.4 恒融合为 `lea 0x1(%eax),%edx`（三变体实测均 lea，见
// 范式库跨版本豁免表 movzwl 段）；`movl $0x0,(%esp)` 系 .o R_386_32 数据地址
// reloc 占位（§6.3），relink 后即 &GlobalData::s_systemTime_。语义等价。
// [b28c-lane17] fresh 复验确认：字段已 unsigned short（movzwl 宽度/偏移全对），
// 唯一残差即该 lea-vs-add+mov 折叠 ⇒ N18a（单段 regmove 不可造）封顶。
void TOD_UserState::IncreaseEnterCount()
{
    // [b28d-solo-CDungeon 判词 · N4/P30 封顶] ORIG 0x0864388a / our 34（ORIG 35，
    // gap 1）；p167 4=4 CLEAR。残余唯一：ORIG `movzwl 0x16; add $0x1,%eax;
    // mov %eax,%edx` vs 4.4 `lea 0x1(%eax),%edx`（add-vs-lea 融合）。本轮再试
    // `++` / `+= 1` / `(unsigned short)(x+1)` 三形全产 lea，与 q1-q6 六形探针
    //（f+1 / +=1 / 1+f / (ushort)cast / register int / register ushort）一致 ⇒ 封顶。
    // [b28d-solo-CDungeon2 复验] fresh 编译仍唯一残差；T1 直证 movzwl/mov %dx ⇒ 宽度
    // 确定（unsigned short），残差仅 add-vs-lea ⇒ 按 P321-T5 禁改类型，N18a 封顶。
    // [L6S2 复验] 真实旗标 4.4.7 下 12 形探针（/tmp/L6S2_probe/pr_incr.cpp f1..fc：
    // `+1` / `+=1` / `++` / `(ushort)(x+1)` / `(ushort)(x+1u)` / 具名 uint 中转 /
    // 具名 ushort / register ushort / `1+x` / 具名 int 中转 / static_cast 双段 /
    // `(ushort)((unsigned)x + 1u)`）全部产 `lea 0x1(%eax),%edx` ⇒ ORIG
    // `add $0x1,%eax; mov %eax,%edx` 两段（4.1 独有）不可复现，封顶维持。
    // p167 4=4 CLEAR（C1/C2 别名已核）。
    m_data.m_enterCount = m_data.m_enterCount + 1;
    m_data.m_lastEnterLayer = m_data.m_lastEnterLayer + 1;
    m_data.m_lastEnterTime = GlobalData::s_systemTime_.getCurSec();
    if (m_data.m_todayLayer.GetLayer() == 0)
        m_data.m_firstEnterTime = GlobalData::s_systemTime_.getCurSec();
    ((charac_expand::CData*)this)->alter();
}