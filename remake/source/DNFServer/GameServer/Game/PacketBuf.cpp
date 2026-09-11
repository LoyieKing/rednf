// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 PacketBuf/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "LogManager.h"
#include "PacketBuf.h"

#include <cstring>

#include "CDataManager.h"
#include "GameWorld.h"
#include "md5.h"

// ===========================================================================
// 生命周期
// ===========================================================================

#include "PacketBuf/PacketBuf.inc"


#include "PacketBuf/dtor_PacketBuf.inc"


// ===========================================================================
// 绑定 / 清空
// ===========================================================================

#include "PacketBuf/bind_packet.inc"


#include "PacketBuf/clear.inc"


#include "PacketBuf/clear_ptr.inc"


#include "PacketBuf/set_packet.inc"


#include "PacketBuf/copy.inc"


// ===========================================================================
// 写入
// ===========================================================================

#include "PacketBuf/put_header.inc"


#include "PacketBuf/put_byte.inc"


#include "PacketBuf/put_short.inc"


#include "PacketBuf/put_int.inc"


#include "PacketBuf/put_binary.inc"


#include "PacketBuf/put_binary_1.inc"


#include "PacketBuf/put_str.inc"


#include "PacketBuf/put_str_1.inc"


// ===========================================================================
// 读取
// ===========================================================================

#include "PacketBuf/is_ptr_end.inc"


#include "PacketBuf/is_len_end.inc"


#include "PacketBuf/get_byte.inc"


#include "PacketBuf/get_byte_1.inc"


#include "PacketBuf/get_short.inc"


#include "PacketBuf/get_short_1.inc"


#include "PacketBuf/get_int.inc"


#include "PacketBuf/get_int_1.inc"


#include "PacketBuf/get_int_2.inc"


#include "PacketBuf/get_binary.inc"


#include "PacketBuf/get_str.inc"


// ===========================================================================
// 收尾 / 加密 / 压缩
// ===========================================================================

#include "PacketBuf/finalize.inc"


#include "PacketBuf/encFinalize.inc"


#include "PacketBuf/encrypt_packet.inc"


#include "PacketBuf/compress_packet.inc"


// ===========================================================================
// 便捷重载 / 索引
// ===========================================================================

#include "PacketBuf/at.inc"


#include "PacketBuf/getLastError.inc"


#include "PacketBuf/setLastError.inc"


#include "PacketBuf/get_index.inc"


#include "PacketBuf/set_index.inc"


#include "PacketBuf/get_item_idx.inc"


#include "PacketBuf/put_item_idx.inc"


#include "PacketBuf/is_finallized.inc"


#include "PacketBuf/isEncryptRequired.inc"


#include "PacketBuf/isCompressRequired.inc"


#include "PacketBuf/get_len.inc"


#include "PacketBuf/get_packet.inc"


#include "PacketBuf/get_buf_ptr.inc"


#include "PacketBuf/put_str_2.inc"


#include "PacketBuf/put_str_3.inc"


#include "PacketBuf/put_binary_2.inc"


#include "PacketBuf/put_binary_3.inc"


#include "PacketBuf/put_byte_1.inc"


#include "PacketBuf/put_int_1.inc"


#include "PacketBuf/put_short_1.inc"


// ===========================================================================
// put_packet（Inven_Item 序列化）
// ===========================================================================

#include "PacketBuf/put_packet.inc"


// ===========================================================================
// _NS_PI_MakeHash_NOTI（ORIG 0x82746f3 T）
// 包尾哈希：对包体（m_ptr + 0xf 起，长度 = get_index() - 0xf）经
// _NS_MakeHash_（ORIG 0x808c8b5，真实定义 GameSecurity.cpp）折叠为 4 字节，
// 以小端 put_int 写包尾；len 越界（<=0 或 >0x190）时不写。
// （ORIG 直接 call _Z13_NS_MakeHash_PcsRi——本 TU 不再本地复制其折叠规则。）
// ORIG 实证（docs/class_func_reports/_lt_global_gt_/_NS_PI_MakeHash_NOTI.md）：
//   -18 base(char*) / -12 未使用的 ushort / -10 region(char*) /
//   -0a len(**short** 槽：`mov %ax,-0xa` + `movswl` 读回)/ -1c hash(int，
//    **无初始化 store**)；调用点为 `_NS_MakeHash_(region, len, hash) == 1`
//    （`xor $0x1,%eax; test %al,%al` ⇒ 返回类型为窄类型 + 显式 ==1 比较）。
// ===========================================================================

#include "PacketBuf/_NS_MakeHash_.inc"

#include "PacketBuf/_NS_PI_MakeHash_NOTI.inc"

