#ifndef DNF_GAME_DISPATCHERCORE_H_
#define DNF_GAME_DISPATCHERCORE_H_

// df_game_r 还原 —— PacketDispatcher 注册表中缺失声明的 79 个具体分发器类。
// 基类模板实参加载自各自 ORIG 构造函数调用的 IPacketDispatcher<...>::C1 实例化；
// 方法集来自 ORIG nm --defined-only。方法体为带 TODO 标注的 stub（后续批次）。

#include "Dispatchers.h"          // CMDPacketStruct::STBaseResponse 等
#include "PacketTypes.h"


class DisPatcher_CheckConn : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_CheckConn();
    virtual ~DisPatcher_CheckConn();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_Login : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_Login();
    virtual ~DisPatcher_Login();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_SetIP : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_SetIP();
    virtual ~DisPatcher_SetIP();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_Exit : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_Exit();
    virtual ~DisPatcher_Exit();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_SelectCharac : public IPacketDispatcher<MSG_CHARAC_SELECT, stSelectCharac, (ch_state)2>
{
public:
    DisPatcher_SelectCharac();
    virtual ~DisPatcher_SelectCharac();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
    // ---- ORIG helper（T 符号；定义见 DisPatcherCore.cpp，报告 docs/class_func_reports/）----
    void RequestDBInventory_forchina(CUser* user, MSG_CHARAC_SELECT& msg);  // 0x081c7c1c
    int RequestDBCharac(CUser* user, MSG_CHARAC_SELECT const& msg);   // 0x081c6ca0
    int RequestBlackCount(CUser* user, MSG_CHARAC_SELECT& msg);       // 0x081c79f2
    void RequestDBInventory(CUser* user, MSG_CHARAC_SELECT& msg);     // 0x081c6ee6（ORIG 返回 void）
    int RequestDBQuest(CUser* user, MSG_CHARAC_SELECT& msg);          // 0x081c70c2
    int RequestDBDungeonClear(CUser* user, MSG_CHARAC_SELECT& msg);   // 0x081c7436
    int RequestDBKillMonsterInfo(CUser* user, MSG_CHARAC_SELECT& msg);// 0x081c7836
    int RequestDBETC(CUser* user, MSG_CHARAC_SELECT& msg);            // 0x081c7602
};

