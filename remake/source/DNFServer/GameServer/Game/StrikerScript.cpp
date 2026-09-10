// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 StrikerScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <map>
#include <vector>

#include "StrikerScript.h"
#include "DNFLexWrapper.h"        // ScanInt(bool*)

class STScriptFileList;
// ORIG T 0x088bbd77
// _Z22loadRDARScriptListFileP16STScriptFileListPKcS2_PSt3mapIiiSt4lessIiESaISt4pairIKiiEEE
// 定义 STScriptFileList.cpp。本 TU 按 ORIG 调用点语义以 bool 视图声明
// （importComboList 的 ORIG 汇编为 xor $1 直接取反，P22：被调函数按 _Bool
// 使用；返回类型不参与 mangling，链接到 STScriptFileList.cpp 的 int 定义，
// 其返回恒为 0/1，ABI 兼容）。
#include "StrikerScript/loadRDARScriptListFile.inc"
                            const char* listPath, std::map<int, int>* unused);

// ============================================================================
// StrikerScript 查询/导入补充（报告全文转写）
// ============================================================================

// ORIG T 0x08a9e79c：m_fileList(+0x90) 直接透传 loadRDARScriptListFile，
// dir="" 且 listPath="etc/LinkSystem/strikercombo.lst"，第 4 参 0；
// 返回非 0 才为 true（P22 xor $1/test 反相形：失败分支 mov $0）。
#include "StrikerScript/importComboList.inc"
{
    if (!loadRDARScriptListFile(reinterpret_cast<STScriptFileList*>(m_fileList),
                                "", "etc/LinkSystem/strikercombo.lst", 0))
        return false;
    return true;
}

// ORIG T 0x08a9e628：两个 ScanInt((bool*)0) 依次写入 +0x64/+0x68。
#include "StrikerScript/importStrikerLevelPenalty.inc"
{
    m_field64 = ScanInt((bool*)0);
    m_field68 = ScanInt((bool*)0);
}

// ORIG T 0x08a9ea96：getStrikerSkillVector(first,second)；空指针返回 0；
// 循环 operator[] 命中 m_field00==skillIndex 返回 m_field08（+8），未命中 0。
// 循环变量 unsigned（Ghidra uint local_10；ORIG seta 无符号判续）；
// vec 先置 0 再赋值（ORIG call 前 movl $0,-0x10 实证）。
#include "StrikerScript/getPrecedeSkillIndex.inc"
                                        int skillIndex)
{
    std::vector<StrikerSkill>* vec = 0;
    vec = getStrikerSkillVector(firstGrowType, secondGrowType);
    if (vec == 0)
        return 0;
    for (unsigned int i = 0; i < vec->size(); ++i)
    {
        if ((*vec)[i].m_field00 == skillIndex)
            return (*vec)[i].m_field08;
    }
    return 0;
}

// ORIG T 0x08a9e97a：外层 i=0..1（setle 有符号判续）；i==0 用
// getStrikerSkillVector(first,second)，否则 (first,0)；空返回 0；
// 迭代器遍历（begin/end/operator->/后置 it++）命中 m_field00==skillIndex
// 返回 m_field04（+4）。
#include "StrikerScript/getComboIndex.inc"
                                 int skillIndex)
{
    std::vector<StrikerSkill>* vec = 0;
    for (int i = 0; i <= 1; ++i)
    {
        if (i == 0)
            vec = getStrikerSkillVector(firstGrowType, secondGrowType);
        else
            vec = getStrikerSkillVector(firstGrowType, 0);
        if (vec == 0)
            return 0;
        for (std::vector<StrikerSkill>::iterator it = vec->begin();
             it != vec->end(); it++)
        {
            if (it->m_field00 == skillIndex)
                return it->m_field04;
        }
    }
    return 0;
}

// ORIG T 0x08a9eb3a：遍历 m_macroCommands(+0x94)，短路双条件
// （first==m_field00 且 command==m_field04，各一次 operator[]），
// 命中返回 m_type(+8)；未命中 0。循环变量 unsigned（Ghidra uint；
// ORIG 循环头 seta 无符号判续）。比较左侧 first 先求值（ORIG 循环体
// 首条 movswl first→ebx 跨 op[] 调用缓存）。
#include "StrikerScript/getAddCommand.inc"
{
    for (unsigned int i = 0; i < m_macroCommands.size(); ++i)
    {
        if (firstGrowType == m_macroCommands[i].m_field00 &&
            command == m_macroCommands[i].m_field04)
        {
            return m_macroCommands[i].m_type;
        }
    }
    return 0;
}

// ORIG T 0x08a9ebee：getStrikerSkillVector(first,0)；空指针返回 0；
// 迭代器遍历命中 m_field00==skillIndex 返回 1，否则 0（无 setne 规范化 → int）。
#include "StrikerScript/isCommonSkill.inc"
{
    std::vector<StrikerSkill>* vec =
        getStrikerSkillVector(firstGrowType, 0);
    if (vec == 0)
        return 0;
    for (std::vector<StrikerSkill>::iterator it = vec->begin();
         it != vec->end(); it++)
    {
        if (it->m_field00 == skillIndex)
            return 1;
    }
    return 0;
}
