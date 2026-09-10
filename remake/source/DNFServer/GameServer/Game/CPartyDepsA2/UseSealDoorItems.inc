// [b28d-round5 §6.5 串盲区] ORIG .rodata 实串 =
//   `bool CParty::UseSealDoorItems(IntVector&)`（我方原为展开
//   `std::vector<int, std::allocator<int> >&`，串长差 25 字节 + 日志契约不符）。
//   全局 typedef 与 ARAD_Database.h:31 同型（C++ 允许同型重声明），
//   仅改 prett 拼写，不改 mangled/sizeof/布局/codegen。
typedef std::vector<int> IntVector;
bool CParty::UseSealDoorItems(IntVector& itemIds)
