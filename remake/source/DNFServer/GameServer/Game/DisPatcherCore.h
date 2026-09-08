#ifndef DNF_GAME_DISPATCHERCORE_H_
#define DNF_GAME_DISPATCHERCORE_H_

// df_game_r 还原 —— PacketDispatcher 注册表中缺失声明的 79 个具体分发器类。
// 基类模板实参加载自各自 ORIG 构造函数调用的 IPacketDispatcher<...>::C1 实例化；
// 方法集来自 ORIG nm --defined-only。方法体为带 TODO 标注的 stub（后续批次）。

#include "Dispatchers.h"          // CMDPacketStruct::STBaseResponse 等
#include "PacketTypes.h"

struct MSG_MAKE_PVP_ROOM;   // wave1: DisPatcher_MakePVP::ExtractPacket 形参前置（完整定义 DisPatcherCore.cpp 声明区）
struct MSG_RES_PVP_RANK;    // wave1: DisPatcher_ResPvpRank 两成员形参前置（同上）


class DisPatcher_CheckConn : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_CheckConn() {}
    virtual ~DisPatcher_CheckConn();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_Login : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_Login() throw();   // ORIG PacketDispatcher ctor 槽1 new 无 EH cleanup
    int m_field4;   // +0x04（ORIG C1 0x081e8b7e 域写 0；登录态标记占位）
    virtual ~DisPatcher_Login();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_SetIP : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_SetIP() {}
    virtual ~DisPatcher_SetIP();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_Exit : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_Exit() {}
    virtual ~DisPatcher_Exit();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_SelectCharac : public IPacketDispatcher<MSG_CHARAC_SELECT, stSelectCharac, (ch_state)2>
{
public:
    DisPatcher_SelectCharac() {}
    virtual ~DisPatcher_SelectCharac();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
    // wave1: ORIG T 0x081c6c22 _ZN23DisPatcher_SelectCharac22checkPvpEnterCharacterEP5CUserR17MSG_CHARAC_SELECT
    // （本 TU 家族；报告 checkPvpEnterCharacter.md 在，定义待落地 → U 引用合规）
    bool checkPvpEnterCharacter(CUser* user, MSG_CHARAC_SELECT& msg);
    // ---- ORIG helper（T 符号；定义见 DisPatcherCore.cpp，报告 docs/class_func_reports/）----
    int RequestDBInventory_forchina(CUser* user, MSG_CHARAC_SELECT& msg);   // 0x081c7c1c
    int RequestDBCharac(CUser* user, MSG_CHARAC_SELECT const& msg);   // 0x081c6ca0
    int RequestBlackCount(CUser* user, MSG_CHARAC_SELECT& msg);       // 0x081c79f2
    void RequestDBInventory(CUser* user, MSG_CHARAC_SELECT& msg);     // 0x081c6ee6（ORIG 返回 void）
    int RequestDBQuest(CUser* user, MSG_CHARAC_SELECT& msg);          // 0x081c70c2
    int RequestDBDungeonClear(CUser* user, MSG_CHARAC_SELECT& msg);   // 0x081c7436
    int RequestDBKillMonsterInfo(CUser* user, MSG_CHARAC_SELECT& msg);// 0x081c7836
    int RequestDBETC(CUser* user, MSG_CHARAC_SELECT& msg);            // 0x081c7602
    int RequestDBSkill(CUser* user, MSG_CHARAC_SELECT& msg);          // 0x081c6f08
    // ORIG T 0x081c727c / 0x081c7bac（定义 DisPatcherCore.cpp；报告 RequestDBTicket.md /
    // RegistTimerSchedules.md）：SelectCharac 登录流程 DB 票请求与定时器注册。
    int RequestDBTicket(CUser* user, MSG_CHARAC_SELECT& msg);        // 0x081c727c
    int RegistTimerSchedules(CUser* user);                           // 0x081c7bac
};

