// ============================================================================
// df_game_r 还原 —— DB_MailBox_Req_System_Multi_Mail（系统多件邮件 DB 任务）
// ORIG 符号（nm 核对，df_game_r elf32-i386）：
//   T _ZN32DB_MailBox_Req_System_Multi_Mail8dispatchEiiP6Stream        0x0844346a
//   T _ZN32DB_MailBox_Req_System_Multi_Mail13_InsertLetterEP5MySQLP34SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL 0x084435d4
//   T _ZN32DB_MailBox_Req_System_Multi_Mail13_InsertPostalEP5MySQLP34SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL 0x0844372e
//   T _ZN32DB_MailBox_Req_System_Multi_Mail10sendResultEi17ENUM_SERVER_GROUP 0x08443b6e
//   W DB_MailBox_Req_System_Multi_Mail::DB_MailBox_Req_System_Multi_Mail() 0x0844f99a
//     （注册表空壳，W8LT_DBTask.cpp 已有；本头只声明不定义）
// 报告：docs/class_func_reports/DB_MailBox_Req_System_Multi_Mail/{dispatch,
//   _InsertLetter,_InsertPostal,sendResult}.md（逐指令转写）。
// 注：W8LT_DBTask.cpp 有同名的 TU-local 注册空壳类（勿改）；本头为其 ORIG
// 成员函数的权威声明点，两者按 DB_JoinGuildInfo.h 先例并存。
// ============================================================================
#ifndef DNF_GAME_DB_MAILBOX_REQ_SYSTEM_MULTI_MAIL_H_
#define DNF_GAME_DB_MAILBOX_REQ_SYSTEM_MULTI_MAIL_H_

#include "DBDispatcher.h"
#include "CEnvironment.h"   // ENUM_SERVER_GROUP / ENUM_DB_HANDLE_IDX

class MySQL;
class Stream;
struct SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL;

class DB_MailBox_Req_System_Multi_Mail : public DBDispatcher
{
public:
    DB_MailBox_Req_System_Multi_Mail() {}   // ORIG C1 0x0844f99a（W8LT_DBTask.cpp 注册表提供）

    // ORIG 0x0844346a（虚覆盖，vtable dispatch 槽）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);

    // ORIG 0x084435d4（普通成员：dispatch 内以 this 传参调用）
    bool _InsertLetter(MySQL* db, SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL* sig);

    // ORIG 0x0844372e（普通成员）
    bool _InsertPostal(MySQL* db, SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL* sig);

    // ORIG 0x08443b6e（普通成员，返回 1）
    int sendResult(int userIdx, ENUM_SERVER_GROUP group);
};

#endif  // DNF_GAME_DB_MAILBOX_REQ_SYSTEM_MULTI_MAIL_H_
