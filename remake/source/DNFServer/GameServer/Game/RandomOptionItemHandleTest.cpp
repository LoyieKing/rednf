// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 RandomOptionItemHandleTest/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// LinkFinal2 核销（/tmp/undef_r26.txt）—— random_option::CRandomOptionItemHandleTest
// 声明：CRandomOptionItemHandleTest.h。ORIG nm -S 核实：
//   C1/C2                  W 0x0822cb70 (0x15)：+0x00/+0x04 两 dword 清零
//   test_regist_auction(CUser*) W 0x085f3f94 (0x5)：纯 ret（ORIG 即空体）
//   test_drop              W 0x085f3cb8 (0x5)：纯 ret
//   test_unseal            W 0x085f3cbe (0x5)：纯 ret
//   test_change            W 0x085f3f9a (0x5)：纯 ret
//   test_reset             W 0x085f3fa0 (0x5)：纯 ret
//（_get_auction_id 0x085f3cc4 / _regist_auction 0x085f3dba 不在本批清单，未转写。）
// ============================================================================

#include "CRandomOptionItemHandleTest.h"

#include "CItemAmplifier.h"   // stAmplifyOption_t（Inven_Item::m_amp 完整类型）
#include "DBMgr.h"           // DBMgr::GetDBHandle / MySQL（含 GlobalData::s_db_mgr）
#include "Inven_Item.h"      // Inven_Item 完整布局（_regist_auction 逐字段读取）

namespace random_option
{

// ORIG C1 0x0822cb70：mov this 一次缓存后连发两 store（P29 初始化列表形态）。
#include "RandomOptionItemHandleTest/CRandomOptionItemHandleTest.inc"
}

#include "RandomOptionItemHandleTest/test_regist_auction.inc"

#include "RandomOptionItemHandleTest/test_drop.inc"
#include "RandomOptionItemHandleTest/test_unseal.inc"
#include "RandomOptionItemHandleTest/test_change.inc"
#include "RandomOptionItemHandleTest/test_reset.inc"
#include "RandomOptionItemHandleTest/_get_auction_id.inc"
}

// ============================================================================
// CRandomOptionItemHandleTest::_regist_auction —— ORIG T 0x085f3dba
// （OrigItemGen20 批；报告 random_option__CRandomOptionItemHandleTest/_regist_auction.md）。
//   测试辅助：向 auction_main 表插入一条拍卖记录。blob_to_str 编码
//   item+0x25（random_option 段 0xe 字节）/ item+0x15（amplify 段 10 字节）；
//   23 个 SQL 变参按 ORIG 压栈序（0x8(%esp) 首参 → 0x64 尾参）照抄。
// ============================================================================
// b28d-31: N 封顶（N82）——ORIG 0x085f3dba；123 vs 126（帧/槽 + 变参装载）。
#include "RandomOptionItemHandleTest/_regist_auction.inc"
}

}  // namespace random_option
