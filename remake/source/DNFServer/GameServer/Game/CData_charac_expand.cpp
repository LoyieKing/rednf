// df_game_r Game/ charac_expand::CData 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 声明以 CBoosterGage.h 为准。
#include "CBoosterGage.h"

namespace charac_expand
{

CData::CData()
{
    // ORIG W 0x80c8cba：仅置基类 vptr（编译器自动）；不触碰 m_bIsSave。
}

CData::~CData()
{
}

void CData::alter()
{
    m_bIsSave = 1;
}

// ORIG _ZN13charac_expand5CData6isSaveEv W 0x832ae80：`movzbl 0x4(%eax); ret` 直返
// m_bIsSave 字节（无 test;setne 布尔归一）。bool 字段 + bool 返回时 4.4 同样直返
// （探针 /tmp/l11p/issave.cpp：bool 字段 isSave 体 = movzbl;ret，调用点 = test %al;je；
// char 返回则调用点物化 setne——P73）。签名 mangled 不含返回类型。
bool CData::isSave()
{
    return m_bIsSave;
}

bool CData::saveData(CUser* pUser)
{
    // [b28c-11] NEAR 封顶（ORIG 0x832ae8c）：残余 = 虚调用 callee 寄存器
    // %edx↔%eax 互换（`mov (%eax),%edx; …call *%edx` vs `mov (%eax),%eax; …call *%eax`），
    // N7 族（4.4 间接 call 目标恒 %eax），助记符序列全等。
    bool result = _saveData(pUser);
    m_bIsSave = 0;
    return result;
}

void CData::reset()
{
    // [b28c-11] NEAR 封顶（ORIG 0x832aebc）：同 saveData（N7 虚调用寄存器互换）。
    m_bIsSave = 0;
    _reset();
}

}  // namespace charac_expand
