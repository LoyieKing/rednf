// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 GameSecurity/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// GameSecurity.cpp —— 包安全自由函数实现（PacketDispatcher 叶子批次）。
// 逐指令转写自 ORIG df_game_r（objdump -d）：
//   DenyPacket(int)                          ORIG T 0x859092a (0x104)
//   _NS_PI_Check_CMD(...)                    ORIG T 0x8274778 (0x1fd)
//   _NS_PI_2ND_MakeChecksumTo1Byte2(...)     ORIG T 0x808cc4b (0x1f)

#include "PacketSecurity.h"

#include "PacketBuf.h"
#include "CUser.h"      // WongWork::ENUM_HACKTYPE / CHackAnalyzer / SetETC / 组队判定
#include "CParty.h"     // CParty::get_member_count（组队误判豁免）
#include "md5.h"        // md5_context/md5_starts/md5_update/md5_finish（_NS_MakeHash_）

#include <map>
#include <utility>
#include <time.h>
#include <string.h>

// ---------------------------------------------------------------------------
// PI 安全域函数（ORIG 符号经 nm 核实；真实定义在本 TU 后段）：
//   char _NS_MakeHash_(char*, short, int&)
//       _Z13_NS_MakeHash_PcsRi            T 0x808c8b5（报告 _lt_global_gt_/_NS_MakeHash_.md）
//   void _NS_MakeHash_(char*, unsigned short, unsigned int&)
//       _Z13_NS_MakeHash_PctRj            T 0x0808c976（报告 _NS_MakeHash__0808c976.md，薄包装）
//   char PI_GetType_CS(char, short)
//       _Z13PI_GetType_CScs               T 0x808c5d8（报告 _lt_global_gt_/PI_GetType_CS.md）
//   void PIAddServerHackCnt(CUser*, WongWork::ENUM_HACKTYPE, int)
//       _Z18PIAddServerHackCntP5CUserN8WongWork13ENUM_HACKTYPEEi  W 0x82772b7
//   int PI_GetHackType(short)
//       _Z14PI_GetHackTypes               W 0x82758e3
//       注：ORIG 符号看着"缺参数编码"，实为 `_Z14` + 名长 14 = "PI_GetHackType"
//       + `s`(short) ⇒ 本名（**单数 HackType**）自然 mangling 即 `_Z14PI_GetHackTypes`。
//       原先我方误写复数 PI_GetHackTypes（会 mangling 成 _Z15PI_GetHackTypess），
//       才不得不用 `__asm__` 标签纠正——按 guide §4.5 已删桥、改名正形（零码变）。
// 返回类型 char（非 bool/int）：ORIG 调用点 _NS_PI_MakeHash_NOTI(0x82746f3) 为
// `xor $0x1,%eax; test %al,%al; jne` —— 只测低字节，char 值域的 ==1 判据；
// ORIG 本体(0x0808c8b5) 两条返回 `mov $0x0,%eax`/`mov $0x1,%eax` 严格 0/1。
// mangled 不含返回类型 ⇒ ABI 无影响（Main 授权，batch-23 S 线）。
#include "GameSecurity/_NS_MakeHash_.inc"
#include "GameSecurity/PI_GetType_CS.inc"
#include "GameSecurity/PI_GetHackType.inc"
#include "GameSecurity/PIAddServerHackCnt.inc"
#include "GameSecurity/_NS_PI_2ND_GetHackType.inc"
#include "GameSecurity/PI_GetType_CS_1.inc"
}

