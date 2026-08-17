#ifndef DNF_GAME_DISPATCHERS_H_
#define DNF_GAME_DISPATCHERS_H_

#include "PacketDispatcher.h"
#include "PacketTypes.h"

namespace CMDPacketStruct
{
struct STBaseResponse {};
}

namespace advancealtar {
class Dispatcher_Pause : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_Pause();
    virtual ~Dispatcher_Pause();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send_packet(CUser* user, CMDPacketStruct::STBaseResponse& resp);
};

class Dispatcher_BuyItem : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_BuyItem();
    virtual ~Dispatcher_BuyItem();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send_packet(CUser* user, CMDPacketStruct::STBaseResponse& resp);
};

class Dispatcher_SetSlot : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_SetSlot();
    virtual ~Dispatcher_SetSlot();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send_packet(CUser* user, CMDPacketStruct::STBaseResponse& resp);
};

class Dispatcher_ResetStar : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_ResetStar();
    virtual ~Dispatcher_ResetStar();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send_packet(CUser* user, CMDPacketStruct::STBaseResponse& resp);
};

class Dispatcher_StartGame : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_StartGame();
    virtual ~Dispatcher_StartGame();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_SummonUnit : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_SummonUnit();
    virtual ~Dispatcher_SummonUnit();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send_packet(CUser* user, CMDPacketStruct::STBaseResponse& resp);
};

class Dispatcher_UpgradeGage : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_UpgradeGage();
    virtual ~Dispatcher_UpgradeGage();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send_packet(CUser* user, CMDPacketStruct::STBaseResponse& resp);
};

class Dispatcher_ExchangeSlot : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_ExchangeSlot();
    virtual ~Dispatcher_ExchangeSlot();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send_packet(CUser* user, CMDPacketStruct::STBaseResponse& resp);
};

