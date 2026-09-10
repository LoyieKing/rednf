// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 WongWorkSecurityCard_A/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// WongWorkSecurityCard_A.cpp — WongWork::CSecurityCard ORIG-only 成员（worklist A 段）
// 逐函数自 ORIG df_game_r 反汇编忠实转写（地址见各处注释）。
// 布局依赖：CUser.h CSecurityCard（+0x40 字节与 +0x44 requestState 由
// OnlyOrig_CUser 批次按 ORIG 补正后，本 TU 与真机偏移一致）。
#include "CUser.h"
#include <string.h>

namespace WongWork
{

// T 0x085fdd68 init：清 +0x2f/0x30/0x34/0x44/0x48/0x38/0x31/0x40。
// 字段顺序与 ORIG 清零序列一致；+0x40 为单字节占位区（init 只清该字节），
// 头中以 m_pad40 区表达。
#include "WongWorkSecurityCard_A/init.inc"
}

// W 0x086959b8：movzbl 0x31(%eax),%eax —— 直返 +0x31 字节（P9：char 直返无规范化）。
#include "WongWorkSecurityCard_A/getCertFlag.inc"
}

// W 0x0826528a：mov 0x38(%eax),%eax —— 读 cert 计数（+0x38）。
#include "WongWorkSecurityCard_A/getCertCnt.inc"
}

// W 0x082652ae：mov 0x34; lea 0x1; 写回; 再读返（++(+0x34) 后返回新值）。
#include "WongWorkSecurityCard_A/incFailCnt.inc"
}

// W 0x082652c8：movl $0x0,0x34(%eax)。
#include "WongWorkSecurityCard_A/resetFailCnt.inc"
}

// W 0x084ebd82：mov %al 截断暂存; mov this; movzbl 回读; mov %dl,0x2f
// （P33/P136：bool 字段接收 bool 值直赋，禁止三元归一化/uchar 中转）。
#include "WongWorkSecurityCard_A/setActivate.inc"
}

// W 0x08265296：同 setActivate 形态，写 +0x30。
#include "WongWorkSecurityCard_A/setCertified.inc"
}

// W 0x084ebd9a：char 形参经 movzbl 截断直写字节 +0x31
// （字段为 uchar 字节存储语义；bool 字段会让 char→bool 走 cmpb/setne 规范化，
//  与 ORIG movzbl 直写不符，故保持 uchar）。
#include "WongWorkSecurityCard_A/setCertFlag.inc"
}

// W 0x0822ef64：int 参数写入 +0x44。
namespace {
struct CSecurityCard44View { char m_pad[0x44]; int m_requestState44; };
}
#include "WongWorkSecurityCard_A/setRequestState.inc"
}

// W 0x0822ef72：movzbl 0x31; cmp $0x1; jne L; mov $1; jmp; L: mov $0 —— 分支式 bool 返回。
#include "WongWorkSecurityCard_A/isCertFlagOn.inc"
}

// W 0x0822ef32：mov 0x48; lea 0x1; 写回; 再读返（++(+0x48) 后返回新值）。
#include "WongWorkSecurityCard_A/incCancelCnt.inc"
}

// T 0x085fde9c setSecurityCardInfo(char const*, bool, bool, int, int, int, char*)：
//   bool 形参截断暂存后直赋 bool 字段（ORIG movzbl 回读直写，无规范化）；
//   strncpy(this+8, serial, 0x27);
//   this[0x2f]=activate; this[0x30]=certified; *(int*)(this+0x34)=certCnt;
//   this[0x31]=(unsigned char)certFlag; *(int*)(this+0x38)=cancelCnt;
//   if (answers != NULL) { strncpy(this+0x3c, answers, 5); 每字节 -'0'（展开 4 次）；}
#include "WongWorkSecurityCard_A/setSecurityCardInfo.inc"
}

} // namespace WongWork