// ---------------------------------------------------------------------------
// DenyPacket  ORIG T 0x859092a (0x104)
// 纯谓词：id 命中封禁集合返回 true。二分比较树结构与 ORIG 一致：
//   true 集合 = {0x05..0x06, 0x1a..0x1b, 0x1d, 0x42, 0x44, 0x53..0x54,
//                0x61, 0x66, 0x69, 0x8a, 0xac, 0xb9..0xc1, 0xcc..0xcd,
//                0xe1, 0xee, 0x100, 0x104, 0x107, 0x10f..0x111,
//                0x120, 0x13c, 0x145}
// ---------------------------------------------------------------------------
// [b28d-solo-GameSecurity] 修法：if-else 二分链 → 稀疏 switch（case 按值升序）。
// ORIG 0x0859092a 每个树节点为 `cmp $K,%eax; je L_true; cmp $K,%eax; jg R`——双 cmp
// 同常量是 GCC switch 决策树签名（P17/P261）；if 链恒多 20 条。
#include "GameSecurity/DenyPacket.inc"
}

// ---------------------------------------------------------------------------
// _NS_PI_2ND_MakeChecksumTo1Byte2  ORIG T 0x808cc4b (0x1f)
// ORIG：*a = movzbl(*a)；*b = movzbl(*b)（各截低 8 位回写 int）。
// ---------------------------------------------------------------------------
#include "GameSecurity/_NS_PI_2ND_MakeChecksumTo1Byte2.inc"
}

// ---------------------------------------------------------------------------
// _NS_PI_Check_CMD  ORIG T 0x8274778 (0x1fd)
// 命令级包完整性校验：
//   1) packetClass 0/1/2 → 类型字节 t（ORIG: 0→2, 1→1, 其余→0）；
//   2) PI_GetType_CS(t, cmd) 为 0 → 放行返回；
//   3) data = get_packet(0)+0xd，idx = get_index()-0xd（包体区）；
//   4) get_int(expected)（返回值忽略）；
//   5) PICS map<pair<char,char>,int>（CUser::GetPICSMap）：无该 key 先置 0，
//      计数 >0x64 → 放行返回；
//   6) _NS_MakeHash_(data, idx, hash) 失败 → 返回；hash==expected → 放行；
//   7) 否则计数 +1，penalty = (t%10)*10+0xc9（ORIG 魔数除法 0x67/5 序列），
//      PIAddServerHackCnt(user, PI_GetHackTypes(cmdType), penalty)。
// ---------------------------------------------------------------------------
// [b28d-24] DIFF 残13: 分支块序/寄存器分配（P47/P222 族）。ORIG 0x08274778
#include "GameSecurity/_NS_PI_Check_CMD.inc"
}

// ===========================================================================
// _NS_MakeHash_  ORIG T 0x808c8b5（报告 _lt_global_gt_/_NS_MakeHash_.md）
// len 越界（<=0 或 >0x190）返回 0；否则一步 md5() 后按 ORIG 逐字节 xor 折叠：
//   d[0] ^= d[8]^d[10]^d[13] ^ 0x81   d[1] ^= d[5]^d[9]^d[12]  ^ 0x78
//   d[2] ^= d[4]^d[6]^d[14]  ^ 0x1a   d[3] ^= d[7]^d[11]^d[15] ^ 0xbf
// 结果以小端 dword 写 *out（ORIG mov (%eax),%edx 读 4 字节）。
// ===========================================================================
// [b23S3C] ORIG 尾 `mov $0x1,%eax; leave; ret`+调用点 xor$0x1 直测低字节 ⇒ 返回 char；
// 体 ORIG 单 call _Z3md5PhiS_（0x80b3d57），勿展开 starts/update/finish 三 call。
// [b28d-24] DIFF 残20: xor 链中转寄存器差异（ORIG 用 %ecx 逐段 `xor %eax,%ecx`；4.4 用 %edx 且多 1 条 xor）。ORIG 0x0808c8b5
#include "GameSecurity/_NS_MakeHash__1.inc"
}

// ---------------------------------------------------------------------------
// _NS_MakeHash_  ORIG T 0x0808c976（报告 _NS_MakeHash__0808c976.md）
// unsigned 形参薄包装：len 经 cwtl 符号扩展为 short 转调 int& 版本。
// ---------------------------------------------------------------------------
#include "GameSecurity/_NS_MakeHash__2.inc"
}

