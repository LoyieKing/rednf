// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DNFLexWrapperStream/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// DNFLexWrapper stream 访问方法（G4 脚本批次；ORIG 地址与汇编证据见
// docs/class_func_reports/DNFLexWrapper.md）
// ----------------------------------------------------------------------------
// 本 TU 提供以下包装器方法（其余 DNFLexWrapper 方法分别由 DNFLexWrapper.cpp、
// DNFLexWrapperLoadStream.cpp 提供）：
//   getInputStream()        _ZN13DNFLexWrapper14getInputStreamEv      ORIG 0x8ace900
//   getInputStreamBuffer()  _ZN13DNFLexWrapper20getInputStreamBufferEv ORIG 0x8ace970
//   getInputStreamSize()    _ZN13DNFLexWrapper18getInputStreamSizeEv   ORIG 0x8ace930
//   getStreamName()         _ZN13DNFLexWrapper13getStreamNameEv        ORIG 0x8ace8a0
//   closeInputStream()      _ZN13DNFLexWrapper16closeInputStreamEv     ORIG 0x8acea70
//   setStringStreamData()   _ZN13DNFLexWrapper19setStringStreamDataEPN6DNFLex13stream_data_tEPcj
//                                                        ORIG 0x8ace9b0
//   switchInputStream()     _ZN13DNFLexWrapper17switchInputStreamEPKcS1_PN6DNFLex13stream_data_tE
//                                                        ORIG 0x8aceaa0
//   switchChannelInputStream() _ZN13DNFLexWrapper24switchChannelInputStreamEPKcPN6DNFLex13stream_data_tE
//                                                        ORIG 0x8acf060
//
// 这些方法按 ORIG 语义在 lexType_ 上分流并转发到底层词法引擎
// （DNFLex::GetInputStream / CompiledDNFLex::GetInputStream /
// DNFLex::getStreamName / DNFLex::CloseInputStream /
// CompiledDNFLex::CloseInputStream），引擎符号由词法引擎批次提供；
// 本 TU 仅按 ORIG ABI 声明并封装，不做固定返回值伪实现。
// ============================================================================

#include <cstdarg>
#include <cstdio>
#include <new>
#include <strstream>
#include <string>

#include "DNFLexWrapper.h"

const char* toMbcs(const char* src);

// ORIG 变参格式化助手：弱模板 _Z10ss_sprintfILj1024EEiRAT__cPKcz @0x8acf0e0
// （紧接本 TU switchChannelInputStream 之后，自然宿主即本文件）。ORIG 体为
// ss_vsprintf(buffer, N, format, ap)；ss_vsprintf @0x8ad457c 的语义：
// n=vsnprintf(buf,size,fmt,ap)；size>n 返回 n，否则清 buf[0] 返回 0（截断
// 报错形）。声明不定义，宿主属字符串工具批次。
#include "DNFLexWrapperStream/ss_vsprintf.inc"

template <unsigned int N>
#include "DNFLexWrapperStream/ss_sprintf.inc"
}

// getInputStream（ORIG 0x8ace900）：仅 NORMAL 分流到 DNFLex::GetInputStream，
// 取其返回流对象偏移 +0x8 的 istream*；其它 lexType 返回 0。
#include "DNFLexWrapperStream/getInputStream.inc"
}

// getInputStreamBuffer（ORIG 0x8ace970）：NORMAL 取 DNFLex stream_t +0x10，
// COMPILED 取 CompiledDNFLex stream_t +0x0c；其它返回 0。无空指针护栏。
#include "DNFLexWrapperStream/getInputStreamBuffer.inc"
}

// getInputStreamSize（ORIG 0x8ace930）：NORMAL 取 DNFLex stream_t +0x0c；
// COMPILED 取 CompiledDNFLex stream_t +0x08；其它返回 -1。无空指针护栏。
#include "DNFLexWrapperStream/getInputStreamSize.inc"
}

// getStreamName（ORIG 0x8ace8a0）：tstring 返回（按 DNFLexWrapper.cpp
// getLastTokenDataAsString 的 by-value std::string 惯例）：lexType_ != 0 时
// 返回 "NONE"；NORMAL 时委托 DNFLex::getStreamName。
// b28d-21 + E06 复测: NEAR 封顶 — 29=29 助记符序列全同，唯一残差为 lexType_
// 载入寄存器（ORIG `mov 0x14(%eax),%edx; test %edx,%edx` vs 4.4.x `%ecx`）。
// E06 实测矩阵：①本 TU 路由 c6446r -O2 → %ecx；②c6root 4.4.7 全旗标 -O2（无
// 路由改写）→ %ecx；③源码形穷举 `lexType_ != 0` / Yoda `NORMAL != lexType_` /
// 三目条件式（此处反劣为 DIFF，已回退）→ 均 %ecx。⇒ 4.4 分配器恒选 %ecx，
// 属 N7 跨版本寄存器角色差（P321-T5：寄存器角色不改类型/源码），无源码旋钮。
// ORIG 0x8ace8a0。
#include "DNFLexWrapperStream/getStreamName.inc"
}

// closeInputStream（ORIG 0x8acea70）：依次调用 DNFLex::CloseInputStream 与
// CompiledDNFLex::CloseInputStream。
#include "DNFLexWrapperStream/closeInputStream.inc"
}

// setStringStreamData（ORIG 0x8ace9b0）：用 buffer/bufferSize 构造 istrstream
// 填充 stream_data_t；bufferSize==0 时先释放 buffer 再以空串+长度 1 构造
// （size=1，buffer 槽不写）。恒返回 true。
#include "DNFLexWrapperStream/setStringStreamData.inc"
}

// switchInputStream（ORIG 0x8aceaa0）：按流内容魔数判断编译态/文本态：
//   - s==0 时先 clear 局部 stream_data_t 并 LoadStream；失败经 lexerError_
//     回调报错返回 0；
//   - size>1 时读 2 字节魔数 0xd0b0 → COMPILED：转发 CompiledDNFLex::
//     SwitchInputStream（buffer 为读取区或新分配拷贝；若用 sd->stream 读则
//     完成后释放 istream）；否则 NORMAL：转发 DNFLex::SwitchInputStream。
// b28d-21 + E06 复测: NEAR 封顶 — 120=120 助记符序列全同，唯一实质残差为
// lexerError_（0x6108(%esi)）载入寄存器（ORIG `mov 0x6108(%esi),%ebx; test %ebx,
// %ebx` vs 4.4.x `%edx`）。E06 实测矩阵：①本 TU 路由 c6446r -O2 → %edx；
// ②c6root 4.4.7 全旗标 -O2（无路由改写）→ %edx。⇒ N7/P321-T5 跨版本寄存器
// 角色差，无源码旋钮（后续同址 `call *0x6108(%esi)` 两侧逐条一致，证明语义等价）。
// 另余 ORIG `nop; lea 0x0(%esi,%eiz,1),%esi` 与本方 `lea…; nop` 的对齐填充顺序差
// （§6.1 填充豁免）。ORIG 0x8aceaa0。
#include "DNFLexWrapperStream/switchInputStream.inc"
}

// switchChannelInputStream（ORIG 0x8acf060）：s==0 时先 clear 局部
// stream_data_t 并 setFileStreamData；置 lexType_=NORMAL 后转发
// DNFLex::SwitchInputStream("", filename, s)。
#include "DNFLexWrapperStream/switchChannelInputStream.inc"
}
