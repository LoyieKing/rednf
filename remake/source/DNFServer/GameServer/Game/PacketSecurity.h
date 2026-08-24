#ifndef DNF_GAME_PACKETSECURITY_H
#define DNF_GAME_PACKETSECURITY_H

// 包完整性 / 反作弊安全自由函数（PacketDispatcher 分发链叶子依赖）。
// 三个符号均经 df_game_r nm 核实：
//   bool DenyPacket(int)
//       _Z10DenyPacketi                             T 0x859092a (size 0x104)
//   void _NS_PI_Check_CMD(ENUM_PACKETCLASS, unsigned short, PacketBuf&, CUser*)
//       _Z16_NS_PI_Check_CMD16ENUM_PACKETCLASStR9PacketBufP5CUser
//                                                   T 0x8274778 (size 0x1fd)
//   void _NS_PI_2ND_MakeChecksumTo1Byte2(unsigned int&, unsigned int&)
//       _Z31_NS_PI_2ND_MakeChecksumTo1Byte2RjS_     T 0x808cc4b (size 0x1f)

#include "GameEnums.h"  // ENUM_PACKETCLASS（全局作用域枚举）

class PacketBuf;
class CUser;

// ORIG 0x859092a：封禁包 id 判定（纯谓词，二分比较树逐指令转写）。
bool DenyPacket(int id);

// ORIG 0x8274778：命令级包校验——PICS 计数、包尾哈希比对、超限记 hack。
void _NS_PI_Check_CMD(ENUM_PACKETCLASS packetClass, unsigned short cmd,
                      PacketBuf& packet, CUser* user);

// ORIG 0x808cc4b：两个出参各截低字节回写（movzbl %al）。
void _NS_PI_2ND_MakeChecksumTo1Byte2(unsigned int& checksum1,
                                     unsigned int& checksum2);

#endif  // DNF_GAME_PACKETSECURITY_H