// ===========================================================================
// PI_GetHackType  ORIG W 0x82758e3（符号 _Z14PI_GetHackTypes = 本名自然 mangling）
// jump table（.rodata 0x8bff0b4，55 项）逐项转写：idx 0→0x514；
// 1..9→0x515..0x51d；10→0x54a；11..52→0x51e..0x547；53→0x548；54→0x549；
// 越界/负值（movswl 符号扩展后 ja）→ 0x514。
// ===========================================================================
#include "GameSecurity/PI_GetHackType_1.inc"
}

// ===========================================================================
// PIAddServerHackCnt  ORIG W 0x082772b7
// type==0x524 且在组队（CParty 成员数>1）时跳过（防组队场景误判）；
// 否则 SetETC((short)type, count) 记 ETC 并把回写值交给 CHackAnalyzer：
//   addServerHackCnt(user, type, 1, etcValue, 0)
// ===========================================================================
#include "GameSecurity/PIAddServerHackCnt_1.inc"
}
// ===========================================================================
// HP_GetHackType  ORIG T 0x82756cf（jump table .rodata 0x8bfefc8，41 项）
// type<=0x29 查表；表项 idx 0/越界 → 0x384；idx 1..9 → 0x385..0x38d；
// idx 10..40 → 0x578..0x596。
// ===========================================================================
#include "GameSecurity/HP_GetHackType.inc"
}

// ===========================================================================
// HIP_GetHackType  ORIG T 0x827585c（jump table .rodata 0x8bff070，17 项）
// type<=0x10 查表：idx 0→0x5dc；1..4→0x5dd..0x5e0；5→0x5dc；6..8→0x5e2..0x5e4；
// 9..11→0x641..0x643；12→0x5dc；13..15→0x646..0x648；16→0x679；越界→0x5dc。
// ===========================================================================
#include "GameSecurity/HIP_GetHackType.inc"
}

// ===========================================================================
// MI_GetHackType  ORIG T 0x8274ad7：value = (unsigned char)c + 0x6a4，
// 上限 0x707（jle 保持）。
// ===========================================================================
#include "GameSecurity/MI_GetHackType.inc"
}

// ===========================================================================
// _NS_IPScan  ORIG T 0x8274676：读 1 字节 → HIP_GetHackType →
// addServerHackCnt(user, type, 1, 0, 0)。读取失败返回 1。
// ===========================================================================
#include "GameSecurity/_NS_IPScan.inc"
}

// ===========================================================================
// _NS_ProcessScan  ORIG T 0x8274523：读 3 字节（b%10 + c%10*10 编码值）→
// HP_GetHackType(a) → SetETC → addServerHackCnt(user, type, 1, etc, 0)。
// 各步读取失败依次返回 1/2/3。
// ===========================================================================
// [b28d-24] DIFF 残24: 控制流/帧差异（P167 待复核）。ORIG 0x08274523
#include "GameSecurity/_NS_ProcessScan.inc"
}

// ===========================================================================
// _NS_PacketIntegrity  ORIG T 0x8274975：读 4 字节，编码值
// d%5 + b%5*10 + a%5*100 → PI_GetHackType(c) → PIAddServerHackCnt。
// user 为空返回 4；各步读取失败依次返回 1/2/3/4。
// ===========================================================================
#include "GameSecurity/_NS_PacketIntegrity.inc"
}

// ===========================================================================
// _NS_MI_RecvMemoryIntegrityModulation  ORIG T 0x8274b09：读 2 字节 →
// MI_GetHackType(a) → SetETC(type, (signed char)b) → addServerHackCnt。
// 读取失败依次返回 1/2。
// ===========================================================================
#include "GameSecurity/_NS_MI_RecvMemoryIntegrityModulation.inc"
}

