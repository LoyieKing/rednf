// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 UserPartyLink4/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// b28d-45 判词汇总（本 TU 3 件 + 1 NEAR）：
//   deleteGuildExpBook 0x0867e898 NEAR2：N98 pair<ENUM,int>→pair<int,int> 转换
//     临时槽序（ORIG first=-0x10/second=-0xc）；右值引用具名化反劣 117→121 已回退。
//   adjust_charac_stat 0x08664766 DIFF127 N82/N89：_Additioal_info 栈对象 +
//     234:199 大 gap（std 模板/槽复用）。
//   RewardTutorial 0x086475d8 DIFF139 N82/N93：帧 0x60/0x50 + vector<int> 槽向。
//   drop_item_gm 0x085a73a6 DIFF245 N82/N89：257:255 CheckInTrade 早退岛 + 槽序。
// 【b28e-M1S2 本波复核】两件复测读数与末次一致：
//   · deleteGuildExpBook 117/117（NEAR）：唯一残差 = make_pair((ENUM)0,0x4b0) 的
//     转换临时槽互换（ORIG 0x0867e8b6 `movl $0x0,-0x10` / 0x0867e8bd
//     `movl $0x4b0,-0xc` vs 我方 `movl $0x4b0,-0x10` / `movl $0x0,-0xc`），
//     其余全部 call 目标逐条同符号（仅显示地址）。P331/P339 核过：右值转换临时
//     （混合生命周期）⇒ 明文不可翻 ⇒ N98 封顶。语义轴：logFormat 三串与 ORIG
//     0x8cefd1c / 0x8cf28c0 / 0x8cee1dd / 0x8cefd74 一致（已核）。
//   · drop_item_gm 257/257（AE）：残差 = 4.4 令 `if(id!=0){…}` 的初始化段与拷贝段
//     相邻 ⇒ `je/jne/jg` 落点重合（我方 0xbf）而 ORIG 分开（0x1a9 / 0xc3）；
//     两落点代码逐字节相同（`movb $0x0,-0xc8(%ebp)`）⇒ 语义等价、CFG 布局差、
//     源级不可造 ⇒ N 登记。字段/调用点与 Ghidra C 逐项核对一致（0x15 次 dword
//     拷贝、drop_item(0xb24)、put_* 序列、map_item 三态输出）。

#include <string.h>

#include <utility>
#include <vector>

#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h   // CBattle_Field::drop_item(map_item) + WarField.h map_item
#include "CDataManager.h"    // G_CDataManager + m_serverParameter(+0x68) + find_item
#include "CInventory.h"      // insert_event_items / NumberToString
#include "Inven_Item.h"      // Inven_Item 布局与访问器
#include "LogManager.h"
#include "PacketGuard.h"
#include "ServerParameterScript.h"  // GetEscaladeTutorialReward

// ORIG 文件级枚举（同 CGameManager.cpp 模式；值 0 = 公会经验书 hardcode item）。
enum ENUM_HARDCODE_ITEM
{
    ENUM_HARDCODE_ITEM_0 = 0
};

#include "UserPartyLink4/RewardTutorial.inc"


#include "UserPartyLink4/adjust_charac_stat.inc"


#include "UserPartyLink4/deleteGuildExpBook.inc"


#include "UserPartyLink4/drop_item_gm.inc"

