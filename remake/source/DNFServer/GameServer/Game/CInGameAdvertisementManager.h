#ifndef GAME_CINGAMEADVERTISEMENTMANAGER_H_
#define GAME_CINGAMEAdvertisementMANAGER_H_

// ============================================================================
// df_game_r 还原 —— CInGameAdvertisementManager / InGameADManagerInstance
// （ORIG 全套 T/W 符号实证，见 docs/class_func_reports/CInGameAdvertisementManager/）。
// 本头为唯一声明点（InterWave1 批建立）；类布局字段化由后续批次按报告完成，
// 当前消费方仅经指针/引用使用，前向即可。
// ============================================================================

#include <map>
#include <utility>
#include <vector>
#include <string>

#include "Inven_Item.h"   // std::pair<Inven_Item,bool> 形参需完整类型
class CMTRand;            // PvPTypes.h（MT19937；ctor/dtor 在 CInGameAdvertisementManager.cpp）
template<class T> class MemPool;   // 前向声明（静态池成员；完整定义在 MemPool.h，cpp 使用）

// 广告曝光条件记录（ORIG stride 0x84c：InsertADExposeInfo 以 +0x204 为
// multimap 键；LoadADExposeInfoAtApp 按条件缓冲逐项 0x84c 载入。字段布局按
// queryADExposeInfo.md 20 列 DB 填充逐偏移还原；成员访问生成 ORIG 的
// imul 0x84c/add cond/add off 形态（探针实证，P1 组件视图同类）。
struct stInGameADExposeCondition
{
    char m_pad0[0x204];       // +0x000..0x203（[0]=性别 [1]=年龄 [2]=职业 [0x203]=取模率）
    short m_key;              // +0x204（expose 多重映射键 / get_short(0x12) 列）
    char m_pad206[2];         // +0x206..0x207
    unsigned int m_minCera;   // +0x208（get_uint(0xd) 商城币下限）
    unsigned int m_maxCera;   // +0x20c（get_uint(0xe) 商城币上限）
    int m_sentinel;           // +0x210（哨兵 -1 / get_uint(0) 列）
    char m_title[0x32];       // +0x214..0x245（get_str(1)）
    char m_field246[0xff];    // +0x246..0x344（get_str(6)）
    char m_field345[0xff];    // +0x345..0x443（get_str(8)）
    unsigned int m_field444;  // +0x444（get_uint(7)）
    char m_field448[0xff];    // +0x448..0x546（get_str(0x14)）
    char m_field547;          // +0x547（get_byte(0xf)）
    char m_field548;          // +0x548（get_byte(0x15)）
    char m_exposeFlag;        // +0x549（checkCondition 暴露标记）
    char m_field54a[0xff];    // +0x54a..0x648（get_str(9)）
    char m_field649[0xff];    // +0x649..0x747（get_str(10)）
    unsigned int m_field748;  // +0x748（get_uint(0xb)）
    char m_field74c[0xff];    // +0x74c..0x84a（get_str(0xc)）
    char m_pad84b;            // +0x84b（凑 stride 0x84c）
};

// CAdvertisementExposeInfo（ORIG sizeof 0x850 = 0x84c 条件 + 4 pad；生命周期
// 符号见 CInGameAdvertisementManager.cpp 文件头）。
class CAdvertisementExposeInfo
{
public:
    // 无用户 ctor/dtor（ORIG nm 无 C1/C2/D1/D2；trivial 构造使 new 不生成 ctor call）
    static void* operator new(unsigned int size);                       // T 0x080fc998
    static void operator delete(void* p);                               // T 0x080fc9ce
    static void operator delete(void* p, unsigned int size);            // T 0x080fc9ac
    void SetAdvertisementExposeInfo(stInGameADExposeCondition& cond);   // W 0x080fdd0a
    stInGameADExposeCondition* getExposeCondition();                    // W 0x080fdd2c

private:
    stInGameADExposeCondition m_condition;  // +0x00..0x84b
    // ORIG 静态池实例 B 0x093fea90（mangled _ZN24CAdvertisementExposeInfo28
    // advertisementExposeInfoPool_E，0x14 字节 = int+uint+vector；ctor count 0x32）。
    static MemPool<CAdvertisementExposeInfo> advertisementExposeInfoPool_;
    unsigned int m_pad84c;                  // +0x84c..0x84f（池块 0x850 对齐余量）
};
// 广告通知信息块（ORIG 0x63c 字节：CheckIngameAdvertisementCondition
// memcpy(…, +0x210, 0x63c) / rep movsl 0x18f 实证；内部字段未还原，pad 视图）。
struct stNotifyIngameADInfo
{
    // ORIG construct/_M_insert_aux 尾 rep movsl 0x18f×4 ⇒ 结构对齐 4（int 视图，
    // 尺寸仍 0x63c；char[0x63c] 会对齐 1 使 gcc 展开通用逐段拷贝）。
    int m_pad[0x18f];
};
class CUser;

class CInGameAdvertisementManager
{
public:
    CInGameAdvertisementManager();   // ORIG W 0x080fdd6a（multimap + new CMTRand(time)）
    ~CInGameAdvertisementManager();  // ORIG W 0x080fca7c（delete m_rand + ~multimap）
    void InitADExposeInfo();                                       // ORIG T 0x080fd122
    bool ReloadADExposeInfo();                                     // ORIG T 0x080fd95e
    bool LoadADExposeInfoAtApp();                                  // ORIG T 0x080fd83c
    void InsertADExposeInfo(stInGameADExposeCondition& cond);      // ORIG T 0x080fd8e4
    bool queryADExposeInfo(stInGameADExposeCondition* cond);       // ORIG T 0x080fd19e
    bool checkLevel(CUser* user, char* data);                      // ORIG T 0x080fca9e
    bool checkCondition(CUser* user, CAdvertisementExposeInfo* info);  // ORIG T 0x080fcd2e
    bool CheckFailMultiBoxLottery(CUser* user,
        std::vector<std::pair<Inven_Item, bool> >& items);         // ORIG T 0x080fcb3e
    bool CheckIngameAdvertisementCondition(CUser* user,
        stNotifyIngameADInfo& info, int flag);                     // ORIG T 0x080fcfa8
    unsigned int RandNum(int range);                               // ORIG W 0x080fdd34
    void WriteLog(CUser* user, char type, short val, char flag,
                  unsigned int extra);                             // ORIG T 0x080fda56

private:
    // ORIG +0x00 起为 multimap<short,CAdvertisementExposeInfo*>（clear(this)
    // / insert 证据；本头此前无成员，尺寸未断言，指针消费方不受影响）。
    std::multimap<short, CAdvertisementExposeInfo*> m_exposeInfo;  // +0x00
    // ORIG ctor 0x080fdd6a：time(0) → new CMTRand(0x9c8) 存 +0x18；D1 delete 之。
    CMTRand* m_rand;                                             // +0x18
};

// ORIG _Z23InGameADManagerInstancev T @0x080fc9e9（函数局部 static 单例，
// guard 0x093feac8 / instance 0x093fead0；定义属主未落地，U 引用合规）
CInGameAdvertisementManager* InGameADManagerInstance();

#endif  // GAME_CINGAMEADVERTISEMENTMANAGER_H_