// ===========================================================================
// _NS_NonClient_GetResponse  ORIG T 0x8274cc3：置 NonClientFlag；读随机数+结果
// 字节。结果为 0 时校验 user 随机数的 _NS_MakeHash_(4 字节)，不匹配 → hack
// 0x33b；结果 1 → hack 0x33d；其它 → hack 0x33e。user 空返回 0；读取失败
// 返回 1/2。
// ===========================================================================
#include "GameSecurity/_NS_NonClient_GetResponse.inc"
}

// ===========================================================================
// _NS_PI_2ND_RecvPacketIntegrityModulation_C2C  ORIG T 0x8275290：读 ushort，
// v = v + 0xe11；v > 0xf9e 则 0xf9f；addServerHackCnt(user, v, 1, 0, 0)。
// ===========================================================================
#include "GameSecurity/_NS_PI_2ND_RecvPacketIntegrityModulation_C2C.inc"
}

// ===========================================================================
// _NS_PI_2ND_RecvPacketIntegrityModulation_S2C  ORIG T 0x8275140：读 byte+short
// → _NS_PI_2ND_GetHackType(1, a, b) → addServerHackCnt(user, type, 1, 0, 0)。
// ===========================================================================
#include "GameSecurity/_NS_PI_2ND_RecvPacketIntegrityModulation_S2C.inc"
}

// ===========================================================================
// checkNewMemberHacklog  ORIG T 0x8275315：hackType 编码为 YYYYMMDD。
// 与当前时间差（天）：<0 / ==0 → hack 0x134；1..3 → 0x135；4..7 → 0x136；
// 8..0x1e → 0x137；其它不记。SetETC(type, p3) 后 addServerHackCnt
// (user, type, 1, etc, hackType)。
// ===========================================================================
#include "GameSecurity/checkNewMemberHacklog.inc"
}

// ===========================================================================
// _NS_PI_2ND_GetHackTypeEnum  ORIG W 0x08275af3 (0x17b6)
// §6.2 巨件核销：真修——原 static 前缀表查找版 mangled 为 _ZL26…（32 条），
// ORIG 权威符号为 weak _Z26…（跳表展开 1230 条）。去 static 并按 ORIG 结构
// 全量重建：if(type==1)/else if(type==0) + 内层 switch(cmd)（报告缺失，
// nm 定址后 objdump -d 0x08275af3..0x082772a9 补证）。
//   · type==1 跳表 @0x8bff190（0x25f 项）、type==0 跳表 @0x8bffb0c（0x233 项）；
//   · case 按返回值分组（同值 cmd 共享叶子 mov$N;jmp）；
//   · 指向 default（0x082772a2 mov$0）的 285/282 个 cmd 不列 case——
//     ORIG 源码未写这些 case，与越界共用 return 0；
//   · cmd→枚举映射逐项对账一致。
// ===========================================================================
// ===========================================================================
// _NS_PI_2ND_GetHackType  ORIG W 0x8277359：v = 2*(enum + 0x3e8) + type，
// 上限 0xe0f（jbe 保持）。
// 【P210】定义前置：调用者须在 _NS_PI_2ND_GetHackTypeEnum 定义之前，
// 否则 4.4 按 callee 帧精确继承 outgoing，0x38 保守大帧塌缩为 0x20。
// ===========================================================================
#include "GameSecurity/_NS_PI_2ND_GetHackTypeEnum.inc"

#include "GameSecurity/_NS_PI_2ND_GetHackType_1.inc"
}

#include "GameSecurity/_NS_PI_2ND_GetHackTypeEnum_1.inc"
}
// ============================================================================
// OnlyOrigBatch11 落地：_NS_HS_NoResponseToHackLog —— ORIG T 0x08274f31
// user->getHackAnalyzer()->addServerHackCnt(user, 0x33c, 1, 0, 0)
// （addServerHackCnt 五参：CUser*, ENUM_HACKTYPE, uint, uint, uint）。
// ============================================================================
#include "GameSecurity/_NS_HS_NoResponseToHackLog.inc"
}
