// 复制 +0x00/+0x02 两个 short、+0x114 int 与两段字符串
// （findString(id@+0x103 区 / +0x4 区) strncpy 0x14 / 0xff）。
// Data 布局实证（objdump C1 @0x081a40a0 清零宽度 + find 读写偏移）：
//   +0x00 short / +0x02 short / +0x04 char[0xff] / +0x103 char[0x10] / +0x114 int
// （sizeof=0x118）。本体布局：parsing movb $0/1,0x4(%eax) flag、
// find lea 0x8(%eax) vector；+0x00 4 字节证据不足（parsing/find 未触碰）。
#ifndef DNF_GAME_USEDFATIGUEREWARDITEM_H_
#define DNF_GAME_USEDFATIGUEREWARDITEM_H_

#include <vector>

class UsedFatigueRewardItem
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

    int parsing(char* text);   // ORIG T @0x081a25d2【推断】返回值 demangled 缺失
    int find(int low, int high, Data& out);   // ORIG T @0x081a28e4【推断】返回 0/1（未命中 0）

private:
    int  m_field0;   // +0x00 证据不足（parsing/find 未触碰），占位保持偏移
    bool m_enable;   // +0x04 实证 parsing movb $0/$1
    std::vector<Data> m_data;  // +0x08 实证 find lea 0x8
};

#endif  // DNF_GAME_USEDFATIGUEREWARDITEM_H_
// 复制 +0x00/+0x02 两个 short 与两段字符串（findString(id@+0x103 区) strncpy 0x14 / 0xff）。
// Data 具体字段偏移证据不足（Ghidra 指针步进歧义），不建成员，不加断言。
#ifndef DNF_GAME_USEDFATIGUEREWARDITEM_H_
#define DNF_GAME_USEDFATIGUEREWARDITEM_H_

#include <vector>

class UsedFatigueRewardItem
{
public:
    class Data
    {
    public:
        Data();    // ORIG W C1 @0x081a40a0（清零）
        ~Data();   // ORIG W D1 @0x081a6bf4
    };  // 嵌套 Data：sizeof 未证实

    int parsing(char* text);   // ORIG T @0x081a25d2【推断】返回值 demangled 缺失
    int find(int low, int high, Data& out);   // ORIG T @0x081a28e4【推断】返回 0/1（未命中 0）

    std::vector<Data> m_data;  // 【推断】parsing push_back 与 find 遍历实证容器存在
};

#endif  // DNF_GAME_USEDFATIGUEREWARDITEM_H_
