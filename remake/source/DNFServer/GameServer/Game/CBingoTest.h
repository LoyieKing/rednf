// CBingoTest.h —— CBingoTest 仅 ORIG 批次（docs/class_func_reports/CBingoTest/ 10 报告全文已读）。
// CBingoTest : public CBingo（ctor 0x080caecc 调 CBingo::CBingo()，D1 0x080caee0 调
// CBingo::~CBingo()，getBingoRandomVaule 调 CBingo::getBingoRandomVaule(int)）。
// CBingo 无权威共享头（BingoEventLink4/CUserOnlyOrig/EventBattleLink/GmCmds_A 均持
// TU-local 视图）；此处按 docs/class_func_reports/CBingo/CBingo.md（ctor 0x080ca82c）与
// docs/class_func_reports/CBingo/~CBingo.md（D1 0x080ca8f0）布局建模 CBingoTest 的基类视图：
// +0x00 vector<int>、+0x0c int、+0x10 boost::shared_ptr<CMTRand>（{px,pn} 8B，ORIG D1
// 0x080ca8f0 call _ZN5boost10shared_ptrI7CMTRandED1Ev@0x080ccf4a 实证）、+0x18 int m_today；
// sizeof 0x1c。两类型均无虚函数（ORIG 无 _ZTV/_ZTI）。
// CBingo 的 C1/D1 定义在本批 CBingoTest.cpp（Main 已确认属主）；getBingoRandomVauleEi
// 为 U 引用（实现归 BingoEventLink4 批）。
#ifndef DNF_GAME_CBINGOTEST_H_
#define DNF_GAME_CBINGOTEST_H_

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

class CMTRand;   // PvPTypes.h（MT19937，0x9c8；shared_ptr 成员仅需前置声明）
struct BingoData;  // CUser.h（resetUserData 引用参数）

class CBingo
{
public:
    std::vector<int> m_boards;      // +0x00
    int m_fieldC;                   // +0x0c
    boost::shared_ptr<CMTRand> m_rand;  // +0x10（8B {px,pn}）
    int m_today;                    // +0x18

    CBingo();                    // ORIG T 0x080ca82c
    ~CBingo();                   // ORIG T 0x080ca8f0
    int getBingoRandomVaule(int); // ORIG T 0x080cae70
    int calNewValue(unsigned int& board, int idx);      // ORIG T 0x080cad4c
    int getIndexTodayBoard(int value);                  // ORIG T 0x080caafa
    int calBoradMatchLine(int board);                   // ORIG T 0x080cab5a
    int getToday();                                     // ORIG T 0x080ca936
    int getToday(long sec, int hourLimit);              // ORIG T 0x080ca942
    int resetBoard(int day);                            // ORIG T 0x080ca9b8
    int resetQuiz(int day);                             // ORIG T 0x080caa64
    CBingo* getTodayBoard();                            // ORIG T 0x080caec4
    void resetUserData(BingoData& data);                // ORIG T 0x080cadfe
};

class CBingoTest : public CBingo
{
public:
    CBingoTest();
    ~CBingoTest();
    void resetBoard();
    void calNewValue();
    void calBoradMatchLine();
    void getTimeFromString(std::string s);
    void getBingoRandomVaule();
    void run();
    void getToday();
    void resetQuiz();
};

#endif // DNF_GAME_CBINGOTEST_H_
