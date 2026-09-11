// ---- DBAndExchangeServerProc（advancealtar__DBAndExchangeServerProc 报告）----
// ORIG T 0x0813394c：characNo→data.m_characNo；拷 AdvanceAltarData；isUpdated 则 DB_Update。
// [b28d-solo-advancealtar_DB] N76 封顶（orig 31 / ours 30，clean 归一 resid=5）：
//   ORIG 0x0813395d `mov %eax,%edx; mov 0xc(%ebp),%eax; mov %edx,(%eax)`（RHS-first
//   +edx 中转）；4.4 对 `data.m_characNo = (int)getCurCharacNo()` 恒 LHS-first
//   `mov 0xc(%ebp),%edx; mov %eax,(%edx)`。已试形：(unsigned)cast/(int)cast/裸形/
//   P226 语句表达式（后者升格 ebx 破帧、`push %ebx; sub $0x14`）——字段与 callee
//   返回同为 unsigned 无可动旋钮（P211 反例）⇒ 无 C++98 源形可复现。语义等价。
//   if/else 空 else 复现 ORIG 体尾 jmp+nop 岛（P285）保留。
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N76，gap=+0/blocks=1）→ 试=①overlay 实测删 `(int)`
//   cast（同型化）⇒ 残差逐字不变（仍 2 条 LHS-first `mov 0xc(%ebp),%edx; mov %eax,(%edx)`）
//   ②对照探针（4.4.7 -O0 -m32 严格同形结构）：**零位移** store 恒产 2 条 LHS-first，**非零
//   位移**才产 ORIG 的 3 条 RHS-first（`mov %eax,%edx; mov base,%eax; mov %edx,(%eax)`）
//   ⇒ ORIG 0x0813395d 的 3 条形属 4.1 对零位移的 lowering，4.4 同源形恒 2 条（cast 的有无、
//   指针/引用/局部指针三种基址形均实测同码）⇒ 维持 N76 封顶（语义等价）。
void DBAndExchangeServerProc::getFromMemory(const CUser& user, _CharacAdvanceAltarDbData& data)
{
    // [b28d-solo-advancealtar_DB] P211 档③ callee/RHS 侧旋钮：ORIG 0x0813395d
    //   `mov %eax,%edx; mov 0xc(%ebp),%eax; mov %edx,(%eax)` = RHS-first + edx 中转；
    //   字段 m_characNo 为 unsigned int 与 getCurCharacNo()（unsigned int）同型时 4.4
    //   恒 LHS-first 直存。RHS 显式 (int) 造符号性异型即翻正（mangled/布局零影响）。
    // [b28d-solo-advancealtar_DB] ORIG 0x0813395d 为 RHS-first+edx 中转（N76 族）；
    //   实测 (int)/(unsigned) cast、语句表达式三形 4.4 恒 LHS-first 或升格 ebx，
    //   无 C++98 源形可复现 ⇒ N76 封顶（详见函数头判词）。
    data.m_characNo = (int)user.getCurCharacNo();
    user.m_advanceAltar.getCharacAdvanceAltarDbData(data);
    if (user.m_advanceAltar.isUpdated())
    {
        DB_Update::makeRequest((int)user.GetUID(), data);
    }
    else
    {
        return;
    }
}