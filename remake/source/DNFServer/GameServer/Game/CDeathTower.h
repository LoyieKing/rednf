#ifndef GAME_CDEATHTOWER_H_
#define GAME_CDEATHTOWER_H_

// ============================================================================
// df_game_r 还原 —— WongWork::CDeathTower（死亡之塔）权威声明。
// （2026-08-27 自 CGameManager.h 原地迁出做唯一完整定义点；
//   原 WongWork 段已由 #include "CDeathTower.h" 替换。）
// ============================================================================

#include "GameEnums.h"     // ENUM_ITEMSPACE / ENUM_CMDPACKET 等（guard 保护枚举）
#include "CParty.h"        // TIMER_MESSAGE / ENUM_DUNGEON_TYPE guard 枚举、CParty 完整类型
#include "Inven_Item.h"
#include "WarField.h"      // MapInfo / map_item / map_monster
#include "PacketGuard.h"   // CPacketHandler : public PacketGuard
#include "CDungeon.h"   // stDungeonAssignItem_t 完整定义（:114；CDungeonMgr::getSpecialPassiveObjectItems 返回 vector<T>* 需完整类型，Main 裁定方案①）

struct MSG_MONSTER_DIE;
class CDungeon;
class CMap;

namespace WongWork
{
    class CDeathTower
    {
    public:

        // CStage::consistMap（ORIG 0x8460634）读取外层塔 m_party 类型（tower 参数，
        // 汇编 0x8461371 实证 +0xcd8==1 时跳过被动物品生成）
        friend class CStage;

        // ORIG 嵌套枚举（mangled NS0_16ENUM_TOWER_STATEE / NS0_16ENUM_STAGE_STATEE）
        enum ENUM_TOWER_STATE
        {
            TOWER_STATE_NONE = 0,
            TOWER_STATE_READY = 1,
            TOWER_STATE_PLAYING = 2,
            TOWER_STATE_CLEAR = 3,
            TOWER_STATE_REWARD = 4,
            TOWER_STATE_EPLP = 5,
        };

        enum ENUM_STAGE_STATE
        {
            STAGE_STATE_NONE = 0,
            STAGE_STATE_START = 1,
            STAGE_STATE_FINISH = 2,
        };
        // ORIG 嵌套类 WongWork::CDeathTower::stMapMonsterKillChecker_t（勿与全局
        // ::stMapMonsterKillChecker_t（WarField.h）混淆——两者符号域/布局均不同：
        // 本嵌套版仅 3 字段，无 setLastMobDieTime/checkLongTermMobDie 等）。
        class stMapMonsterKillChecker_t
        {
        public:
            void init();                                       // ORIG W 0x0823462a（清零全部 3 字段）
            void setUseSkillMaterial(unsigned int material);   // ORIG W 0x0823464c（m_useSkillMaterialTime = material）
            unsigned int getUseSkillMaterialTime();            // ORIG W 0x08469cda（返回 m_useSkillMaterialTime）
            void initMonsterInfo();                            // ORIG W 0x08469c86（仅清 m_dieCnt）
            unsigned int dieMonster(unsigned int tick, unsigned int limit); // ORIG W 0x08469c96

            unsigned int m_useSkillMaterialTime;   // +0x00
            unsigned int m_lastMobDieTime;         // +0x04
            unsigned int m_dieCnt;                 // +0x08
        };
        class CDungeonMgr
        {
        public:
            CDungeonMgr();                      // ORIG W 0x82a74b0（空函数：push ebp/mov/pop ebp/ret）
            ~CDungeonMgr();                     // ORIG W 0x82a74b6（空函数）
            void reset();                       // ORIG W 0x82a7498
            bool initDungeonMgr(int dungeonIdx);// ORIG 0x84605a2
            CDungeon* getDungeon() const;
            CMap* getStageMap(unsigned int stage);  // ORIG 0x84605ee
            int getEndStage();                  // ORIG W 0x8469ab4
            // W6_RankMailDeps（ORIG W 0x08469ac0/0x08469ad6/0x08469aec）：
            int getStandardLevel() const;                                              // 转发 CDungeon::get_standard_level
            std::vector<std::pair<int, int> >* getObjectDropItems() const;             // 转发 CDungeon::getObjectDropItems
            std::vector<stDungeonAssignItem_t>* getSpecialPassiveObjectItems() const;  // 转发 CDungeon::getSpecialPassiveObjectItems
            CDungeon* m_pDungeon;   // +0x00
            unsigned int m_endStage; // +0x04（getDeathTowerMaxStage 写入；b28d-D13：
                                     // 与 int 返回值异型化 ⇒ 4.4 RHS-first+中转，复现
                                     // ORIG 0x084605e0 mov %eax,%edx 形，P211）
        };


