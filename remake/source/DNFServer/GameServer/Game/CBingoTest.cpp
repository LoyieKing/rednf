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
bool isSetBit_Uint(unsigned int value, int bit);
void setBit_Uint(unsigned int& value, bool set, int bit);

// CBingo::CBingo @ 0x080ca82c（docs/class_func_reports/CBingo/CBingo.md 全文已读）：
//   vector<int> C1(this) → time(0) → new CMTRand(0x9c8, seed) →
//   shared_ptr<CMTRand>(this+0x10, px) → vector::clear(this) → +0xc=0 → +0x18=0。
CBingo::CBingo()
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
CBingo::~CBingo()
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
int CBingo::calBoradMatchLine(int board)
{
    std::bitset<25> bits(static_cast<unsigned long>(board));
    int lines = 0;
    int diag1 = 0;
    int diag2 = 0;
    for (int r = 0; r < 5; ++r)
    {
        int rowCount = 0;
        int colCount = 0;
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
int CBingo::calNewValue(unsigned int& board, int idx)
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
    if (isSetBit_Uint(board, index))
    {
        return 0;
    }
    setBit_Uint(board, true, index);
    return calBoradMatchLine(static_cast<int>(board));
}

// getToday @ 0x080ca936（docs/class_func_reports/CBingo/getToday.md）：返回 this+0x18。
int CBingo::getToday()
{
    return m_today;
}

// getToday(long,int) @ 0x080ca942（docs/class_func_reports/CBingo/getToday_080ca942.md）：
// 当前秒数未过当日小时槽（hourLimit，即 G_CEnvironment+0x37c 的 m_fatigueTime）时取
// 昨天日号，否则取今天日号（跨零点结算窗口）。localtime_r 双展开。
int CBingo::getToday(long sec, int hourLimit)
{
    long yesterday = sec - 0x15180;   // 86400 秒 = 1 天
    struct tm tmToday;
    struct tm tmYest;
    struct tm* today = localtime_r(&sec, &tmToday);
    struct tm* yest = localtime_r(&yesterday, &tmYest);
    if (today->tm_hour < 0 || hourLimit <= today->tm_hour)
    {
        m_today = today->tm_mday;
    }
    else
    {
        m_today = yest->tm_mday;
    }
    return m_today;
}

 // ORIG 反汇编：if (day>0 && day<=0x1f) 短路布局（jg/jle 同跳 body）。
int CBingo::resetBoard(int day)
{
    if (!(day > 0 && day <= 0x1f))
    {
        return 0;
    }
    std::map<int, std::vector<int> >& boardMap = G_CDataManager()->m_bingo.m_map18;
    std::map<int, std::vector<int> >::iterator it = boardMap.find(day);
    if (it != boardMap.end())
    {
        m_boards.clear();
        m_boards = it->second;
        return 1;
    }
    return 0;
}

// resetQuiz @ 0x080caa64（docs/class_func_reports/CBingo/resetQuiz.md）：
// 按日取 stBingoScript.m_map0（CDataManager+0xb568，map<int,int>）当日 quiz 值写入
// +0x0c（m_fieldC）。day 越界/无配置返回 0。
 // ORIG 反汇编：if (day>0 && day<=0x1f) 短路布局（jg/jle 同跳 body）。
int CBingo::resetQuiz(int day)
{
    if (!(day > 0 && day <= 0x1f))
    {
        return 0;
    }
    std::map<int, int>& quizMap = G_CDataManager()->m_bingo.m_map0;
    std::map<int, int>::iterator it = quizMap.find(day);
    if (it != quizMap.end())
    {
        m_fieldC = it->second;
        return 1;
    }
    return 0;
}

int CBingo::getBingoRandomVaule(int param_1)
{
    unsigned long range = 9;
    int iVar1 = m_rand->randInt(range);
    iVar1 = iVar1 - 9 + param_1;
    if (iVar1 < 1 || iVar1 > 0x32)
    {
        return 0;
    }
    return iVar1;
}


// resetQuiz @ 0x080caa64（docs/class_func_reports/CBingo/resetQuiz.md）：
// 按日取 stBingoScript.m_map0（CDataManager+0xb568，map<int,int>）当日 quiz 值写入
// +0x0c（m_fieldC）。day 越界/无配置返回 0。
// getTodayBoard @ 0x080caec4（docs/class_func_reports/CBingo/getTodayBoard.md）：
// 返回 this（空函数体）。
CBingo* CBingo::getTodayBoard()
{
    return this;
}

// resetUserData @ 0x080cadfe（docs/class_func_reports/CBingo/resetUserData.md）：
// BingoData::clear(data)。BingoData::clear 定义属 CUser.cpp 域批次（U 引用）。
void CBingo::resetUserData(BingoData& data)
{
    data.clear();
}

CBingoTest::CBingoTest()
{
}

CBingoTest::~CBingoTest()
{
}

void CBingoTest::run()
{
    getToday();
    resetBoard();
    resetQuiz();
    calBoradMatchLine();
    calNewValue();
    getBingoRandomVaule();
}

void CBingoTest::getToday()
{
    std::string s;
    s = "2012-07-19 05:00:00";
    s = "2012-07-19 06:00:00";
    s = "2012-07-20 12:00:00";
    s = "2012-07-31 11:00:00";
    s = "2012-08-01 05:00:00";
    s = "2012-08-01 07:00:00";
}

void CBingoTest::getTimeFromString(std::string s)
{
    boost::posix_time::ptime pt = boost::posix_time::time_from_string(s);
    std::tm t = boost::posix_time::to_tm(pt);
    std::mktime(&t);
}

void CBingoTest::resetBoard()
{
}

void CBingoTest::resetQuiz()
{
}

void CBingoTest::calBoradMatchLine()
{
    int v = 0x10843f;
    v = 0x14610c7;
    v = 0x1f9110;
    v = 0x1576dd5;
    v = 0x1577dd5;
    v = 0x1ffffff;
}

void CBingoTest::calNewValue()
{
    int v = 0;
}

void CBingoTest::getBingoRandomVaule()
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
