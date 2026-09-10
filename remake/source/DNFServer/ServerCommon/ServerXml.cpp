// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 ServerXml/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// Rebuilt from df_auction_r (DWARF + Ghidra decompile), 2026-08-08
#include "ServerXml.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "AuctionItem.h"
#include "DNFFunctionLib.h"

namespace np_server_xml
{

// ORIG：命名空间内静态 FILE_NAME（_ZN13np_server_xmlL9FILE_NAMEE），
// StrLoading 与 Load Fail 日志引用该符号。
static const char FILE_NAME[] = "server_str.xml";

}  // namespace np_server_xml

// [T1-GI · P327] 键名位：ORIG `_GLOBAL__I_g_ServerString_`（0x80c6a95）⇒ ORIG 源码中本对象
// 定义先于本 TU 所有函数定义（P327 选名 = 首个「强符号 · 外部链接 · 本 TU 定义」实体；
// 非限定全局对象 ⇒ 裸名），故对象定义前移至此。
// 本件属「带动态初始化对象」（ORIG 0x80c6a2c / 我方 static_init 均调 CServerXml C1 +
// __cxa_atexit），默认受 P327 移动禁令；此处经实测豁免：改前/改后
// `__static_initialization_and_destruction_0` 体**逐字节相同**（ioinit 由行 6 <iostream> 定序，
// 恒在其前；ORIG 亦为 ioinit → CServerXml 同序）、.text/.data/.bss/.ctors 段字节全同、
// 符号集合仅 GI 名一项变化 ⇒ 构造序与可观察行为零变化。
// 实测：键名 `_GLOBAL__I_g_ServerString_` == ORIG；四数 68/88/4/1 → 69/88/4/1（missing 45→44）。
np_server_xml::CServerXml g_ServerString_;

namespace np_server_xml
{

#include "ServerXml/CServerXml.inc"
}

#include "ServerXml/dtor_CServerXml.inc"
}

#include "ServerXml/InitString.inc"
}

#include "ServerXml/StrLoading.inc"
}

#include "ServerXml/StrLoading_1.inc"
}

#include "ServerXml/CharsetInit.inc"
}

#include "ServerXml/EventLoad.inc"
}

#include "ServerXml/RGBALoad.inc"
}

#include "ServerXml/ProcessLoad.inc"
}

#include "ServerXml/StrPunish.inc"
}

#include "ServerXml/GetServerString.inc"
}

#include "ServerXml/GetEventRGBA.inc"
}

#include "ServerXml/GetEventString.inc"
}

} // namespace np_server_xml
