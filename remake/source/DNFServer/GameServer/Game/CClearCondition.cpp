// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CClearCondition/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CClearCondition.h"

#include "CClearCondition/CClearCondition.inc"
{
    // ORIG 0x82a4342：m_vec(std::vector<int> @+8) 默认构造后 call Clear()（0x82a438a，
    // 出线）清 +0/+4/+0x14 并 vec.clear。此处不得内联 Clear 体——ORIG ctor 是
    // `vector<int>::C1` + `call CClearCondition::Clear`，内联会多出逐字段 mov。
    Clear();
}

#include "CClearCondition/dtor_CClearCondition.inc"
{
}

#include "CClearCondition/Clear.inc"
{
    m_flag = 0;
    m_count = 0;
    m_vec.clear();
    m_pCondVec = 0;
}

#include "CClearCondition/Empty.inc"
{
    return m_vec.empty();
}

#include "CClearCondition/IsClearCondition.inc"
{
    return m_flag <= m_count;
}

// [b28d-51] 判词 N（ORIG 0x082fefce，N82 族）：switch 决策树 + 帧槽方向差；
// 语义/调用集齐，byte 层封顶。
// [F2d-C05② 甄别] 无「两具名局部」比较（比较对象为 enum/int 形参 + 指针解引用）⇒ F2 不适用；
//   A/B 实测删 `(int)type` 零效（show_func_diff 两态同形 orig=110 our=112 gap=-2 ndiff=18；
//   ORIG-only={jg×2} vs OURS-only={mov,jle×2,jmp} = 块排布镜像 N82/N93附）⇒ 回滚维持原形。
#include "CClearCondition/ClearCondition.inc"
{
    if ((int)type < 0 || (int)type > 4)
    {
        return false;
    }
    // ORIG 0x82fefce：+0x14 指向 vector<DungeonClearCondition>（外部条件表），
    // 与 m_vec 同步遍历；条目 m_0/m_4 匹配 type/idx 时，若 m_8 > 当前计数
    // 则计数自增并 m_count++，否则返回 false；结尾 (m_flag <= m_count)。
    std::vector<DungeonClearCondition>::iterator itd = m_pCondVec->begin();
    std::vector<DungeonClearCondition>::iterator endd = m_pCondVec->end();
    std::vector<int>::iterator itv = m_vec.begin();
    std::vector<int>::iterator endv = m_vec.end();
    while (itd != endd && itv != endv)
    {
        DungeonClearCondition* d = &*itd;
        int* v = &*itv;
        // [实测回退] 操作数序交换（(int)type == d->m_0）零效（d=10 不变，反引入
        // `mov -0x10(%ebp),%eax` 新装载）⇒ 维持原序；残余为 N82 块排布族。
        if (d->m_0 == (int)type && d->m_4 == idx)
        {
            if (d->m_8 > *v)
            {
                ++(*v);
                ++m_count;
            }
            else
            {
                return false;
            }
        }
        ++itd;
        ++itv;
    }
    if (m_flag > m_count)
    {
        return false;
    }
    return true;
}

// [b28d-51] 判词 N（ORIG 0x082fef12，N82/N86 族）：vector 迭代器/临时槽
// 阶梯方向差（ORIG 帧 0x28 vs 4.4 0x24+ebx）；语义/调用集齐，byte 层封顶。
#include "CClearCondition/SetDungeonClearConditionVector.inc"
    std::vector<DungeonClearCondition>& vec)
{
    // ORIG 0x82fef12：Clear 后若 vec 非空，+0x14 存 vec 地址，并对每个
    // 条目：m_flag(+0x00) += 条目 m_8 字段，同时向 m_vec push 一个 0。
    Clear();
    if (!vec.empty())
    {
        m_pCondVec = &vec;
        for (std::vector<DungeonClearCondition>::iterator it = vec.begin();
             it != vec.end(); ++it)
        {
            m_flag += it->m_8;
            m_vec.push_back(0);
        }
    }
}
