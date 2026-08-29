// df_game_r 还原 —— DB_AradDatabaseDispatcher 实现（报告 docs/class_func_reports/
// DB_AradDatabaseDispatcher/）。C1 0x0844bc38：基类 DBDispatcher() + 设 vptr +
// Arad_DatabaseDispatcher C1 + Init()；dispatch 0x0844bcac：Stream>>code →
// GetDispatcher → 空 cMyTrace(0xbc8b,5) 返 false → 否则虚调槽2 dispatch。
#include "DB_AradDatabaseDispatcher.h"

#include "GameTypes.h"  // cMyTrace

DB_AradDatabaseDispatcher::DB_AradDatabaseDispatcher()
{
    m_disp.Init();   // ORIG @0x0844bc6b call Arad_DatabaseDispatcher::Init()
}

DB_AradDatabaseDispatcher::~DB_AradDatabaseDispatcher()
{
}

bool DB_AradDatabaseDispatcher::dispatch(int userIdx, int slot, Stream* stream)
{
    int code = 0;   // ORIG movl $0,-0x20(%ebp)
    *stream >> code;   // ORIG call Stream::operator>>(int&)

    ARAD::DISPATCHER::Arad_IDatabaseDispatcher* disp =
        m_disp.GetDispatcher((ARAD::ENUM_INTERNALPACKET_JPN)code);   // ORIG call GetDispatcher
    if (disp == 0)
    {
        // ORIG：cMyTrace 临时链式（code 提前入 ebx 跨构造调用缓存）
        cMyTrace(__PRETTY_FUNCTION__, 0xbc8b, 5)
            ("[DISPATCHER_ARAD] Invalid Internal Packet Code. (code:%d)",
             code);
        return false;   // ORIG mov $0,%eax
    }
    return disp->dispatch(userIdx, slot, stream);   // ORIG 虚槽2（vptr+8）直调
}
