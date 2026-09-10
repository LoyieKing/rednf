// [b28d-GrpD 判词·无实差异] ORIG 0x086813be 逐条对照：除 `.o` 未解析重定位
//   （我方 `call 7c19 <本函数+0x13>` vs ORIG `call 80da3a7 <_Z11G_GameWorldv>`）
//   与 je 目标地址文本外，助记符+操作数序**逐条一致**（norm 后两侧序列相等）
//   ⇒ 非可改项；Main 做 link 后重定位解析即应转 AE/IDENTICAL。
int CUser::get_area(bool param)
{
    if (param) {
        if (G_GameWorld()->IsSchoolPvPChannel() &&
            getCurCharacVill() == 2 &&
            m_field8cfc0 == 4) {
            return m_field8cfc0;
        }
    }
    if (param && getCurCharacVill() == 8) {
        if (m_field8cfc0 == 0 || m_field8cfc0 == 1 ||
            m_field8cfc0 == 2 || m_field8cfc0 == 3) {
            return m_field8cfc0;
        }
    }
    return m_field8cfbc;
