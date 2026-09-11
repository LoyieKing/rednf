// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 WarField/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— WarField（G2-4 战场域，PvP WarRoom 内嵌）。
// 逐函数对照 docs/class_func_reports/WarField.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径：分支/调用目标→<T>、
// 大绝对地址→<A>、去 @plt、剥尾部 nop、C2/D2 同址别名良性）。
// map_item/map_monster/MapInfo 方法（ctor/dtor/Add_Mob/Add_Item 等）定义于
// 本批次 map_item.cpp；跨类方法（CUser/CDataManager/CDungeon/WongWork 掉落链）
// 以 asm-label extern 直引 ORIG mangled 符号，避免与并行批次头文件冲突。
#include <list>
#include <map>
#include <utility>
#include <vector>
#include <cstdlib>
#include <time.h>

#include "WarField.h"
#include "CSystemTime.h"
#include "GameTypes.h"
#include "InterfacePacketBuf.h"
#include "CDataManager.h"
#include "LogManager.h"
#include "CUser.h"            // 权威 CUser / CUserCharacInfo / 共享枚举
#include "CInventory.h"       // stAvatarEmblemInfo_t / stAvatarExpansionInfo_t（含 init）
#include "CEquipItem.h"
#include "CDungeon.h"
#include "CMap.h"             // 权威 CMap / STRivalMapData（m_rivalData1=+0x90）
#include "CMonster.h"         // 权威 CMonster（m_weight=+0x1d24，ConsistMap 086b8d97 movl 直读）
#include "GlobalData.h"       // 权威 GlobalData::s_systemTime_
#include "Arad_DataManager.h" // 权威 ARAD::Singleton<T> 模板（本文件不再重复定义）
#include "ServiceRestrictManager.h" // 权威 ServiceRestrictManager（含 map 成员，Meyers 单例非平凡
                                    // → Get() 发 guard；此前局部平凡类致 Get 退化为 5 指令 stub）

#include "WarField/get_rand_int.inc"

// ============================================================================
// 本地支撑类型（名称/布局与 ORIG 一致；成员在各自批次 TU 定义）
// ============================================================================

// 掉落生成域类型（WongWork::stGenerateRefData_t/stGenerateResult_t/CItemGenerator）
// 对战地图数据 STRivalMapData 用 CMap.h 权威定义（本文件曾用局部重复定义已删，§4.4）。
// 权威声明见 CItemGenerator.h（唯一声明点，经 CDataManager.h 引入）。
// ORIG vtable 槽位（_ZTVN8WongWork14CItemGeneratorE @0x08c9a788）：
//   +0x08 generateRandomItem、+0x0c generateSpecificItem。
using WongWork::stGenerateRefData_t;
using WongWork::stGenerateResult_t;
using WongWork::CItemGenerator;

// ============================================================================
// Reset（086b8a14：MapInfo::Clear + stageIdx=0）
// ============================================================================

#include "WarField/Reset.inc"


// ============================================================================
// 构造/析构（ORIG 086c0b8e / 086c0ba4：仅 MapInfo 子对象）
// ============================================================================

#include "WarField/WarField.inc"


#include "WarField/dtor_WarField.inc"


// ============================================================================
// IsGridClear（086b954c：怪物表为空）
// ============================================================================

#include "WarField/IsGridClear.inc"


// ============================================================================
// PickupItem（086ba082：erase）
// ============================================================================

#include "WarField/PickupItem.inc"


// ============================================================================
// CheckPickupItem（086ba0a0）
// ============================================================================

#include "WarField/CheckPickupItem.inc"


// ============================================================================
// DropItem（086ba190：计数器+1，写回 item 索引，加入地图）
// ============================================================================

#include "WarField/DropItem.inc"


// ============================================================================
// GetFieldItem（086b9fe6）
// ============================================================================

#include "WarField/GetFieldItem.inc"


// ============================================================================
// GetFieldMob（086b9578）
// ============================================================================

#include "WarField/GetFieldMob.inc"


// ============================================================================
// MakePacket（086b92e2：0x1d 战场怪物信息包）
// ============================================================================

#include "WarField/MakePacket.inc"


// ============================================================================
// HandleWpPerMonster（086ba1ea：战争点数累计）
// ============================================================================

#include "WarField/HandleWpPerMonster.inc"


// ============================================================================
// ConsistMap（086b8a38：战场怪物刷出/发包）
// ============================================================================

