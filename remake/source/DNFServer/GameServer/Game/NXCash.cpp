// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 NXCash/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "nexon_cash.h"

#include "GameTypes.h"

// ORIG 该 TU 含 file-scope 动态初始化（0x081ae4d1
// __static_initialization_and_destruction_0 → ios_base::Init::C1Ev +
// __cxa_atexit(D1)）；include <iostream> 是 GI 的发射前提（P327）。
#include <iostream>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <wchar.h>

namespace Encryption
{
unsigned int Adler32(char* data, unsigned int length);                      // _ZN10Encryption7Adler32EPcj
#include "NXCash/EncodeDressStringWide.inc"
                           unsigned long param);                            // _ZN10Encryption21EncodeDressStringWideEPKwPwm
// [b28d-Main·检查点22] 删除原先的 `__attribute__((weak))` 空 stub：
//   ① 二者在 ORIG 里是 **U 引用**（`nm $O` = U，由 NEEDED `libnxencryption.so`
//      在运行期提供；0x0807e190/0x0807e8e0 为 PLT 调用点），我方在 Game/ 源码内
//      发射 W 定义 ⇒ 属「我方 .cpp 发明导出符号」（only_new 2 项）。
//   ② 空 stub 运行期返回 0/不写 out，与厂商实现不一致（行为差异）。
//   现由 `toolchains/nxencryption/libnxencryption.so`（厂商真实二进制，已入仓）
//   参与链接：本 TU 只留声明 ⇒ 我方 df_game_r 变成 U + NEEDED libnxencryption.so，
//   与 ORIG readelf/nm 完全同型。
}  // namespace Encryption

namespace nexon {
namespace cash {

#include "NXCash/NCashMessage.inc"


#include "NXCash/data.inc"


#include "NXCash/body.inc"


#include "NXCash/body_length.inc"


#include "NXCash/length.inc"


#include "NXCash/encode_header.inc"


#include "NXCash/encode_header_1.inc"


#include "NXCash/decode_header.inc"

#include "NXCash/NCashImpl.inc"


#include "NXCash/_increaseRequestID.inc"


// NCashImpl 四个 coupon 定义在文件尾：
// ORIG 编译单元内调用点仅见原型 → 宽松帧 sub $0x18（技巧31 扩展）。

#include "NXCash/recv_msg.inc"


#include "NXCash/recv_body.inc"


#include "NXCash/send_msg.inc"


#include "NXCash/reconnect.inc"


#include "NXCash/set_data_header.inc"


#include "NXCash/get_data_header.inc"


#include "NXCash/handle_cash_error.inc"


#include "NXCash/handle_coupon_error.inc"


#include "NXCash/checkEntrance.inc"


#include "NXCash/getBalance.inc"


#include "NXCash/purchase.inc"


#include "NXCash/purchase_1.inc"


#include "NXCash/gift.inc"


#include "NXCash/confirm.inc"


#include "NXCash/cancel.inc"


#include "NXCash/chargeCash.inc"


#include "NXCash/checkCoupon.inc"
#include "NXCash/useCoupon.inc"
#include "NXCash/checkCafeCoupon.inc"
#include "NXCash/useCafeCoupon.inc"

}  // namespace cash
}  // namespace nexon
