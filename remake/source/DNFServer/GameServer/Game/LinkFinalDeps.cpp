// df_game_r 还原 —— Wave6 IPG 批依赖收口(LinkFinal):全局数值串缓冲与睡眠原语。
//
// ORIG 证据(df_game_r elf32-i386,nm -S + objdump):
//   gNumberToStringBuffer        B 0x200(共 8 槽 × 0x40)
//   _Z14NumberToStringji         T 0x0810904b (0x4d) unsigned int 版:
//       slot = buf + slotIdx*0x40;sprintf(slot,"%u"@0x8b446ea,value);
//       返回 slot(radix 形参未被读取,保留以对齐调用约定)
//   _Z14NumberToStringyi         T 0x08109098 (0x62) unsigned long long 版:
//       同槽位公式;sprintf(slot,"%qu"@0x8b446ed,value);返回 slot
//   _ZN7OS_API5SleepEi           T 0x0858c4e0 (0x19):
//       imul $0x3e8 → usleep(ms*1000);无返回值装载(void 等价)
// 注:ORIG 以 %gs 相对寻址取静态缓冲基址(i386 寻址技巧),重建直取同名数组;
//   "int"/"void" 等非类返回类型不参与 mangled,void 选择与 ORIG 符号严格一致。

// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN6OS_API5SleepEi`（ORIG 0x858c890）。
#include <iostream>

#include <cstdio>
#include <unistd.h>

// W7_BuildHarmonize 去重：NumberToString(_Z14NumberToStringji/_Z14NumberToStringyi)
// 与 gNumberToStringBuffer 唯一 owner 为 ServerCommon/DNFFunctionLib.cpp
// （ORIG TLS 缓冲语义），此处定义移除、仅保留声明。
char* NumberToString(unsigned int value, int radix);
char* NumberToString(unsigned long long value, int radix);

namespace OS_API {

// ORIG T 0x0858c4e0:usleep(ms*1000)
void Sleep(int ms)
{
    usleep(static_cast<useconds_t>(ms) * 1000u);
}

}  // namespace OS_API
