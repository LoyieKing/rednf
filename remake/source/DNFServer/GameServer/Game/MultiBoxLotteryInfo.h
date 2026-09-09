#ifndef GAME_MULTIBOXLOTTERYINFO_H_
#define GAME_MULTIBOXLOTTERYINFO_H_

// df_game_r 还原 —— MultiBoxLotteryInfo（ORIG class，含 C1/D1/clear 成员函数）。
// 报告缺失（docs/class_func_reports 无 MultiBoxLotteryInfo 目录），依据
// nm -S --defined-only ORIG 定址 + objdump 逐函数转写（2026-08-28 OrigScriptInfo）。
// 布局（ORIG C1 0x8513ed6 / clear 0x8513f44 / D1 0x85145b2 实证）：
//   +0x00 int m_field0   +0x04 int m_field4   +0x08 int m_field8
//   +0x0c int m_fieldc   +0x10 vector<LotteryUnit> m_units
//   +0x1c stDefaultItemInfo m_default（0x1c） → sizeof = 0x38
// 嵌套类（ORIG W 弱符号，全部含 C1/D1）：
//   LotteryUnit         0x89c351e/0x8515236，含 2×vector<pair<int,int>>（0x28）
//   stDefaultItem       0x89c35d4/0x89d48b2，4 字段（0x10）
//   stDefaultItemInfo   0x8513e50/0x8513ec0，3 int + char + vector<stDefaultItem>（0x1c）
//   LotteryDefaultArad  0x8513e30/0x8522962，3×uint（0xc）
// 注：此头为 MultiBoxLotteryInfo 唯一权威声明点（CStackableItem.h 的
// namespace+MultiBoxLotteryInfoBody 旧建模已删除，见 §4.4 合并）。

#include <utility>   // std::pair
#include <vector>

class MultiBoxLotteryInfo
{
public:
    class stDefaultItem
    {
    public:
        stDefaultItem();       // ORIG W 0x89c35d4：清零 4 字段
        // ~stDefaultItem() 不声明：ORIG 0x085219d1 实例化 _Destroy_aux<true>
        // （has_trivial_destructor=true）⇒ ORIG 析构平凡（其 W D1 0x089d48b2 size=5
        // 为使用点隐式发射，邻域 = vector<stDefaultItem> _M_insert_aux/
        // __uninitialized_move_a 模板簇；同 LotteryDefaultArad 先例 P12/P71）。
        // 我方 user-provided 空析构曾致 CStackableItem.cpp p167 EXTRA
        // stDefaultItemD1×2 + _Unwind_Resume×1（PoolCera b28d，Main 批准）。

        int m_itemIdx;   // +0x00
        int m_count;     // +0x04
        // [b28d-Main·检查点14 P321-S1] int→unsigned int（同宽 4B，偏移/sizeof 不变）：
        //   T1 仅定 32 位（ORIG 0x0850f1ba `mov 0x8(%eax),%edx` 无扩展），符号性不可由 T1 区分；
        //   T2 消费者形（0x0850f1bd 读-加-写四段）在 4.4 下仅 unsigned 字段+int 累加器可复现
        //   （探针 /tmp/probe_sum2.cpp a1 vs a5/a6）⇒ S1 一次替代形命中。
        unsigned int m_prob;   // +0x08
        char m_flag;     // +0x0c
    };

    class LotteryUnit
    {
    public:
        LotteryUnit();                                  // ORIG W 0x89c351e
        LotteryUnit(const LotteryUnit& other);          // ORIG W 0x85151a8
        ~LotteryUnit();                                 // ORIG W 0x8515236
        void clear();                                   // ORIG W 0x89c358c
        LotteryUnit& operator=(const LotteryUnit& other);  // ORIG W 0x89cdf46

        int m_count;                       // +0x00
        int m_defaultIdx;                  // +0x04
        int m_defaultNum;                  // +0x08
        // b28d-solo：char→bool（P248 档②，ORIG get_multibox_lottery_item 0x0850ef76
        // `movzbl -0x5c; mov %al,-0xd` 直存无 setne 归一化 ⇒ 源字段 bool）。
        bool m_defaultFlag;                // +0x0c
        std::vector<std::pair<int, int> > m_items;   // +0x10
        std::vector<std::pair<int, int> > m_bonus;   // +0x1c
    };

    class stDefaultItemInfo
    {
    public:
        stDefaultItemInfo();   // ORIG W 0x8513e50
        ~stDefaultItemInfo();  // ORIG W 0x8513ec0

        // [b28d-solo-CStackableItem 2026-09-09] 三字段 int→unsigned int（同宽 4B，
        // sizeof/偏移/mangled 零变化）。直证：ORIG createStackableMultiBoxLotteryInfo
        // 0x089bafd1 内同一 RHS `*it`（int）对 4B 字段 store 分两类——本类三字段
        // `mov (%eax),%eax; mov %eax,%edx; ...; mov %edx,0x1c(%eax)`（0x089bb6af/
        // 0x089bb719/0x089bb783，带 eax→edx 中转=异型），而同函数 MultiBoxLotteryInfo::
        // m_field8/m_fieldc（源码 int）为 `mov (%eax),%edx; ...`（0x089bb5c7/0x089bb62f，
        // 无中转=同型）⇒ 三字段非 int。旁证：get_multibox_default_item 0x0850f371 读
        // m_defaultCount 后 `cmp i; seta`（无符号 above，循环变量为 unsigned int）。
        // 探针 /tmp/probe_mb.cpp、/tmp/probe_mb2.cpp（446 后端）：unsigned 字段+int RHS
        // =4 条带中转；unsigned 字段读/unsigned 循环上界=与 ORIG 同形且零回退。
        unsigned int m_defaultCount;           // +0x00
        unsigned int m_defaultIdx;             // +0x04
        unsigned int m_defaultNum;             // +0x08
        // b28d-solo：**保持 char**（与 LotteryUnit 相反）——本类字段的消费点
        // get_multibox_default_item `!=0` 与 createStackableMultiBoxLotteryInfo
        // `= (char)*it` 在 ORIG 均要求 char 形（test/setne 物化 / 直存）。
        char m_defaultFlag;                    // +0x0c
        std::vector<stDefaultItem> m_items;    // +0x10
    };

    class LotteryDefaultArad
    {
    public:
        LotteryDefaultArad(unsigned int a, unsigned int b, unsigned int c);  // ORIG W 0x8513e30
        // ~LotteryDefaultArad() 不声明：ORIG 实例化 _ZNSt12_Destroy_auxILb1EE（trivial 析构）
        // + W D1 0x8522962 由使用点隐式发射（b28d-Main P12/P71）。

        unsigned int m_field0;   // +0x00
        unsigned int m_field4;   // +0x04
        unsigned int m_field8;   // +0x08
    };

    MultiBoxLotteryInfo();       // ORIG W 0x8513ed6
    ~MultiBoxLotteryInfo();      // ORIG W 0x85145b2
    void clear();                // ORIG W 0x8513f44

    int m_field0;                    // +0x00
    int m_field4;                    // +0x04
    int m_field8;                    // +0x08
    int m_fieldc;                    // +0x0c
    std::vector<LotteryUnit> m_units;  // +0x10
    stDefaultItemInfo m_default;       // +0x1c
};

#endif  // GAME_MULTIBOXLOTTERYINFO_H_