class DisPatcher_CreateCharac : public IPacketDispatcher<MSG_CREATE_CHARACTER, stCreateCharac, (ch_state)2>
{
public:
    DisPatcher_CreateCharac();
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
    DisPatcher_DeleteCharac();
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
    DisPatcher_ReturnToSelectCharacter();
    virtual ~DisPatcher_ReturnToSelectCharacter();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_GetUserInfo : public IPacketDispatcher<MSG_USERINFO_REQ, stOnGetUserInfoParam, (ch_state)1>
{
public:
    DisPatcher_GetUserInfo();
    virtual ~DisPatcher_GetUserInfo();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_RecoverStamina : public IPacketDispatcher<MSG_USERINFO_REQ, stOnRecoverStaminaParam, (ch_state)3>
{
public:
    DisPatcher_RecoverStamina();
    virtual ~DisPatcher_RecoverStamina();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_ReqPeer : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_ReqPeer();
    virtual ~DisPatcher_ReqPeer();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_ResPeer : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_ResPeer();
    virtual ~DisPatcher_ResPeer();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_SetPartyInfo : public IPacketDispatcher<MSG_PARTY_SET_INFO, stSetPartyInfo, (ch_state)3>
{
public:
    DisPatcher_SetPartyInfo();
    virtual ~DisPatcher_SetPartyInfo();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_LeaveParty : public IPacketDispatcher<MSG_BASE, stLeavePartyParam, (ch_state)0>
{
public:
    DisPatcher_LeaveParty();
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
    DisPatcher_WalkoutParty();
    virtual ~DisPatcher_WalkoutParty();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_StartGame : public IPacketDispatcher<MSG_BASE, stStartGameParam, (ch_state)0>
{
public:
    DisPatcher_StartGame();
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
    DisPatcher_SelectDungeon();
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
    DisPatcher_SendMess();
    virtual ~DisPatcher_SendMess();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_DeleteItem : public IPacketDispatcher<MSG_DELETE_ITEM, stDeleteItemParam, (ch_state)3>
{
public:
    DisPatcher_DeleteItem();
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
    DisPatcher_MoveItem();
    virtual ~DisPatcher_MoveItem();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_SortItem : public IPacketDispatcher<MSG_SORT_ITEM, stOnSortItemParam, (ch_state)3>
{
public:
    DisPatcher_SortItem();
    virtual ~DisPatcher_SortItem();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_BuyItem : public IPacketDispatcher<MSG_BUY_ITEM, BuyItemParam, (ch_state)3>
{
public:
    DisPatcher_BuyItem();
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
    DisPatcher_RentAvatar();
    virtual ~DisPatcher_RentAvatar();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_ExtendAvatar : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_ExtendAvatar();
    virtual ~DisPatcher_ExtendAvatar();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_SellItem : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_SellItem();
    virtual ~DisPatcher_SellItem();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_RepairEquip : public IPacketDispatcher<MSG_REPAIR_EQUIP, stRepairEquipParam, (ch_state)0>
{
public:
    DisPatcher_RepairEquip();
    virtual ~DisPatcher_RepairEquip();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_SetTradeState : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_SetTradeState();
    virtual ~DisPatcher_SetTradeState();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_Compound : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_Compound();
    virtual ~DisPatcher_Compound();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_DisJointItem : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_DisJointItem();
    virtual ~DisPatcher_DisJointItem();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_Lottery : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_Lottery();
    virtual ~DisPatcher_Lottery();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_ChangeSkill : public IPacketDispatcher<MSG_CHANGE_SKILLSLOT, stOnChangeSkillParam, (ch_state)3>
{
public:
    DisPatcher_ChangeSkill();
    virtual ~DisPatcher_ChangeSkill();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_BuySkill : public IPacketDispatcher<MSG_BUY_SKILL, stOnBuySkillParam, (ch_state)3>
{
public:
    DisPatcher_BuySkill();
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
    DisPatcher_IncreStatus();
    virtual ~DisPatcher_IncreStatus();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_QuestAction : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_QuestAction();
    virtual ~DisPatcher_QuestAction();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_SetPos : public IPacketDispatcher<MSG_USER_SET_POS, ParamBase, (ch_state)0>
{
public:
    DisPatcher_SetPos();
    virtual ~DisPatcher_SetPos();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_SetArea : public IPacketDispatcher<MSG_SET_USER_AREA, stSetAreaParam, (ch_state)3>
{
public:
    DisPatcher_SetArea();
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
    DisPatcher_Fish();
    virtual ~DisPatcher_Fish();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_FinishLoad : public IPacketDispatcher<MSG_FINISH_LOADING, stFinishLoadParam, (ch_state)0>
{
public:
    DisPatcher_FinishLoad();
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
    DisPatcher_UseSkill();
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
    DisPatcher_DieMob();
    virtual ~DisPatcher_DieMob();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_DieCharacInDungeon : public IPacketDispatcher<MSG_DIE_CHARACTER, stDieCharacInDungeon, (ch_state)0>
{
public:
    DisPatcher_DieCharacInDungeon();
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
    DisPatcher_UseCoin();
    virtual ~DisPatcher_UseCoin();
    int check_error(CUser* user, MSG_BASE& msg, ParamBase& param);  // ORIG _ZN18DisPatcher_UseCoin11check_errorEP5CUserR8MSG_BASER9ParamBase
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_GiveUpGame : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_GiveUpGame();
    virtual ~DisPatcher_GiveUpGame();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_GetItem : public IPacketDispatcher<MSG_GET_ITEM, stGetItemParam, (ch_state)5>
{
public:
    DisPatcher_GetItem();
    virtual ~DisPatcher_GetItem();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_UseStack : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_UseStack();
    virtual ~DisPatcher_UseStack();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_MoveMap : public IPacketDispatcher<MSG_MOVE_MAP, stMoveMapParam, (ch_state)0>
{
public:
    DisPatcher_MoveMap();
    virtual ~DisPatcher_MoveMap();
    int check_error(CUser* user, MSG_BASE& msg);
    int check_state(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    // ORIG T 0x081c4b7a / 0x081c4d26（process 直呼；定义见 DisPatcherCore.cpp 本批）
    void CheckTrapBitsHackCnt(CUser* user, MSG_MOVE_MAP& info);
    void CheckOneMapPlayTimeHackCnt(CUser* user, MSG_MOVE_MAP& info);
};

class DisPatcher_SetPlayResult : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_SetPlayResult();
    virtual ~DisPatcher_SetPlayResult();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_DropItem : public IPacketDispatcher<MSG_DROP_ITEM, stDropItemParam, (ch_state)3>
{
public:
    DisPatcher_DropItem();
    virtual ~DisPatcher_DropItem();
    int check_error(CUser* user, MSG_BASE& msg, ParamBase& param);  // ORIG _ZN19DisPatcher_DropItem11check_errorEP5CUserR8MSG_BASER9ParamBase
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_UseEquip : public IPacketDispatcher<MSG_DECREASE_DURABILITY, stOnUseEquipParam, (ch_state)0>
{
public:
    DisPatcher_UseEquip();
    virtual ~DisPatcher_UseEquip();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_MakePVP : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_MakePVP();
    virtual ~DisPatcher_MakePVP();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_EnterPVP : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_EnterPVP();
    virtual ~DisPatcher_EnterPVP();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_SetPVPSeat : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_SetPVPSeat();
    virtual ~DisPatcher_SetPVPSeat();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_SetPVPReady : public IPacketDispatcher<MSG_SET_READY_STATE, stOnSetPvPReadyParam, (ch_state)0>
{
public:
    DisPatcher_SetPVPReady();
    virtual ~DisPatcher_SetPVPReady();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_SetPVPTeam : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_SetPVPTeam();
    virtual ~DisPatcher_SetPVPTeam();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_DiePvP : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_DiePvP();
    virtual ~DisPatcher_DiePvP();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_PvPTimeout : public IPacketDispatcher<MSG_PVP_TIMEOUT, stOnPvPTimeOut, (ch_state)0>
{
public:
    DisPatcher_PvPTimeout();
    virtual ~DisPatcher_PvPTimeout();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_EndPvPResult : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_EndPvPResult();
    virtual ~DisPatcher_EndPvPResult();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_ResPvpRank : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_ResPvpRank();
    virtual ~DisPatcher_ResPvpRank();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_SetPVPMapIndex : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_SetPVPMapIndex();
    virtual ~DisPatcher_SetPVPMapIndex();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_DebugCommand : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_DebugCommand();
    virtual ~DisPatcher_DebugCommand();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_AddFriend : public IPacketDispatcher<MSG_ADD_FRIEND, stAddFriendParam, (ch_state)3>
{
public:
    DisPatcher_AddFriend();
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
    DisPatcher_RemoveFriend();
    virtual ~DisPatcher_RemoveFriend();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_Cera : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_Cera();
    virtual ~DisPatcher_Cera();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_EnterWarRoom : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_EnterWarRoom();
    virtual ~DisPatcher_EnterWarRoom();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_SetWarRoomSeatState : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_SetWarRoomSeatState();
    virtual ~DisPatcher_SetWarRoomSeatState();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_DieWarRoomCharacter : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_DieWarRoomCharacter();
    virtual ~DisPatcher_DieWarRoomCharacter();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_StartWarRoomTemp : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_StartWarRoomTemp();
    virtual ~DisPatcher_StartWarRoomTemp();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_ReportClient4Hack : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_ReportClient4Hack();
    virtual ~DisPatcher_ReportClient4Hack();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_RequestGuildWarInfo : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_RequestGuildWarInfo();
    virtual ~DisPatcher_RequestGuildWarInfo();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_PvpHeartBeat : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_PvpHeartBeat();
    virtual ~DisPatcher_PvpHeartBeat();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_MultiBoxLottery : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_MultiBoxLottery();
    virtual ~DisPatcher_MultiBoxLottery();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_UdpCharacteristic : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DisPatcher_UdpCharacteristic();
    virtual ~DisPatcher_UdpCharacteristic();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DIspatcher_OneDayLetheTicket : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    DIspatcher_OneDayLetheTicket();
    virtual ~DIspatcher_OneDayLetheTicket();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcer_polic_one_to_one_chat_disable : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)0>
{
public:
    Dispatcer_polic_one_to_one_chat_disable();
    virtual ~Dispatcer_polic_one_to_one_chat_disable();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class DisPatcher_CheckSecurityProtection : public IPacketDispatcher<MSG_BASE, CheckSecurityProtection, (ch_state)0>
{
public:
    DisPatcher_CheckSecurityProtection();
    virtual ~DisPatcher_CheckSecurityProtection();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_EventDungeon_DestoryObject : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)3>
{
public:
    DisPatcher_EventDungeon_DestoryObject();
    virtual ~DisPatcher_EventDungeon_DestoryObject();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DisPatcher_EventDungeon_ClearRoom : public IPacketDispatcher<MSG_BASE, ParamBase, (ch_state)3>
{
public:
    DisPatcher_EventDungeon_ClearRoom();
    virtual ~DisPatcher_EventDungeon_ClearRoom();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispather_RegenerationRandomOption : public IPacketDispatcher<MSG_REGENERATION_RANDOM_OPTION, stRegenROIResult, (ch_state)0>
{
public:
    Dispather_RegenerationRandomOption();
    virtual ~Dispather_RegenerationRandomOption();
    int check_state(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class DispatcherResetRandomOption : public IPacketDispatcher<MSG_RESET_RANDOM_OPTION, ParamBase, (ch_state)0>
{
public:
    DispatcherResetRandomOption();
    virtual ~DispatcherResetRandomOption();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

#endif  // DNF_GAME_DISPATCHERCORE_H_