class DisPatcher_CreateCharac : public IPacketDispatcher<MSG_CREATE_CHARACTER, stCreateCharac, (ch_state)2>
{
public:
    DisPatcher_CreateCharac() {}
    virtual ~DisPatcher_CreateCharac();
    int check_error(CUser* user, MSG_BASE& msg);
    int check_state(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void makeGift(CUser* user, SIG_CREATE_CHARAC* sig);  // ORIG T 0x081c0a1a（定义见 DisPatcherCore.cpp）
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_DeleteCharac : public IPacketDispatcher<MSG_DELETE_CHARACTER, stDeleteCharac, (ch_state)2>
{
public:
    DisPatcher_DeleteCharac() {}
    virtual ~DisPatcher_DeleteCharac();
    int check_error(CUser* user, MSG_BASE& msg);
    int check_state(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_ReturnToSelectCharacter : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_ReturnToSelectCharacter() {}   // ORIG C1 0x085959e0 仅 28B（不初始化 +4）
    int m_field4;   // +0x04 占位（ORIG PacketDispatcher ctor 槽7 new(8) 实证对象 8B；C1 不写、
                    //  dispatch_sig 0x081fd25c 无 this+4 访问 → 纯数据成员，无名可依）
    virtual ~DisPatcher_ReturnToSelectCharacter();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_GetUserInfo : public IPacketDispatcher<MSG_USERINFO_REQ, stOnGetUserInfoParam, (ch_state)1>
{
public:
    DisPatcher_GetUserInfo() {}
    virtual ~DisPatcher_GetUserInfo();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_RecoverStamina : public IPacketDispatcher<MSG_USERINFO_REQ, stOnRecoverStaminaParam, (ch_state)3>
{
public:
    DisPatcher_RecoverStamina() {}
    virtual ~DisPatcher_RecoverStamina();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_ReqPeer : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_ReqPeer() {}
    virtual ~DisPatcher_ReqPeer();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_ResPeer : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_ResPeer() {}
    virtual ~DisPatcher_ResPeer();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_SetPartyInfo : public IPacketDispatcher<MSG_PARTY_SET_INFO, stSetPartyInfo, (ch_state)3>
{
public:
    DisPatcher_SetPartyInfo() {}
    virtual ~DisPatcher_SetPartyInfo();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_LeaveParty : public IPacketDispatcher<MSG_BASE, stLeavePartyParam, (ch_state)0>
{
public:
    DisPatcher_LeaveParty() {}
    virtual ~DisPatcher_LeaveParty();
    int check_error(CUser* user, MSG_BASE& msg);
    int check_state(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_WalkoutParty : public IPacketDispatcher<MSG_WALKOUT_PARTY, stWalkoutPartyParam, (ch_state)3>
{
public:
    DisPatcher_WalkoutParty() {}
    virtual ~DisPatcher_WalkoutParty();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_StartGame : public IPacketDispatcher<MSG_BASE, stStartGameParam, (ch_state)0>
{
public:
    DisPatcher_StartGame() {}
    virtual ~DisPatcher_StartGame();
    // ORIG mangled _ZN20DisPatcher_StartGame11check_errorEP5CUserR8MSG_BASER9ParamBase
    // （nm 实证三参；stub 体保持 return 0，与 UseCoin/DropItem 同规约）
    int check_error(CUser* user, MSG_BASE& msg, ParamBase& param);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    int excludeAddHackCount(CUser* user);   // ORIG T 0x081ca2a8
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_SelectDungeon : public IPacketDispatcher<MSG_SELECT_DUNGEON, stSelectDungeonParam, (ch_state)0>
{
public:
    DisPatcher_SelectDungeon() {}
    virtual ~DisPatcher_SelectDungeon();
    // ORIG mangled _ZN24DisPatcher_SelectDungeon11check_errorEP5CUserR8MSG_BASER9ParamBase
    int check_error(CUser* user, MSG_BASE& msg, ParamBase& param);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    int checkBlueMarble(CParty* party);     // ORIG T 0x081c9172
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_SendMess : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_SendMess() {}
    virtual ~DisPatcher_SendMess();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_DeleteItem : public IPacketDispatcher<MSG_DELETE_ITEM, stDeleteItemParam, (ch_state)3>
{
public:
    DisPatcher_DeleteItem() {}
    virtual ~DisPatcher_DeleteItem();
    // ORIG check_error 为二参（_ZN21DisPatcher_DeleteItem11check_errorEP5CUserR8MSG_BASE）
    int check_error(CUser* user, MSG_BASE& msg);
    bool exercise_mode(CUser* user, MSG_BASE& msg, ParamBase& param);  // 0x081c1ad6
    int restricted_check(CUser* user, MSG_BASE& msg);                  // 0x081c1cf6
    int condition_check(CUser* user, MSG_BASE& msg, INVEN_TYPE& type, int idx);  // 0x081c1d5c
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_MoveItem : public IPacketDispatcher<MSG_MOVE_ITEMSPACE, stMoveItemParam, (ch_state)3>
{
public:
    DisPatcher_MoveItem() {}
    virtual ~DisPatcher_MoveItem();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_SortItem : public IPacketDispatcher<MSG_SORT_ITEM, stOnSortItemParam, (ch_state)3>
{
public:
    DisPatcher_SortItem() {}
    virtual ~DisPatcher_SortItem();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_BuyItem : public IPacketDispatcher<MSG_BUY_ITEM, BuyItemParam, (ch_state)3>
{
public:
    DisPatcher_BuyItem() {}
    virtual ~DisPatcher_BuyItem();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
    // ---- ORIG T 符号（报告 docs/class_func_reports/DisPatcher_BuyItem/）----
    int get_data(CUser* user, MSG_BUY_ITEM& msg, BuyItemParam& param);            // 0x081be658
    unsigned int get_nMaxBuyableCount(CUser* user, BuyItemParam& param);          // 0x081beab0
    void _onBuyItemEvent(const BuyItemParam& param, CUser* user);                 // 0x081bed44（ORIG 纯空）
    void statistic_process(CUser* user, BuyItemParam& param);                     // 0x081bec5e
};

class DisPatcher_RentAvatar : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_RentAvatar() {}
    virtual ~DisPatcher_RentAvatar();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_ExtendAvatar : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_ExtendAvatar() {}
    virtual ~DisPatcher_ExtendAvatar();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_SellItem : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_SellItem() {}
    virtual ~DisPatcher_SellItem();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_RepairEquip : public IPacketDispatcher<MSG_REPAIR_EQUIP, stRepairEquipParam, (ch_state)0>
{
public:
    DisPatcher_RepairEquip() {}
    virtual ~DisPatcher_RepairEquip();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_SetTradeState : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_SetTradeState() {}
    virtual ~DisPatcher_SetTradeState();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_Compound : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_Compound() {}
    virtual ~DisPatcher_Compound();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Inven_Item;
class DisPatcher_DisJointItem : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_DisJointItem() {}
    virtual ~DisPatcher_DisJointItem();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);

    // ORIG T 0x081fa0f6 _ZN23DisPatcher_DisJointItem22_checkDisjointableItemERK10Inven_ItemP5CUser
    // （静态成员；定义 ExpertJobLink4.cpp）
    static int _checkDisjointableItem(const Inven_Item& item, CUser* targetUser);
    // wave1: ORIG T 0x081f92ca _ZN23DisPatcher_DisJointItem9_disjointEP5CUserii14ENUM_CMDPACKETS1_t
    // （静态成员无 this；当前逻辑由 DispDebugJoint4.cpp 自由函数承载，需迁回真实符号）
    static int _disjoint(CUser* user, int a, int b, ENUM_CMDPACKET cmd, CUser* target, unsigned short w);
};

class DisPatcher_Lottery : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_Lottery() {}
    virtual ~DisPatcher_Lottery();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_ChangeSkill : public IPacketDispatcher<MSG_CHANGE_SKILLSLOT, stOnChangeSkillParam, (ch_state)3>
{
public:
    DisPatcher_ChangeSkill() {}
    virtual ~DisPatcher_ChangeSkill();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_BuySkill : public IPacketDispatcher<MSG_BUY_SKILL, stOnBuySkillParam, (ch_state)3>
{
public:
    DisPatcher_BuySkill() {}
    virtual ~DisPatcher_BuySkill();
    int check_error(CUser* user, MSG_BASE& msg);
    int check_state(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
    // ---- ORIG helper（T 符号；报告 docs/class_func_reports/DisPatcher_BuySkill/）----
    // 四者当前无 TU 定义（待专项批次按各自报告还原）；process 已按其签名调用。
    void master_new_skill_process(CUser* user, char treeKind, buySkillInfo& info);        // 0x081bf952
    void refund_skill_process(CUser* user, char treeKind, buySkillInfo& info);            // 0x081bfc5c
    void try_refund_skills_process(CUser* user, char treeKind, buySkillInfo& info);       // 0x081bfd3c
    void try_master_new_skills_process(CUser* user, char treeKind, buySkillInfo& info);   // 0x081bfdf0
};

class DisPatcher_IncreStatus : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_IncreStatus() {}
    virtual ~DisPatcher_IncreStatus();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_QuestAction : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_QuestAction() {}
    virtual ~DisPatcher_QuestAction();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_SetPos : public IPacketDispatcher<MSG_USER_SET_POS, ParamBase, (ch_state)0>
{
public:
    DisPatcher_SetPos() {}
    virtual ~DisPatcher_SetPos();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_SetArea : public IPacketDispatcher<MSG_SET_USER_AREA, stSetAreaParam, (ch_state)3>
{
public:
    DisPatcher_SetArea() {}
    virtual ~DisPatcher_SetArea();
    // ORIG mangled _ZN18DisPatcher_SetArea11check_errorEP5CUserR8MSG_BASER9ParamBase
    int check_error(CUser* user, MSG_BASE& msg, ParamBase& param);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_Fish : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_Fish() {}
    virtual ~DisPatcher_Fish();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_FinishLoad : public IPacketDispatcher<MSG_FINISH_LOADING, stFinishLoadParam, (ch_state)0>
{
public:
    DisPatcher_FinishLoad() {}
    virtual ~DisPatcher_FinishLoad();
    int check_error(CUser* user, MSG_BASE& msg);
    int check_state(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    // ORIG T 0x08264c28 _ZN21DisPatcher_FinishLoad16Party_hack_checkEP5CUser
    void Party_hack_check(CUser* user);   // 定义见 DisPatcherCore.cpp（本批）
};

class DisPatcher_UseSkill : public IPacketDispatcher<MSG_USE_SKILL, stUseSkillParam, (ch_state)0>
{
public:
    DisPatcher_UseSkill() {}
    virtual ~DisPatcher_UseSkill();
    // ORIG _ZN19DisPatcher_UseSkill11check_errorEP5CUserR8MSG_BASER9ParamBase（三参实证）
    int check_error(CUser* user, MSG_BASE& msg, ParamBase& param);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_DieMob : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_DieMob() {}
    virtual ~DisPatcher_DieMob();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_DieCharacInDungeon : public IPacketDispatcher<MSG_DIE_CHARACTER, stDieCharacInDungeon, (ch_state)0>
{
public:
    DisPatcher_DieCharacInDungeon() {}
    virtual ~DisPatcher_DieCharacInDungeon();
    int check_error(CUser* user, MSG_BASE& msg);
    int check_state(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_UseCoin : public IPacketDispatcher<MSG_USE_COIN, stUseCoinParam, (ch_state)0>
{
public:
    DisPatcher_UseCoin() {}
    virtual ~DisPatcher_UseCoin();
    int check_error(CUser* user, MSG_BASE& msg, ParamBase& param);  // ORIG _ZN18DisPatcher_UseCoin11check_errorEP5CUserR8MSG_BASER9ParamBase
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    // wave1/wave2: 本 TU 家族 helper（ORIG T 0x081ca63a / 0x081ca75c / 0x081cab3a）
    int Dungeon_check_error(CUser* user, MSG_BASE& msg, ParamBase& param);
    int WarRoom_check_error(CUser* user, MSG_BASE& msg, ParamBase& param);
    int usedCoinDungeonCheck(CUser* user, CParty* party);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_GiveUpGame : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_GiveUpGame() {}
    virtual ~DisPatcher_GiveUpGame();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_GetItem : public IPacketDispatcher<MSG_GET_ITEM, stGetItemParam, (ch_state)5>
{
public:
    DisPatcher_GetItem() {}
    virtual ~DisPatcher_GetItem();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_UseStack : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_UseStack() {}
    virtual ~DisPatcher_UseStack();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_MoveMap : public IPacketDispatcher<MSG_MOVE_MAP, stMoveMapParam, (ch_state)0>
{
public:
    DisPatcher_MoveMap() {}
    virtual ~DisPatcher_MoveMap();
    int check_error(CUser* user, MSG_BASE& msg);
    int check_state(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    // ORIG T 0x081c4b7a / 0x081c4d26（process 直呼；定义见 DisPatcherCore.cpp 本批）
    void CheckTrapBitsHackCnt(CUser* user, MSG_MOVE_MAP& info);
    void CheckOneMapPlayTimeHackCnt(CUser* user, MSG_MOVE_MAP& info);
};

class GameResultSet;

class DisPatcher_SetPlayResult : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_SetPlayResult() {}
    virtual ~DisPatcher_SetPlayResult();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    static int SendResult(CParty* party, GameResultSet& result, int step);  // ORIG T 0x081f5db6
};

class DisPatcher_DropItem : public IPacketDispatcher<MSG_DROP_ITEM, stDropItemParam, (ch_state)3>
{
public:
    DisPatcher_DropItem() {}
    virtual ~DisPatcher_DropItem();
    int check_error(CUser* user, MSG_BASE& msg, ParamBase& param);  // ORIG _ZN19DisPatcher_DropItem11check_errorEP5CUserR8MSG_BASER9ParamBase
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_UseEquip : public IPacketDispatcher<MSG_DECREASE_DURABILITY, stOnUseEquipParam, (ch_state)0>
{
public:
    DisPatcher_UseEquip() {}
    virtual ~DisPatcher_UseEquip();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_MakePVP : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_MakePVP() {}
    virtual ~DisPatcher_MakePVP();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    // wave1: ORIG T 0x081f81ba _ZN18DisPatcher_MakePVP13ExtractPacketER9PacketBufR17MSG_MAKE_PVP_ROOM
    // （本 TU 家族；MSG_MAKE_PVP_ROOM 局部建模于 DisPatcherCore.cpp 声明区）
    bool ExtractPacket(PacketBuf& packet, MSG_MAKE_PVP_ROOM& msg);
};

class DisPatcher_EnterPVP : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_EnterPVP() {}
    virtual ~DisPatcher_EnterPVP();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_SetPVPSeat : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_SetPVPSeat() {}
    virtual ~DisPatcher_SetPVPSeat();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_SetPVPReady : public IPacketDispatcher<MSG_SET_READY_STATE, stOnSetPvPReadyParam, (ch_state)0>
{
public:
    DisPatcher_SetPVPReady() {}
    virtual ~DisPatcher_SetPVPReady();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_SetPVPTeam : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_SetPVPTeam() {}
    virtual ~DisPatcher_SetPVPTeam();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_DiePvP : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_DiePvP() {}
    virtual ~DisPatcher_DiePvP();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_PvPTimeout : public IPacketDispatcher<MSG_PVP_TIMEOUT, stOnPvPTimeOut, (ch_state)0>
{
public:
    DisPatcher_PvPTimeout() {}
    virtual ~DisPatcher_PvPTimeout();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_EndPvPResult : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_EndPvPResult() {}
    virtual ~DisPatcher_EndPvPResult();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_ResPvpRank : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_ResPvpRank() {}
    virtual ~DisPatcher_ResPvpRank();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    // wave1: ORIG T 0x081f039e ExtractPacket(MSG_RES_PVP_RANK&,PacketBuf&) 与 0x081f05ea SavePvpRank(CUser*,MSG_RES_PVP_RANK&)
    // （本 TU 家族定义；MSG_RES_PVP_RANK 局部建模于 DisPatcherCore.cpp 声明区）
    bool ExtractPacket(MSG_RES_PVP_RANK& msg, PacketBuf& packet);
    bool SavePvpRank(CUser* user, MSG_RES_PVP_RANK& msg);
};

class DisPatcher_SetPVPMapIndex : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_SetPVPMapIndex() {}
    virtual ~DisPatcher_SetPVPMapIndex();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

#pragma pack(push, 1)
// ORIG R17MSG_DEBUG_COMMAND —— DebugCommand 系 GM 命令参数包（原定义位于
// DisPatcher_DebugCommand_cmds.cpp，现提升为权威共享定义；布局口径不变）：
//   +0x00..0x0c    包头原始区（dispatch 不消费）
//   +0x0d byte     命令号（dispatch_sig switch 0..0x30）
//   +0x0e int      命令主参数（int 口径）
//   AddAvatar 口径：+0x12 char 槽位 / +0x13 u8 天数 / +0x14 u16 数量
//   SetEnchant 口径：+0x12 int 槽位 / +0x16 int 强化值
//   RequestMid/UserRepel 口径：+0x12 起为名称缓冲（长度取 +0x0e，≤0x1d）
struct MSG_DEBUG_COMMAND
{
    unsigned char m_head[0xd];       // +0x00..0x0c
    unsigned char m_command;         // +0x0d
    int m_arg;                       // +0x0e（b28c-lane12 曾试 int→unsigned int
                                     //   以复现 setBlackCount 单段直落，TU 级
                                     //   ae +1/diff −1 零回归，但 C28L19 指出
                                     //   DisPatcherCore.cpp:13237/13252 有
                                     //   `m_arg < 1`/`0 < m_arg` 有符号判定，
                                     //   换型会 jl→jb 改他 lane 语义 ⇒ 回退，
                                     //   改由消费点本 TU 私有视图承载）
    union                            // +0x12 起（按命令互斥）
    {
        struct {
            signed char slotByte;            // AddAvatar：+0x12 槽位（sbyte 口径）
            unsigned char periodKind;        // AddAvatar：+0x13 天数（0=取物品表、
                                             //            0x63 视作 0）
            unsigned short count;            // AddAvatar：+0x14 数量
        } avatar;
        struct {
            int slotInt;                     // SetEnchant：+0x12 槽位（int 口径）
            int enchantVal;                  // SetEnchant：+0x16 强化值
        } ench;
        char nameBuf[0x2c];                  // RequestMid/UserRepel：名称缓冲起点
    } payload;                               // 尺寸 0x2c → 总 sizeof = 0x3e
};
#pragma pack(pop)
typedef char MSG_DEBUG_COMMAND_size_check[(sizeof(MSG_DEBUG_COMMAND) == 0x3e) ? 1 : -1];
class DisPatcher_DebugCommand : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_DebugCommand() {}
    virtual ~DisPatcher_DebugCommand();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    char _debugCommandSetLevel(CUser* user, int level);  // ORIG T 0x0858efde _ZN23DisPatcher_DebugCommand21_debugCommandSetLevelEP5CUseri
    // ---- ORIG T _debugCommand* 家族（0x0858e724..0x085908d6；各报告见
    // docs/class_func_reports/DisPatcher_DebugCommand/；定义于
    // DisPatcher_DebugCommand_cmds.cpp）----
    bool _debugCommandOn(CUser* user, MSG_DEBUG_COMMAND& msg);            // 0x0858e724
    bool _debugCommandOff(CUser* user, MSG_DEBUG_COMMAND& msg);           // 0x0858e918
    bool _debugCommandAddItem(CUser* user, unsigned long itemIdxOnlyServer, int count);  // 0x0858ec9c
    bool _debugCommandVisible(CUser* user, MSG_DEBUG_COMMAND& msg);       // 0x085906ac
    bool _debugCommandSetSkill(CUser* user, int skillIdx, int level);     // 0x0858eab8
    bool _debugCommandAddAvatar(CUser* user, MSG_DEBUG_COMMAND& msg);     // 0x0858f7b6
    bool _debugCommandInvisible(CUser* user, MSG_DEBUG_COMMAND& msg);     // 0x0859058a
    bool _debugCommandPowerUpOn(CUser* user, MSG_DEBUG_COMMAND& msg);     // 0x0858ffee
    bool _debugCommandUserRepel(CUser* user, MSG_DEBUG_COMMAND& msg);     // 0x0859048e
    bool _debugCommandClearInven(CUser* user, MSG_DEBUG_COMMAND& msg);    // 0x0859019e
    bool _debugCommandClearSkill(CUser* user, MSG_DEBUG_COMMAND& msg);    // 0x0858e9c4
    bool _debugCommandPVPRankSet(CUser* user, MSG_DEBUG_COMMAND& msg);    // 0x0858faba
    bool _debugCommandRequestMid(CUser* user, MSG_DEBUG_COMMAND& msg);    // 0x08590506
    bool _debugCommandSetEnchant(CUser* user, MSG_DEBUG_COMMAND& msg);    // 0x0858fb4e
    bool _debugCommandLevelUpDown(CUser* user, int levelDelta);           // 0x0858f3ba
    bool _debugCommandPowerWarEnd(CUser* user);                           // 0x0859044e
    bool _debugCommandSetGrowType(CUser* user, int kind, int value);      // 0x0858ee54
    bool _debugCommandGuildLevelUp(CUser* user);                          // 0x0858f64c
    bool _debugCommandCargoCapacity(CUser* user, MSG_DEBUG_COMMAND& msg); // 0x0858fdc8
    bool _debugCommandClearAvtInven(CUser* user, MSG_DEBUG_COMMAND& msg); // 0x08590254
    bool _debugCommandPowerWarStart(CUser* user);                         // 0x0859040e
    bool _debugCommandSetBlackCount(CUser* user, MSG_DEBUG_COMMAND& msg); // 0x0858fcc4
    bool _debugCommandSetUserDefine(CUser* user, PacketBuf& packet);      // 0x085908ba
    bool _debugCommandMasterNewSkill(CUser* user, int skillIdx, int level);  // 0x0858ebcc
    bool _debugCommandPowerwarPointUp(CUser* user, MSG_DEBUG_COMMAND& msg);  // 0x0858fe62
    bool _debugCommandUserPowerwarPointUp(CUser* user, MSG_DEBUG_COMMAND& msg);  // 0x0858fee8
    bool _debugCommandInitUserPowerwarPoint(CUser* user, MSG_DEBUG_COMMAND& msg);  // 0x08590092
};

class DisPatcher_AddFriend : public IPacketDispatcher<MSG_ADD_FRIEND, stAddFriendParam, (ch_state)3>
{
public:
    DisPatcher_AddFriend() {}
    virtual ~DisPatcher_AddFriend();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
    int process_packet_jpn(CUser* user, MSG_BASE& msg, ParamBase& param);  // ORIG T 0x081cb30a（process 直呼的 jpn 变体）
};

class DisPatcher_RemoveFriend : public IPacketDispatcher<MSG_REMOVE_FRIEND, stRemoveFriendParam, (ch_state)3>
{
public:
    DisPatcher_RemoveFriend() {}
    virtual ~DisPatcher_RemoveFriend();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_Cera : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_Cera() {}
    virtual ~DisPatcher_Cera();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_EnterWarRoom : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_EnterWarRoom() {}
    virtual ~DisPatcher_EnterWarRoom();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_SetWarRoomSeatState : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_SetWarRoomSeatState() {}
    virtual ~DisPatcher_SetWarRoomSeatState();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_DieWarRoomCharacter : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_DieWarRoomCharacter() {}
    virtual ~DisPatcher_DieWarRoomCharacter();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_StartWarRoomTemp : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_StartWarRoomTemp() {}
    virtual ~DisPatcher_StartWarRoomTemp();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_ReportClient4Hack : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_ReportClient4Hack() {}
    virtual ~DisPatcher_ReportClient4Hack();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_RequestGuildWarInfo : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_RequestGuildWarInfo() {}
    virtual ~DisPatcher_RequestGuildWarInfo();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_PvpHeartBeat : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_PvpHeartBeat() {}
    virtual ~DisPatcher_PvpHeartBeat();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_MultiBoxLottery : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_MultiBoxLottery() {}
    virtual ~DisPatcher_MultiBoxLottery();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_UdpCharacteristic : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_UdpCharacteristic() {}
    virtual ~DisPatcher_UdpCharacteristic();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    // wave1: ORIG W 三 ping（0x0821b098/0x0821afbc/0x0821b02a）。ORIG 尾
    // `mov $0x0,%eax; leave…ret` ⇒ int 返回尾 0（mangled 不含返回类型，零 ABI 风险）。
    int partyPingStat(CUser* user, unsigned short& a, unsigned short& b);
    int pvpPingStat(CUser* user, unsigned short& a, unsigned short& b);
    int fairPvpPingStat(CUser* user, unsigned short& a, unsigned short& b);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DIspatcher_OneDayLetheTicket : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DIspatcher_OneDayLetheTicket() {}
    virtual ~DIspatcher_OneDayLetheTicket();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcer_polic_one_to_one_chat_disable : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    Dispatcer_polic_one_to_one_chat_disable() {}
    virtual ~Dispatcer_polic_one_to_one_chat_disable();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_CheckSecurityProtection : public IPacketDispatcher<MSG_BASE, CheckSecurityProtection, (ch_state)0>
{
public:
    DisPatcher_CheckSecurityProtection() {}
    virtual ~DisPatcher_CheckSecurityProtection();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_EventDungeon_DestoryObject : public IPacketDispatcher<CMDPacketStruct::_STReqEventDungeonDestoryObject, CMDPacketStruct::_STResEventDungeonDestoryObject, (ch_state)3>
{
public:
    DisPatcher_EventDungeon_DestoryObject() {}
    virtual ~DisPatcher_EventDungeon_DestoryObject();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_EventDungeon_ClearRoom : public IPacketDispatcher<CMDPacketStruct::_STReqEventDungeonClearRoom, CMDPacketStruct::_STResEventDungeonClearRoom, (ch_state)3>
{
public:
    DisPatcher_EventDungeon_ClearRoom() {}
    virtual ~DisPatcher_EventDungeon_ClearRoom();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispather_RegenerationRandomOption : public IPacketDispatcher<MSG_REGENERATION_RANDOM_OPTION, stRegenROIResult, (ch_state)0>
{
public:
    Dispather_RegenerationRandomOption() {}
    virtual ~Dispather_RegenerationRandomOption();
    int check_state(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DispatcherResetRandomOption : public IPacketDispatcher<MSG_RESET_RANDOM_OPTION, ParamBase, (ch_state)0>
{
public:
    DispatcherResetRandomOption() {}
    virtual ~DispatcherResetRandomOption();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

#endif  // DNF_GAME_DISPATCHERCORE_H_
