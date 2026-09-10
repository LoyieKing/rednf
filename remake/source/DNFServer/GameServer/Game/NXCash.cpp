// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 NXCash/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// nexon::cash::NCashImpl —— NEXON 计费协议实现（独立 TU）。
//
// [GI 拆分] 本文件由 nexon_cash.cpp 拆出：ORIG 侧 NCashImpl 属独立编译单元
// `NXCash.cpp`，其 _GLOBAL__I_ 键名 = 本 TU 首个「强符号 · 外部链接 · 本 TU
// 定义」实体 = _ZN5nexon4cash9NCashImplC2EPKcshts（ORIG @0x081ae511，
// 对应函数体 @0x081acfdc）。故 NCashImpl ctor 必须保持为本文件第一处外联
// 定义（P327：外联 ctor 取 C2 形）。
//
// NexonCash 薄包装仍留在 nexon_cash.cpp：ORIG 该 TU 内 NexonCash ctor 位于
// NCashImpl ctor 之后，而我方需 NexonCash ctor「只见 NCashImpl ctor 原型」
// 以复现 new-expression 的保守 EH cleanup（delete+_Unwind_Resume 岛，
// ORIG 0x081ae21b-0x081ae232）。若把 NexonCash 并入本文件则其定义必须先于
// NCashImpl ctor（同 TU 前置），GI 键名将变成 NexonCashC2 而非 want。
//
// 逐符号按 df_game_r ORIG 反汇编转写；地址见各函数注释。
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
}  // namespace Encryption

namespace nexon {
namespace cash {

// ---------------------------------------------------------------------------
// NCashMessage 定义必须前置于 NCashImpl ctor 且保持 inline（P210 帧继承）
//
// NCashImpl ctor 调用 m_inMsg()/m_outMsg()（NCashMessage::C1）。拆 TU 前二者
// 同处 nexon_cash.cpp 且 NCashMessage 定义在前 ⇒ 4.4 精确继承 callee outgoing；
// 拆分后若本 TU 只见声明，4.4 转保守形，C1 prologue 由 ORIG 的
// `push %ebx; sub $0x24` 退化为 `push %esi; push %ebx; sub $0x20`（DIFF）。
// 实测三形（均定义前置）：
//   · 普通 out-of-line 定义 ⇒ C1 恢复，但 NCashMessageC2 成为 TU 首强符号，
//     GI 键名被顶成 NCashMessageC2（≠ want）；
//   · __attribute__((weak)) 定义 ⇒ C1 仍 DIFF（4.4 对 weak callee 不做帧继承）；
//   · inline 定义 ⇒ C1 恢复且符号为 W（不参与 GI 命名）⇒ 唯一可行形。
// 符号类型随之与 ORIG 一致（ORIG 该 8 符号全为 W）；代价为 4.4 额外发射
// local 别名 `_ZN5nexon4cash12NCashMessageC5Ev`（n 类型、地址 0、结构性噪音）。
// ---------------------------------------------------------------------------
#include "NXCash/NCashMessage.inc"
}

#include "NXCash/data.inc"
}

#include "NXCash/body.inc"
}

#include "NXCash/body_length.inc"
}

// ORIG 0x081ae54e：return u16(this+2)+3。
#include "NXCash/length.inc"
}

// ORIG 0x081ae578（W）：wireMagic='U'; *(u16*)netLen=htons(bodyLength) 指针直存
// （dst 0x081ae585 add $5 单步 + 0x081ae588 mov %eax,%ebx 先载入，技巧27）。
#include "NXCash/encode_header.inc"
}

// ORIG 0x081ae5ae：bodyLength 参数直存 +0x02 后调用 encode_header()。
#include "NXCash/encode_header_1.inc"
}

// ORIG 0x081ae5ce（W）：m_bodyLength=ntohs(*(u16*)netLen)（0x081ae5d7 add $5
// 单步后 movzwl 直读，无 copy 槽）；u16 无符号 cmp $0x200 超 → 清零返 false。
#include "NXCash/decode_header.inc"
}
// ORIG 0x081acfdc：NCashImpl ctor 本体（TU 首符号 ⇒ GI 键名 C2 形）。
#include "NXCash/NCashImpl.inc"
}

#include "NXCash/_increaseRequestID.inc"
}

// NCashImpl 四个 coupon 定义在文件尾：
// ORIG 编译单元内调用点仅见原型 → 宽松帧 sub $0x18（技巧31 扩展）。

// ORIG 0x081aea46（W）：MSG_WAITALL(0x100) 预读头并解码，随后转 recv_body。
// buf → %ebx（0x081aea58 call data 后直落寄存器，技巧2），getHandle 结果免槽
// 直填第1参（0x081aea66-0x081aea7b），n 槽 -0xc，frame 0x24 + pop ebx。
#include "NXCash/recv_msg.inc"
}

// ORIG 0x081aeb3c：按 body_length 继续收 body 区（flags 同 0x100）。
#include "NXCash/recv_body.inc"
}

