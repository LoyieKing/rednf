DisPatcher_AddFriend::~DisPatcher_AddFriend() {}
// ORIG 0x081cb274 T _ZN20DisPatcher_AddFriend11check_errorEP5CUserR8MSG_BASE
// （报告 DisPatcher_AddFriend/check_error.md 逐块映射）：state<=2 → 0x7fffffff；
// (msg+0x11) 指针空或与自身角色名 strcmp 相同 → 1；否则 0。
// 注：ORIG 汇编对 (msg+0x11) 地址值做 test/jne（非解引用检查）。
