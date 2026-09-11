// ============================================================================
// importCharacterCreateCreatureGiftScript —— ORIG T 0x0896001a
//   bool (CharacterCreateCreatureGiftScript*, const char*)
// ORIG 双相派发（与 importTitleBookEtcScript/importAttendanceScript 同构）：
//   函数级静态 __lookup(map<const char*,int,less_TCHAR>)/__init_lock(SpinLock)/
//   __inited(bool) 三件套位于外层 token 循环体内（每轮穿过 guard 测试，
// ORIG +0x8e/+0x108），注册完成 SpinLock::leave + __inited=1 后 jmp +0x8e 以
// 当前令牌重查表派发。case 贯穿链按 [event job]→[start]→[end]→[/event job]→
// [change able creature lev]→[change infinity creature]→[client infinity
// creature] 顺序 insert(0x277/0x27d/0x281/0x285/0x28c/0x290/0x2a2)。
// 收敛口径：orig=ours=487，助记符序列全等、栈槽逐条一致；残余差异仅为
// .o 未链接的 guard/静态对象/串字面量重定位显示（.rodata 内容与 5 个局部
// 静态符号名 _ZZ…E8__lookup/11__init_lock/8__inited+双 guard 实测与 ORIG
// 一致），链接后按 AE 口径判等。
// ============================================================================
// b28d-solo-InitScriptProducers_6: IDENTICAL（482=482）— 清单标注 NEAR2 为旧读数；
// 本波 fresh 复验已闭合（P164 EOF 落点形）。ORIG T 0x0896001a。
bool importCharacterCreateCreatureGiftScript(CharacterCreateCreatureGiftScript* s,
