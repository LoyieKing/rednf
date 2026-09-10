// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 RewardUserList/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN15CRewardUserList6insertEjii`（ORIG 0x81bbf66）；insert 已前移为首强符号。
#include <iostream>

#include "RewardUserList.h"

// [GI-ioinit L21] 首强符号前移（P327）：ORIG 0x081bb690。
#include "RewardUserList/insert.inc"
{
    if (!is_exist(key)) {
        RewardUserData d;   // 默认构造：a=b=0，mark=0
        d.a = a;
        d.b = b;
        d.mark = false;     // ORIG 在 ctor 后再次显式清零 mark（offset 0x8）
        m_map.insert(std::make_pair(key, d));
        m_index.push_back(key);
        return true;
    }
    return false;
}

#include "RewardUserList/CRewardUserList.inc"
{
    // map(+0x00) 与 deque(+0x18) 由成员默认构造（ORIG ctor 0x081bbfb6 显式构造二者）
}

#include "RewardUserList/RewardUserData.inc"
{
    a = 0;
    b = 0;
    mark = false;
}

// ORIG 0x081bb73c：find→iterator 临时→const_iterator 拷贝→end→拷贝→
// operator== 取反汇聚（test/je/mov $0/jmp/mov $1）。
#include "RewardUserList/is_exist.inc"
{
    std::map<unsigned int, RewardUserData>::const_iterator it =
        m_map.find(key);
    return it == m_map.end() ? false : true;
}

#include "RewardUserList/delete_mark.inc"
{
    std::map<unsigned int, RewardUserData>::iterator it = m_map.find(key);
    if (it != m_map.end()) {
        it->second.mark = true;
        return true;
    }
    return false;
}

#include "RewardUserList/dtor_CRewardUserList.inc"
{
    // 成员按声明反序自动析构：deque(+0x18) → map(+0x00)（ORIG D1/D2 0x081bbffc 同序）
}

#include "RewardUserList/clear.inc"
{
    m_map.clear();     // ORIG 先 map（0x081bc04e）
    m_index.clear();   // 后 deque（0x081bc05c）
}

#include "RewardUserList/get_index_size.inc"
{
    return m_index.size();   // ORIG 0x081bc064
}