// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CItemDictionary/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CItemDictionary.h"
#include <cstring>

#include "CUser.h"   // 经 SigTypes.h 提供权威 SIG_ITEM_DICTIONARY_FLAG（0x138c）
#include "CDataManager.h"  // G_CDataManager / find_item / m_mapA928/m_mapA940
#include "CItem.h"         // CItem::is_stackable
#include "CStreamGuard.h"
#include "GlobalData.h"
#include "LogManager.h"
#include "MsgQueueMgr.h"
#include "PacketGuard.h"
#include "StreamPool.h"

// ORIG 少量 TU 各一份 _ZL18kItemDictionaryBit（r 0x1）：字节位图每字节位数。
static const unsigned char kItemDictionaryBit = 8;

namespace {
// ORIG 无对应数据符号（nm 实证）；用 enum 常量避免发射仅-NEW 的
// (anonymous namespace)::kBinaryLength 只读符号（P154）。
enum { kBinaryLength = 5000 };
}

// ORIG 0x0811d5d4：仅 CDataC1（置基类 vptr）+ 编译器置派生 vptr，不调用 _reset。
CItemDictionary::CItemDictionary() {}
CItemDictionary::~CItemDictionary() {}
// ORIG 0x0811d774：memset(this+5,0,0x1388)；不触碰 m_gainItemList。
void CItemDictionary::_reset() { std::memset(m_data, 0, kBinaryLength); }
#include "CItemDictionary/_saveData.inc"
#include "CItemDictionary/getData.inc"
#include "CItemDictionary/getGainItemCount.inc"
#include "CItemDictionary/getGainItemList.inc"
#include "CItemDictionary/getItemCheckFlag.inc"

#include "CItemDictionary/getLengthBinary.inc"
#include "CItemDictionary/loadData.inc"
#include "CItemDictionary/sendItemDictionaryInfo.inc"
#include "CItemDictionary/sendItemDictionaryInfoTotal.inc"
#include "CItemDictionary/setGainItemFlag.inc"
#include "CItemDictionary/setItemCheckFlag.inc"