        class CStage
        {
        public:
            // [b28d-Main·检查点14 A/B] 去 throw()：S411 实测两处 throw() 全带时
            //   C1 30/D1 17（ORIG 45/33，EH 段被整体消掉）⇒ CStage ctor 在 ORIG
            //   语义为**可能抛**（ORIG C1 landing 0x82a763f 只调 ~CDungeonMgr）；
            //   正确组合 = CPlayData 带 throw()（nothrow）+ CStage 不带。
            CStage();                           // ORIG W 0x82a74ca：MapInfo ctor + resetCurrentStage
            ~CStage();                          // ORIG W 0x82a74ec：MapInfo dtor
            void reset();                       // ORIG 0x84614ec
            void resetCurrentStage();           // ORIG W 0x82a74bc（m_currentStage = 1）
            bool setState(ENUM_STAGE_STATE state);  // ORIG 0x8461c5a
            int getState();
            bool checkClearStage();
            int killMonster(int idx, map_monster& monster);
            bool moveNextStage(unsigned int endStage);
            void onStartStage();
            void onFinishStage();
            int getStageClearTime();
            int getStageMapIndex();
            int getCurrentStage();
            bool peekItem(int idx, map_item& item);
            bool pickupItem(int idx, map_item& item);
            int dropItem(map_item& item);       // ORIG 0x84616a8（非 const 引用）
            bool consistMap(const CDungeonMgr& dungeonMgr, const CMap* map,
                            CDeathTower* tower);    // ORIG 0x8460634
            void makeStagePacket(PacketGuard& packet);

            int m_currentStage;      // +0x00
            unsigned int m_startTick;   // +0x04
            unsigned int m_finishTick;  // +0x08
            MapInfo m_mapInfo;       // +0x0c..+0xf8（ORIG C1/D1/reset/dropItem 实证内嵌）
            ENUM_STAGE_STATE m_state;  // +0xf8（原 int；ORIG 0x8461ca0 store 序证据 → 同型直落）
        };

        class CPlayData
        {
        public:
            // ORIG W 0x82a7502 C1（10× Inven_Item ctor，步长 0x3d）/ 0x8461cb4 reset；
            // 尺寸 0x268 = 4 + 10*0x3d 对齐到 4。
            class stRewardItem_t
            {
            public:
                stRewardItem_t();               // ORIG W 0x82a7502：逐个 Inven_Item() 构造 10 项

                void reset();

                int m_itemCnt;           // +0x00
                Inven_Item m_items[10];  // +0x04
            };

            // b28d-D13：P216 throw() 实测过度折叠（CPlayData C1/D1 空体，4.4 连 call
            // 一起消除：C1 53→29、D1 41→17 insn）⇒ 回退，C1/D1 残差按 N91 封顶。
            // [b28d-Main·检查点14 A/B] S411：加 throw() 使 4.4 判定 CPlayData::C1 nothrow，
            // 消调用点多余 ~CStage cleanup（ORIG 0x82a75da 的 landing 段只调 ~CDungeonMgr）。
            CPlayData() throw();                // ORIG W 0x82a7538：m_rewards[4] 逐个 stRewardItem_t()
            ~CPlayData() throw();               // ORIG W 0x82a7572（空函数）

            void generateRewardExp(int memberIdx, int stage, unsigned int baseExp,
                                   CParty* party, CDungeonMgr& dungeonMgr);
                                   // ORIG T 0x08462538
            void generateRewardItem(int memberIdx, int count, const CDungeonMgr& dungeonMgr,
                                    unsigned int param4, unsigned int param5, CParty* party);
                                   // ORIG T 0x08461fc0
            void reset();
            int getStartMemberCnt();
            void makeStartMemberInfo(CParty* party);
            void resetMemberReady();
            void setMemberAlive(int idx, bool alive);
            bool isMemberAlive(int idx);
            bool isMemberReady(int idx);
            void setLastRoutedTurn(int turn);
            char* getStartMemberInfo();
            void setMemberReady(int idx, bool ready);
            int getLastRoutedTurn();
            bool checkEnterStartMap(int idx);
            bool checkAllMemberReady(int memberCnt);
            void addPlayTime(unsigned int time);
            unsigned int getPlayTime();
            unsigned int getLastPlayTime();
            Inven_Item* getRewardItem(int idx);
            int getRewardItemCount(int idx);
            int getRewardExp(int idx);

