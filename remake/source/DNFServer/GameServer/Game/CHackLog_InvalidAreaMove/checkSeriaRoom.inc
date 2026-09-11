// ORIG 0x08285e14
//   _ZN24CHackLog_InvalidAreaMove14checkSeriaRoomEii
//   直接返回 IsSeriaRoom(a, b) 的结果（eax 保留）。
// [A31-GI · P327] 前移为本 TU 首个强外部符号 ⇒ GI 键名
// `_GLOBAL__I__ZN24CHackLog_InvalidAreaMove14checkSeriaRoomEii`（ORIG 0x828619a）。
bool CHackLog_InvalidAreaMove::checkSeriaRoom(int a, int b)
{
    return IsSeriaRoom(a, b);
}