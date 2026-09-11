// ---- insertStart(int, CMap*) —— ORIG 0x0834be5a（insertStart.md 全文） ----
// 以 CMap::get_index() 为键把起始地图插入 m_maplist[idx]（hash_map @+0x64）。
// [b28d-solo] NEAR 封顶（N86/N82）时的读数：31=31 助记符序列全等、P167 CLEAR，
// pair 构造符号与 ORIG 全等（`C1IiRS2_EEOT_OT0_`）；仅三临时槽分配位互换
// （ORIG int 临时@-0xc/pair@-0x14/insert-sret@-0x20 vs 4.4 sret@-0x14/
// 临时@-0x18/pair@-0x20）。当时负结论：具名 key 形改 pair 符号为 `C1IRiRS2_E`
// （左值）不可用；static_cast<int&&> 形劣化 DIFF —— 该两点仍成立。
// [b28e 落地] NEAR→AE（31=31，助记符 hunks=0）：正解 = 把 insert 的 **sret
// 结果具名接收**（下行 `ins`）⇒ 4.4 把 sret 槽从 -0x14 重排到 ORIG 的 -0x20，
// 三临时槽序（int 临时/pair/sret）与 ORIG 全对齐，verdict 转 AE（数据地址
// 伪差口径）。注意类型名不可猜：`m_maplist` 实为 CDungeon.h:765 的
// `__gnu_cxx::hash_map<int,CMap*>`（写成 multimap 即时编不过）。
void CDungeon::insertStart(int idx, CMap* map)
// [P8S4 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
{
    std::multimap<int, CMap*>::value_type value(map->get_index(), map);
    std::pair<__gnu_cxx::hash_map<int, CMap*>::iterator, bool> ins =
        m_maplist[idx].insert(value);
    (void)ins;
}