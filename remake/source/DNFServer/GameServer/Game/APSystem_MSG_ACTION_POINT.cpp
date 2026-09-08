// df_game_r Game/ 迁移 TU —— APSystem::_MSG_ACTION_POINT_ACTION_CLEAR（2026-08-21）。
// 从 GameStubs.cpp 拆出。权威命名空间 APSystem（同名定义也在 APSystem.cpp，但该 TU
// 未入 game CMake；本 TU 独立提供该构造符号，保证 0 依赖、可链接）。
// ORIG 0x08122aaa：memset(this, 0, 0xb)。
#include <string.h>
#include "APSystemTypes.h"  // CDBAndExchangeServerProc 声明（CUser/MSG_CHARAC_SELECT 仅前向引用）

namespace APSystem
{
class _MSG_ACTION_POINT_ACTION_CLEAR
{
public:
    _MSG_ACTION_POINT_ACTION_CLEAR();
};
_MSG_ACTION_POINT_ACTION_CLEAR::_MSG_ACTION_POINT_ACTION_CLEAR()
{
    memset(this, 0, 0xb);
}

// ---- CDBAndExchangeServerProc（APSystemTypes.h 声明；Prod_GlobalAdv 2026-08-24）----
// ORIG T 0x08121608 _ZN8APSystem24CDBAndExchangeServerProc33RequestLoadActionPointFromDBAndESER5CUserR17MSG_CHARAC_SELECT
//（docs/class_func_reports/APSystem__CDBAndExchangeServerProc/ 全文已读）：
// ORIG 体 13 字节 push/mov/sub + mov $0x1,%eax + leave/ret —— 忠实转写 return true。
bool CDBAndExchangeServerProc::RequestLoadActionPointFromDBAndES(CUser&, MSG_CHARAC_SELECT&)
{
    // [b28c-4] ORIG 0x08121608：sub $0x10 + mov $1,%eax ⇒ 空体带未使用局部帧。
    char framePad[0x10];
    (void)framePad;
    return true;
}
} // namespace APSystem