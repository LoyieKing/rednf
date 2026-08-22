// df_game_r Game/ 迁移 TU —— APSystem::_MSG_ACTION_POINT_ACTION_CLEAR（2026-08-21）。
// 从 GameStubs.cpp 拆出。权威命名空间 APSystem（同名定义也在 APSystem.cpp，但该 TU
// 未入 game CMake；本 TU 独立提供该构造符号，保证 0 依赖、可链接）。
// ORIG 0x08122aaa：memset(this, 0, 0xb)。
#include <string.h>

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
} // namespace APSystem