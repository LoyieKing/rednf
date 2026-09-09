// UsedFatigueRewardItem —— 疲劳奖励档位表（ORIG T parsing @0x081a25d2 / find
// @0x081a28e4）。
//
// 谱系实证（nm -S + vtable/typeinfo 逐字节 dump）：
//   _ZTV21UsedFatigueRewardItem @0x8bad7e8 sz=0x10 ⇒ 2 虚槽：
//     [0] 0x081a1eec = _ZN17Arad_ScriptLoader4openEPKc（未覆盖，继承基类实现）
//     [1] 0x081a25d2 = _ZN21UsedFatigueRewardItem7parsingEPc（覆盖基类 parsing）
//   _ZTI21UsedFatigueRewardItem sz=0xc = __si_class_type_info ⇒ 单继承
//   ⇒ **public Arad_ScriptLoader**，本类唯一自覆盖虚函数 = parsing。
//   基类 Arad_ScriptLoader 虚表 2 槽（open/parsing），无虚析构（双方均无 D* 符号）。
//
// 对象布局（vptr 实证后修正）：+0x00 vptr（原以 int m_field0 占位——parsing/find
// 未触碰、其值由基类 ctor 写 vptr 所致，即 _ZTV 地址）、+0x04 m_enable
// （parsing movb $0/1,0x4(%eax)）、+0x08 m_data（find lea 0x8(%eax)）。
// sizeof 0xc 不变。
//
// Data 布局实证（objdump C1 @0x081a40a0 清零宽度 + find 读写偏移）：
//   +0x00 short / +0x02 short / +0x04 char[0xff] / +0x103 char[0x10] / +0x114 int
//   （sizeof=0x118）；find 复制 +0x00/+0x02 两个 short、+0x114 int 与两段字符串
//   （findString(id@+0x103 区 / +0x4 区) strncpy 0x14 / 0xff）。
#ifndef DNF_GAME_USEDFATIGUEREWARDITEM_H_
#define DNF_GAME_USEDFATIGUEREWARDITEM_H_

#include <vector>

#include "Arad_ScriptLoader.h"

class UsedFatigueRewardItem : public Arad_ScriptLoader
{
public:
    class Data
    {
    public:
        Data();    // ORIG W C1/C2 @0x081a40a0（清零）
        ~Data();   // ORIG W D1/D2 @0x081a6bf4（空体）

        short m_begin;       // +0x00 实证 movw 清零 / find movzwl 比较
        short m_end;         // +0x02 实证 movw 清零 / find movzwl 0x2
        char  m_str[0xff];   // +0x04 实证 memset(,0,0xff) / find strncpy 0xff
        char  m_name[0x10];  // +0x103 实证 memset(,0,0x10) / find strncpy 0x14
        int   m_id;          // +0x114 实证 movl 清零 / find 复制
    };

    virtual int parsing(char* text);   // ORIG T @0x081a25d2；vtable 槽[1] 覆盖基类
                                       // Arad_ScriptLoader::parsing（W 0x081a4054）
    int find(int low, int high, Data& out);   // ORIG T @0x081a28e4 返回 0/1（未命中 0）

private:
    bool m_enable;   // +0x04 实证 parsing movb $0/$1
    std::vector<Data> m_data;  // +0x08 实证 find lea 0x8
};

#endif  // DNF_GAME_USEDFATIGUEREWARDITEM_H_
