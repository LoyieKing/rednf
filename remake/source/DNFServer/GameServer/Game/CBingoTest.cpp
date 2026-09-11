#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CBingoTest/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// CBingoTest.cpp —— CBingoTest 仅 ORIG 批次（docs/class_func_reports/CBingoTest/ 10 报告全文已读）。
// 各函数与报告对应：
//   C1/D1         0x080caecc/0x080caee0：仅调 CBingo::C1/D1（基类构造/析构）。
//   run           0x080caef4：顺序调 getToday/resetBoard/resetQuiz/calBoradMatchLine/
//                 calNewValue/getBingoRandomVaule（Ghidra C 逐项一致）。
//   getToday      0x080caf7c：string 局部 + 6 次 operator=(const char*)，析构收尾。
//   getTimeFromString 0x080caf3e：time_from_string + to_tm + mktime（探针已证同形）。
//   resetBoard/resetQuiz 0x080cb030/0x080cb036：空函数体（push ebp; mov; pop; ret）。
//   calBoradMatchLine 0x080cb03c：6 次立即数写 -0x4（死代码局部，逐条 movl 保留）。
//   calNewValue   0x080cb06e：movl $0,-0x4 一次。
//   getBingoRandomVaule 0x080cb07e：5 个 i<100 循环，分别调 CBingo::getBingoRandomVaule
//                 (0xa/0x14/0x1e/0x28/0x32)，返回值写共享局部 -0x20。
// CBingo::getBingoRandomVaule(int) 为基类符号，U 引用（实现归 BingoEventLink4 批）。
#include "CBingoTest.h"

#include "CUser.h"         // BingoData 完整定义（resetUserData 调 data.clear()）

#include <bitset>
#include <ctime>
#include <cstdio>
#include <cstring>
#include <map>

#include "CBingoTest/isSetBit_Uint.inc"
#include "CBingoTest/setBit_Uint.inc"

#include "CBingoTest/CBingo.inc"


#include "CBingoTest/dtor_CBingo.inc"


// getIndexTodayBoard @ 0x080caafa（docs/class_func_reports/CBingo/getIndexTodayBoard.md）：
// 在今日棋盘索引表（m_boards）中查找 value 的下标。
 int CBingo::getIndexTodayBoard(int value)
{
    if (value < 0)
    {
        return -1;
    }
    // ORIG：i 为 int，size() 与 i 作有符号比较（setg）。
    for (int i = 0; i < static_cast<int>(m_boards.size()); ++i)
    {
        if (m_boards[i] == value)
        {
            return i;
        }
    }
    return -1;
}

#include "CBingoTest/calBoradMatchLine.inc"


#include "CBingoTest/calNewValue.inc"


#include "CBingoTest/getToday.inc"
{
    return m_today;
}

#include "CBingoTest/getToday_1.inc"
{
    long yesterday = sec - 0x15180;   // 86400 秒 = 1 天
    struct tm tmToday;
    struct tm tmYest;
    struct tm* today = localtime_r(&sec, &tmToday);
    struct tm* yest = localtime_r(&yesterday, &tmYest);
    if (today->tm_hour >= 0 && hourLimit > today->tm_hour)
    {
        m_today = yest->tm_mday;
    }
    else
    {
        m_today = today->tm_mday;
    }
    return m_today;
}

#include "CBingoTest/resetBoard.inc"


#include "CBingoTest/resetQuiz.inc"


#include "CBingoTest/getBingoRandomVaule.inc"



#include "CBingoTest/getTodayBoard.inc"


#include "CBingoTest/resetUserData.inc"


#include "CBingoTest/CBingoTest.inc"


#include "CBingoTest/dtor_CBingoTest.inc"


#include "CBingoTest/run.inc"


#include "CBingoTest/getToday_2.inc"


#include "CBingoTest/getTimeFromString.inc"


#include "CBingoTest/resetBoard_1.inc"


#include "CBingoTest/resetQuiz_1.inc"


#include "CBingoTest/calBoradMatchLine_1.inc"


#include "CBingoTest/calNewValue_1.inc"


#include "CBingoTest/getBingoRandomVaule_1.inc"

