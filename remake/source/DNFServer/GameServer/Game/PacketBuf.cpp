// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 PacketBuf/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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
}

#include "PacketBuf/dtor_PacketBuf.inc"
}

// ===========================================================================
// 绑定 / 清空
// ===========================================================================

#include "PacketBuf/bind_packet.inc"
}

#include "PacketBuf/clear.inc"
}

#include "PacketBuf/clear_ptr.inc"
}

#include "PacketBuf/set_packet.inc"
}

#include "PacketBuf/copy.inc"
}

// ===========================================================================
// 写入
// ===========================================================================

#include "PacketBuf/put_header.inc"
}

#include "PacketBuf/put_byte.inc"
}

#include "PacketBuf/put_short.inc"
}

#include "PacketBuf/put_int.inc"
}

#include "PacketBuf/put_binary.inc"
}

#include "PacketBuf/put_binary_1.inc"
}

#include "PacketBuf/put_str.inc"
}

#include "PacketBuf/put_str_1.inc"
}

// ===========================================================================
// 读取
// ===========================================================================

#include "PacketBuf/is_ptr_end.inc"
}

#include "PacketBuf/is_len_end.inc"
}

#include "PacketBuf/get_byte.inc"
}

#include "PacketBuf/get_byte_1.inc"
}

#include "PacketBuf/get_short.inc"
}

#include "PacketBuf/get_short_1.inc"
}

#include "PacketBuf/get_int.inc"
}

#include "PacketBuf/get_int_1.inc"
}

#include "PacketBuf/get_int_2.inc"
}

#include "PacketBuf/get_binary.inc"
}

#include "PacketBuf/get_str.inc"
}

// ===========================================================================
// 收尾 / 加密 / 压缩
// ===========================================================================

#include "PacketBuf/finalize.inc"
}

#include "PacketBuf/encFinalize.inc"
}

// [b28d-solo-PacketBuf] verdict=AE（check_tu_routed clean；p167 9/9 CLEAR，129=129）。
// 修法：①P207 显式 `operator delete[](buffer)` 消 4.4 -O0 delete[] 自带的
// test/je 判空（ORIG 4.1 只有源码 if 的一次判空，原 delete[] 多一对 cmpl/je+jmp/nop）；
// ②`int bufferSize` + 显式括号 `m_size + ((PKT_BODY_OFF - m_size) + bufferSize)`
// 阻止 4.4 把 `m + (C - m)` 代数折成 `C`（探针 /tmp/probe_enc.cpp f1 折成
// `buf+0xf` 4 条；/tmp/probe_enc4.cpp h6 保留 ORIG 的 `(0xf-m)+buf` 两段链）。
#include "PacketBuf/encrypt_packet.inc"
}

#include "PacketBuf/compress_packet.inc"
}

// ===========================================================================
// 便捷重载 / 索引
// ===========================================================================

#include "PacketBuf/at.inc"
}

#include "PacketBuf/getLastError.inc"
}

#include "PacketBuf/setLastError.inc"
}

#include "PacketBuf/get_index.inc"
}

#include "PacketBuf/set_index.inc"
}

#include "PacketBuf/get_item_idx.inc"
}

#include "PacketBuf/put_item_idx.inc"
}

#include "PacketBuf/is_finallized.inc"
}

#include "PacketBuf/isEncryptRequired.inc"
}

#include "PacketBuf/isCompressRequired.inc"
}

#include "PacketBuf/get_len.inc"
}

#include "PacketBuf/get_packet.inc"
}

#include "PacketBuf/get_buf_ptr.inc"
}

#include "PacketBuf/put_str_2.inc"
}

#include "PacketBuf/put_str_3.inc"
}

#include "PacketBuf/put_binary_2.inc"
}

#include "PacketBuf/put_binary_3.inc"
}

#include "PacketBuf/put_byte_1.inc"
}

#include "PacketBuf/put_int_1.inc"
}

#include "PacketBuf/put_short_1.inc"
}

// ===========================================================================
// put_packet（Inven_Item 序列化）
// ===========================================================================

#include "PacketBuf/put_packet.inc"
}

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

// 权威定义 GameSecurity.cpp（ORIG T 0x0808c8b5，属主 C28L22 已按 Main 授权声明
// 为 **bool**）；本 TU 的局部声明必须同步——返回类型不入 mangled 但**决定调用点
// 归一化形**（P168a/lane14/lane7 实测：char callee ⇒ 4.4 对 `== 1` 恒产
// `cmp $0x1,%al; sete; test; je`；bool callee ⇒ 直产 ORIG 的
// `call; xor $0x1,%eax; test %al,%al; jne`，b28c-l23 档①）。
#include "PacketBuf/_NS_MakeHash_.inc"
#include "PacketBuf/_NS_PI_MakeHash_NOTI.inc"
}
