// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 RestrictGeolocation/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— RestrictGeolocation 实现（原 GlobalData.cpp 局部镜像收敛，
// Main 批准：W6_ARAD；声明=权威 RestrictGeolocation.h）。
//
// ORIG 证据：docs/class_func_reports/RestrictGeolocation/*.md（地址见头文件注释）。
// [判词·已闭合] _GLOBAL__I__ZN19RestrictGeolocationC2Ev @0x0817113b：ORIG 8 条 /
// 我方 8 条，原唯一差 = sub $0x18 vs $0x8（NEAR(2)）。根因：ORIG __static_init
// @0x081710fb(0x40B) 含 std::ios_base::Init 全局对象（_ZStL8__ioinit @0x9404752，
// 1B local bss；C1 @0x86da1e0 + __cxa_atexit(func=Init::D1 @0x86da140, arg=ioinit,
// dso=__dso_handle @0x8ae5848)）。旧注释把 0x8ae5848 误认作全局 std::string，实为
// __dso_handle（该地址 nm 实证 R __dso_handle）。本 TU 原缺 `<iostream>` ⇒
// __static_init 为空壳、4.4 的 GI 按 callee 帧塌缩为 0x8（P210 帧继承）。
// 修法：补 `#include <iostream>` ⇒ 4.4 生成同形 0x40B __static_init（sub $0x18 +
// ioinit C1 + __cxa_atexit），GI 继承 0x18，8/8 逐条 identical（near 1→0，四数
// 60/113/1/3 → 60/114/0/3）。新增符号 _ZStL8__ioinit / ios_base::Init C1/D1 /
// __cxa_atexit / __dso_handle 全部为 ORIG 实有（无发明符号）。全库对照：ORIG 509
// 个 GI 体中 483 个为 sub $0x18，即 ORIG 侧绝大多数 TU 都带 ioinit 链。
// 工程未链 libGeoIP，GeoIP_* 经 dlsym 动态查找【推断，保持原重建口径】。
// ============================================================================

#include "RestrictGeolocation.h"

#include "GlobalData.h"        // s_stream_pool / s_msgq_mgr
#include "StreamPool.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "SigTypes.h"          // SIG_SAVE_GEO_REJECT
#include "GameTypes.h"         // cMyTrace
#include "LogManager.h"

#include <cstring>
// ORIG __static_init @0x081710fb(0x40B) 含 std::ios_base::Init 全局对象
// （_ZStL8__ioinit @0x9404752，C1 + __cxa_atexit dtor 0x86da140）⇒ ORIG 本 TU
// 由 <iostream> 链引入 ioinit。补该 include 复现 __static_init 与 _GLOBAL__I 帧形。
#include <iostream>

// Taiwan 域 internal_stream（TaiwanQueries.h 权威声明；包类型枚举见该头）。
#include "TaiwanInternal.h"

extern CEnvironment* G_CEnvironment();

// libGeoIP C API（ORIG `nm -u df_game_r` 权威：U GeoIP_new / U GeoIP_delete /
// U GeoIP_country_code_by_addr，调用点为 @plt：closeGeolocation 0x08170a96 →
// GeoIP_delete@plt、initGeolocation 0x08170a5c → GeoIP_new@plt、
// getCountryCode 0x08170fb0/0x08170fd5 → GeoIP_country_code_by_addr@plt）。
// 工具链无 GeoIP.h，按第三方 C 库惯例手写 extern "C" 声明（同 Dispatchers.cpp
// uncompress、APSystemOnlyOrig.cpp compress2 口径；非 §4.5 asm 桥）。
extern "C" {
void* GeoIP_new(int db_type);                       // GEOIP_STANDARD = 0
#include "RestrictGeolocation/GeoIP_delete.inc"
}

#include "RestrictGeolocation/RestrictGeolocation.inc"
}

#include "RestrictGeolocation/dtor_RestrictGeolocation.inc"
}

#include "RestrictGeolocation/initGeolocation.inc"
}

#include "RestrictGeolocation/closeGeolocation.inc"
}

#include "RestrictGeolocation/clearAllowCountry.inc"
}

#include "RestrictGeolocation/addAllowCountry.inc"
}

#include "RestrictGeolocation/isAllowCountryCode.inc"
}

#include "RestrictGeolocation/loadData.inc"
}

#include "RestrictGeolocation/isAllow.inc"
}

#include "RestrictGeolocation/getCountryCode.inc"
}

#include "RestrictGeolocation/isAllow_1.inc"
}