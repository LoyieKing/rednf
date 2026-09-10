// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CBingoTest/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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

// BingoEventLink4.cpp 定义的 ORIG 全局位工具（强符号，仅声明引用）：
//   _Z13isSetBit_Uintji T 0x0889232e：(value & (1<<bit)) != 0
//   _Z11setBit_UintRjbi T 0x088922d7：set ? |= : &=~
#include "CBingoTest/isSetBit_Uint.inc"
#include "CBingoTest/setBit_Uint.inc"
#include "CBingoTest/CBingo.inc"
}

// CBingo::~CBingo @ 0x080ca8f0（docs/class_func_reports/CBingo/~CBingo.md 全文已读）：
//   try { ~shared_ptr<CMTRand>(this+0x10) } catch { ~vector<int>(this); rethrow }
//   → ~vector<int>(this)。空体即编译器按逆声明序析构成员：
//   m_today(int)→m_rand(shared_ptr dtor)→m_fieldC(int)→m_boards(vector dtor)，
//   shared_ptr 先于 vector，与 ORIG 调用序一致。
#include "CBingoTest/dtor_CBingo.inc"
}

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

// calBoradMatchLine @ 0x080cab5a（docs/class_func_reports/CBingo/calBoradMatchLine.md）：
// 统计 25 位棋盘的连线数（5 行 + 5 列 + 2 对角线；对角线1 = bit[r*6]，
// 对角线2 = bit[(r+1)*4]）。
// [DualTailor 2026-09-09 复核] 旧判 N82「槽序不同 ⇒ 封顶」不成立：帧/槽差根因是**源声明序**——
// ORIG 另有写后不读的 int 局部（=5，报告反编译 local_28）且 rowCount/colCount 在循环外声明、
// 循环体首重置；照此改写后帧 $0x64、槽 -0x28..-0xc、尺寸 0x1f2 与 ORIG 全等（DIFF→AE）。
#include "CBingoTest/calBoradMatchLine.inc"
}

// calNewValue @ 0x080cad4c（docs/class_func_reports/CBingo/calNewValue.md）：
// idx 在当日棋盘上首次标记并返回新连线数。
#include "CBingoTest/calNewValue.inc"
}

// getToday @ 0x080ca936（docs/class_func_reports/CBingo/getToday.md）：返回 this+0x18。
#include "CBingoTest/getToday.inc"
}

// getToday(long,int) @ 0x080ca942（docs/class_func_reports/CBingo/getToday_080ca942.md）：
// 当前秒数未过当日小时槽（hourLimit，即 G_CEnvironment+0x37c 的 m_fatigueTime）时取
// 昨天日号，否则取今天日号（跨零点结算窗口）。localtime_r 双展开。
#include "CBingoTest/getToday_1.inc"
}

 // ORIG 反汇编：if (day>0 && day<=0x1f) 短路布局（jg/jle 同跳 body）。
#include "CBingoTest/resetBoard.inc"
}

// resetQuiz @ 0x080caa64（docs/class_func_reports/CBingo/resetQuiz.md）：
// 按日取 stBingoScript.m_map0（CDataManager+0xb568，map<int,int>）当日 quiz 值写入
// +0x0c（m_fieldC）。day 越界/无配置返回 0。
 // ORIG 反汇编：if (day>0 && day<=0x1f) 短路布局（jg/jle 同跳 body）。
#include "CBingoTest/resetQuiz.inc"
}

#include "CBingoTest/getBingoRandomVaule.inc"
}


// resetQuiz @ 0x080caa64（docs/class_func_reports/CBingo/resetQuiz.md）：
// 按日取 stBingoScript.m_map0（CDataManager+0xb568，map<int,int>）当日 quiz 值写入
// +0x0c（m_fieldC）。day 越界/无配置返回 0。
// getTodayBoard @ 0x080caec4（docs/class_func_reports/CBingo/getTodayBoard.md）：
// 返回 this（空函数体）。
#include "CBingoTest/getTodayBoard.inc"
}

// resetUserData @ 0x080cadfe（docs/class_func_reports/CBingo/resetUserData.md）：
// BingoData::clear(data)。BingoData::clear 定义属 CUser.cpp 域批次（U 引用）。
#include "CBingoTest/resetUserData.inc"
}

#include "CBingoTest/CBingoTest.inc"
}

#include "CBingoTest/dtor_CBingoTest.inc"
}

#include "CBingoTest/run.inc"
}

#include "CBingoTest/getToday_2.inc"
}

#include "CBingoTest/getTimeFromString.inc"
}

#include "CBingoTest/resetBoard_1.inc"
}

#include "CBingoTest/resetQuiz_1.inc"
}

#include "CBingoTest/calBoradMatchLine_1.inc"
}

#include "CBingoTest/calNewValue_1.inc"
}

#include "CBingoTest/getBingoRandomVaule_1.inc"
}
