// df_game_r 还原 —— XNuclear::CHades（Hades 反外挂客户端成员类，内嵌于 CUser +0x8e954）。
// 参照 docs/class_func_reports/XNuclear__CHades/。
// Send_ReturnToVillage @ ORIG 0x084b9386：纯空函数（push ebp / mov esp,ebp / pop ebp / ret，
// 不访问任何成员、无任何调用），符号 _ZN8XNuclear6CHades20Send_ReturnToVillageEv。
#include "CUser.h"

namespace XNuclear
{

void CHades::Send_ReturnToVillage()
{
    // ORIG 0x084b9386：函数体为空（Ghidra: void Send_ReturnToVillage(void){ return; }）
}

}  // namespace XNuclear
