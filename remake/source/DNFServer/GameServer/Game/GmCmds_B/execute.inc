// ORIG execute 0x084ab558。
// b28d-21: NEAR 封顶 — 承 b26 判词：ORIG 0x084ab558 首结果→%ebx/次→%esi，
// 4.4 恒反（N7 callee-saved 角色互换）；实参求值序 R2L 由编译器固定，无源码自由度。
void CChangeCharacName::execute()
{
    CUser* user = GetUser();
    // b26 NEAR 封顶:makeRequest 两实参 getter 结果的 callee-saved 缓存寄存器
    // 分配序互换(ORIG 首结果→%ebx/次→%esi,4.4 恒反;N7 同族,无槽无局部,
    // 实参求值序 R2L 由编译器固定,无源码自由度),余逐条相等。
    DB_ReqChangeCharacName::makeRequest(user->GetUID(), user->getCurCharacNo(),
                                        m_str0.c_str(), 1);
}