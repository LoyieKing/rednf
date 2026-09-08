// df_game_r WongWork::CDeathTowerRanking（死亡之塔排行榜）。
// 全局 3 张排行榜表（ORIG BSS 0x941f8c0 起，每表 0x70f4，getRankTable 懒构造）。
// 布局依据 ctor 0x082a787c / _getRankingTable 0x08469d90 / _getBestRecordTable
// 0x08469dae / stBestRecord_t ctor 0x082a76c0 / getVirtualRank 0x08468de0：
//   +0x0000 塔记录块 m_towers[5]（每塔 0x390 = 6×0x98 stBestRecord_t 槽 0..5，
//            槽 1 = 该塔当前榜首（+0x128 = 0x98+0x90），+0x90 标志位为
//            checkRenewTopRecord 的“有纪录”位；槽 0 为未用前缀但同样被构造）
//   +0x11d0 hash_map<uint,uint> m_rankingTables[5]（stride 0x14）
//   +0x1234 hash_map<uint,stBestRecord_t> m_bestTables[5]
//   +0x1298 std::map<uint,uint> m_rankMap[5][200]（rank map；[塔][层]，层 stride 0x18，
//            塔 stride 0x12c0，键=通关时间，值=名次；总 0x5dc0 → +0x7058）
//   +0x7058 stBestRecord_t m_tableBest（表级最优纪录；+0x94 即 +0x70ec 为存故名次）
//   +0x70f0 unsigned int m_tableType（init 写入的 type）
//   总尺寸 0x70f4。
#ifndef DNF_CDEATHTOWERRANKING_H
#define DNF_CDEATHTOWERRANKING_H

#include <string.h>
#include <map>
#include <ext/hash_map>

class PacketGuard;   // makeMyBestRecordPacket/makeTopRankerRecordPacket 参数（真实定义
                     //   在 PacketGuard.h，class；此处必须同为 class 以免 tag 冲突）

// W6_RankMailDeps 需要（registRanker/registBestRecord 参数；真实布局仅按
// ORIG 字节序引用，见 W6_RankMailDeps.cpp 注释）：
struct stDeathTowerRecordMemberInfo_t;

namespace WongWork
{

// [b28d-solo-CDeathTower] 隐式 C1 verdict=DIFF（218/208，P167：MISSING
// stBestRecord_t::C1 ×1 / EXTRA stTowerBlock::C1 ×1）——N109/N91：4.4 对含非平凡
// 成员的嵌套类数组发射 stTowerBlock::C1 并调用，4.1 在 C1 内联展开 6×
// stBestRecord_t::C1；二维数组化需同步改 CompoundDeathLCB.cpp/W6_RankMailDeps.cpp
// 消费点（跨泳道，R1 禁）⇒ N 封顶。
class CDeathTowerRanking
{
public:
    // 死亡之塔排名层数/塔数（getVirtualRank 层扫描上界 <200，塔 stride = 200*0x18）。
    static const unsigned int kRankStageCount = 200;
    static const unsigned int kRankTowerCount = 5;

    // [b28d-D13] ORIG C1 0x082a787c 尾部 `movl $0x0,0x70f0(%eax)`（m_tableType
    // 清零，218 insn）——隐式 C1 不初始化标量成员 ⇒ 用户默认 ctor 补 m_tableType(0)。
    CDeathTowerRanking() : m_tableType(0) {}

    typedef __gnu_cxx::hash_map<unsigned int, unsigned int,
                                __gnu_cxx::hash<unsigned int>,
                                std::equal_to<unsigned int>,
                                std::allocator<unsigned int> > RankTable;

