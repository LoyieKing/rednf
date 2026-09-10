// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CItemDictionary/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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
#include "CItemDictionary/CItemDictionary.inc"
#include "CItemDictionary/dtor_CItemDictionary.inc"
#include "CItemDictionary/_reset.inc"
#include "CItemDictionary/_saveData.inc"
}
// ORIG 0x0811d7e0：memcpy(out+4, this+5, 0x1388)，无空检查。
// ORIG out 参数先物化到局部（-0xc）再回读 +4 ⇒ 源码经局部指针。
#include "CItemDictionary/getData.inc"
}
#include "CItemDictionary/getGainItemCount.inc"
}
// ORIG 0x0811dba6（ret $0x4 克隆；逐条转写）：遍历位图（+0x5 起），置位处
// itemIdx=i*8+bit 经 G_CDataManager()->m_mapA940（+0xa940）翻译后
// push_back(map[itemIdx])；map 无此键则跳过。
// ORIG 为按值返回（ret $0x4 NRV 隐藏参数 @ ebp+8、this @ ebp+0xc；引用返回
// 少 NRV 参数、直接返回 &成员，符号同 mangled 但语义不同），故用局部对象。
#include "CItemDictionary/getGainItemList.inc"
}
// ORIG 0x0811da32：index=flag>>3、bit=flag&7 越界（index∉[0,0x1387] 或 bit∉[0,7]）
// → logFormat 后返回 1；否则返回 (m_data[index]>>bit)&1。item 参数仅入日志。
#include "CItemDictionary/getItemCheckFlag.inc"
}

// ORIG 0x0811db60：最后一个非零字节下标+1（无非零字节返回 0）。
#include "CItemDictionary/getLengthBinary.inc"
}
// ORIG 0x0811d79a：先经虚表槽7 调 _reset()，再 memcpy(data+4 → this+5, 0x1388)，返回 true。
#include "CItemDictionary/loadData.inc"
}
// ORIG 0x0811dce6：向用户发送单项词典标记（接口包 0x181：byte=1 + int item）。
#include "CItemDictionary/sendItemDictionaryInfo.inc"
}
#include "CItemDictionary/sendItemDictionaryInfoTotal.inc"
}
// ORIG 0x0811d80e：user==0 返回；item 存在（find_item）且 is_stackable → 返回
// （仅"存在且非堆叠"继续）；m_mapA928.find(item) 命中则
// setItemCheckFlag(user, item, map[item])。
#include "CItemDictionary/setGainItemFlag.inc"
}
// ORIG 0x0811d9d7：user==0 直接返回；index=flag>>3、bit=flag&7 越界（
// index∉[0,0x1387] 或 bit∉[0,7]）→ logFormat 后返回；getItemCheckFlag(-1,flag)
// 已置位则跳过；否则 m_data[index] += (1<<bit)（加法非 or），
// sendItemDictionaryInfo + CData::alter()。
#include "CItemDictionary/setItemCheckFlag.inc"
}
