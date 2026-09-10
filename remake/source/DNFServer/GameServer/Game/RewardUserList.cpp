// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 RewardUserList/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— CRewardUserList（奖励用户列表基础容器，见 CRewardUserList.md）
//   RewardUserData::RewardUserData()  0x081bbf82：a=b=0，mark=0
//   CRewardUserList::CRewardUserList() 0x081bbfb6：构造 map 于 +0x00、deque 于 +0x18
//   insert     0x081bb690：is_exist 不命中才插入
//   is_exist   0x081bb73c：find != end
//   delete_mark 0x081bb7bc：find 命中置 mark=1，返回 true
// ============================================================================

// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN15CRewardUserList6insertEjii`（ORIG 0x81bbf66）；insert 已前移为首强符号。
#include <iostream>

#include "RewardUserList.h"

// [GI-ioinit L21] 首强符号前移（P327）：ORIG 0x081bb690。
#include "RewardUserList/insert.inc"
}

#include "RewardUserList/CRewardUserList.inc"
}

#include "RewardUserList/RewardUserData.inc"
}

// ORIG 0x081bb73c：find→iterator 临时→const_iterator 拷贝→end→拷贝→
// operator== 取反汇聚（test/je/mov $0/jmp/mov $1）。
#include "RewardUserList/is_exist.inc"
}

#include "RewardUserList/delete_mark.inc"
}

#include "RewardUserList/dtor_CRewardUserList.inc"
}

#include "RewardUserList/clear.inc"
}

#include "RewardUserList/get_index_size.inc"
}