    // ORIG 0x082a774c W（_ZN8WongWork18CDeathTowerRanking12getRankTableEj）：
    // 返回 3 张排行榜表（0x941f8c0 起，每表 0x70f4）中 type 对应表指针；
    // type==0 返回首表，否则 0x941f8c0 + (type-1)*0x70f4。
    static void* getRankTable(unsigned int type);
    // ORIG _ZN8WongWork18CDeathTowerRanking4initEj（T 0x084680a4）：非静态成员
    // （this=表对象）。ORIG Init 0x082a1f60..0x082a1f75 实证：先
    // call getRankTable(i) 返回表指针入 esp[0]、i 入 esp[4]，再 call init。
    void init(unsigned int type);
    // ORIG 0x08469d69 W（_ZN8WongWork18CDeathTowerRanking23makeDungeonIdx2TowerIdxEj）：
    // 死亡之塔副本 idx（0x2af8..0x2afd）→ 塔序号（0..5），其余返回 0xffffffff。
    static unsigned int makeDungeonIdx2TowerIdx(unsigned int dungeonIdx);
    // ORIG 0x084682fe T（_ZN8WongWork18CDeathTowerRanking18checkRenewMyRecordEjjjj，
    // 成员 this=表）：按 towerIdx/key/stage/playTime 查 best 表，返回 -1/0/1。
    int checkRenewMyRecord(unsigned int towerIdx, unsigned int key,
                           unsigned int stage, unsigned int playTime);
    // ORIG 0x08468290 T（_ZN8WongWork18CDeathTowerRanking19checkRenewTopRecordEjjj，
    // 成员 this=表）：返回 1=破纪录，0=未破。
    // [b28d-solo-CDeathTower] 返回类型 bool：ORIG 尾 `call operator>; leave; ret`
    // 无 movzbl 加宽（P9/P292），调用点 _checkRenewTopRecord 侧 `test %al,%al`。
    // 返回类型不入 mangled、不改布局/sizeof。
    bool checkRenewTopRecord(unsigned int towerIdx, unsigned int a,
                             unsigned int b);
    // ORIG 0x08468de0 T（_ZN8WongWork18CDeathTowerRanking14getVirtualRankEjjj，
    // 成员 this=表）：返回虚拟名次（-1=更好，详见报告）。
    int getVirtualRank(unsigned int towerIdx, unsigned int stage,
                       unsigned int playTime);
    // ORIG 0x08468b4c T（_ZN8WongWork18CDeathTowerRanking18unregistBestRecordEj，
    // 成员 this=表）：对 5 张 best-record 子表按 characNo erase。
    void unregistBestRecord(unsigned int characNo);
    // ORIG 0x08468552 T（_ZN8WongWork18CDeathTowerRanking15unregistRankingEj，
    // 成员 this=表）：对 5 张 ranking 子表按 characNo erase。
    void unregistRanking(unsigned int characNo);
    // ORIG 0x08468b8e T（_ZN8WongWork18CDeathTowerRanking22makeMyBestRecordPacketEjjR11PacketGuard）：
    // 把 key 对应个人最优记录序列化进 packet。
    void makeMyBestRecordPacket(unsigned int towerIdx, unsigned int key,
                                PacketGuard& packet);
    // ORIG 0x08468c68 T（_ZN8WongWork18CDeathTowerRanking25makeTopRankerRecordPacketEjR11PacketGuard）：
    // 把该塔榜首记录序列化进 packet。
    void makeTopRankerRecordPacket(unsigned int towerIdx, PacketGuard& packet);

    // ---- W6_RankMailDeps 补齐（2026-08-27，报告 WongWork__CDeathTowerRanking/*）----
    // ORIG T 0x08468614（_ZN8WongWork18CDeathTowerRanking12registRankerEjjPK30stDeathTowerRecordMemberInfo_tjj）：
    // 塔记录槽登记（members 为成员信息数组，stride 0x17）。
    void registRanker(unsigned int towerIdx, unsigned int slotIdx,
                      const stDeathTowerRecordMemberInfo_t* members,
                      unsigned int stageVal, unsigned int playTime);
    // ORIG T 0x08468594：清空全部排行榜/best/层 rank map。
    void resetRanking();
    // ORIG T 0x084683da：key→rank 插入（已存在则改写），并同步 best 记录存故名次。
    void registRanking(unsigned int tableIdx, unsigned int key, unsigned int rank);
    // ORIG T 0x084684ce：仅排名表部分。
    void updateRanking(unsigned int tableIdx, unsigned int key, unsigned int rank);
    // ORIG T 0x08468914：个人最优纪录登记（record +0 存 key）。
    void registBestRecord(unsigned int towerIdx, unsigned int key,
                          const stDeathTowerRecordMemberInfo_t* members,
                          unsigned int stageVal, unsigned int playTime);

