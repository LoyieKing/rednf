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
// m_bIsSave 字节（无 test;setne 布尔归一）→ 返回类型实为 char（原 bool 引入
// test;setne 2 insn 差）。改 char 后 IDENTICAL；调用点 CDataMgr::update
// `test %al;je` 按字节判非零，ABI 兼容。签名 mangled 不含返回类型。
char CData::isSave()
{
    return m_bIsSave;
}

bool CData::saveData(CUser* pUser)
{
    bool result = _saveData(pUser);
    m_bIsSave = 0;
    return result;
}

void CData::reset()
{
    m_bIsSave = 0;
    _reset();
}

}  // namespace charac_expand
