// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Method/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "Method.h"

namespace nsl {

const char* IMethod::sm_szErrorMsg1 = "FileCrypt ERROR: Encryption/Decryption Object not Initialized!";
const char* IMethod::sm_szErrorMsg2 = "FileCrypt ERROR: Illegal Operation Mode!";
const char* IMethod::sm_szErrorMsg3 = "FileCrypt ERROR: Illegal Padding Mode!";
const char* IMethod::sm_szErrorMsg4 = "FileCrypt ERROR: No Key DataSpecified!";
const char* IMethod::sm_szErrorMsg5 = "FileCrypt ERROR: Key Data Length should be > 0!";
const char* IMethod::sm_szErrorMsg6 = "FileCrypt ERROR: Cannot open File ";
const char* IMethod::sm_szErrorMsg7 = "FileCrypt ERROR: The same File for Input and Output ";
const char* IMethod::sm_szErrorMsg8 = "FileCrypt ERROR: File ";
const char* IMethod::sm_szErrorMsg9 = " cannot be Correctly Decrypted!";
const char* IMethod::sm_szErrorMsg10 = " cannot be Correctly Decrypted!";

#include "Method/IMethod.inc"
}

#include "Method/dtor_IMethod.inc"
}

#include "Method/Xor.inc"
}

#include "Method/SetMode.inc"
}

#include "Method/SetPadding.inc"
}

#include "Method/GetKeyLength.inc"
}

#include "Method/GetBlockSize.inc"
}

#include "Method/GetMode.inc"
}

#include "Method/GetPadding.inc"
}

#include "Method/Pad.inc"
}

#include "Method/BytesToWord.inc"
}

#include "Method/WordToBytes.inc"
}

} // namespace nsl