            int m_lastRoutedTurn;               // +0x00（getLast/setLastRoutedTurn）
            bool m_memberAlive[4];              // +0x04
            bool m_memberReady[4];              // +0x08
            char m_allReady;                    // +0x0c（byte，checkAllMemberReady）
            char m_pad0d[3];
            unsigned int m_playTime;            // +0x10
            unsigned int m_lastPlayTime;        // +0x14
            stRewardItem_t m_rewards[4];        // +0x18..+0x9b8（步长 0x268）
            int m_rewardExp[4];                 // +0x9b8..+0x9c7
            // [b28d-solo-CDeathTower] P211 档③：ORIG makeStartMemberInfo 尾
            // `mov %eax,%edx; mov this,%eax; mov %edx,0x9c8(%eax)`（RHS-first+中转）
            // ⇒ 字段与 get_member_count() 返回类型异型（unsigned int）。
            unsigned int m_startMemberCnt;      // +0x9c8
            // [b28d-solo-CDeathTower] 具名元素视图（sizeof/偏移不变 0x5c@+0x9cc）：
            // ORIG makeStartMemberInfo 0x08461d96 起对 strcpy 目标/字段发
            // `imul $0x17` + add 链（P30/P311），一维 `[count*0x17+K]` 在 4.4 恒折
            // lea 0x9cc(%base,%idx)。
            struct stStartMemberInfo_t
            {
                char m_name[0x14];
                char m_level;
                char m_job;
                char m_growType;
            };
            stStartMemberInfo_t m_startMemberInfo[4];   // +0x9cc（元素 0x17）
            bool m_enterStartMap[4];            // +0xa28（checkEnterStartMap）
        };

        // ORIG 嵌套包处理器：CPacketHandler : public PacketGuard
        //（ctor 0x08469a8c / dtor 0x08469aa0 仅转发 PacketGuard C1/D1）。
        class CPacketHandler : public PacketGuard
        {
        public:
            // [b28d-Main 复核·S214] 曾试加 throw()（P216）——A/B 实测对 handleMoveMap 零影响
            // （两向均 0 处 _Unwind_Resume，TU 计数同为 994）：本处是**局部对象**非 new 表达式，
            // 且 ORIG 的 landing pad 确实调用 CPacketHandlerD1Ev（0x8465cfe/0x8465d80/0x8465f8b/
            // 0x8466004）⇒ S214 的「ORIG 判 nothrow 不布」根因不成立，已回退。
            CPacketHandler();           // ORIG W 0x08469a8c
            ~CPacketHandler();          // ORIG W 0x08469aa0

            void makePickupItemHeader(unsigned short a, unsigned short b);   // ORIG T 0x0845fc6c
            void appendPickupMoney(int* moneyA, int* moneyB, CParty* party);// ORIG T 0x0845fcc4
            void appendPickupItem(char* name, unsigned short uid, int v3, int v4); // ORIG T 0x0845fdc0
            void makeCommonError(ENUM_CMDPACKET cmd, ENUM_ERROR err);       // ORIG T 0x0845fe4c
            void makeDropItemError(ENUM_CMDPACKET cmd, ENUM_ERROR err, char slot); // ORIG T 0x0845fea6
            void makeDropItemNotify(unsigned short uid, int x, int y,
                                    unsigned short dropIdx, const map_item& item); // ORIG T 0x0845ff1a
            void makeDropItem(char order, int xy, int itemKey);             // ORIG T 0x084600a6
            void makeFinishLoading();                                       // ORIG T 0x0846012c
            void makeDieState(unsigned short uid);                          // ORIG T 0x08460176
            void makeDisplayRangking(unsigned int tableType, unsigned int key,
                                     unsigned int stage, unsigned int clearTime, bool flag,
                                     unsigned int dungeonIdx, unsigned int playTime); // ORIG T 0x084601da
            void makeDisplayReward(int v1, int cnt1, const Inven_Item* items1,
                                   int cnt2, const Inven_Item* items2,
                                   int cnt3, const Inven_Item* items3,
                                   int cnt4, const Inven_Item* items4);     // ORIG T 0x084602c4
            void makeDisplayEPLP(bool flag);                                // ORIG T 0x084604b0
            void makeUpdateItemList(CUser* user, ENUM_ITEMSPACE space, int slot); // ORIG T 0x0846050e
        };

