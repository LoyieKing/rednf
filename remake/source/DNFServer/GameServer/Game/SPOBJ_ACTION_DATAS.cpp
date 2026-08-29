// df_game_r SPOBJ_ACTION_DATAS 成员定义（ORIG 符号均为 W，头内联；本 TU 外联）。
// ORIG 证据（init/df_game_r）：
//   C1 0x089e86e4: memset(this, 0xff, 0x18) —— movl $0x18,0x8(%esp);
//                  movl $0xffffffff,0x4(%esp); 全字段初始化为 -1。
//   D1 0x089f0b92: push ebp/mov esp,ebp/pop ebp/ret（空体）。
//   getActionType 0x089e8708: mov 0x8(%ebp),%eax; mov (%eax),%eax; ret ——
//                  非 const 成员、返回 int，直返 m_type（+0x00）。
#include <cstring>
#include "SPOBJ_ACTION_DATAS.h"

SPOBJ_ACTION_DATAS::SPOBJ_ACTION_DATAS()
{
    memset((char*)this, -1, 0x18);
}

SPOBJ_ACTION_DATAS::~SPOBJ_ACTION_DATAS()
{
}

int SPOBJ_ACTION_DATAS::getActionType()
{
    return m_type;
}
