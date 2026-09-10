// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CNexonMsgTable/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CNexonMsgTable（LinkE2 批）。
// 报告：docs/class_func_reports/CNexonMsgTable/{CNexonMsgTable,GetMessageString}.md
// ctor @0x0858001c：构造 map<int,std::string>，循环 0x1c=28 次插入
//   make_pair(msgs[i].key, msgs[i].str)（ORIG 全局表 msgs D @0x093abb80，
//   stride 0x8 = {int key; const char* str}，0xe0 = 28×0x8）。
// GetMessageString @0x0858010c：find(idx)；it==end → ""（ORIG &DAT_08ca9390），
//   否则 it->second.c_str()。
#include "CNexonMsgTable.h"

#include <cassert>
#include <cstddef>

// ORIG msgs 全局表逐条转写（objdump -s .data @0x093abb80；字符串指针解引用自
// .rodata 0x08ca91xx/0x08ca92xx/0x08ca93xx 区）。键序即 ORIG 存储序。
struct NexonMsgEntry
{
    int key;
    const char* str;
};

NexonMsgEntry msgs[28] =
{
    { 0x00, "nexon_pcroom_msg_00" },
    { 0x01, "nexon_pcroom_msg_01" },
    { 0x02, "nexon_pcroom_msg_02" },
    { 0x03, "nexon_pcroom_msg_03" },
    { 0x04, "nexon_pcroom_msg_04" },
    { 0x05, "nexon_pcroom_msg_05" },
    { 0x06, "nexon_pcroom_msg_06" },
    { 0x0a, "nexon_pcroom_msg_10" },
    { 0x0b, "nexon_pcroom_msg_11" },
    { 0x0d, "nexon_pcroom_msg_13" },
    { 0x0e, "nexon_pcroom_msg_14" },
    { 0x0f, "nexon_pcroom_msg_15" },
    { 0x10, "nexon_pcroom_msg_16" },
    { 0x11, "nexon_pcroom_msg_17" },
    { 0x12, "nexon_pcroom_msg_18" },
    { 0x13, "nexon_pcroom_msg_19" },
    { 0x16, "nexon_pcroom_msg_22" },
    { 0x17, "nexon_pcroom_msg_23" },
    { 0x18, "nexon_pcroom_msg_24" },
    { 0x19, "nexon_pcroom_msg_25" },
    { 0x1a, "nexon_pcroom_msg_26" },
    { 0x1b, "nexon_pcroom_msg_27" },
    { 0x5a, "nexon_pcroom_msg_90" },
    { 0x5b, "nexon_pcroom_msg_91" },
    { 0x5c, "nexon_pcroom_msg_92" },
    { 0x5d, "nexon_pcroom_msg_93" },
    { 0x5e, "nexon_pcroom_msg_94" },
    { 0x5f, "nexon_pcroom_msg_95" },
};

static_assert(sizeof(NexonMsgEntry) == 0x8, "ORIG msgs stride");
static_assert(sizeof(CNexonMsgTable) == 0x18, "ORIG g_nexonMsgTable B size (== sizeof std::map)");

CNexonMsgTable g_nexonMsgTable;   // ORIG B @0x0945b8e4

// [b28d-L14 P209附] ORIG 0x0857ffcc：unsigned 局部循环尾测 4.4 恒折
// `cmpl $0x1b,-0xc` 内存直比（ORIG 先 mov 到寄存器再 cmp+setbe）⇒ 封顶。
#include "CNexonMsgTable/CNexonMsgTable.inc"
}

#include "CNexonMsgTable/GetMessageString.inc"
}

#include "CNexonMsgTable/dtor_CNexonMsgTable.inc"
}
