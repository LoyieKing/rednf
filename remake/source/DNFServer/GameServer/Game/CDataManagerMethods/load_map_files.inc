// [P8S5 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
// [l5f 2026-09-11 · §6.5/P118 串盲区] ORIG 定义签名形参写作 typedef
// PVPMAP_VECTOR（ORIG .rodata 串实证：`bool CDataManager::load_map_files(
// PVPMAP_VECTOR&)`，我方旧串为 std::vector<PVPMAP_DATA, std::allocator<...> >&）。
// typedef 与底层类型同一 ⇒ mangled/布局/sizeof 零变化，仅改 __PRETTY_FUNCTION__
// 展开串（AE 口径把 .rodata 归 <A> 故此前不可见）。声明侧 CDataManager.h:3675
// 拼写不同但类型同一，无需改动。
typedef std::vector<PVPMAP_DATA> PVPMAP_VECTOR;
bool CDataManager::load_map_files(PVPMAP_VECTOR& pvpMaps)