class Dispatcher_GetAchievementReward : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_GetAchievementReward();
    virtual ~Dispatcher_GetAchievementReward();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send_packet(CUser* user, CMDPacketStruct::STBaseResponse& resp);
};

}
class Dispatcher_Redeem : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_Redeem();
    virtual ~Dispatcher_Redeem();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_UseDye : public IPacketDispatcher<MSG_USE_DYE, stUseDye, ch_state0>
{
public:
    Dispatcher_UseDye();
    virtual ~Dispatcher_UseDye();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Antibot : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_Antibot();
    virtual ~Dispatcher_Antibot();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_ChangeHp : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_ChangeHp();
    virtual ~Dispatcher_ChangeHp();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Teleport : public IPacketDispatcher<MSG_TELEPORT, stTeleportParam, ch_state3>
{
public:
    Dispatcher_Teleport();
    virtual ~Dispatcher_Teleport();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_UseJewel : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_UseJewel();
    virtual ~Dispatcher_UseJewel();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_BingoQuiz : public IPacketDispatcher<MSG_BINGO_QUIZ, stBingoQuiz, ch_state3>
{
public:
    Dispatcher_BingoQuiz();
    virtual ~Dispatcher_BingoQuiz();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_DieMobAll : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_DieMobAll();
    virtual ~Dispatcher_DieMobAll();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_SkillInit : public IPacketDispatcher<MSG_SKILL_INIT, stSkillInit, ch_state0>
{
public:
    Dispatcher_SkillInit();
    virtual ~Dispatcher_SkillInit();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int process_skill_init();
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_Antibot_DP : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_Antibot_DP();
    virtual ~Dispatcher_Antibot_DP();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_AskRematch : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_AskRematch();
    virtual ~Dispatcher_AskRematch();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_BVHackInfo : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_BVHackInfo();
    virtual ~Dispatcher_BVHackInfo();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_BlueMarble : public IPacketDispatcher<MSG_BLUE_MARBLE, stBlueMarble, ch_state3>
{
public:
    Dispatcher_BlueMarble();
    virtual ~Dispatcher_BlueMarble();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_ChangeHost : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_ChangeHost();
    virtual ~Dispatcher_ChangeHost();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_ComboSkill : public IPacketDispatcher<MSG_COMBO_SKILL, stComboSkill, ch_state3>
{
public:
    Dispatcher_ComboSkill();
    virtual ~Dispatcher_ComboSkill();
    int checkComboSkillIndex();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_GM_Command : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_GM_Command();
    virtual ~Dispatcher_GM_Command();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_GuildCargo : public IPacketDispatcher<MSG_BASE, stGuildCargoParam, ch_state0>
{
public:
    Dispatcher_GuildCargo();
    virtual ~Dispatcher_GuildCargo();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_Join_Power : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_Join_Power();
    virtual ~Dispatcher_Join_Power();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_MotionHack : public IPacketDispatcher<MSG_MOTION_HACK, ParamBase, ch_state0>
{
public:
    Dispatcher_MotionHack();
    virtual ~Dispatcher_MotionHack();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_MoveToGate : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_MoveToGate();
    virtual ~Dispatcher_MoveToGate();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_PurifyItem : public IPacketDispatcher<MSG_PURIFY_ITEM, ParamBase, ch_state0>
{
public:
    Dispatcher_PurifyItem();
    virtual ~Dispatcher_PurifyItem();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int processPurifyType();
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_RedeemList : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_RedeemList();
    virtual ~Dispatcher_RedeemList();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_SelectItem : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_SelectItem();
    virtual ~Dispatcher_SelectItem();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_TraceError : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_TraceError();
    virtual ~Dispatcher_TraceError();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_TryEnchant : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_TryEnchant();
    virtual ~Dispatcher_TryEnchant();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_VerifyGold : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_VerifyGold();
    virtual ~Dispatcher_VerifyGold();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_BingoReward : public IPacketDispatcher<MSG_BASE, stBingoReward, ch_state3>
{
public:
    Dispatcher_BingoReward();
    virtual ~Dispatcher_BingoReward();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_CranePickUp : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_CranePickUp();
    virtual ~Dispatcher_CranePickUp();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_EPLPCommand : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_EPLPCommand();
    virtual ~Dispatcher_EPLPCommand();
    int _BroadCastPacket();
    int _SendEPLPError();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int excludeEplpState();
    int isReTurnToVillage();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_ModItemAttr : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_ModItemAttr();
    virtual ~Dispatcher_ModItemAttr();
    int _SendResult();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_RefundSkill : public IPacketDispatcher<MSG_REFUND_SKILL, stRefundSkill, ch_state0>
{
public:
    Dispatcher_RefundSkill();
    virtual ~Dispatcher_RefundSkill();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_SecurityLog : public IPacketDispatcher<MSG_SECURITY_LOG_PACKET, stSecurityLogParam, ch_state0>
{
public:
    Dispatcher_SecurityLog();
    virtual ~Dispatcher_SecurityLog();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_Select_Card : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_Select_Card();
    virtual ~Dispatcher_Select_Card();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_UpgradeItem : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_UpgradeItem();
    virtual ~Dispatcher_UpgradeItem();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_AgreeEnchant : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_AgreeEnchant();
    virtual ~Dispatcher_AgreeEnchant();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Back2Village : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_Back2Village();
    virtual ~Dispatcher_Back2Village();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_BossDieCheck : public IPacketDispatcher<MSG_BOSS_DIE_CHECK, stBossDieCheckParam, ch_state5>
{
public:
    Dispatcher_BossDieCheck();
    virtual ~Dispatcher_BossDieCheck();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_CodeCheckSum : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_CodeCheckSum();
    virtual ~Dispatcher_CodeCheckSum();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_CollectItems : public IPacketDispatcher<MSG_COLLECT_ITEMS, stCollectItems, ch_state0>
{
public:
    Dispatcher_CollectItems();
    virtual ~Dispatcher_CollectItems();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_DepositMoney : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_DepositMoney();
    virtual ~Dispatcher_DepositMoney();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_EvadeAssault : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state3>
{
public:
    Dispatcher_EvadeAssault();
    virtual ~Dispatcher_EvadeAssault();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_FairPvPScore : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_FairPvPScore();
    virtual ~Dispatcher_FairPvPScore();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_HumanCertify : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_HumanCertify();
    virtual ~Dispatcher_HumanCertify();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_InformNotice : public IPacketDispatcher<MSG_INFORM_NOTICE, InformNoticeData, ch_state3>
{
public:
    Dispatcher_InformNotice();
    virtual ~Dispatcher_InformNotice();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_JoinPowerWar : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_JoinPowerWar();
    virtual ~Dispatcher_JoinPowerWar();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_MailBox_Open : public IPacketDispatcher<MSG_BASE, stMailboxOpenParam, ch_state0>
{
public:
    Dispatcher_MailBox_Open();
    virtual ~Dispatcher_MailBox_Open();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_MailBox_Send : public IPacketDispatcher<MSG_MAILBOX_SEND, stMailboxSendParam, ch_state0>
{
public:
    Dispatcher_MailBox_Send();
    virtual ~Dispatcher_MailBox_Send();
    int GuildMailCheck();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_MemberSecede : public IPacketDispatcher<MSG_MEMBER_SECEDE, stMemberSecedeParam, ch_state3>
{
public:
    Dispatcher_MemberSecede();
    virtual ~Dispatcher_MemberSecede();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_RequestMatch : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_RequestMatch();
    virtual ~Dispatcher_RequestMatch();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_ScanBotByDll : public IPacketDispatcher<MSG_SCAN_DLL, ScanBotByDllParam, ch_state0>
{
public:
    Dispatcher_ScanBotByDll();
    virtual ~Dispatcher_ScanBotByDll();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_ScanBotByDrv : public IPacketDispatcher<MSG_SCAN_DRV, ScanBotByDrvParam, ch_state0>
{
public:
    Dispatcher_ScanBotByDrv();
    virtual ~Dispatcher_ScanBotByDrv();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Secede_Power : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_Secede_Power();
    virtual ~Dispatcher_Secede_Power();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_SoloTeleport : public IPacketDispatcher<MSG_SOLO_TELEPORT, stSoloTeleportParam, ch_state0>
{
public:
    Dispatcher_SoloTeleport();
    virtual ~Dispatcher_SoloTeleport();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_TitleBookGet : public IPacketDispatcher<MSG_TITLEBOOK_GET, ParamBase, ch_state3>
{
public:
    Dispatcher_TitleBookGet();
    virtual ~Dispatcher_TitleBookGet();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_TitleBookPut : public IPacketDispatcher<MSG_TITLEBOOK_PUT, ParamBase, ch_state3>
{
public:
    Dispatcher_TitleBookPut();
    virtual ~Dispatcher_TitleBookPut();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_UpgradeCargo : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_UpgradeCargo();
    virtual ~Dispatcher_UpgradeCargo();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_UseLimitCube : public IPacketDispatcher<MSG_USE_LIMIT_CUBE, stUseLimitCube, ch_state3>
{
public:
    Dispatcher_UseLimitCube();
    virtual ~Dispatcher_UseLimitCube();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_AssertManager : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_AssertManager();
    virtual ~Dispatcher_AssertManager();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_CallGuildInfo : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_CallGuildInfo();
    virtual ~Dispatcher_CallGuildInfo();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_ChangeEmotion : public IPacketDispatcher<MSG_CHANGE_EMOTION, stChangeEmotionParam, ch_state3>
{
public:
    Dispatcher_ChangeEmotion();
    virtual ~Dispatcher_ChangeEmotion();
    int check_error(CUser* user, MSG_BASE& msg);
    int isVaildEmotionValue();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_Clear_Used_QP : public IPacketDispatcher<MSG_BASE, STQuestShopParam, ch_state0>
{
public:
    Dispatcher_Clear_Used_QP();
    virtual ~Dispatcher_Clear_Used_QP();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_ConnectP2PPvP : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_ConnectP2PPvP();
    virtual ~Dispatcher_ConnectP2PPvP();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_DenyJoinGuild : public IPacketDispatcher<MSG_DENY_JOIN_GUILD, ParamBase, ch_state0>
{
public:
    Dispatcher_DenyJoinGuild();
    virtual ~Dispatcher_DenyJoinGuild();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_EnchantByBead : public IPacketDispatcher<MSG_ENCHANT_BY_BEAD, stEnchantByBeadParam, ch_state3>
{
public:
    Dispatcher_EnchantByBead();
    virtual ~Dispatcher_EnchantByBead();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_GenCeraTicket : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_GenCeraTicket();
    virtual ~Dispatcher_GenCeraTicket();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_GiveGiftToNPC : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_GiveGiftToNPC();
    virtual ~Dispatcher_GiveGiftToNPC();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_GuildJoinList : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_GuildJoinList();
    virtual ~Dispatcher_GuildJoinList();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_HatchCreature : public IPacketDispatcher<MSG_HATCH_CREATURE, stHatchCreature, ch_state3>
{
public:
    Dispatcher_HatchCreature();
    virtual ~Dispatcher_HatchCreature();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_JoinGuildInfo : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_JoinGuildInfo();
    virtual ~Dispatcher_JoinGuildInfo();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_MouseRegister : public IPacketDispatcher<MSG_TEMP_STRUCT, TempParam, ch_state2>
{
public:
    Dispatcher_MouseRegister();
    virtual ~Dispatcher_MouseRegister();
    int checkValiedPassword();
    int check_error(CUser* user, MSG_BASE& msg);
    int decryptPassword();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int doPassPadReplay();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_ObjectBringUp : public IPacketDispatcher<MSG_OBJECT_BRING_UP, stObjectBringUp, ch_state3>
{
public:
    Dispatcher_ObjectBringUp();
    virtual ~Dispatcher_ObjectBringUp();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_Overflow_Info : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_Overflow_Info();
    virtual ~Dispatcher_Overflow_Info();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_PartyTeleport : public IPacketDispatcher<MSG_PARTY_TELEPORT, stPartyTeleport, ch_state0>
{
public:
    Dispatcher_PartyTeleport();
    virtual ~Dispatcher_PartyTeleport();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_QuickJoinRoom : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_QuickJoinRoom();
    virtual ~Dispatcher_QuickJoinRoom();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_SelectStriker : public IPacketDispatcher<MSG_SELECT_STRIKER, stSelectStriker, ch_state3>
{
public:
    Dispatcher_SelectStriker();
    virtual ~Dispatcher_SelectStriker();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_UseCraneStart : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_UseCraneStart();
    virtual ~Dispatcher_UseCraneStart();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_UseRenameCard : public IPacketDispatcher<MSG_USE_RENAME_CARD, stUseRenameCard, ch_state0>
{
public:
    Dispatcher_UseRenameCard();
    virtual ~Dispatcher_UseRenameCard();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_WithdrawMoney : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_WithdrawMoney();
    virtual ~Dispatcher_WithdrawMoney();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_AuctionBidding : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_AuctionBidding();
    virtual ~Dispatcher_AuctionBidding();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_BuyAutomatItem : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_BuyAutomatItem();
    virtual ~Dispatcher_BuyAutomatItem();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_CallMemberList : public IPacketDispatcher<MSG_BASE, stCallMemberListParam, ch_state3>
{
public:
    Dispatcher_CallMemberList();
    virtual ~Dispatcher_CallMemberList();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_CheckJoinGuild : public IPacketDispatcher<MSG_CHECK_GUILD_JOIN, ParamBase, ch_state0>
{
public:
    Dispatcher_CheckJoinGuild();
    virtual ~Dispatcher_CheckJoinGuild();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_CompoundEmblem : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_CompoundEmblem();
    virtual ~Dispatcher_CompoundEmblem();
    int SendCreateEmblemInfo();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_DisJointAvatar : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_DisJointAvatar();
    virtual ~Dispatcher_DisJointAvatar();
    int SendCreateEmblemInfo();
    int SendDisjointAvatarInfo();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_DisguiseCancel : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_DisguiseCancel();
    virtual ~Dispatcher_DisguiseCancel();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_HellPartyStart : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_HellPartyStart();
    virtual ~Dispatcher_HellPartyStart();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_ItemDictionary : public IPacketDispatcher<MSG_ITEM_DICTIONARY, stItemDictionary, ch_state3>
{
public:
    Dispatcher_ItemDictionary();
    virtual ~Dispatcher_ItemDictionary();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_Lag_Statistics : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_Lag_Statistics();
    virtual ~Dispatcher_Lag_Statistics();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Mercenary_Info : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state2>
{
public:
    Dispatcher_Mercenary_Info();
    virtual ~Dispatcher_Mercenary_Info();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_P2P_Statistics : public IPacketDispatcher<MSG_P2P_STATISTICS, ParamBase, ch_state0>
{
public:
    Dispatcher_P2P_Statistics();
    virtual ~Dispatcher_P2P_Statistics();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_PcRoomRentItem : public IPacketDispatcher<MSG_RENT_ITEM, stRentItemParam, ch_state0>
{
public:
    Dispatcher_PcRoomRentItem();
    virtual ~Dispatcher_PcRoomRentItem();
    int check_error(CUser* user, MSG_BASE& msg);
    int giveDimensionItems();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_PvPChannelInfo : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_PvPChannelInfo();
    virtual ~Dispatcher_PvPChannelInfo();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_RemovePvPBuddy : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_RemovePvPBuddy();
    virtual ~Dispatcher_RemovePvPBuddy();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_RenameCreature : public IPacketDispatcher<MSG_RENAME_CREATURE, stRenameCreature, ch_state0>
{
public:
    Dispatcher_RenameCreature();
    virtual ~Dispatcher_RenameCreature();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_UseBoosterItem : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_UseBoosterItem();
    virtual ~Dispatcher_UseBoosterItem();
    int _onBoosterItemUseResult();
    int _onBoosterItemUseResult_OnlyOne();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    int sendBroadCastItems();
};

class Dispatcher_UserHistoryLog : public IPacketDispatcher<MSG_USER_HISTORY_LOG, ParamBase, ch_state0>
{
public:
    Dispatcher_UserHistoryLog();
    virtual ~Dispatcher_UserHistoryLog();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_BuyCeraShopItem : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_BuyCeraShopItem();
    virtual ~Dispatcher_BuyCeraShopItem();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    int useCountDownCoinInFreeCoinDungeon();
};

class Dispatcher_CallGuildInvite : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_CallGuildInvite();
    virtual ~Dispatcher_CallGuildInvite();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    int send_packet_guild_invite();
};

class Dispatcher_CancelJoinGuild : public IPacketDispatcher<MSG_CANCEL_JOIN_GUILD, ParamBase, ch_state0>
{
public:
    Dispatcher_CancelJoinGuild();
    virtual ~Dispatcher_CancelJoinGuild();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_CeraPackageOpen : public IPacketDispatcher<MSG_OPEN_CERAPACKAGE, stOpenCeraPackage, ch_state0>
{
public:
    Dispatcher_CeraPackageOpen();
    virtual ~Dispatcher_CeraPackageOpen();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_ChangeGuildName : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_ChangeGuildName();
    virtual ~Dispatcher_ChangeGuildName();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_CompleteDisplay : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_CompleteDisplay();
    virtual ~Dispatcher_CompleteDisplay();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_CompleteLoadPvP : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_CompleteLoadPvP();
    virtual ~Dispatcher_CompleteLoadPvP();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Compound_Avatar : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_Compound_Avatar();
    virtual ~Dispatcher_Compound_Avatar();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_DieBloodMonster : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_DieBloodMonster();
    virtual ~Dispatcher_DieBloodMonster();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_DisguiseRequest : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_DisguiseRequest();
    virtual ~Dispatcher_DisguiseRequest();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_DonateGuildFund : public IPacketDispatcher<MSG_DONATE_GUILD_FUND, ParamBase, ch_state3>
{
public:
    Dispatcher_DonateGuildFund();
    virtual ~Dispatcher_DonateGuildFund();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_GiveupExpertJob : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_GiveupExpertJob();
    virtual ~Dispatcher_GiveupExpertJob();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_GoblinPadStatus : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_GoblinPadStatus();
    virtual ~Dispatcher_GoblinPadStatus();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_LimitNpcBuyItem : public IPacketDispatcher<MsgLimitNpcBuyItem, LimitNpcBuyItem, ch_state3>
{
public:
    Dispatcher_LimitNpcBuyItem();
    virtual ~Dispatcher_LimitNpcBuyItem();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_MailBox_Extract : public IPacketDispatcher<MSG_MAILBOX_EXTRACT_ITEM, stMailboxExtractParam, ch_state0>
{
public:
    Dispatcher_MailBox_Extract();
    virtual ~Dispatcher_MailBox_Extract();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_MonstercardBind : public IPacketDispatcher<MSG_MONSTERCARD_BIND, stMonstercardBind, ch_state3>
{
public:
    Dispatcher_MonstercardBind();
    virtual ~Dispatcher_MonstercardBind();
    int _bind_process();
    int calcurate_rarity();
    int check_error(CUser* user, MSG_BASE& msg);
    int check_need_slot();
    int get_bind_result();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_PVPRequestFight : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_PVPRequestFight();
    virtual ~Dispatcher_PVPRequestFight();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_QueryCharacInfo : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_QueryCharacInfo();
    virtual ~Dispatcher_QueryCharacInfo();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_ReportAbuseUser : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_ReportAbuseUser();
    virtual ~Dispatcher_ReportAbuseUser();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_RequestItemLock : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_RequestItemLock();
    virtual ~Dispatcher_RequestItemLock();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_SaveQuestNotify : public IPacketDispatcher<MSG_SAVE_QUEST_NOTIFY, saveQuestNotifyParam, ch_state3>
{
public:
    Dispatcher_SaveQuestNotify();
    virtual ~Dispatcher_SaveQuestNotify();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_SecuDataControl : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_SecuDataControl();
    virtual ~Dispatcher_SecuDataControl();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Security_IPScan : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_Security_IPScan();
    virtual ~Dispatcher_Security_IPScan();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Shop_Coin_Event : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_Shop_Coin_Event();
    virtual ~Dispatcher_Shop_Coin_Event();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_TutorialLevelUp : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_TutorialLevelUp();
    virtual ~Dispatcher_TutorialLevelUp();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_ApproveJoinGuild : public IPacketDispatcher<MSG_APPROVE_JOIN_GUILD, ParamBase, ch_state0>
{
public:
    Dispatcher_ApproveJoinGuild();
    virtual ~Dispatcher_ApproveJoinGuild();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_CallGuildLevelUp : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_CallGuildLevelUp();
    virtual ~Dispatcher_CallGuildLevelUp();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    int send_packet_guild_levelup();
};

class Dispatcher_CallGuildMembers : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_CallGuildMembers();
    virtual ~Dispatcher_CallGuildMembers();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_ChangeCharacName : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_ChangeCharacName();
    virtual ~Dispatcher_ChangeCharacName();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_ChangeLetterStat : public IPacketDispatcher<MSG_CHANGE_LETTER_STAT, stChangeLetterStatParam, ch_state3>
{
public:
    Dispatcher_ChangeLetterStat();
    virtual ~Dispatcher_ChangeLetterStat();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_DieAssaultPlayer : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_DieAssaultPlayer();
    virtual ~Dispatcher_DieAssaultPlayer();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Equipslot_Switch : public IPacketDispatcher<MSG_EQUIPSLOT_SWITCH, stEquipslotSwitch, ch_state3>
{
public:
    Dispatcher_Equipslot_Switch();
    virtual ~Dispatcher_Equipslot_Switch();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_ExitPrivateStore : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_ExitPrivateStore();
    virtual ~Dispatcher_ExitPrivateStore();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_ExpertExtraction : public IPacketDispatcher<MSG_EXPERT_EXTRACTION, stExpertExtraction, ch_state3>
{
public:
    Dispatcher_ExpertExtraction();
    virtual ~Dispatcher_ExpertExtraction();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_InformNotice_2nd : public IPacketDispatcher<MSG_INFORM_NOTICE_2ND, InformNoticeData2nd, ch_state3>
{
public:
    Dispatcher_InformNotice_2nd();
    virtual ~Dispatcher_InformNotice_2nd();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_MemberEnterReply : public IPacketDispatcher<MSG_MEMBER_ENTER_REPLY, stMemberEnterReplyParam, ch_state3>
{
public:
    Dispatcher_MemberEnterReply();
    virtual ~Dispatcher_MemberEnterReply();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Mercenary_Return : public IPacketDispatcher<MsgMercenaryReturn, SendMercenaryReturnResult, ch_state0>
{
public:
    Dispatcher_Mercenary_Return();
    virtual ~Dispatcher_Mercenary_Return();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_MultiMailBoxSend : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_MultiMailBoxSend();
    virtual ~Dispatcher_MultiMailBoxSend();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Open_Guild_Board : public IPacketDispatcher<MSG_GUILD_BOARD, stGuildBoardParam, ch_state3>
{
public:
    Dispatcher_Open_Guild_Board();
    virtual ~Dispatcher_Open_Guild_Board();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_PvPBuddyConnList : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_PvPBuddyConnList();
    virtual ~Dispatcher_PvPBuddyConnList();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_RefreshGuildInfo : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state3>
{
public:
    Dispatcher_RefreshGuildInfo();
    virtual ~Dispatcher_RefreshGuildInfo();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_ReplyGuildInvite : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_ReplyGuildInvite();
    virtual ~Dispatcher_ReplyGuildInvite();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_ReportBadP2PUser : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_ReportBadP2PUser();
    virtual ~Dispatcher_ReportBadP2PUser();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Req_Urgent_Quest : public IPacketDispatcher<MSG_BASE, ReqResult, ch_state3>
{
public:
    Dispatcher_Req_Urgent_Quest();
    virtual ~Dispatcher_Req_Urgent_Quest();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_RequestBlackList : public IPacketDispatcher<MSG_BASE, stRequestBlackListParam, ch_state3>
{
public:
    Dispatcher_RequestBlackList();
    virtual ~Dispatcher_RequestBlackList();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_RequestJoinGuild : public IPacketDispatcher<MSG_REQUEST_GUILD_JOIN, ParamBase, ch_state0>
{
public:
    Dispatcher_RequestJoinGuild();
    virtual ~Dispatcher_RequestJoinGuild();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_ResponseCreature : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_ResponseCreature();
    virtual ~Dispatcher_ResponseCreature();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_SDC_Damage_Check : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_SDC_Damage_Check();
    virtual ~Dispatcher_SDC_Damage_Check();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_SaveGameOption_1 : public IPacketDispatcher<MSG_SAVE_GAME_OPTION_1, ParamBase, ch_state2>
{
public:
    Dispatcher_SaveGameOption_1();
    virtual ~Dispatcher_SaveGameOption_1();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_SaveGameOption_2 : public IPacketDispatcher<MSG_SAVE_GAME_OPTION_2, ParamBase, ch_state2>
{
public:
    Dispatcher_SaveGameOption_2();
    virtual ~Dispatcher_SaveGameOption_2();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_UpgradeChronicle : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_UpgradeChronicle();
    virtual ~Dispatcher_UpgradeChronicle();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_UpgradeInventory : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_UpgradeInventory();
    virtual ~Dispatcher_UpgradeInventory();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_AddSocketToAvatar : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_AddSocketToAvatar();
    virtual ~Dispatcher_AddSocketToAvatar();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_AuctionRegistItem : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_AuctionRegistItem();
    virtual ~Dispatcher_AuctionRegistItem();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_ChangeHostWarRoom : public IPacketDispatcher<MSG_CHANGE_HOST_WARROOM, stChangeHostWarRoom, ch_state0>
{
public:
    Dispatcher_ChangeHostWarRoom();
    virtual ~Dispatcher_ChangeHostWarRoom();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_ConnectP2PAssault : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_ConnectP2PAssault();
    virtual ~Dispatcher_ConnectP2PAssault();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Debug_Clear_Quest : public IPacketDispatcher<MsgClearQuestIndex, ReqDebugClearQuestResult, ch_state0>
{
public:
    Dispatcher_Debug_Clear_Quest();
    virtual ~Dispatcher_Debug_Clear_Quest();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_DeleteToBlackList : public IPacketDispatcher<MSG_DELETE_TO_BLACKLIST, stDeleteToBlackListParam, ch_state3>
{
public:
    Dispatcher_DeleteToBlackList();
    virtual ~Dispatcher_DeleteToBlackList();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_EnterPrivateStore : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_EnterPrivateStore();
    virtual ~Dispatcher_EnterPrivateStore();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_FatigueAttendance : public IPacketDispatcher<MSG_FATIGUE_ATTENDANCE, stFatigueAttendance, ch_state0>
{
public:
    Dispatcher_FatigueAttendance();
    virtual ~Dispatcher_FatigueAttendance();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_GuildCargoHistory : public IPacketDispatcher<MSG_BASE, stGuildCargoHistoryParam, ch_state0>
{
public:
    Dispatcher_GuildCargoHistory();
    virtual ~Dispatcher_GuildCargoHistory();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_GuildCargoPopItem : public IPacketDispatcher<MSG_GUILD_CARGO_POP_ITEM, stGuildCargoPopItemParam, ch_state0>
{
public:
    Dispatcher_GuildCargoPopItem();
    virtual ~Dispatcher_GuildCargoPopItem();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_LoadingTimeReport : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_LoadingTimeReport();
    virtual ~Dispatcher_LoadingTimeReport();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_PeerConnectResult : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_PeerConnectResult();
    virtual ~Dispatcher_PeerConnectResult();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_PutItemForEnchant : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_PutItemForEnchant();
    virtual ~Dispatcher_PutItemForEnchant();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_RequestItemUnlock : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_RequestItemUnlock();
    virtual ~Dispatcher_RequestItemUnlock();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_SecretShopBuyItem : public IPacketDispatcher<MSG_SECRET_SHOP_BUY_ITEM, stSecretShopBuyItemParam, ch_state0>
{
public:
    Dispatcher_SecretShopBuyItem();
    virtual ~Dispatcher_SecretShopBuyItem();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_SecurityCardIssue : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_SecurityCardIssue();
    virtual ~Dispatcher_SecurityCardIssue();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_SetSubGuildMaster : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_SetSubGuildMaster();
    virtual ~Dispatcher_SetSubGuildMaster();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_UpgradeGuildCargo : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_UpgradeGuildCargo();
    virtual ~Dispatcher_UpgradeGuildCargo();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_UseVendingMachine : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_UseVendingMachine();
    virtual ~Dispatcher_UseVendingMachine();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_AchievementTrigger : public IPacketDispatcher<MSG_ACHIEVEMENT_TRIGGER, ParamBase, ch_state3>
{
public:
    Dispatcher_AchievementTrigger();
    virtual ~Dispatcher_AchievementTrigger();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Antibot_DPCallBack : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_Antibot_DPCallBack();
    virtual ~Dispatcher_Antibot_DPCallBack();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_AvatarOptionChange : public IPacketDispatcher<MSG_CHANGE_AVATAR_OPTION, stChangeAvatarOption, ch_state3>
{
public:
    Dispatcher_AvatarOptionChange();
    virtual ~Dispatcher_AvatarOptionChange();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_BiddingRoutingItem : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_BiddingRoutingItem();
    virtual ~Dispatcher_BiddingRoutingItem();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Cancel_Quick_Party : public IPacketDispatcher<MSG_BASE, stQuickPartyParam, ch_state5>
{
public:
    Dispatcher_Cancel_Quick_Party();
    virtual ~Dispatcher_Cancel_Quick_Party();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_ChangeRandomOption : public IPacketDispatcher<MSG_CHANGE_RANDOM_OPTION, ParamBase, ch_state0>
{
public:
    Dispatcher_ChangeRandomOption();
    virtual ~Dispatcher_ChangeRandomOption();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_ChangeTutorialFlag : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_ChangeTutorialFlag();
    virtual ~Dispatcher_ChangeTutorialFlag();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Change_Charac_Slot : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_Change_Charac_Slot();
    virtual ~Dispatcher_Change_Charac_Slot();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_CharacterStatistic : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_CharacterStatistic();
    virtual ~Dispatcher_CharacterStatistic();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_CloseDisjointStore : public IPacketDispatcher<MSG_BASE, stCloseDisjointStoreParam, ch_state3>
{
public:
    Dispatcher_CloseDisjointStore();
    virtual ~Dispatcher_CloseDisjointStore();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_CompatibilityIndex : public IPacketDispatcher<MSG_COMPATIBILITY_INDEX, ParamBase, ch_state0>
{
public:
    Dispatcher_CompatibilityIndex();
    virtual ~Dispatcher_CompatibilityIndex();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_CreateAccountCargo : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_CreateAccountCargo();
    virtual ~Dispatcher_CreateAccountCargo();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_CreatePrivateStore : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_CreatePrivateStore();
    virtual ~Dispatcher_CreatePrivateStore();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_DungeonNPCBuffInfo : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_DungeonNPCBuffInfo();
    virtual ~Dispatcher_DungeonNPCBuffInfo();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_EnterDisjointStore : public IPacketDispatcher<MSG_ENTER_DISJOINT_STORE, stEnterDisjointStoreParam, ch_state3>
{
public:
    Dispatcher_EnterDisjointStore();
    virtual ~Dispatcher_EnterDisjointStore();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_FrameLagStatistics : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_FrameLagStatistics();
    virtual ~Dispatcher_FrameLagStatistics();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_GuildCargoMoveItem : public IPacketDispatcher<MSG_GUILD_CARGO_MOVE_ITEM, stGuildCargoMoveItemParam, ch_state0>
{
public:
    Dispatcher_GuildCargoMoveItem();
    virtual ~Dispatcher_GuildCargoMoveItem();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_GuildCargoPushItem : public IPacketDispatcher<MSG_GUILD_CARGO_PUSH_ITEM, stGuildCargoPushItemParam, ch_state0>
{
public:
    Dispatcher_GuildCargoPushItem();
    virtual ~Dispatcher_GuildCargoPushItem();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_RemovePrivateStore : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_RemovePrivateStore();
    virtual ~Dispatcher_RemovePrivateStore();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_RequestAddPvPBuddy : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_RequestAddPvPBuddy();
    virtual ~Dispatcher_RequestAddPvPBuddy();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_RequestGuildSecede : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_RequestGuildSecede();
    virtual ~Dispatcher_RequestGuildSecede();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_RequestMemberEnter : public IPacketDispatcher<MSG_REQUEST_MEMBER_ENTER, stRequestMemberEnterParam, ch_state3>
{
public:
    Dispatcher_RequestMemberEnter();
    virtual ~Dispatcher_RequestMemberEnter();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_Request_Seria_Buff : public IPacketDispatcher<MSG_BASE, stGrowthPowerParam, ch_state0>
{
public:
    Dispatcher_Request_Seria_Buff();
    virtual ~Dispatcher_Request_Seria_Buff();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_Score_Scroll_State : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_Score_Scroll_State();
    virtual ~Dispatcher_Score_Scroll_State();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_SecurityCardDisuse : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_SecurityCardDisuse();
    virtual ~Dispatcher_SecurityCardDisuse();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_StackableActionUse : public IPacketDispatcher<MSG_STACKABLE_ACTION_USE, stStackableActionUse, ch_state3>
{
public:
    Dispatcher_StackableActionUse();
    virtual ~Dispatcher_StackableActionUse();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_TitleBookOtherUser : public IPacketDispatcher<MSG_TITLEBOOK_OTHERUSER, ParamBase, ch_state3>
{
public:
    Dispatcher_TitleBookOtherUser();
    virtual ~Dispatcher_TitleBookOtherUser();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_UnsealRandomOption : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_UnsealRandomOption();
    virtual ~Dispatcher_UnsealRandomOption();
    int _check_unseal_scroll_item();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_ValidateScriptHash : public IPacketDispatcher<MSG_SCRIPT_HASH, ScriptHashParam, ch_state0>
{
public:
    Dispatcher_ValidateScriptHash();
    virtual ~Dispatcher_ValidateScriptHash();
    int check_error(CUser* user, MSG_BASE& msg);
    int get_hack_type();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_AuctionRegistCancel : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_AuctionRegistCancel();
    virtual ~Dispatcher_AuctionRegistCancel();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_BuyPrivateStoreItem : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_BuyPrivateStoreItem();
    virtual ~Dispatcher_BuyPrivateStoreItem();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_CallGuildAllMembers : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_CallGuildAllMembers();
    virtual ~Dispatcher_CallGuildAllMembers();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_CompleteLoadAssault : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_CompleteLoadAssault();
    virtual ~Dispatcher_CompleteLoadAssault();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_CreateDisjointStore : public IPacketDispatcher<MSG_CREATE_DISJOINT_STORE, stCreateDisjointStoreParam, ch_state3>
{
public:
    Dispatcher_CreateDisjointStore();
    virtual ~Dispatcher_CreateDisjointStore();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_CreatureSendMessage : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_CreatureSendMessage();
    virtual ~Dispatcher_CreatureSendMessage();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_FightVillageMonster : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_FightVillageMonster();
    virtual ~Dispatcher_FightVillageMonster();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_GuildAttendanceInfo : public IPacketDispatcher<MSG_GUILD_ATTENDANCE, ParamBase, ch_state0>
{
public:
    Dispatcher_GuildAttendanceInfo();
    virtual ~Dispatcher_GuildAttendanceInfo();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_GuildMasterDelegate : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_GuildMasterDelegate();
    virtual ~Dispatcher_GuildMasterDelegate();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_New_Gmdebug_Command : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_New_Gmdebug_Command();
    virtual ~Dispatcher_New_Gmdebug_Command();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int oldGmRoutine();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_PowerWarProcessInfo : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_PowerWarProcessInfo();
    virtual ~Dispatcher_PowerWarProcessInfo();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_PvpMissionHPPercent : public IPacketDispatcher<MSG_PVP_MISSION_HP_PERCENT, stSendClearedMission, ch_state3>
{
public:
    Dispatcher_PvpMissionHPPercent();
    virtual ~Dispatcher_PvpMissionHPPercent();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_RegisterToBlackList : public IPacketDispatcher<MSG_REGISTER_TO_BLACKLIST, stRegisterToBlackListParam, ch_state3>
{
public:
    Dispatcher_RegisterToBlackList();
    virtual ~Dispatcher_RegisterToBlackList();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_RequestAssaultPrice : public IPacketDispatcher<MsgRequestAssaultPrice, stRequestAssaultPrice, ch_state3>
{
public:
    Dispatcher_RequestAssaultPrice();
    virtual ~Dispatcher_RequestAssaultPrice();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_RequestDisjointItem : public IPacketDispatcher<MSG_REQUEST_DISJOINT_ITEM, stRequestDisjointItemParam, ch_state3>
{
public:
    Dispatcher_RequestDisjointItem();
    virtual ~Dispatcher_RequestDisjointItem();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_ResponseAddPvPBuddy : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_ResponseAddPvPBuddy();
    virtual ~Dispatcher_ResponseAddPvPBuddy();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_SaveCharacterOption : public IPacketDispatcher<MSG_SAVE_CHARACTER_OPTION, stSaveCharacOption, ch_state2>
{
public:
    Dispatcher_SaveCharacterOption();
    virtual ~Dispatcher_SaveCharacterOption();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_SecretShopOpenClose : public IPacketDispatcher<MSG_SECRET_SHOP_OPEN_CLOSE, stSecretShopOpenCloseParam, ch_state0>
{
public:
    Dispatcher_SecretShopOpenClose();
    virtual ~Dispatcher_SecretShopOpenClose();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_SecurityCardAuthReq : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_SecurityCardAuthReq();
    virtual ~Dispatcher_SecurityCardAuthReq();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_SecurityCardAuthRpy : public IPacketDispatcher<MSG_SECU_RPY, SecuCardRpyParam, ch_state2>
{
public:
    Dispatcher_SecurityCardAuthRpy();
    virtual ~Dispatcher_SecurityCardAuthRpy();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_SecurityCardCertKey : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_SecurityCardCertKey();
    virtual ~Dispatcher_SecurityCardCertKey();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_UpdateServerMessage : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_UpdateServerMessage();
    virtual ~Dispatcher_UpdateServerMessage();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_UpgradeAccountCargo : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_UpgradeAccountCargo();
    virtual ~Dispatcher_UpgradeAccountCargo();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_UpgradeItemSeparate : public IPacketDispatcher<MSG_ITEM_UPGRADE, STUdateItemParam, ch_state0>
{
public:
    Dispatcher_UpgradeItemSeparate();
    virtual ~Dispatcher_UpgradeItemSeparate();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_UseSharedEffectItem : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_UseSharedEffectItem();
    virtual ~Dispatcher_UseSharedEffectItem();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_VerifyCreatureQuest : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state3>
{
public:
    Dispatcher_VerifyCreatureQuest();
    virtual ~Dispatcher_VerifyCreatureQuest();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_WarRoomWpPerMonster : public IPacketDispatcher<MSG_WARROOM_WP_PER_MONSTER, stWarRoomWp, ch_state0>
{
public:
    Dispatcher_WarRoomWpPerMonster();
    virtual ~Dispatcher_WarRoomWpPerMonster();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_AbnormalUseStackable : public IPacketDispatcher<MSG_ABNORMAL_USE_STACKABLE, AbnormalUseStackable, ch_state0>
{
public:
    Dispatcher_AbnormalUseStackable();
    virtual ~Dispatcher_AbnormalUseStackable();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_AuctionBuyItemApiece : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_AuctionBuyItemApiece();
    virtual ~Dispatcher_AuctionBuyItemApiece();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_AuctionMyBiddingInfo : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_AuctionMyBiddingInfo();
    virtual ~Dispatcher_AuctionMyBiddingInfo();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_BuyCeraShopLimitItem : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_BuyCeraShopLimitItem();
    virtual ~Dispatcher_BuyCeraShopLimitItem();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_CallGuildCreateRight : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_CallGuildCreateRight();
    virtual ~Dispatcher_CallGuildCreateRight();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    int send_packet_guildcreateright();
};

class Dispatcher_Check3rdPartyConcent : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_Check3rdPartyConcent();
    virtual ~Dispatcher_Check3rdPartyConcent();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_CheckGuildNameDouble : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_CheckGuildNameDouble();
    virtual ~Dispatcher_CheckGuildNameDouble();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Compound_ExtreamItem : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_Compound_ExtreamItem();
    virtual ~Dispatcher_Compound_ExtreamItem();
    int _push_newitem();
    int _read_packet();
    int _remove_victim();
    int _reset();
    int _send_result();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_GatheringPartyStatus : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_GatheringPartyStatus();
    virtual ~Dispatcher_GatheringPartyStatus();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_ItemHyperLinkMessage : public IPacketDispatcher<MSG_ITEM_HYPER_LINK_MESSAGE, stItemHyperLinkMessage, ch_state3>
{
public:
    Dispatcher_ItemHyperLinkMessage();
    virtual ~Dispatcher_ItemHyperLinkMessage();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_NotifyMessageToGuild : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_NotifyMessageToGuild();
    virtual ~Dispatcher_NotifyMessageToGuild();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_OperateRidableObject : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_OperateRidableObject();
    virtual ~Dispatcher_OperateRidableObject();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_PartyTeleportConfirm : public IPacketDispatcher<MSG_PARTY_TELEPORT_CONFIRM, stPartyTeleportConfirm, ch_state0>
{
public:
    Dispatcher_PartyTeleportConfirm();
    virtual ~Dispatcher_PartyTeleportConfirm();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_PcRoomPlayTimeReward : public IPacketDispatcher<MSG_BASE, PcRoomPlayTimeRewardParam, ch_state3>
{
public:
    Dispatcher_PcRoomPlayTimeReward();
    virtual ~Dispatcher_PcRoomPlayTimeReward();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_PrecheckSoloTeleport : public IPacketDispatcher<MSG_SOLO_TELEPORT, stSoloTeleportParam, ch_state0>
{
public:
    Dispatcher_PrecheckSoloTeleport();
    virtual ~Dispatcher_PrecheckSoloTeleport();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_PvpMissionComboClear : public IPacketDispatcher<MSG_PVP_MISSION_COMBO_CLEAR, ParamBase, ch_state3>
{
public:
    Dispatcher_PvpMissionComboClear();
    virtual ~Dispatcher_PvpMissionComboClear();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Register_Quick_Party : public IPacketDispatcher<MSG_QUICK_PARTY, stQuickPartyParam, ch_state5>
{
public:
    Dispatcher_Register_Quick_Party();
    virtual ~Dispatcher_Register_Quick_Party();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_ReportMannerlessUser : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_ReportMannerlessUser();
    virtual ~Dispatcher_ReportMannerlessUser();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Request_PvpExpOfWeek : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_Request_PvpExpOfWeek();
    virtual ~Dispatcher_Request_PvpExpOfWeek();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Security_ProcessScan : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_Security_ProcessScan();
    virtual ~Dispatcher_Security_ProcessScan();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_SeriaRoom_Deco_Event : public IPacketDispatcher<MSG_SERIAROOM_DECO_EVENT, stSeriaRoomDecoEvent, ch_state0>
{
public:
    Dispatcher_SeriaRoom_Deco_Event();
    virtual ~Dispatcher_SeriaRoom_Deco_Event();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_WriteGuildMemberMemo : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_WriteGuildMemberMemo();
    virtual ~Dispatcher_WriteGuildMemberMemo();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_BlueMarbleConfirmInfo : public IPacketDispatcher<MSG_BLUE_MARBLE_CONFIRM_INFO, stBlueMarbleConfirmInfo, ch_state3>
{
public:
    Dispatcher_BlueMarbleConfirmInfo();
    virtual ~Dispatcher_BlueMarbleConfirmInfo();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_Client_Spec_Statistic : public IPacketDispatcher<MSG_CLIENT_SPEC_STATISTIC, stClientSpecStatisticParam, ch_state2>
{
public:
    Dispatcher_Client_Spec_Statistic();
    virtual ~Dispatcher_Client_Spec_Statistic();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_CreatureScriptMessage : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_CreatureScriptMessage();
    virtual ~Dispatcher_CreatureScriptMessage();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Mercenary_Competition : public IPacketDispatcher<MsgMercenaryCompetition, SendMercenaryCompetitionResult, ch_state0>
{
public:
    Dispatcher_Mercenary_Competition();
    virtual ~Dispatcher_Mercenary_Competition();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_OpenGuildCreateWindow : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_OpenGuildCreateWindow();
    virtual ~Dispatcher_OpenGuildCreateWindow();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_RepairDisjointMachine : public IPacketDispatcher<MSG_BASE, stRepairDisjointMachineParam, ch_state3>
{
public:
    Dispatcher_RepairDisjointMachine();
    virtual ~Dispatcher_RepairDisjointMachine();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_AuctionAskAveragePrice : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_AuctionAskAveragePrice();
    virtual ~Dispatcher_AuctionAskAveragePrice();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_AuctionSearchByItemKey : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_AuctionSearchByItemKey();
    virtual ~Dispatcher_AuctionSearchByItemKey();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Break_Away_Quest_Check : public IPacketDispatcher<MSG_BREAK_AWAY, stBreakAwayParam, ch_state3>
{
public:
    Dispatcher_Break_Away_Quest_Check();
    virtual ~Dispatcher_Break_Away_Quest_Check();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_ChangeAnotherSkillTree : public IPacketDispatcher<MSG_CHANGE_ANOTHER_SKILL_TREE, stChangeAnotherSkillTree, ch_state0>
{
public:
    Dispatcher_ChangeAnotherSkillTree();
    virtual ~Dispatcher_ChangeAnotherSkillTree();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_CharacSlotExtendEffect : public IPacketDispatcher<MSG_BASE, stCharacSlotExtendEffect, ch_state0>
{
public:
    Dispatcher_CharacSlotExtendEffect();
    virtual ~Dispatcher_CharacSlotExtendEffect();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_CheckAssaultMotionHack : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_CheckAssaultMotionHack();
    virtual ~Dispatcher_CheckAssaultMotionHack();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_ConnectP2PAfterAssault : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_ConnectP2PAfterAssault();
    virtual ~Dispatcher_ConnectP2PAfterAssault();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_DeathTowerStageCommand : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_DeathTowerStageCommand();
    virtual ~Dispatcher_DeathTowerStageCommand();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Delete_GuildBoard_Text : public IPacketDispatcher<MSG_GUILD_BOARD, stGuildBoardParam, ch_state3>
{
public:
    Dispatcher_Delete_GuildBoard_Text();
    virtual ~Dispatcher_Delete_GuildBoard_Text();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_DungeonEventStoryPause : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_DungeonEventStoryPause();
    virtual ~Dispatcher_DungeonEventStoryPause();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Float_RDATA_Modulation : public IPacketDispatcher<MSG_FLOAT_RDATA_MODULATION, ParamBase, ch_state0>
{
public:
    Dispatcher_Float_RDATA_Modulation();
    virtual ~Dispatcher_Float_RDATA_Modulation();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_LogIngameAdvertisement : public IPacketDispatcher<MSG_LOG_INGAME_ADVERTISEMENT, stLogIngameAdvertisement, ch_state2>
{
public:
    Dispatcher_LogIngameAdvertisement();
    virtual ~Dispatcher_LogIngameAdvertisement();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_QueryCharacInfoMailbox : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_QueryCharacInfoMailbox();
    virtual ~Dispatcher_QueryCharacInfoMailbox();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_RequestCharacSkillInfo : public IPacketDispatcher<MSG_REQUEST_CHARAC_SKILL_INFO, stRequestCharacSkillInfo, ch_state3>
{
public:
    Dispatcher_RequestCharacSkillInfo();
    virtual ~Dispatcher_RequestCharacSkillInfo();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_SecurityCardAuthCancel : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_SecurityCardAuthCancel();
    virtual ~Dispatcher_SecurityCardAuthCancel();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_SecurityCardRetransfer : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_SecurityCardRetransfer();
    virtual ~Dispatcher_SecurityCardRetransfer();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_TournamentRewardSelect : public IPacketDispatcher<MsgTournamentRewardSelect, stTournamentRewardSelect, ch_state0>
{
public:
    Dispatcher_TournamentRewardSelect();
    virtual ~Dispatcher_TournamentRewardSelect();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_UpgradeDisjointMachine : public IPacketDispatcher<MSG_BASE, stUpgradeDisjointMachineParam, ch_state3>
{
public:
    Dispatcher_UpgradeDisjointMachine();
    virtual ~Dispatcher_UpgradeDisjointMachine();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_VerifyPrivateStoreItem : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_VerifyPrivateStoreItem();
    virtual ~Dispatcher_VerifyPrivateStoreItem();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_AuctionMyAuctionHistory : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_AuctionMyAuctionHistory();
    virtual ~Dispatcher_AuctionMyAuctionHistory();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Card_Select_Right_State : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_Card_Select_Right_State();
    virtual ~Dispatcher_Card_Select_Right_State();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_CheckGuildAddressDouble : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_CheckGuildAddressDouble();
    virtual ~Dispatcher_CheckGuildAddressDouble();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_CompoundItemByExpertJob : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_CompoundItemByExpertJob();
    virtual ~Dispatcher_CompoundItemByExpertJob();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_InvestItemAmplifyOption : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_InvestItemAmplifyOption();
    virtual ~Dispatcher_InvestItemAmplifyOption();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_RequestItemUnlockCancel : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_RequestItemUnlockCancel();
    virtual ~Dispatcher_RequestItemUnlockCancel();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_RequestPCRoomPlayerList : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_RequestPCRoomPlayerList();
    virtual ~Dispatcher_RequestPCRoomPlayerList();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Write_On_The_GuildBoard : public IPacketDispatcher<MSG_GUILD_BOARD, stGuildBoardParam, ch_state3>
{
public:
    Dispatcher_Write_On_The_GuildBoard();
    virtual ~Dispatcher_Write_On_The_GuildBoard();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_AuctionSearchByNoItmeKey : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_AuctionSearchByNoItmeKey();
    virtual ~Dispatcher_AuctionSearchByNoItmeKey();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_CheckEnableServerMessage : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_CheckEnableServerMessage();
    virtual ~Dispatcher_CheckEnableServerMessage();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_CompleteLoadAfterAssault : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_CompleteLoadAfterAssault();
    virtual ~Dispatcher_CompleteLoadAfterAssault();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Enchant_3rdChronicleItem : public IPacketDispatcher<MSG_ENCHANT, stEnchantParam, ch_state0>
{
public:
    Dispatcher_Enchant_3rdChronicleItem();
    virtual ~Dispatcher_Enchant_3rdChronicleItem();
    int check_error(CUser* user, MSG_BASE& msg);
    int check_state();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_GoblinPadRequestCryptKey : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_GoblinPadRequestCryptKey();
    virtual ~Dispatcher_GoblinPadRequestCryptKey();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_GoldTakeIncreasingAmount : public IPacketDispatcher<MSG_GOLD_TAKE_INCREASING_AMOUNT, stGoldTakeIncreasingAmount, ch_state0>
{
public:
    Dispatcher_GoldTakeIncreasingAmount();
    virtual ~Dispatcher_GoldTakeIncreasingAmount();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_LevelupSupportReqestItem : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_LevelupSupportReqestItem();
    virtual ~Dispatcher_LevelupSupportReqestItem();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_RequestOnTimeEventReward : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_RequestOnTimeEventReward();
    virtual ~Dispatcher_RequestOnTimeEventReward();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_RequestPCRoomPlayerCount : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_RequestPCRoomPlayerCount();
    virtual ~Dispatcher_RequestPCRoomPlayerCount();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Security_MemoryIntegrity : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_Security_MemoryIntegrity();
    virtual ~Dispatcher_Security_MemoryIntegrity();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Security_PacketIntegrity : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_Security_PacketIntegrity();
    virtual ~Dispatcher_Security_PacketIntegrity();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_SelectUltimateDifficulty : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_SelectUltimateDifficulty();
    virtual ~Dispatcher_SelectUltimateDifficulty();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Select_Item_Grwoth_Power : public IPacketDispatcher<MSG_GROWTH_POWER, stGrowthPowerParam, ch_state3>
{
public:
    Dispatcher_Select_Item_Grwoth_Power();
    virtual ~Dispatcher_Select_Item_Grwoth_Power();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_SeriaRoom_Ani_Deco_Event : public IPacketDispatcher<MSG_SERIAROOM_DECO_EVENT, stSeriaRoomDecoEvent, ch_state3>
{
public:
    Dispatcher_SeriaRoom_Ani_Deco_Event();
    virtual ~Dispatcher_SeriaRoom_Ani_Deco_Event();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_UpdateContractOfCubeInfo : public IPacketDispatcher<MSG_UPDATE_CONTRACT_OF_CUBE_INFO, stUpdateContractOfCubeInfo, ch_state0>
{
public:
    Dispatcher_UpdateContractOfCubeInfo();
    virtual ~Dispatcher_UpdateContractOfCubeInfo();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_UseCreatureEvolutionItem : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_UseCreatureEvolutionItem();
    virtual ~Dispatcher_UseCreatureEvolutionItem();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_UseRightOfChangeGrowType : public IPacketDispatcher<MSG_RIGHT_OF_CHANGE_GROW_TYPE, stRightOfChangeGrowType, ch_state3>
{
public:
    Dispatcher_UseRightOfChangeGrowType();
    virtual ~Dispatcher_UseRightOfChangeGrowType();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_AuctionMyRegistedItemInfo : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_AuctionMyRegistedItemInfo();
    virtual ~Dispatcher_AuctionMyRegistedItemInfo();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_BloodRoundUiPrepareFinish : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_BloodRoundUiPrepareFinish();
    virtual ~Dispatcher_BloodRoundUiPrepareFinish();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Skill_Command_All_Default : public IPacketDispatcher<MSG_SKILL_COMMAND, stSkillCommandParam, ch_state3>
{
public:
    Dispatcher_Skill_Command_All_Default();
    virtual ~Dispatcher_Skill_Command_All_Default();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_Skill_Command_Customizing : public IPacketDispatcher<MSG_SKILL_COMMAND, stSkillCommandParam, ch_state3>
{
public:
    Dispatcher_Skill_Command_Customizing();
    virtual ~Dispatcher_Skill_Command_Customizing();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_Buy_Charac_Status_Using_QP : public IPacketDispatcher<MSG_QUEST_SHOP, STQuestShopBuyParam, ch_state0>
{
public:
    Dispatcher_Buy_Charac_Status_Using_QP();
    virtual ~Dispatcher_Buy_Charac_Status_Using_QP();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_Change_PartyMemberPosition : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_Change_PartyMemberPosition();
    virtual ~Dispatcher_Change_PartyMemberPosition();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_RequestIngameAdvertisement : public IPacketDispatcher<MSG_REQUEST_INGAME_ADVERTISEMENT, stRequestIngameAdvertisement, ch_state2>
{
public:
    Dispatcher_RequestIngameAdvertisement();
    virtual ~Dispatcher_RequestIngameAdvertisement();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
    int sendRewardItemSystemMail();
};

class Dispatcher_CallPartyMemberRealtimeInfo : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state3>
{
public:
    Dispatcher_CallPartyMemberRealtimeInfo();
    virtual ~Dispatcher_CallPartyMemberRealtimeInfo();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_Direct_Entrance_Quick_Party : public IPacketDispatcher<MSG_BASE, stQuickPartyParam, ch_state5>
{
public:
    Dispatcher_Direct_Entrance_Quick_Party();
    virtual ~Dispatcher_Direct_Entrance_Quick_Party();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_Exchange_Random_Item_Reward : public IPacketDispatcher<MSG_EXCHANGE_RANDOM_ITEM_REWARD, stExchangeRandomItemReward, ch_state0>
{
public:
    Dispatcher_Exchange_Random_Item_Reward();
    virtual ~Dispatcher_Exchange_Random_Item_Reward();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_RequestConditionEventReward : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_RequestConditionEventReward();
    virtual ~Dispatcher_RequestConditionEventReward();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_TournamentRewardSelectState : public IPacketDispatcher<MSG_BASE, stTournamentRewardSelectState, ch_state0>
{
public:
    Dispatcher_TournamentRewardSelectState();
    virtual ~Dispatcher_TournamentRewardSelectState();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_Expand_Equipslot_Flag_Update : public IPacketDispatcher<MSG_EXPAND_EQUIPSLOT_FLAG_UPDATE, stExpandEquipslotFlagUpdate, ch_state3>
{
public:
    Dispatcher_Expand_Equipslot_Flag_Update();
    virtual ~Dispatcher_Expand_Equipslot_Flag_Update();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_FinishVillageMonsterFighting : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_FinishVillageMonsterFighting();
    virtual ~Dispatcher_FinishVillageMonsterFighting();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_GrowthWeaponEventUseMaterial : public IPacketDispatcher<MSG_GROWTH_EQUIPMENT, stGrowthEquipment, ch_state3>
{
public:
    Dispatcher_GrowthWeaponEventUseMaterial();
    virtual ~Dispatcher_GrowthWeaponEventUseMaterial();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_HeroMissionEventState_Reward : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_HeroMissionEventState_Reward();
    virtual ~Dispatcher_HeroMissionEventState_Reward();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_SaveGameOption_QuickChatting : public IPacketDispatcher<MSG_QUICK_CHAT, stQuickChatParam, ch_state2>
{
public:
    Dispatcher_SaveGameOption_QuickChatting();
    virtual ~Dispatcher_SaveGameOption_QuickChatting();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Avatar_Disjoint_Random_Reward : public IPacketDispatcher<MSG_AVATAR_DISJOINT_RANDOM_REWARD, stAvatarDisjointRandomReward, ch_state0>
{
public:
    Dispatcher_Avatar_Disjoint_Random_Reward();
    virtual ~Dispatcher_Avatar_Disjoint_Random_Reward();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_SDC_ActiveStatus_Damage_Check : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_SDC_ActiveStatus_Damage_Check();
    virtual ~Dispatcher_SDC_ActiveStatus_Damage_Check();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_police_queryCharacName_useMid : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_police_queryCharacName_useMid();
    virtual ~Dispatcher_police_queryCharacName_useMid();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_ImageCommunicationEquipmentUse : public IPacketDispatcher<MSG_BASE, stImageCommunicationEquipmentUse, ch_state3>
{
public:
    Dispatcher_ImageCommunicationEquipmentUse();
    virtual ~Dispatcher_ImageCommunicationEquipmentUse();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_Security_NonClient_GetResponse : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_Security_NonClient_GetResponse();
    virtual ~Dispatcher_Security_NonClient_GetResponse();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Security_PI_HashModulation_C2C : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_Security_PI_HashModulation_C2C();
    virtual ~Dispatcher_Security_PI_HashModulation_C2C();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Security_PI_HashModulation_S2C : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_Security_PI_HashModulation_S2C();
    virtual ~Dispatcher_Security_PI_HashModulation_S2C();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_Compound_Equipment_Upgrade_Card : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_Compound_Equipment_Upgrade_Card();
    virtual ~Dispatcher_Compound_Equipment_Upgrade_Card();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_GrowthWeaponEventChangeInfinity : public IPacketDispatcher<MSG_GROWTH_EQUIPMENT, stGrowthEquipment, ch_state3>
{
public:
    Dispatcher_GrowthWeaponEventChangeInfinity();
    virtual ~Dispatcher_GrowthWeaponEventChangeInfinity();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_SaveGameOption_ChattingEmoticon : public IPacketDispatcher<MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON, ParamBase, ch_state2>
{
public:
    Dispatcher_SaveGameOption_ChattingEmoticon();
    virtual ~Dispatcher_SaveGameOption_ChattingEmoticon();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_ComboSkillExtensionQuickSlotReset : public IPacketDispatcher<MSG_COMBO_SKILL_EXTENSION_QUICK_SLOT_RESET, stComboSkillExtensionQuickSlotReset, ch_state3>
{
public:
    Dispatcher_ComboSkillExtensionQuickSlotReset();
    virtual ~Dispatcher_ComboSkillExtensionQuickSlotReset();
    int check_error(CUser* user, MSG_BASE& msg);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_Select_3rdChronicleItem_ForEnchant : public IPacketDispatcher<MSG_BASE, stEnchantParam, ch_state0>
{
public:
    Dispatcher_Select_3rdChronicleItem_ForEnchant();
    virtual ~Dispatcher_Select_3rdChronicleItem_ForEnchant();
    int check_state();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_GrowthCreatureChangeInfinityCreature : public IPacketDispatcher<MSG_BASE, stGrowthCreature, ch_state3>
{
public:
    Dispatcher_GrowthCreatureChangeInfinityCreature();
    virtual ~Dispatcher_GrowthCreatureChangeInfinityCreature();
    int check_error(CUser* user, MSG_BASE& msg);
    int check_error_event();
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int process_event();
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

class Dispatcher_P2PHolePunchingSuccessRateStatistics : public IPacketDispatcher<MSG_P2P_HOLE_PUNCHING_SUCCESS_RATE, ParamBase, ch_state0>
{
public:
    Dispatcher_P2PHolePunchingSuccessRateStatistics();
    virtual ~Dispatcher_P2PHolePunchingSuccessRateStatistics();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    void send(CUser* user, ParamBase& param);
};

namespace ARAD {
namespace DISPATCHER {
class Dispatcher_AvartarRoulette : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_AvartarRoulette();
    virtual ~Dispatcher_AvartarRoulette();
    int checkWorkState(CUser* user);
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_AvartarCoinCount : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_AvartarCoinCount();
    virtual ~Dispatcher_AvartarCoinCount();
    int checkWorkState(CUser* user);
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_AradAvatarConvert : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_AradAvatarConvert();
    virtual ~Dispatcher_AradAvatarConvert();
    int checkWorkState(CUser* user);
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_AradEmblemCompound : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_AradEmblemCompound();
    virtual ~Dispatcher_AradEmblemCompound();
    int checkWorkState(CUser* user);
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_AradJumpingCharacter : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_AradJumpingCharacter();
    virtual ~Dispatcher_AradJumpingCharacter();
    int checkWorkState(CUser* user);
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_AradUseAvatarRecharge : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_AradUseAvatarRecharge();
    virtual ~Dispatcher_AradUseAvatarRecharge();
    int checkWorkState(CUser* user);
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_EventCreateDnfRequest : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_EventCreateDnfRequest();
    virtual ~Dispatcher_EventCreateDnfRequest();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_AvartarHiddenOptionChange : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_AvartarHiddenOptionChange();
    virtual ~Dispatcher_AvartarHiddenOptionChange();
    int checkWorkState(CUser* user);
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_RequestPrcoomDayilyReward : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_RequestPrcoomDayilyReward();
    virtual ~Dispatcher_RequestPrcoomDayilyReward();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

}
}
namespace ARAD {
class Arad_Dispatcher_Manager : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Arad_Dispatcher_Manager();
    virtual ~Arad_Dispatcher_Manager();
    int db_dispatch();
    int inter_dispatch();
    int open();
    int register_dispatcher();
    int unregister_dispatcher();
};

class Arad_Dispatcher_Login_Impl : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Arad_Dispatcher_Login_Impl();
    virtual ~Arad_Dispatcher_Login_Impl();
    int account_password_check();
    int db_dispatch();
    int inter_dispatch();
    int pad_certifiaction_check();
    int premium_info_set();
    int punish_user_check();
};

class Arad_Dispatcher_BuyItemCheck_Impl : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Arad_Dispatcher_BuyItemCheck_Impl();
    virtual ~Arad_Dispatcher_BuyItemCheck_Impl();
    int db_dispatch();
    int inter_dispatch();
};

}
namespace APSystem {
class Dispatcher_ClearAction : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_ClearAction();
    virtual ~Dispatcher_ClearAction();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

class Dispatcher_GetTodayReward : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_GetTodayReward();
    virtual ~Dispatcher_GetTodayReward();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
};

}
#endif  // DNF_GAME_DISPATCHERS_H_
