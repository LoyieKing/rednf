// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x086813be | _ZN5CUser8get_areaEb
// [b28d-GrpD 判词·无实差异] ORIG 0x086813be 逐条对照：除 `.o` 未解析重定位
//   （我方 `call 7c19 <本函数+0x13>` vs ORIG `call 80da3a7 <_Z11G_GameWorldv>`）
//   与 je 目标地址文本外，助记符+操作数序**逐条一致**（norm 后两侧序列相等）
//   ⇒ 非可改项；Main 做 link 后重定位解析即应转 AE/IDENTICAL。
int CUser::get_area(bool param)
{
    // [R13a01 形态] ORIG 0x086813ce 首条 `je` 落点是**条件物化的 false 汇合块**
    //   `mov $0x0,%eax`（0x08681405），而非本段之后的第二段 —— 说明原码首段是
    //   `param` 也在同一 `&&` 短路链内的**单一条件**（4.1 短路链共同落 false 块），
    //   不是「外层 if(param) 包内层 if(&&)」。写成单链即复现（我方旧形 je 落
    //   0x084715b5 = 第二段的 cmpb）。
    if (param && G_GameWorld()->IsSchoolPvPChannel() &&
        getCurCharacVill() == 2 &&
        m_trickArea == 4) {
        return m_trickArea;
    }
    if (param && getCurCharacVill() == 8) {
        if (m_trickArea == 0 || m_trickArea == 1 ||
            m_trickArea == 2 || m_trickArea == 3) {
            return m_trickArea;
        }
    }
    return m_area;
}