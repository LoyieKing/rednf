bool CData::saveData(CUser* pUser)
{
    // [b28c-11] NEAR 封顶（ORIG 0x832ae8c）：残余 = 虚调用 callee 寄存器
    // %edx↔%eax 互换（`mov (%eax),%edx; …call *%edx` vs `mov (%eax),%eax; …call *%eax`），
    // N7 族（4.4 间接 call 目标恒 %eax），助记符序列全等。
    bool result = _saveData(pUser);
    m_bIsSave = 0;
    return result;
