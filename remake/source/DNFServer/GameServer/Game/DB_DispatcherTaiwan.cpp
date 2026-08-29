// df_game_r 还原 —— DB_DispatcherTaiwan 实现（报告 docs/class_func_reports/
// DB_DispatcherTaiwan/）。C1 0x0844c9d4：基类 DBDispatcher() + 设 vptr +
// TaiwanDBDispatcher C1 + init()；dispatch 0x0844ca48：Stream>>code →
// getDispatcher → 空 cMyTrace(0xc17a,5) 返 false → 否则虚调槽2 dispatch。
#include "DB_DispatcherTaiwan.h"

#include "GameTypes.h"                       // cMyTrace
#include "TaiwanInterfaceDBDispatcher.h"     // dispatch 虚调用需完整类型

DB_DispatcherTaiwan::DB_DispatcherTaiwan()
{
    m_disp.init();   // ORIG @0x0844ca07 call TaiwanDBDispatcher::init()
}

DB_DispatcherTaiwan::~DB_DispatcherTaiwan()
{
}

bool DB_DispatcherTaiwan::dispatch(int userIdx, int slot, Stream* stream)
{
    int code = 0;   // ORIG movl $0,-0x20(%ebp)
    *stream >> code;   // ORIG call Stream::operator>>(int&)

    TaiwanInterfaceDBDispatcher* disp =
        m_disp.getDispatcher((TaiwanInternalPack::T)code);   // ORIG call getDispatcher
    if (disp == 0)
    {
        // ORIG：cMyTrace 临时链式（code 提前入 ebx 跨构造调用缓存）
        cMyTrace(__PRETTY_FUNCTION__, 0xc17a, 5)
            ("[TaiwanDispater] Invalid internal packet code. Code:%d, Uid:%d",
             code, slot);
        return false;   // ORIG mov $0,%eax
    }
    return disp->dispatch(userIdx, slot, stream);   // ORIG 虚槽2（vptr+8）直调
}
