// df_game_r 还原 —— 战斗/地图小结构基础方法（独立 TU）。
//   布局与 WarField.h 权威定义一致（stMapMonsterKillChecker_t/stMapPlayInfo_t）：
//     stMapMonsterKillChecker_t::init()  ORIG W 0x81512ec（清空 6×int=0x18 字节）
//     stMapPlayInfo_t::init()            ORIG W 0x815132c（清空 4×int=0x10 字节）
//     stGenerateResult_t ctor/dtor       ORIG W 0x814ab06/0x814ab4c
//                                        （+0x00 vector<Inven_Item>=0xc，默认构造=置空）
//   ORIG mangled: _ZN25stMapMonsterKillChecker_t4initEv /
//                 _ZN15stMapPlayInfo_t4initEv /
//                 _ZN18stGenerateResult_tC1Ev / C2Ev / D1Ev / D2Ev
//   （objdump 实证：两 init 均逐 movl $0x0 清对应偏移；ctor 调用 St6vectorI10Inven_Item 默认构造。）

#include <cstring>

struct stMapMonsterKillChecker_t
{
    unsigned int m_useSkillMaterialTime;   // +0x00
    unsigned int m_lastMobDieTime;         // +0x04
    int m_uncontinuallyMobDieCnt;          // +0x08
    int m_dieCnt;                          // +0x0c
    int m_field10;                         // +0x10
    int m_field14;                         // +0x14
    void init();                           // ORIG W 0x81512ec
};

void stMapMonsterKillChecker_t::init()
{
    m_useSkillMaterialTime = 0;
    m_lastMobDieTime = 0;
    m_uncontinuallyMobDieCnt = 0;
    m_dieCnt = 0;
    m_field10 = 0;
    m_field14 = 0;
}

struct stMapPlayInfo_t
{
    unsigned int m_startTick;    // +0x00
    unsigned int m_clearTick;    // +0x04
    int m_useItemType2Count;     // +0x08
    int m_useItemOtherCount;     // +0x0c
    void init();                 // ORIG W 0x815132c
};

void stMapPlayInfo_t::init()
{
    m_startTick = 0;
    m_clearTick = 0;
    m_useItemType2Count = 0;
    m_useItemOtherCount = 0;
}

struct stGenerateResult_t
{
    char m_pad[0xc];   // 对应 vector<Inven_Item>（0xc 字节）
    stGenerateResult_t();
    ~stGenerateResult_t();
};

stGenerateResult_t::stGenerateResult_t()
{
    std::memset(m_pad, 0, sizeof(m_pad));   // vector 默认构造 = 全空
}

stGenerateResult_t::~stGenerateResult_t()
{
}