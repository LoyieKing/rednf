// ============================================================================
// df_game_r 还原 —— DB_MailBox_Req_Send（邮件发送 DB 任务，OrigMailSend 批）
// ORIG 符号（nm 核对，df_game_r elf32-i386）：
//   T _ZN19DB_MailBox_Req_Send12InsertLetterEP5MySQLiiPKcS3_Ril16ENUM_LETTER_STAT 0x0841f15a
//   T _ZN19DB_MailBox_Req_Send14insertMailDataEP5MySQLPK25SIG_MAILBOX_SEND_NEW_MAIL 0x0841f2fa
//   T _ZN19DB_MailBox_Req_Send10sendResultEiii17ENUM_SERVER_GROUP 0x0841f75a
//   T _ZN19DB_MailBox_Req_Send8dispatchEiiP6Stream 0x0841f84e
//   W DB_MailBox_Req_Send::DB_MailBox_Req_Send() 0x0844e32e（注册表空壳，W8LT_DBTask.cpp 已有）
//   W ~DB_MailBox_Req_Send() 0x0845f4b4/0x0845f484（编译器隐式；本头不写用户析构）
// 报告：docs/class_func_reports/DB_MailBox_Req_Send/{InsertLetter,insertMailData,
//   sendResult,dispatch}.md（逐指令转写）。
// InsertLetter/insertMailData 的 __PRETTY_FUNCTION__ 均为 "static bool ..."；
// 调用点（dispatch 内）不传 this → 三函数为 static 成员，仅 dispatch 为虚覆盖。
// 注：W8LT_DBTask.cpp 有同名的 TU-local 注册空壳类（勿改）；本头为其 ORIG
// 成员函数的权威声明点，两者按 DB_JoinGuildInfo.h 先例并存。
// ============================================================================
#ifndef DNF_GAME_DB_MAILBOX_REQ_SEND_H_
#define DNF_GAME_DB_MAILBOX_REQ_SEND_H_

#include <ctime>

#include "DBDispatcher.h"
#include "CEnvironment.h"   // ENUM_SERVER_GROUP / ENUM_DB_HANDLE_IDX

class MySQL;
class Stream;
struct SIG_MAILBOX_SEND_NEW_MAIL;

// ORIG 全局枚举（InsertLetter 第 8 参类型；mangled 名含 16ENUM_LETTER_STAT）。
// 值集 ORIG 无符号可考，仅 InsertLetter 调用点传 1。
enum ENUM_LETTER_STAT
{
    LETTER_STAT_NONE = 0,
    LETTER_STAT_ONE = 1
};

class DB_MailBox_Req_Send : public DBDispatcher
{
public:
    DB_MailBox_Req_Send() {}   // ORIG C1 0x844e32e（W8LT_DBTask.cpp 注册表提供）

    // ORIG 0x0841f84e（虚覆盖，vtable dispatch 槽）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);

    // ORIG 0x0841f15a（static；__PRETTY_FUNCTION__ 实证 "static bool ..."）
    static bool InsertLetter(MySQL* db, int characNo, int sendCharacNo,
                             const char* name, const char* title, int& letterId,
                             time_t regDate, ENUM_LETTER_STAT stat);

    // ORIG 0x0841f2fa（static；__PRETTY_FUNCTION__ 实证 "static bool ..."）
    static bool insertMailData(MySQL* db, const SIG_MAILBOX_SEND_NEW_MAIL* sig);

    // ORIG 0x0841f75a（static；调用点不传 this，返回 1）
    static int sendResult(int userIdx, int slot, int mailIdx,
                          ENUM_SERVER_GROUP group);
};

#endif  // DNF_GAME_DB_MAILBOX_REQ_SEND_H_