    CDeathTower();
    ~CDeathTower();

    void _beginTowerClearProcess(bool);
    bool _checkMemberDie();   // 原 int；ORIG caller 0x84661bc `test %al,%al` 直用 al 无归一 ⇒ bool 返回（mangled 无返回类型）
    void _checkMemberReady();  // 原 int；ORIG 0x84670f8 尾无 %eax 置值 ⇒ void（mangled 无返回类型）
    int _checkRenewMyRecord(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
    // [b28d-solo-CDeathTower] ORIG 0x0846801e 尾透传 callee 的 %al 无加宽 ⇒ bool。
    bool _checkRenewTopRecord(unsigned int, unsigned int, unsigned int, unsigned int);
    int _checkStartGameCondition(unsigned char&);
    bool _checkTimerKey(TIMER_MESSAGE, unsigned int);   // ORIG 0x8469c68
    void _destroy();
    int _genTimerKey(TIMER_MESSAGE);
    bool _makeDropItem(CUser*, char, int, int, map_item&);
    void _onClear(bool);
    void _onFinishDeathTower();
    void _onFinishStage();
    void _onPrepareFinishTower();
    void _onStartDeathTower();
    void _onStartStage();
    CUser* _pickupItem(CUser*, char*, int, const map_item&, bool, char, const CItem*);
    void _pickupItemMoney(CUser*, int, const map_item&, int*, int*);  // 原 int；ORIG 0x8466cf8 尾无 %eax 置值 ⇒ void（mangled 无返回类型）
    void attachParty(CParty*);
    void deathTowerCardStatistic(int, CUser*, const Inven_Item*);
    int genTimerKey(TIMER_MESSAGE);
            CDungeonMgr* getCDungeonMGr() const;   // ORIG _ZNK…14getCDungeonMGrEv
            CStage* getCStage();
    int getIdx();
    int getTowerState();
    int handleDieUser(CUser*);
    int handleDropItem(CUser*, int, int, char, int, int);
    int handleFinishLoading(CUser*);
    int handleKillMonster(CUser*, unsigned short, unsigned short, const MSG_MONSTER_DIE&, int*);
    int handleLeaveUser(CUser*);
    int handleMoveMap();
    // [K9GameRestA-S322 · R2] P292 档①：ORIG 调用点 0x081c3b93 `call; test %al,%al;
    // je 0x081c3cd2`（单字节直判、无 setne 归一化）；我方原 int 返回使调用点多
    // `test %eax,%eax; sete %al; test %al,%al`。返回类型不入 mangling ⇒ 符号名/
    // vtable 槽序/sizeof 零影响。消费点：DisPatcherCore/process_43.inc:141（全仓唯一）。
    bool handlePickupItem(CUser*, int, bool, char);
    int handleSelectEPLPCommand(CUser*, int);
    int handleStageCommand(CUser*, unsigned char);
    int handleStartGame(int, char, ENUM_DUNGEON_TYPE);
    bool handleUseStackable(CUser*, ENUM_ITEMSPACE, unsigned short);  // [b28d-Main·检查点14 P292] 定义 0x084662cc 返 1；调用点 xor $1
    bool onLeaveUser(CUser*);  /* ORIG 0x084636f2 尾恒 mov $1,ret；caller xor$1/test%al bool 归一形（P9/P159） */
    int onTimer(TIMER_MESSAGE, unsigned int);
    void reset();
    void setIdx(int);
    int setTowerState(ENUM_TOWER_STATE);

private:
    CParty* m_party;        // +0x00
    int m_idx;              // +0x04
    ENUM_TOWER_STATE m_towerState;  // +0x08（原 int；ORIG 0x8469c4b store 序证据 → 同型直落）
    CDungeonMgr m_dungeonMgr;  // +0x0c
    CStage m_stage;         // +0x14
    CPlayData m_playData;   // +0x110（尺寸 0xa2c，至 +0xb3c）
    int m_timerKeys[10];    // +0xb3c (TIMER_MESSAGE 0x48..)
    // 布局修正（Layout_D 2026-08-30）：ORIG StaticPool<CDeathTower,600>::CNodeC1
    // 实证 bUse@0xb68 → sizeof(CDeathTower)=0xb64；原 m_pad_end[0x18] 尾垫多余。
};                                                  // sizeof 0xb64

// CBossTower 类声明见 CBossTower.h（唯一声明点）
}

#endif  // GAME_CDEATHTOWER_H_
