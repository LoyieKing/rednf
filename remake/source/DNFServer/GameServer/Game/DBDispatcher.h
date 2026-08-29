// df_game_r 还原 —— 全局作用域 DBDispatcher（DB 消息分发器公共基类）。
// 定稿（2026-08-27 AltarDB 协调）：dispatch 返回 bool（与 ORIG 实现 0/1 一致；
// DB_Login.cpp 提供唯一定义 _ZN12DBDispatcher8dispatchEiiP6Stream T @0x83fbf7c）。
// ctor/dtor inline 空体由各 TU 弱符号展开（ORIG W C1/C2 @0x844d7a6 仅设 vptr；
// D1/D2 @0x80e2378 设 vptr 后按恒假条件跳过 delete —— 与空体语义一致）。
// vtable _ZTV12DBDispatcher @0x8c5fd30 [offtop][typeinfo][dispatch][~D1][~D0]；
// typeinfo @0x8c6318c。对象尺寸 4：仅 vptr。
//
// 注意：ServerLab/ServerLib/basic_header/DBDispatcher.h 是另一套 nsl::DBDispatcher
// （dispatch(Message*)），与本头无冲突。
#pragma once

class Stream;

class DBDispatcher
{
public:
    DBDispatcher() {}

    // ORIG T @0x83fbf7c [objdump]：slot(第2参)==-1 直接通过；否则要求
    // G_CGameManager()->getUser(slot, userIdx+0x7d0) 非空。
    // 虚函数声明顺序（dispatch 在前）与 ORIG vtable 一致：
    // vtable [offtop][typeinfo][dispatch][~D1][~D0]。
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
    virtual ~DBDispatcher() {}
};