    // ---- stBestRecord_t（尺寸 0x98；ctor 0x082a76c0 / operator> 0x08469ce4 /
    // operator== 0x08469d30；ORIG 两运算符均为非 const 成员——已按 nm 对齐）----
    struct stBestRecord_t
    {
        stBestRecord_t()
        {
            m_flag90 = 0;
            m_field94 = -1;
            for (int i = 0; i <= 3; ++i)
            {
                m_memberData[i][0] = 0;
                m_pad7c[0][i] = 0;
                m_pad7c[1][i] = 0;
                m_pad7c[2][i] = 0;
            }
            m_stage = 0;
            m_playTime = 0;
        }
        // ORIG 两运算符均为非 const 成员（mangled 无 K：_ZN...teq/gtERKS1_）
        // [b28d-solo-W6_RankMailDeps] AE（compare_tu clean，23 insn 与 ORIG 逐条
        //   同形）——唯一残差 = jmp 后 1 条对齐 nop（§6.1/N112，compare_tu 口径
        //   已剥离）；P167 CLEAR(0/0)。ORIG 0x08469ce4。
        bool operator>(const stBestRecord_t& other)
        {
            // ORIG 0x08469ce4：cmp 0x88 jne→GT岛@8469d16（seta）；
            // fall-through 比 0x8c setb@8469d11（范式23 goto 钉块序）。
            if (m_stage != other.m_stage)
                goto STAGE_GT;
            return m_playTime < other.m_playTime;
        STAGE_GT:
            return m_stage > other.m_stage;
        }
        bool operator==(const stBestRecord_t& other)
        {
            // [b28d-solo-CDeathTower] verdict=AE/IDENTICAL（18/18，仅尾 nop 填充，
            // §6.1 豁免）：N96 清绑后自然比较返回形与 ORIG 逐条同形。
            // ORIG 0x08469d30：首比较 jne→mov$0 岛@8469d62；次比较 sete %al 直出@8469d5d
            // （N96 清绑：改自然比较返回形，禁 register asm；残差按 P168a④/N96 判词。）
            if (m_stage != other.m_stage)
                goto RET0;
            return m_playTime == other.m_playTime;
        RET0:
            return false;
        }

        // m_key 字段化（SecuLink 批次 2026-09-05 裸偏移清理；union 保基名）。
        // 证据：registBestRecord ORIG 0x08468914 +0x01d `mov %eax,-0x220(%ebp)`
        // （4B key dword 写，docs/class_func_reports/
        // WongWork__CDeathTowerRanking/registBestRecord.md）。
        union
        {
            char m_pad0[4];          // +0x00（基名保留）
            unsigned int m_key;      // +0x00（characNo key 槽）
        };
        char m_memberData[4][30];    // +0x04（ctor 循环清零）
        unsigned char m_pad7c[3][4]; // +0x7c（+0x7c/80/84 三条 byte lane；ORIG _makeRecordPacket
                                     // 0x8468d4a/d6a movzbl ⇒ 无符号）
        unsigned int m_stage;        // +0x88
        unsigned int m_playTime;     // +0x8c
        // [b28d-D13] P248 档②：ORIG checkRenewTopRecord 0x084682ae
        // `movzbl (%eax); xor $1; test %al; je` ⇒ 字段 bool 化 + 裸 if(!field)。
        bool m_flag90;               // +0x90（ctor 置 0；塔榜首槽此位=“有纪录”）
        unsigned int m_field94;      // +0x94（ctor 置 -1；表级纪录此位=存故名次；ORIG
                                     // _makeRecordPacket 0x8468dc6 两 mov = unsigned→int 转换形）
    };
    typedef __gnu_cxx::hash_map<unsigned int, stBestRecord_t,
                                __gnu_cxx::hash<unsigned int>,
                                std::equal_to<unsigned int>,
                                std::allocator<stBestRecord_t> > BestMap;
    // 每层排行榜：键=通关时间，值=该层名次（getVirtualRank 用其插值算虚拟名次）。
    typedef std::map<unsigned int, unsigned int> RankMap;

