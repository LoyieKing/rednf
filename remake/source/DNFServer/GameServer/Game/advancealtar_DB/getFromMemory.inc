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
