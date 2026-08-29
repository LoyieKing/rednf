#ifndef GAME_FUNCTIONGUILDMEMBER_H_
#define GAME_FUNCTIONGUILDMEMBER_H_

class PacketGuard;         // InterfacePacketBuf 派生（PacketGuard.h）
struct ST_Guild_Mem_Info;  // shared/packet/include/Packet_Monitor_Call_Guild_Members_ToChannel.h

// HdrCreate2 批（/tmp/missing_h_chunk_ab）：ORIG 有导出符号但仓库无 .h 声明的类。
// 仅声明骨架（不入 CMake，无 cpp 实现）。参数类型按 ORIG demangled 原样；
// 返回类型与 static/成员属性未逐项核实者标【推断】，实现时须以 docs/class_func_reports 为准。

class FunctionGuildMember
{
public:
    // ORIG 0x84ca55a T _ZN19FunctionGuildMember23LogMakeGuildMembersInfoEjiP17ST_Guild_Mem_Info
    // 报告 FunctionGuildMember/LogMakeGuildMembersInfo.md：static void（反编译显式注明）。
    static void LogMakeGuildMembersInfo(unsigned int guildKey, int count,
                                        ST_Guild_Mem_Info* members);

    // ORIG 0x84ca37a T _ZN19FunctionGuildMember26MakePacketGuildMembersInfoER11PacketGuardiP17ST_Guild_Mem_Info
    // 报告 MakePacketGuildMembersInfo.md：void（无返回值）；调用点均以静态形式调用。
    static void MakePacketGuildMembersInfo(PacketGuard& packet, int count,
                                           ST_Guild_Mem_Info* members);
};


#endif  // GAME_FUNCTIONGUILDMEMBER_H_
