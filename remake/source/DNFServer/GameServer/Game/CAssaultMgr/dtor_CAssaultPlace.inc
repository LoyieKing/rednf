CAssaultPlace::~CAssaultPlace()
{
    // b28d-solo-CAssaultMgr: AE（原 NEAR 2，旧 N-帧判词作废）——P210 三分律②：
    // 把 CAssaulter::~CAssaulter 由头内 inline 空体改为本 TU 外联定义并置于本
    // caller 之前（CAssaultMgr.cpp:189），4.4 对 assaulters[8] 析构循环由保守
    // sub $0x14 翻为精确继承 ORIG 0x82f13b6 的 sub $0x4，逐条 AE。
    // [b28d-PoolSmall 复验+反例] 本 TU 内把本 dtor 定义后置到 CloseAssault 之后
    // （P210 三分律③方向）实测**零效**：StaticPool<CAssaultPlace,128>::CNodeD1
    // 仍 `sub $0x4`（ORIG 0x82f1389 为 `sub $0x18`）——gcc4.4 在 finish_file 统一
    // 输出 cgraph，COMDAT 发射点不受源码顺序影响 ⇒ 该件残差纯为 N115（4.4 -O0
    // 恒预留对齐 outgoing 区，4.1 参数直写帧底），唯一解是 TU 级
    // `-mpreferred-stack-boundary=2`（会塌陷同 TU 其它帧）⇒ 已回滚位置，封 N115。
    // [round5-A29 + E02DungeonAltarMgr 双实验定论，2026-09-10] E02 另从「让 callee
    // 不再是可精确继承的简单符号」方向攻同一件：给 StaticPool.h 的 `CNode` 加用户空
    // 析构 `~CNode(){}`（+ 预留死局部撑帧），fresh 实测（19:37:08）**仍 `sub $0x4`**、
    // TU 四数逐值不变，E02 已回退该头。⇒ 两条路（源码顺序后置 / 加用户 dtor）全部
    // 证伪：4.4 的帧继承只认「callee 定义对该 COMDAT 实例化点是否可见」，而本 TU 内
    // 该定义恒可见（`T::~T` 外联定义与模板实例同 TU）⇒ 该件封 N115/P210②，
    // 后续泳道勿再为此烧轮次（除非把 `CAssaultPlace::~CAssaultPlace` 定义迁出本 TU，
    // 那属跨 TU 迁定义、需 Main 裁决且收益仅 1 件）。
    // ORIG 侧比对：`CAssaultPlaceD1Ev` @0x082f13b6 size 0x31（**sub $0x4**，与本 TU
    // 一致、本函数 AE）vs `StaticPool<CAssaultPlace,128>::CNodeD1Ev` @0x082f13e8
    // size 0x13（**sub $0x18**，保守）——ORIG 自身两件帧不同，正是「发射点可见性」
    // 差异的化石证据（ORIG 的 CNode 实例由看不到 `~CAssaultPlace` 定义的 TU 发射）。
}