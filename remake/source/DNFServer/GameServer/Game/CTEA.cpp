// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CTEA/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— 全局命名空间 CTEA（G7 加密栈，算法正确即可，不要求 identical）。
// 参照 docs/class_func_reports/（G7 报告：CTEA 为 TEA 分组密码，CBC chain+Signature）。
#include <cstring>
#include "ChecksumAlgo.h"   // CSHA（ORIG 全局类，AddData/FinalDigest）
#include "CEnvironment.h"

// [A31-GI · P338/P327] 补 <iostream> 恢复 file-scope 动态初始化 __ioinit；sm_chain0 定义
// 前移为本 TU 首个强外部符号 ⇒ GI 键名 `_GLOBAL__I__ZN4CTEA9sm_chain0E`。
// （另一 gap 键名 `_ZN7IMethodC2Ev` 属 ORIG 它的 TU ⇒ 本方同 TU 合并互斥，不可同时对齐。）
#include <iostream>

const char CTEA::sm_chain0[8] = {0, 0, 0, 0, 0, 0, 0, 0};

// ---- IMethod 抽象基类（G7 加密模式基类，算法正确即可）----
#include "CTEA/IMethod.inc"
}

#include "CTEA/dtor_IMethod.inc"
}

// ---- 全局 IMethod 工具成员（ORIG T 0x080b459e/0x080b4616；声明 CEnvironment.h:42-43 static，
// ServerLib 侧为 nsl::IMethod 不同 mangled ⇒ game 全局形定义须落本 TU——Main 按 DBMW/Method.cpp
// :134-149 语义复制，G7 算法豁免面，big-endian 字组双向转换）----
#include "CTEA/BytesToWord.inc"
}
#include "CTEA/WordToBytes.inc"
}

#include "CTEA/CTEA.inc"
}

#include "CTEA/dtor_CTEA.inc"
}

// ORIG _ZN4CTEA9SignatureEPc T @0x080c35ec：
//   char buf[23] 内建清零；strcpy "TEA"；接 m_key(m_keyLength)；sprintf "%d%d"
//   拼 m_mode/m_padding；CSHA 摘要写出。
#include "CTEA/Signature.inc"
}

// ---- ORIG _ZN4CTEA12EncryptBlockEPKhPh（0x080c3724）TEA 分组加密核心 ----
#include "CTEA/EncryptBlock.inc"
}

// ---- ORIG _ZN4CTEA12DecryptBlockEPKhPh（0x080c3804）TEA 分组解密核心 ----
#include "CTEA/DecryptBlock.inc"
}

// ---- ORIG _ZN4CTEA7EncryptEPKcPcj（0x080c38e4）：CBC 链式，三模式 ----
#include "CTEA/Encrypt.inc"
}

// ---- ORIG _ZN4CTEA7DecryptEPKcPcj（0x080c3afe）：CBC 链式，三模式 ----
#include "CTEA/Decrypt.inc"
}

// ORIG _ZN4CTEA10ResetChainEv T @0x080c35a8：
//   !m_bInit → 0x70000005；否则 chain(+0x38) 复制回 chain2(+0x40)，
//   长度取 m_blockSize(this+0x08)，成功返 0x6fffffff。
#include "CTEA/ResetChain.inc"
}

// [b23S3C] ORIG T 0x080c339c 全量重转写：旧形为猜测版（缺 +0x14 写/双 memcmp 复用
// 判定/BytesToWord×4），参数语义照 ORIG：第 3 参是与 m_chain2 比较的另一 key，
// 第 4/5 参分别落 +0x10/+0x14 并做 0..2 值域守卫。
#include "CTEA/Initialize.inc"
}