    // ORIG W 0x08469d90 / 0x08469dae：表内子表寻址（stride 0x14，具名数组直取）。
    RankTable* _getRankingTable(unsigned int idx);
    BestMap*   _getBestRecordTable(unsigned int idx);

    // 每塔纪录块（ORIG 线性寻址见 W6_RankMailDeps.cpp；槽 0 为塔当前榜首，
    // 见 checkRenewTopRecord）。
    struct stTowerBlock
    {
        // 注意（SecuLink 批次 2026-09-05 取证）：registRanker ORIG 0x08468614
        // asm +0x021..+0x032 `imul $0x98,%ecx(slotIdx); imul $0x390,%edx
        // (towerIdx); lea; add; add $0x90`（docs/class_func_reports/
        // WongWork__CDeathTowerRanking/registRanker.md）实证塔块为 6×0x98 连续
        // 纪录槽（slotIdx 0..5）。
        // [b28d-Main 代落·S108 提案] 原建模 `char m_pad0[0x98] + m_slots[5]` 与 ORIG
        // 两处直接反汇编矛盾：①C1 0x082a787c 内层倒计数初值 5 ⇒ 每塔 6 次
        // stBestRecord_t::C1（前缀槽 0 同样被构造，char pad 不构造）；②init
        // 0x084680a4 实证槽 1..5 的 m_flag90/memcpy(A..E)/m_field94/m_playTime/
        // m_stage 全部按 `slot*0x98 + 塔基` 直算（memcpy 目的 +0x98/+0x130/+0x1c8/
        // +0x260/+0x2f8，各 +0x04），即槽 0..5 起于塔基 +0x00，槽 1 为榜首
        // （+0x128 = 0x98+0x90，checkRenewTopRecord 0x08468290 实证）。
        // 旧模型下 CDeathTower.cpp::init 的 m_slots[1..5] 实为槽 2..6 且越界写。
        // sizeof(stTowerBlock) 仍 0x390（6×0x98），m_towers 之后全部成员偏移不变。
        stBestRecord_t m_slots[6];   // +0x000..+0x390（ORIG 槽 0..5；槽 1 = 榜首）
    };
    // C++98 兼容尺寸校验（不用 static_assert：本头未注入 SA_COMPAT）
    typedef char _stTowerBlock_size_check[(sizeof(stTowerBlock) == 0x390) ? 1 : -1];
    // ORIG T 0x08468cb6（_ZN8WongWork18CDeathTowerRanking17_makeRecordPacketERKNS0_14stBestRecord_tER11PacketGuard，
    // 定义 CompoundDeathLCB.cpp）：单条纪录序列化——4 成员名 strlen+put_str、
    // 各 put_byte(+0x80/+0x84 槽内 flag 对)，末尾 stage(short)/playTime(int)/名次(int)。
    void _makeRecordPacket(const stBestRecord_t& record, PacketGuard& packet);

private:
    stTowerBlock m_towers[kRankTowerCount];               // +0x0000（0x11d0）
    RankTable m_rankingTables[5];                         // +0x11d0
    BestMap   m_bestTables[5];                            // +0x1234
    RankMap m_rankMap[kRankTowerCount][kRankStageCount];  // +0x1298（0x5dc0）
    stBestRecord_t m_tableBest;                           // +0x7058（0x98）
    unsigned int m_tableType;                             // +0x70f0
};
}

#endif  // DNF_CDEATHTOWERRANKING_H
