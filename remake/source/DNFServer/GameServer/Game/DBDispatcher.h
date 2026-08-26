// df_game_r 还原 —— 全局作用域 DBDispatcher（DB 消息分发器公共基类）。
// 注意：ServerLab/ServerLib/basic_header/DBDispatcher.h 是另一套 nsl::DBDispatcher
// （dispatch(Message*)），与本头无冲突。ORIG vtable _ZTV12DBDispatcher @0x8c5fd30
// （[offtop][typeinfo][dispatch][~D1][~D0]），typeinfo @0x8c6318c。
#pragma once

#include "Stream.h"  // Stream 完整类型（dispatch 第 3 参）

class DBDispatcher
{
public:
    DBDispatcher();            // ORIG W C1/C2 @0x844d7a6：仅设 vptr
    virtual ~DBDispatcher();   // W D1/D2 @0x80e2378：设 vptr；D0 @0x80e239e + 条件 operator delete（ORIG 恒不删除路径）

    // ORIG T @0x83fbf7c [objdump]：uniqueId(第2参)==-1 直接通过；否则
    // user = G_CGameManager()->getUser(uniqueId, handleIdx + 0x7d0)，user 为空则失败。
    virtual bool dispatch(int handleIdx, int uniqueId, Stream* stream);
};
