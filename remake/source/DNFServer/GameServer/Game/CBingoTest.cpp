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

// BingoEventLink4.cpp 定义的 ORIG 全局位工具（强符号，仅声明引用）：
//   _Z13isSetBit_Uintji T 0x0889232e：(value & (1<<bit)) != 0
//   _Z11setBit_UintRjbi T 0x088922d7：set ? |= : &=~
#include "CBingoTest/isSetBit_Uint.inc"
#include "CBingoTest/setBit_Uint.inc"

// CBingo::CBingo @ 0x080ca82c（docs/class_func_reports/CBingo/CBingo.md 全文已读）：
//   vector<int> C1(this) → time(0) → new CMTRand(0x9c8, seed) →
//   shared_ptr<CMTRand>(this+0x10, px) → vector::clear(this) → +0xc=0 → +0x18=0。
#include "CBingoTest/CBingo.inc"
    : m_boards(),
      m_rand(new CMTRand(static_cast<unsigned long>(time(0))))
{
    m_boards.clear();
    m_fieldC = 0;
    m_today = 0;
}

// CBingo::~CBingo @ 0x080ca8f0（docs/class_func_reports/CBingo/~CBingo.md 全文已读）：
//   try { ~shared_ptr<CMTRand>(this+0x10) } catch { ~vector<int>(this); rethrow }
//   → ~vector<int>(this)。空体即编译器按逆声明序析构成员：
//   m_today(int)→m_rand(shared_ptr dtor)→m_fieldC(int)→m_boards(vector dtor)，
//   shared_ptr 先于 vector，与 ORIG 调用序一致。
#include "CBingoTest/dtor_CBingo.inc"
{
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
{
    int lines = 0;
    // [DualTailor 2026-09-09] ORIG 0x080cab68 `movl $0x5,-0x24(%ebp)`：写后不读的死局部
    // （报告反编译列作 local_28 undefined4，仅赋 5 无任何读取）⇒ 无语义作用，保留以对齐槽序/帧。
    int lineMax = 5;
    std::bitset<25> bits(static_cast<unsigned long>(board));
    // [DualTailor 2026-09-09] ORIG 0x080cab81/0x080cab88 在循环外声明 rowCount/colCount，
    // 循环体首（0x080caba9/0x080cabb0）再各写一次 0 ⇒ 原码为「循环外定义 + 体内重置」。
    int rowCount = 0;
    int colCount = 0;
    int diag1 = 0;
    int diag2 = 0;
    for (int r = 0; r < 5; ++r)
    {
        rowCount = 0;
        colCount = 0;
        for (int c = 0; c < 5; ++c)
        {
            if (bits[5 * r + c])
            {
                ++rowCount;
            }
            if (bits[5 * c + r])
            {
                ++colCount;
            }
            if (rowCount < c + 1 && colCount < c + 1)
            {
                break;  // ORIG 早退：行列均已不可能凑满
            }
        }
        if (rowCount == 5)
        {
            ++lines;
        }
        if (colCount == 5)
        {
            ++lines;
        }
        if (bits[6 * r])
        {
            ++diag1;
        }
        if (bits[4 * (r + 1)])
        {
            ++diag2;
        }
    }
    if (diag1 == 5)
    {
        ++lines;
    }
    if (diag2 == 5)
    {
        ++lines;
    }
    return lines;
}

// calNewValue @ 0x080cad4c（docs/class_func_reports/CBingo/calNewValue.md）：
// idx 在当日棋盘上首次标记并返回新连线数。
#include "CBingoTest/calNewValue.inc"
{
    int index = getIndexTodayBoard(idx);
    if (index == -1)
    {
        return 0;
    }
    if (index > 24)  // ORIG：cmp $0x18 / jle
    {
        return -1;
    }
    if (!isSetBit_Uint(board, index))
    {
        setBit_Uint(board, true, index);
        return calBoradMatchLine(static_cast<int>(board));
    }
    return 0;
}

// getToday @ 0x080ca936（docs/class_func_reports/CBingo/getToday.md）：返回 this+0x18。
#include "CBingoTest/getToday.inc"
{
    return m_today;
}

// getToday(long,int) @ 0x080ca942（docs/class_func_reports/CBingo/getToday_080ca942.md）：
// 当前秒数未过当日小时槽（hourLimit，即 G_CEnvironment+0x37c 的 m_fatigueTime）时取
// 昨天日号，否则取今天日号（跨零点结算窗口）。localtime_r 双展开。
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

 // ORIG 反汇编：if (day>0 && day<=0x1f) 短路布局（jg/jle 同跳 body）。
#include "CBingoTest/resetBoard.inc"
{
    // [b28d-7] P171 原码死分支照抄：ORIG `test; jg BODY; mov day; cmp $0x1f;
    // jle BODY; mov$0` = `day<1 && day>0x1f`（永假，永不早退）。旧版德摩根
    // 改写 `!(day>0 && day<=0x1f)` 引入真语义偏差，已修正为原码形。
    if ((day < 1) && (day > 0x1f))
    {
        return 0;
    }
    // [b28d-7] P183/P236：ORIG 0x080ca9xx 每处完整重读 G_CDataManager()->m_bingo.m_map18
    // （lea 0xb580(%eax) 直接入参，无 map 地址局部槽）⇒ 删具名 map 引用。
    std::map<int, std::vector<int> >::iterator it =
        G_CDataManager()->m_bingo.m_map18.find(day);
    // [b28d-7] ORIG `je SKIP_RET0; mov$0; jmp END; SKIP_RET0: 取值` ⇒ 早返 0 形。
    if (it == G_CDataManager()->m_bingo.m_map18.end())
    {
        return 0;
    }
    m_boards.clear();
    m_boards = it->second;
    return 1;
}

// resetQuiz @ 0x080caa64（docs/class_func_reports/CBingo/resetQuiz.md）：
// 按日取 stBingoScript.m_map0（CDataManager+0xb568，map<int,int>）当日 quiz 值写入
// +0x0c（m_fieldC）。day 越界/无配置返回 0。
 // ORIG 反汇编：if (day>0 && day<=0x1f) 短路布局（jg/jle 同跳 body）。
#include "CBingoTest/resetQuiz.inc"
{
    // [b28d-7] P171 原码死分支照抄：ORIG 0x080caa6d `test; jg BODY; mov day;
    // cmp $0x1f; jle BODY; mov$0` = `day<1 && day>0x1f`（永假）。旧版德摩根
    // 改写引入真语义偏差，已修正为原码形。
    if ((day < 1) && (day > 0x1f))
    {
        return 0;
    }
    // [b28d-7] P183/P236：ORIG 0x080caa85 每处完整重读 m_map0（lea 0xb568(%eax) 直入参）。
    std::map<int, int>::iterator it = G_CDataManager()->m_bingo.m_map0.find(day);
    // [b28d-7] ORIG `je SKIP_RET0; mov$0; jmp END; SKIP_RET0: 取值` ⇒ 早返 0 形。
    if (it == G_CDataManager()->m_bingo.m_map0.end())
    {
        return 0;
    }
    m_fieldC = it->second;
    return 1;
}

#include "CBingoTest/getBingoRandomVaule.inc"
{
    unsigned long range = 9;
    int iVar1 = m_rand->randInt(range) + 1;
    iVar1 = iVar1 - 10 + param_1;
    return (iVar1 <= 0 || iVar1 > 0x32) ? 0 : iVar1;
}


// resetQuiz @ 0x080caa64（docs/class_func_reports/CBingo/resetQuiz.md）：
// 按日取 stBingoScript.m_map0（CDataManager+0xb568，map<int,int>）当日 quiz 值写入
// +0x0c（m_fieldC）。day 越界/无配置返回 0。
// getTodayBoard @ 0x080caec4（docs/class_func_reports/CBingo/getTodayBoard.md）：
// 返回 this（空函数体）。
#include "CBingoTest/getTodayBoard.inc"
{
    return this;
}

// resetUserData @ 0x080cadfe（docs/class_func_reports/CBingo/resetUserData.md）：
// BingoData::clear(data)。BingoData::clear 定义属 CUser.cpp 域批次（U 引用）。
#include "CBingoTest/resetUserData.inc"
{
    data.clear();
}

#include "CBingoTest/CBingoTest.inc"
{
}

#include "CBingoTest/dtor_CBingoTest.inc"
{
}

#include "CBingoTest/run.inc"
{
    getToday();
    resetBoard();
    resetQuiz();
    calBoradMatchLine();
    calNewValue();
    getBingoRandomVaule();
}

#include "CBingoTest/getToday_2.inc"
{
    std::string s;
    s = "2012-07-19 05:00:00";
    s = "2012-07-19 06:00:00";
    s = "2012-07-20 12:00:00";
    s = "2012-07-31 11:00:00";
    s = "2012-08-01 05:00:00";
    s = "2012-08-01 07:00:00";
}

#include "CBingoTest/getTimeFromString.inc"
{
    boost::posix_time::ptime pt = boost::posix_time::time_from_string(s);
    std::tm t = boost::posix_time::to_tm(pt);
    std::mktime(&t);
}

#include "CBingoTest/resetBoard_1.inc"
{
}

#include "CBingoTest/resetQuiz_1.inc"
{
}

#include "CBingoTest/calBoradMatchLine_1.inc"
{
    int v = 0x10843f;
    v = 0x14610c7;
    v = 0x1f9110;
    v = 0x1576dd5;
    v = 0x1577dd5;
    v = 0x1ffffff;
}

#include "CBingoTest/calNewValue_1.inc"
{
    int v = 0;
}

#include "CBingoTest/getBingoRandomVaule_1.inc"
{
    int v = 0;
    for (int i = 0; i < 100; ++i)
    {
        v = CBingo::getBingoRandomVaule(0xa);
    }
    for (int i = 0; i < 100; ++i)
    {
        v = CBingo::getBingoRandomVaule(0x14);
    }
    for (int i = 0; i < 100; ++i)
    {
        v = CBingo::getBingoRandomVaule(0x1e);
    }
    for (int i = 0; i < 100; ++i)
    {
        v = CBingo::getBingoRandomVaule(0x28);
    }
    for (int i = 0; i < 100; ++i)
    {
        v = CBingo::getBingoRandomVaule(0x32);
    }
}
