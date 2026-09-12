// ORIG D1 0x84922d6 W：仅对 +0x1fd20 SIG_LOAD_ETC 调用一次析构；其余七个
//   子对象/POD 区无用户析构 ⇒ 编译器析构序一致。
//
// [SemA06 2026-09-12 · P299/P318 CWG434] **本定义已停用**（与 C1 件同因）：
//   用户声明的析构同属 user-provided ⇒ 与 C1 一并删除，改由编译器隐式合成
//   （合成版 D1/D2 体与 ORIG 同为 0x18 字节、W 绑定）。
//
// CACHE_CHARACTER_TYPE::~CACHE_CHARACTER_TYPE()
// {
// }
