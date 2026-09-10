// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 WarField/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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
using WongWork::stGenerateRefData_t;
using WongWork::stGenerateResult_t;
using WongWork::CItemGenerator;

// ============================================================================
// Reset（086b8a14：MapInfo::Clear + stageIdx=0）
// ============================================================================

#include "WarField/Reset.inc"
}

// ============================================================================
// 构造/析构（ORIG 086c0b8e / 086c0ba4：仅 MapInfo 子对象）
// ============================================================================

#include "WarField/WarField.inc"
}

#include "WarField/dtor_WarField.inc"
}

// ============================================================================
// IsGridClear（086b954c：怪物表为空）
// ============================================================================

#include "WarField/IsGridClear.inc"
}

// ============================================================================
// PickupItem（086ba082：erase）
// ============================================================================

#include "WarField/PickupItem.inc"
}

// ============================================================================
// CheckPickupItem（086ba0a0）
// ============================================================================

#include "WarField/CheckPickupItem.inc"
}

// ============================================================================
// DropItem（086ba190：计数器+1，写回 item 索引，加入地图）
// ============================================================================

#include "WarField/DropItem.inc"
}

// ============================================================================
// GetFieldItem（086b9fe6）
// ============================================================================

#include "WarField/GetFieldItem.inc"
}

// ============================================================================
// GetFieldMob（086b9578）
// ============================================================================

// [b28d-solo-WarField] NEAR 封顶（68=68、语义完整）：mob 作用域外提（块尾析构）已把返回
// 值汇聚 %ebx→%eax、并复现 ORIG「D1 后 mov $1,%eax」。残差全为槽序：ORIG it/end=-0x35c/-0x358
// （深槽）、mob=-0x354；4.4 聚合优先占低槽 ⇒ mob=-0x35c、it/end=-0x10/-0xc。属 N82附
// （4.4 聚合优先占低槽/标量分区 vs 4.1 生命周期穿插），无合法源形可择。@0x086b9578
#include "WarField/GetFieldMob.inc"
}

// ============================================================================
// MakePacket（086b92e2：0x1d 战场怪物信息包）
// ============================================================================

// [b28d-solo-WarField] AE（158=158）：P286 单 case switch 复现「形参载寄存器比较」
// （if 形 4.4 恒折 cmpl mem）+ P68 cast-this 形（void* buf + ((InterfacePacketBuf*)buf)->）
// 复现 4.1 每个成员调用「先载 this 再压常量」的实参装载序。@0x086b92e2
#include "WarField/MakePacket.inc"
}

// ============================================================================
// HandleWpPerMonster（086ba1ea：战争点数累计）
// ============================================================================

#include "WarField/HandleWpPerMonster.inc"
}

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
#include "WarField/ConsistMap.inc"
}

// ============================================================================
// HandleMonsterKill（086b9672：掉落生成 + 击杀通知包）
// ============================================================================

// [b28d-solo-WarField] 语义完成、byte 层 N 封顶（605/606，p167 CLEAR 94/94）：
// ①P235 具名字节视图（refData.m_field38/39/3c/3c_byte/3d/3e_byte/40_byte/41/43/44_byte）
// 消 29 条 lea+add（裸 *(char*)& 取址）；②P68/P0 删具名 buf 引用，内联
// ((InterfacePacketBuf*)&packet)->（ORIG 每调用点重载 0x10(%ebp)，无 buf 槽）。
// 残差：整体槽差 4（ORIG userUid=-0x40/refData=-0x110/map_item=-0x164 vs 我们 -0x3c/-0x10c/-0x160，
// 帧同为 0x1bc ⇒ N82 槽分配）；list 迭代器 end() 布局（ORIG 循环外预求值+jmp cond，4.4 旋转到
// 体尾 ⇒ N86/P164）；m_field11 逐字节取址 add $0x11（需 map_monster 匿名 union 字节视图，跨泳道 .h）；
// 2 处寄存器名（%edx vs %eax ⇒ N7 族）+ 尾部对齐 nop（§6.1 豁免）。@0x086b9672
#include "WarField/HandleMonsterKill.inc"
}

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

// ORIG 0x89024c4 T（_ZN14WarAreaCounter18GetCurrenTimeTableEv）：
// 取当前小时，在 m_table[0..m_num-2] 中查找所在时段，返回时段索引。
#include "WarField/GetCurrenTimeTable.inc"
}

// ORIG 0x82a3d80 W（_ZN14WarAreaCounter25GetWarRoomCountAtPeekTimeEi）：
// 返回当前时段（m_curIdx）第 idx 个 WarRoom 区的目标房间数；idx 越界记日志返 0。
#include "WarField/GetWarRoomCountAtPeekTime.inc"
}

// ORIG 0x82a3df4 W（_ZN14WarAreaCounter25GetWarRoomCountFirstIndexEi）：
// 当前时段前 idx 个区的计数和（供 WarRoom 扩容定位首索引）。
#include "WarField/GetWarRoomCountFirstIndex.inc"
}

// ORIG 0x82a3e90 W（_ZN14WarAreaCounter24GetWarRoomCountLastIndexEi）：
// 当前时段前 idx+1 个区的计数和（供 WarRoom 收缩定位末索引）。
#include "WarField/GetWarRoomCountLastIndex.inc"
}