// [b28d-solo-WarField] 语义完成、byte 层 N 封顶（539=539，p167 CLEAR 55/55）：
// ①p167 揪出真 bug——ORIG 调 const 版 at/operator[]，旧 const_cast 非 const 版（3+2+1 处）；
// ②P183 去引用缓存（每使用点重算 &m_vec44/&m_vec14[type]/&m_rivalData1）；
// ③P68 cast-this 形；④P244 形参 a 直用（删具名 b，消 movzbl+movb 双槽）；
// ⑤P196/P2 循环向 size>i 翻 setg（原 i<size 产 setl）；⑥P160-162 循环变量 i 复用；
// ⑦totalWeight 声明前移复现 movl 落点。
// 残差：ORIG 5 条死比较（m_stageIdx vs m_warRoomData.m_50，无消费者；4.4 tree-DCE 删除，
// `(void)(cond)`/具名 bool/空 if 三形实测均被删或反多 setl+store ⇒ N99 族）+ 标量区槽差 0xc
// （N82附，帧同为 0xb40）。@0x086b8a38
// [M3S1 2026-09-11 §6.5/§6.5.1 语义轴落地] 4 处发明串已换回 ORIG 实字节（EUC-KR 转义）：
//   trace 0x53  = "WarField::ConsistMap, 이미 몬스터가 존재합니다."（ORIG .rodata 0x08cf85a0）
//   logFmt 0x65 = "WARROOM::SpawnMonster : 몬스터타입확률의 총합이 0입니다"
//   logFmt 0x9f = "WARROOM::SpawnMonster : 타입(%d)의 몬스터리스트 확률의 총합이 0입니다"
//   logFmt 0xd0 = "WARROOM::SpawnMonster : 스폰위치확률의 총합이 0입니다"
//   四数 66/88/1/3 不变（地址归一，语义轴不改 verdict）。
// [待 .h·只报告] ORIG prett 0x08cf9400 = `void WarField::ConsistMap(void*, const CDungeon*,
//   const CMap*, byte, int)`，我方为 `unsigned char` ⇒ 定义签名改 `byte` 即逐字节匹配，
//   但 `byte` typedef 在本 TU 可见集中不存在（试验编译失败）⇒ 需在共享头补
//   `typedef unsigned char byte;`（或在 WarField.h 内定义），落地后本 TU 复编即可。
// [判 N] 唯一残余结构差 = ORIG 5 条死比较（086b8b19 段 `mov 0x8(%ebp),%eax;
//   mov 0xf4(%eax),%edx; mov 0x10(%ebp),%eax; mov 0x744(%eax),%eax; cmp %eax,%edx`，
//   结果丢弃）⇒ P337 形（`volatile bool` 守卫）会引入 ≥4 条守卫 + 该 5 条 ⇒ 我方
//   536→541 净劣化（gap 0 → −5），按「gap 严格改善」闸门不予落地。
// [M3S1 · 父泳道授权 TU-local 手法] `__PRETTY_FUNCTION__` 打印的是**定义处**形参拼写
//   ⇒ 本 TU 内补 `typedef unsigned char byte;`（Game 域原先无 byte，不 shadow 任何东西），
//   定义签名写 `byte a` 即复现 ORIG 串（.rodata 0x08cf9400 = "...const CMap*, byte, int)"），
//   声明侧 WarField.h 保持 `unsigned char`（底层类型/mangled/sizeof/布局全不变）。
typedef unsigned char byte;
#include "WarField/ConsistMap.inc"


// ============================================================================
// HandleMonsterKill（086b9672：掉落生成 + 击杀通知包）
// ============================================================================

#include "WarField/HandleMonsterKill.inc"


// ============================================================================
// WarAreaCounter（ORIG 0x83708c6 C1 / 0x83707a4 clear / Get* 族；类定义见
// CDataManager.h，C1/D1 位于 GameStubs.cpp）。布局（clear 实证）：
//   +0x60  int m_table[24]（时段边界，GetCurrenTimeTable 遍历）
//   +0xc0  int m_fieldC0[10]          +0xe8 int m_curIdx（当前时段行号）
//   +0xec/+0xf0/+0xf4 int             +0xf8 int m_fieldF8[10]
//   +0x120 std::map<int,stWarPoint>   +0x138/+0x13c int
//   +0x140 int m_num（时段边界数）    +0x144 int m_countTable[240]
//       （10 行 × 10 列 = 每时段 10 个 WarRoom 区的计数表，行号 = m_curIdx）
// ============================================================================

#include "WarField/GetCurrenTimeTable.inc"
{
    // b28d-lane11：ORIG 0x089024d4 `lea -0x18(%ebp),%eax; mov %eax,(%esp); call time`
    // = `time(&t)` 形；循环条件直读 m_num（无 num 局部）、t0/t1 内联无槽（P0）。
    time_t t;
    time(&t);
    struct tm tmbuf;
    struct tm* ptm = localtime_r(&t, &tmbuf);
    int curHour = ptm->tm_hour;
    int i = 0;
    for (; i < m_num - 1; ++i)
    {
        // b28d-lane11：P223 配套——单 continue 产额外 jmp 岛，改正向包裹。
        if (m_timeHourList[i] <= curHour)
        {
            if (m_timeHourList[i + 1] > curHour)
                break;
        }
    }
    return i;
}

#include "WarField/GetWarRoomCountAtPeekTime.inc"


#include "WarField/GetWarRoomCountFirstIndex.inc"


#include "WarField/GetWarRoomCountLastIndex.inc"

