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
        // [M4S2 · P211 类型档旋钮] ORIG 0x082ff06b 三条形
        //   `mov -0x10(%ebp),%eax; mov (%eax),%eax; cmp 0xc(%ebp),%eax`
        //   ⇒ 字段侧是**同型 enum 左值**（op1 形参留内存直比）。我方 m_0 声明为
        //   int ⇒ `int == enum` 需转换 ⇒ 反产 `mov (%eax),%edx; mov 0xc(%ebp),%eax;
        //   cmp %eax,%edx`（+1 条）。实测：删 (int)/换序均零效；probe
        //   /tmp/m4s2_probe/pr_cmp{,2}.cpp 中 f2（字段 enum）与 f7/f8（同址 enum
        //   视图）产 ORIG 逐条形，f7/f8 不引入额外槽。
        //   ⇒ 本处按 P211 以同址 enum 视图取值；**建议 .h 侧把
        //     DungeonClearCondition::m_0 声明为 ENUM_DUNGEON_CLEAR_CONDITION_TYPE**
        //     （属他泳道写域，仅上报）。
        if (*(ENUM_DUNGEON_CLEAR_CONDITION_TYPE*)d == type && d->m_4 == idx)
        {
            // [L6S2b · P222/P194 块序翻正] ORIG 为 `cmp %eax,%edx; jg <自增块>`
            // + 失败块 fall-through（0x082ff08d/08f/094：`jg +0xcb` → `mov $0x0,
            // %eax; jmp 出口`），即否定早返形；旧译 if/else 被 4.4 反产
            // `jle <失败>` + 自增 fall-through + 多一条 `jmp`（gap −2 的来源）。
            // 探针 /tmp/L6S2_probe/pr_pol.cpp：f2(`if (m8 <= *v) return 0; ++…`) 产
            // jg+失败块 fall-through（= ORIG），f1/f3 均产 jle 反形。
            if (d->m_8 <= *v)
            {
                return false;
            }
            ++(*v);
            ++m_count;
        }
        ++itd;
        ++itv;
    }
    // [M4S2 · P222 镜像] ORIG 0x082ff116..0x082ff121 形为
    //   `cmp %eax,%edx; jg +0x153; mov $0x1,%eax; jmp +0x158; +0x153: mov $0x0,%eax`
    //   ⇒ then 块 = return 1（fall-through）、else 块 = return 0（越过 jmp 落点）。
    //   旧译 `if (m_flag > m_count) return false; return true;` 反产
    //   `jle +0x155; mov $0x0; jmp; +0x155: mov $0x1`（镜像 + 多 1 条 jmp），
    //   改为 if/else 正条件形后与 ORIG 逐条一致。
    if (m_flag <= m_count)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// [b28d-51] 判词 N（ORIG 0x082fef12，N82/N86 族）：vector 迭代器/临时槽
// 阶梯方向差（ORIG 帧 0x28 vs 4.4 0x24+ebx）；语义/调用集齐，byte 层封顶。
#include "CClearCondition/SetDungeonClearConditionVector.inc"
    std::vector<DungeonClearCondition>& vec)
{
    // ORIG 0x82fef12：Clear 后若 vec 非空，+0x14 存 vec 地址，并对每个
    // 条目：m_flag(+0x00) += 条目 m_8 字段，同时向 m_vec push 一个 0。
    // [L6S2b · P13 end 缓存] ORIG 在循环外各求值一次 begin()/end()
    //（0x082fef42..f54 → it@-0x14 / 0x082fef57..f69 → end@-0x18），循环条件只调
    // `__gnu_cxx::operator!=`（0x082fefc2）；旧译 `it != vec.end()` 每次迭代重调
    // end()（+3 条/次、临时槽 -0x10 挤占）。语义不变，仅求值次数对齐。
    Clear();
    if (!vec.empty())
    {
        m_pCondVec = &vec;
        std::vector<DungeonClearCondition>::iterator it = vec.begin();
        std::vector<DungeonClearCondition>::iterator itEnd = vec.end();
        while (it != itEnd)
        {
            // [L6S2b · P167 调用点判据] p167 在基线 .o 上给出 MISSING
            // `_ZNK9__gnu_cxx17__normal_iteratorIP21DungeonClearCondition...EdeEv`(−1) +
            // EXTRA `...ptEv`(+1) ⇒ ORIG 用 `*it`（deEv），旧译 `it->m_8` 走 ptEv。
            // [M4S2 · P183 具名指针物化] ORIG 0x082fef6e..f79 先 `call deEv` 并把
            //   结果落槽 -0xc，随后 `add %eax,%edx` 双语句形（op0=this->m_flag 先
            //   加载进 edx、op1=p->m_8 后加载）；旧译单语句 `m_flag += (*it).m_8`
            //   使 op1 内含 call ⇒ 编译器把 m_flag 提升进 callee-saved %ebx 跨 call
            //   保活（多 push/pop、帧 0x24+ebx、`lea (%ebx,%eax,1)`）⇒ 具名指针
            //   两语句形消去该跨 call 保活。
            DungeonClearCondition* pCond = &*it;
            m_flag += pCond->m_8;
            m_vec.push_back(0);
            ++it;
        }
    }
}
