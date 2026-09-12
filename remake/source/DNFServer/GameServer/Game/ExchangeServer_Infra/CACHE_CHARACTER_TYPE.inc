// ORIG C1 0x8492262 W（objdump 逐指令转写）：sub $0x18 帧内依次七次子对象构造
//   +0x00000 SIG_LOAD_CHARAC / +0x00703 SIG_LOAD_INVENTORY / +0x15b4f SIG_LOAD_QUEST /
//   +0x1eb13 APSystem::_SIG_LOAD_ACTION_POINT / +0x1f804 advancealtar::_CharacAdvanceAltarDbData /
//   +0x1fd20 SIG_LOAD_ETC / +0x34884 SIG_LOAD_KILL_MONSTER_INFO——与头中成员
//   类型化一一对应，编译器按序自动调用。
//
// [SemA06 2026-09-12 · P299/P318 CWG434] **本定义已停用**（同 dtor 件）：
//   用户声明的 ctor 会抑制 value-initialization 的前置零初始化（CWG434），
//   而 ORIG 的 C1 是**隐式合成**的（W 弱符号、无用户 ctor）⇒ 删除
//   CacheCharacterType.h 里的用户声明、改由编译器合成，使
//   `StaticPool<CACHE_CHARACTER_TYPE,1000>::CNode::CNode() : T()` 的 T() 成为
//   真正的 value-init，恢复 ORIG CNodeC1 0x8495dba 的
//   `memset(this,0,0x3f85c)` → `call CACHE_CHARACTER_TYPE::C1` 序。
//   本体内提供的“体内容”为零（空体，全部指令由编译器按成员布局生成），
//   故删除无信息损失；合成版 C1/C2 体与 ORIG 同为 0x73 字节。
//
// CACHE_CHARACTER_TYPE::CACHE_CHARACTER_TYPE()
// {
// }
