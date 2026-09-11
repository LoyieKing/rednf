// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 RestrictGeolocation/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
const char* GeoIP_country_code_by_addr(void* handle, const char* qry);
}

#include "RestrictGeolocation/RestrictGeolocation.inc"


#include "RestrictGeolocation/dtor_RestrictGeolocation.inc"


#include "RestrictGeolocation/initGeolocation.inc"


#include "RestrictGeolocation/closeGeolocation.inc"


#include "RestrictGeolocation/clearAllowCountry.inc"


#include "RestrictGeolocation/addAllowCountry.inc"


#include "RestrictGeolocation/isAllowCountryCode.inc"


#include "RestrictGeolocation/loadData.inc"


#include "RestrictGeolocation/isAllow.inc"


#include "RestrictGeolocation/getCountryCode.inc"


#include "RestrictGeolocation/isAllow_1.inc"
