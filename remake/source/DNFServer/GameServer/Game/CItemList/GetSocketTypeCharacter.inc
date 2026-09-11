// ============================================================================
// 流缓冲/字符串拼接使用权威类：
//   ::CStreamBuffer（CStreamBuffer.h，ORIG 0x86269be 域，外部 TU 提供实现）
//   sync_script::CStringMaker（声明于下方 namespace sync_script 块；实现体在
//   sync_script.cpp，ORIG 0x8375f6c/0x8626cd0 域）。ORIG 本二函数全部字段
//   输出经 CStreamBuffer::operator<<(char const*/int/unsigned/float)，
//   列表拼接经 std::for_each(..., CStringMaker)。
// ============================================================================
// ORIG T 0x0861ca1b：socket 类型 → 字符（1→'1' … 0xffff→'7'，其余 0）。
// [b28d-Lane16] TU 归属修正：ORIG 中它与 static_init @0x0862690c 同 TU
//   （`orig_tu_names.tsv:107` 键名 `_GLOBAL__I__Z22GetSocketTypeCharactert`
//   = SyncScript.cpp），定义已迁至 CFieldLengthChecker.cpp；本 TU 仅引用。
int GetSocketTypeCharacter(unsigned short type);
