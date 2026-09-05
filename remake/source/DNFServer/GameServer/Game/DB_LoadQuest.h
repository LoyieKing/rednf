#ifndef GAME_DB_LOADQUEST_H_
#define GAME_DB_LOADQUEST_H_

// ============================================================================
// df_game_r 还原 —— DB_LoadQuest（SIG_LOAD_QUEST DB 装载分发器，DB 线程侧）。
// ORIG 符号：ctor 0x844d930 / D0 0x844d97c / dispatch 0x0840e236 /
// GetUserQuest 0x0840e39a（报告 docs/class_func_reports/DB_LoadQuest/）。
// dispatch/GetUserQuest 返回 0/1（ORIG 调用侧 xor $1/test %al bool 形态）。
// 对象尺寸 4（仅 vptr），基类 DBDispatcher 见同目录头。
// ============================================================================

#include "SigTypes.h"      // SIG_LOAD_QUEST
#include "DBDispatcher.h"
class Stream;

class DB_LoadQuest : public DBDispatcher
{
public:
    DB_LoadQuest() __attribute__((nothrow)); // C1 0x0844d930；try 区无 pad ⇒ nothrow
    virtual ~DB_LoadQuest();                 // D0 0x0844d97c（D1/D2 编译器产出）

    virtual bool dispatch(int userIdx, int slot, Stream* stream);
                                             // ORIG T 0x0840e236

    bool GetUserQuest(SIG_LOAD_QUEST* sig);  // 0x0840e39a
};

#endif  // GAME_DB_LOADQUEST_H_
