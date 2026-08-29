#ifndef GAME_DISPATCHER_MAKEWARROOMTEMP_H_
#define GAME_DISPATCHER_MAKEWARROOMTEMP_H_

// HdrCreate2 批（/tmp/missing_h_chunk_ab）：ORIG 有导出符号但仓库无 .h 声明的类。
// 参数类型按 ORIG demangled 原样；返回类型与 static/成员属性未逐项核实者标【推断】。
// 实现（OrigWarRoom 批次）：基类/方法集由 ORIG vtable @0x8bd8cc8 实证——
//   8 槽 = IPacketDispatcher<MSG_BASE,ParamBase,(ch_state)0> 派生 vtable：
//   [0] dispatch_template(模板弱) [1] ~D1 0x82431aa [2] ~D0 0x82431da
//   [3] dispatch_sig 0x8201b32 [4] check_state(模板弱) [5] read 0x82335f8
//   [6] process 0x8233602 [7] send(模板弱)。
// D1 反汇编（0x82431aa）：写 vptr → 调 IPacketDispatcher<MSG_BASE,ParamBase,
//   (ch_state)0>::~IPacketDispatcher() → 恒假 delete 守卫 → ret。
// 定义落 WarRoom.cpp（OrigWarRoom 独占 TU）。

#include "PacketDispatcher.h"   // IPacketDispatcher/MSG_BASE/ParamBase

class DisPatcher_MakeWarRoomTemp
    : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    // ORIG 0x82431da _ZN26DisPatcher_MakeWarRoomTempD0Ev (size 0x1e)
    //     0x82431aa _ZN26DisPatcher_MakeWarRoomTempD1Ev/D2Ev (size 0x30)
    virtual ~DisPatcher_MakeWarRoomTemp();
    // ORIG 0x8201b32 _ZN26DisPatcher_MakeWarRoomTemp12dispatch_sigEP5CUserR9PacketBuf
    //   (size 0x348) 报告: DisPatcher_MakeWarRoomTemp/dispatch_sig.md
    int dispatch_sig(CUser*, PacketBuf&);
    // ORIG 0x82335f8 _ZN26DisPatcher_MakeWarRoomTemp4readER9PacketBufR8MSG_BASE
    //   (size 0xa)：恒定返回 0
    int read(PacketBuf&, MSG_BASE&);
    // ORIG 0x8233602 _ZN26DisPatcher_MakeWarRoomTemp7processEP5CUserR8MSG_BASER9ParamBase
    //   (size 0xa)：恒定返回 0
    int process(CUser*, MSG_BASE&, ParamBase&);
};

#endif  // GAME_DISPATCHER_MAKEWARROOMTEMP_H_