// ORIG 0x081aec2c（W）：encode(totalLen) → 缓存 length→%esi / data→%ebx
// （0x081aec5c/0x081aec6e 直用寄存器，技巧2），getHandle 结果免槽直填第1参
// （0x081aec79-0x081aec8e，技巧37）；n 槽 -0xc；比较对象是**重调用** length()
// （0x081aeca4-0x081aeca9 cmp -0xc,%eax + setne/test，技巧9 bool 形）；
// 错误分支 strerror 结果 → %ebx 后重 call length 填 printf（0x081aecc4-0x081aecd4）。
#include "NXCash/send_msg.inc"
}

// ORIG 0x081ad080：未连接则重开重连；返回连接态。
#include "NXCash/reconnect.inc"
}

// ORIG 0x081ad0e6（sret）：ByteBuffer(out.body(),0x200)<<pid<<flag<<requestID。
#include "NXCash/set_data_header.inc"
}

// ORIG 0x081ad168：从入包 body 读 pid/flag/requestId 三元组。
// [b23S3C] P192：sret 返回入包视图对象（NRVO 直构 caller 槽，无额外拷贝）。
// [b28d-D43] NEAR 帧量化：ORIG sub $0x10 vs 4.4 $0xc（P190附注②；体 39/39 全等）（ORIG 0x081ad168）。
#include "NXCash/get_data_header.inc"
}

// ORIG 0x081ad1e0：switch(err-5) 跳转表分支（默认 16）。
#include "NXCash/handle_cash_error.inc"
}

// ORIG 0x081ad2c0：0..3 静默；default 打印；恒返回 16。
// case 0..3 → goto 共用出口标号（技巧23/24）：复现 ORIG 0x081ad2d0-0x081ad2ff
// 二分树 dispatch（cmp$1 je / cmp$1 jg / test je / jmp / cmp$2 je / cmp$3 je）
// + 每 case `nop; jmp merge` 岛；直接 break 会被 4.4 折成 jbe 范围形。
#include "NXCash/handle_coupon_error.inc"
}

// ORIG 0x081ad316：手组包头（不走 set_data_header！仅自增 requestID 不写入）。
// [b28d-solo-nexon_cash] N 封顶：rc 预清/三元组无预清/声明序已归位后，残余
// 仅为 ORIG 0x081ad430 wrap 实参装载寄存器差（ORIG `lea in,%edx; mov body,%eax`
// vs 4.4 `mov body,%edx; lea in,%eax`，P75/N7 跨版本装载调度，our 107 vs
// orig 106），语义等价。
#include "NXCash/checkEntrance.inc"
}

// ORIG 0x081ad49e：packet_id=0 应答 err==0 后读余额。
// [b28d-D43] N82/N95 帧/槽平移（81/104）（ORIG 0x081ad49e）。
#include "NXCash/getBalance.inc"
}

// ORIG 0x081ad5c2。
#include "NXCash/purchase.inc"
}

// ORIG 0x081ad7ae：subCmd∈{60,62} 时 streamType 替换为 subCmd，其余同上。
#include "NXCash/purchase_1.inc"
}

// ORIG 0x081ad9c4：两段附加串位于 m_paramB/m_fieldC 之后。
// [b28d-D43] N82 帧：ORIG sub $0x68 vs 4.4 push %ebx+sub $0x74（ORIG 0x081ad9c4）。
#include "NXCash/gift.inc"
}

// ORIG 0x081adbd4：err!=3 时 handle_cash_error 结果被丢弃仍打印成功行返 0。
// [b28d-D43] N82/N95 同 cancel（帧/槽平移）（ORIG 0x081adbd4）。
#include "NXCash/confirm.inc"
}

// ORIG 0x081add04：与 confirm 同构，packet_id=4、__FUNCTION__="cancel"。
// [b28d-D43] N82/N95 帧/槽平移：set_data_header 返回槽 ORIG -0x1c vs 4.4 -0x2c（ORIG 0x081add04）。
#include "NXCash/cancel.inc"
}

// ORIG 0x081ade34：无 reconnect 前置；组"%s|%i|%i|%i|%s|%s|%s|%s|%i" 明文 →
// mbstowcs → Adler32(宽串字节) → EncodeDressStringWide(key=0x1b1bde64) →
// wcstombs 回 ASCII 密文入包 + 协议键 qword + adler qword。
// [b28d-solo-nexon_cash] 本波修真 bug 3 处：①4 个函数级 static 缓冲（ORIG 符号
// _ZZ...E4data/E9wide_data/E8enc_data/E9send_data，尺寸 0x3e8/0xfa0/0xfa0/0x3e8）
// 替代 struct+getter（旧形发明符号 + memset 长度 0x1f40 错）；②格式串 9 token
// （iv/code 为 %i，旧形漏传 pD/code 且 iv 当 char*）；③adler 为 unsigned long long
// （8 字节槽 0x081adfdd）。帧/槽已全对齐；N 封顶残余 = 4.1 局部物化/EH 段差
// （P0/N82 族，our 200 vs orig 196，多数为 .o 重定位伪差）。
#include "NXCash/chargeCash.inc"
}

#include "NXCash/checkCoupon.inc"
#include "NXCash/useCoupon.inc"
#include "NXCash/checkCafeCoupon.inc"
#include "NXCash/useCafeCoupon.inc"

}  // namespace cash
}  // namespace nexon
