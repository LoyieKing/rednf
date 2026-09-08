#ifndef DNF_GAME_INTER_DISPATCHER_H_
#define DNF_GAME_INTER_DISPATCHER_H_

// df_game_r 还原 —— InterDispatcher / ISigDispatcher / 全部 Inter_* 信号分发器
// 本头为这些类型的唯一声明点。类名/槽位/注册顺序取自 ORIG 构造函数
// （0x84bbafa）objdump 逐条转写；绝大多数对象尺寸=4（仅 vptr），
// 例外：Inter_NpcLimitBuyItemInfo=0x98（含 pair+双 Inven_Item 成员）。
// ORIG 槽 724/846 的 Inter_DispatcherAradOnly(0x1c)/Inter_DispatcherTaiwan(0x1c)
// 分别内嵌 ARAD/Taiwan 内部分发子系统（TaiwanInternalDispatcher /
// ARAD::DISPATCHER::Arad_InternalDispatcher，当前工程无定义），待该域实现后补注册。
// ISigDispatcher 虚函数顺序：dispatch_sig 在虚析构之前（ORIG dispatch 经
// vtable[0] 调用，见 0x84be970）；Inter_LoadEventLevelupSupport/
// Inter_SelectAllowProxyUser 无用户声明 ctor/dtor（new 值初始化形态）。

class CUser;
class LimitNpcBuyItemInfo;  // game_master::NpcLimitBuyItem 脚本域类型（checkError 参数）
class SkillSlot;
struct _Mastered_skill;
class CSkillTreeParam;      // Inter_LoadEtc friend（SetSkillTree 0x84ef55c 调用 private 辅助）

#include <utility>          // std::pair（Inter_NpcLimitBuyItemInfo 成员）
#include "Inven_Item.h"     // Inven_Item（pack(1)，成员对齐 1）
#include "SigTypes.h"       // SIG_LOAD_ETC / SIG_ATTENDANCE_EVENT（Inter_LoadEct 方法声明）

// ORIG _Z7getUserj T @0x084bb9cf（全局自由函数；定义属主未落地，U 引用合规）
CUser* getUser(unsigned int uid);
#include "CUserCharacInfo.h" // seriaRoom_AniDeco（按值参数需完整类型）

// ORIG dispatch() 经 vtable[0] 调用 dispatch_sig（0x84be970 处 call *%edx 无位移），
class ISigDispatcher
{
public:
    ISigDispatcher() {}
    virtual int dispatch_sig(CUser* user, char* data, int len) = 0;
    virtual ~ISigDispatcher();
};

class Inter_Disconnect : public ISigDispatcher
{
public:
    Inter_Disconnect() {}
    virtual ~Inter_Disconnect();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_SessionClose : public ISigDispatcher
{
public:
    Inter_SessionClose() {}
    virtual ~Inter_SessionClose();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoginInfo : public ISigDispatcher
{
public:
    Inter_LoginInfo() {}
    virtual ~Inter_LoginInfo();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_CreateCharac : public ISigDispatcher
{
public:
    Inter_CreateCharac() {}
    virtual ~Inter_CreateCharac();
    virtual int dispatch_sig(CUser* user, char* data, int len);

    void process_event(CUser* user, struct SIG_CREATE_CHARAC* sig);          // ORIG T 0x084bf4ac
    void process_createGift(CUser* user, struct SIG_CREATE_CHARAC* sig);     // ORIG T 0x084bf4ce

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_DeleteCharac : public ISigDispatcher
{
public:
    Inter_DeleteCharac() {}
    virtual ~Inter_DeleteCharac();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadCharac : public ISigDispatcher
{
public:
    Inter_LoadCharac() {}
    virtual ~Inter_LoadCharac();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadInven : public ISigDispatcher
{
public:
    Inter_LoadInven() {}
    virtual ~Inter_LoadInven();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG T 私有辅助（符号 _ZN15Inter_LoadInven 前缀；Main 已批准，实现落
    // InterDispatcher.cpp）：
    void _CheckOverEquipItem(CUser* user);                                        // 0x084c4d38
    void _CheckUsableExpertJobEquipment(CUser* user);                             // 0x084c4b38
    void _UpdateRentalItems(CUser* user, SIG_LOAD_INVENTORY* sig);                // 0x084c455c
    void _UpdateDimensionRentalItems(CUser* user, SIG_LOAD_INVENTORY* sig);       // 0x084c4858
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadSkill : public ISigDispatcher
{
public:
    Inter_LoadSkill() {}
    virtual ~Inter_LoadSkill();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadQuest : public ISigDispatcher
{
public:
    Inter_LoadQuest() {}
    virtual ~Inter_LoadQuest();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadTicket : public ISigDispatcher
{
public:
    Inter_LoadTicket() {}
    virtual ~Inter_LoadTicket();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadDungeonClear : public ISigDispatcher
{
public:
    Inter_LoadDungeonClear() {}
    virtual ~Inter_LoadDungeonClear();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadEtc : public ISigDispatcher
{
public:
    // ORIG T 0x084d8e68（ORIG 调用点无 this 压栈 → static；外部调用方
    // break_away_prevent::CBreakAwayPreventSystem::giveBreakAwayDungeonClearReward）
    static void giveBreakAwayReward(CUser* user);
    Inter_LoadEtc() {}
    virtual ~Inter_LoadEtc();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    friend class CSkillTreeParam;   // ORIG SetSkillTree 0x84ef55c 普通成员调用 private 辅助
    // ---- ORIG 辅助方法（docs/class_func_reports/Inter_LoadEtc）----
    int ProcessOneDayLethe(SIG_LOAD_ETC* sig, CUser* user, int& err);   // ORIG 返 0x40c6 非布尔
    int ProcessOneDayLethe2ND(SIG_LOAD_ETC* sig, CUser* user, int& err);  // ORIG 返 0x4111
    bool RequestDBEventArad(CUser* user);                    // ORIG T 0x084c266e
    bool SendResult(CUser* user);                            // ORIG T 0x084c2b82
    bool SetFatigue(CUser* user);                            // ORIG T 0x084c311a
    bool SetPvpRecord(CUser* user, SIG_LOAD_ETC* sig);       // ORIG T 0x084c2964
    bool SetRankRecord(CUser* user, SIG_LOAD_ETC* sig);      // ORIG T 0x084c2a98
    void _checkUserDisadvantage(CUser* user);                // ORIG T 0x084c2760
    static int _fnEnumQuestClear(unsigned int questIdx, char* ctx);// ORIG T 0x084c2b08（ORIG 汇编 0x8=questIdx 无 this，P88 static）
    void processAttendAnceEvent(SIG_ATTENDANCE_EVENT* sig, CUser& user);  // ORIG T 0x084e807e
    void processCheckInvalidExpPerLevel(CUser& user);        // ORIG T 0x084e84f6
    void processGrowthCreatureEvent(CUser& user);            // ORIG T 0x084e83c2
    void processGrowthWeaponEvent(CUser& user);              // ORIG T 0x084e834a
    void processSeriaRoomAniDecoEvent(seriaRoom_AniDeco info, CUser& user); // ORIG T 0x084e8428
    void verifyCharacVillage(CUser* user);                   // ORIG T 0x084e866c
};

class Inter_DeleteChannel : public ISigDispatcher
{
public:
    Inter_DeleteChannel() {}
    virtual ~Inter_DeleteChannel();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_Notice : public ISigDispatcher
{
public:
    Inter_Notice() {}
    virtual ~Inter_Notice();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_Terminate : public ISigDispatcher
{
public:
    Inter_Terminate() {}
    virtual ~Inter_Terminate();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_ReadyToTerminate : public ISigDispatcher
{
public:
    Inter_ReadyToTerminate() {}
    virtual ~Inter_ReadyToTerminate();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorBuddyInOut : public ISigDispatcher
{
public:
    Inter_MonitorBuddyInOut() {}
    virtual ~Inter_MonitorBuddyInOut();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorLoginOK : public ISigDispatcher
{
public:
    Inter_MonitorLoginOK() {}
    virtual ~Inter_MonitorLoginOK();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorCallUserInfo : public ISigDispatcher
{
public:
    Inter_MonitorCallUserInfo() {}
    virtual ~Inter_MonitorCallUserInfo();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_CutOffCallUserInfo : public ISigDispatcher
{
public:
    Inter_CutOffCallUserInfo() {}
    virtual ~Inter_CutOffCallUserInfo();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorUserGetout : public ISigDispatcher
{
public:
    Inter_MonitorUserGetout() {}
    virtual ~Inter_MonitorUserGetout();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_WebNoticeIngameAdvertisement : public ISigDispatcher
{
public:
    Inter_WebNoticeIngameAdvertisement() {}
    virtual ~Inter_WebNoticeIngameAdvertisement();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_ReloadIngameAdvertisement : public ISigDispatcher
{
public:
    Inter_ReloadIngameAdvertisement() {}
    virtual ~Inter_ReloadIngameAdvertisement();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_TcpServerConnect : public ISigDispatcher
{
public:
    Inter_TcpServerConnect() {}
    virtual ~Inter_TcpServerConnect();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorReplyBuddyList : public ISigDispatcher
{
public:
    Inter_MonitorReplyBuddyList() {}
    virtual ~Inter_MonitorReplyBuddyList();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorCeraUpdate : public ISigDispatcher
{
public:
    Inter_MonitorCeraUpdate() {}
    virtual ~Inter_MonitorCeraUpdate();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_IPGResult : public ISigDispatcher
{
public:
    Inter_IPGResult() {}
    virtual ~Inter_IPGResult();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_GenCeraTicket : public ISigDispatcher
{
public:
    Inter_GenCeraTicket() {}
    virtual ~Inter_GenCeraTicket();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadEventItem : public ISigDispatcher
{
public:
    Inter_LoadEventItem() {}
    virtual ~Inter_LoadEventItem();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorEventItemUpdate : public ISigDispatcher
{
public:
    Inter_MonitorEventItemUpdate() {}
    virtual ~Inter_MonitorEventItemUpdate();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorServerEventStart : public ISigDispatcher
{
public:
    Inter_MonitorServerEventStart() {}
    virtual ~Inter_MonitorServerEventStart();
    virtual int dispatch_sig(CUser* user, char* data, int len);
    // ORIG T _ZN29Inter_MonitorServerEventStart19SendEventNotiPacketEbssP5CUser
    // 0x084cf2de（静态语义：调用点无实例）。TimerEventChristmas 等调用。
    static void SendEventNotiPacket(bool noti, short eventIdx, short interval,
                                    CUser* user);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_FatigueResetDBReply : public ISigDispatcher
{
public:
    Inter_FatigueResetDBReply() {}
    virtual ~Inter_FatigueResetDBReply();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_SuicideWhenQueueIsEmpty : public ISigDispatcher
{
public:
    Inter_SuicideWhenQueueIsEmpty() {}
    virtual ~Inter_SuicideWhenQueueIsEmpty();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorNoticeGuildEnter : public ISigDispatcher
{
public:
    Inter_MonitorNoticeGuildEnter() {}
    virtual ~Inter_MonitorNoticeGuildEnter();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorNoticeGuildSecede : public ISigDispatcher
{
public:
    Inter_MonitorNoticeGuildSecede() {}
    virtual ~Inter_MonitorNoticeGuildSecede();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorNoticeGuildMarkChange : public ISigDispatcher
{
public:
    Inter_MonitorNoticeGuildMarkChange() {}
    virtual ~Inter_MonitorNoticeGuildMarkChange();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorNoticeGuildDismiss : public ISigDispatcher
{
public:
    Inter_MonitorNoticeGuildDismiss() {}
    virtual ~Inter_MonitorNoticeGuildDismiss();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorNoticeGuildChatMsg : public ISigDispatcher
{
public:
    Inter_MonitorNoticeGuildChatMsg() {}
    virtual ~Inter_MonitorNoticeGuildChatMsg();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorCallGuildMembers : public ISigDispatcher
{
public:
    Inter_MonitorCallGuildMembers() {}
    virtual ~Inter_MonitorCallGuildMembers();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorCallGuildMembersNext : public ISigDispatcher
{
public:
    Inter_MonitorCallGuildMembersNext() {}
    virtual ~Inter_MonitorCallGuildMembersNext();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorCallGuildAllMembers : public ISigDispatcher
{
public:
    Inter_MonitorCallGuildAllMembers() {}
    virtual ~Inter_MonitorCallGuildAllMembers();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorCallGuildAllMembersNext : public ISigDispatcher
{
public:
    Inter_MonitorCallGuildAllMembersNext() {}
    virtual ~Inter_MonitorCallGuildAllMembersNext();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorNoticeGuildNames : public ISigDispatcher
{
public:
    Inter_MonitorNoticeGuildNames() {}
    virtual ~Inter_MonitorNoticeGuildNames();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorNoticeGuildLoginOut : public ISigDispatcher
{
public:
    Inter_MonitorNoticeGuildLoginOut() {}
    virtual ~Inter_MonitorNoticeGuildLoginOut();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorNoticeOtherChannelChat : public ISigDispatcher
{
public:
    Inter_MonitorNoticeOtherChannelChat() {}
    virtual ~Inter_MonitorNoticeOtherChannelChat();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorCallGuildInfo : public ISigDispatcher
{
public:
    Inter_MonitorCallGuildInfo() {}
    virtual ~Inter_MonitorCallGuildInfo();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorNoticeGuildInfo : public ISigDispatcher
{
public:
    Inter_MonitorNoticeGuildInfo() {}
    virtual ~Inter_MonitorNoticeGuildInfo();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorSetGuildKey : public ISigDispatcher
{
public:
    Inter_MonitorSetGuildKey() {}
    virtual ~Inter_MonitorSetGuildKey();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorNoticeGuildCreate : public ISigDispatcher
{
public:
    Inter_MonitorNoticeGuildCreate() {}
    virtual ~Inter_MonitorNoticeGuildCreate();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorGuildMemberNamingReply : public ISigDispatcher
{
public:
    Inter_MonitorGuildMemberNamingReply() {}
    virtual ~Inter_MonitorGuildMemberNamingReply();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorSetSubGuildMasterReply : public ISigDispatcher
{
public:
    Inter_MonitorSetSubGuildMasterReply() {}
    virtual ~Inter_MonitorSetSubGuildMasterReply();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorNoticeGuildMemberInfo : public ISigDispatcher
{
public:
    Inter_MonitorNoticeGuildMemberInfo() {}
    virtual ~Inter_MonitorNoticeGuildMemberInfo();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorNoticeChangeGuildMemberGrade : public ISigDispatcher
{
public:
    Inter_MonitorNoticeChangeGuildMemberGrade() {}
    virtual ~Inter_MonitorNoticeChangeGuildMemberGrade();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_GuildNoticeGuildMessage : public ISigDispatcher
{
public:
    Inter_GuildNoticeGuildMessage() {}
    virtual ~Inter_GuildNoticeGuildMessage();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorNoticeMemberLoginOut : public ISigDispatcher
{
public:
    Inter_MonitorNoticeMemberLoginOut() {}
    virtual ~Inter_MonitorNoticeMemberLoginOut();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorNoticeRequestMemberEnterToRequester : public ISigDispatcher
{
public:
    Inter_MonitorNoticeRequestMemberEnterToRequester() {}
    virtual ~Inter_MonitorNoticeRequestMemberEnterToRequester();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorNoticeRequestMemberEnterToResponser : public ISigDispatcher
{
public:
    Inter_MonitorNoticeRequestMemberEnterToResponser() {}
    virtual ~Inter_MonitorNoticeRequestMemberEnterToResponser();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorNoticeMemberEnterOk : public ISigDispatcher
{
public:
    Inter_MonitorNoticeMemberEnterOk() {}
    virtual ~Inter_MonitorNoticeMemberEnterOk();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorNoticeMemberEnterReply : public ISigDispatcher
{
public:
    Inter_MonitorNoticeMemberEnterReply() {}
    virtual ~Inter_MonitorNoticeMemberEnterReply();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorNoticeMemberSecede : public ISigDispatcher
{
public:
    Inter_MonitorNoticeMemberSecede() {}
    virtual ~Inter_MonitorNoticeMemberSecede();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorNoticeMemberSecedeToSeceder : public ISigDispatcher
{
public:
    Inter_MonitorNoticeMemberSecedeToSeceder() {}
    virtual ~Inter_MonitorNoticeMemberSecedeToSeceder();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorNoticeDeleteMemberId : public ISigDispatcher
{
public:
    Inter_MonitorNoticeDeleteMemberId() {}
    virtual ~Inter_MonitorNoticeDeleteMemberId();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorNoticeMemberChatMsg : public ISigDispatcher
{
public:
    Inter_MonitorNoticeMemberChatMsg() {}
    virtual ~Inter_MonitorNoticeMemberChatMsg();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorCallMemberList : public ISigDispatcher
{
public:
    Inter_MonitorCallMemberList() {}
    virtual ~Inter_MonitorCallMemberList();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorPayTaxToUpper : public ISigDispatcher
{
public:
    Inter_MonitorPayTaxToUpper() {}
    virtual ~Inter_MonitorPayTaxToUpper();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadPrivateStore : public ISigDispatcher
{
public:
    Inter_LoadPrivateStore() {}
    virtual ~Inter_LoadPrivateStore();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_UpdateLogout : public ISigDispatcher
{
public:
    Inter_UpdateLogout() {}
    virtual ~Inter_UpdateLogout();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_UserRepel : public ISigDispatcher
{
public:
    Inter_UserRepel() {}
    virtual ~Inter_UserRepel();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_AvatarInsert : public ISigDispatcher
{
public:
    Inter_AvatarInsert() {}
    virtual ~Inter_AvatarInsert();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_AvatarChangeStat : public ISigDispatcher
{
public:
    Inter_AvatarChangeStat() {}
    virtual ~Inter_AvatarChangeStat();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_TriggerEventStart : public ISigDispatcher
{
public:
    Inter_TriggerEventStart() {}
    virtual ~Inter_TriggerEventStart();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_TriggerEventEnd : public ISigDispatcher
{
public:
    Inter_TriggerEventEnd() {}
    virtual ~Inter_TriggerEventEnd();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MailBox_Req_List : public ISigDispatcher
{
public:
    Inter_MailBox_Req_List() {}
    virtual ~Inter_MailBox_Req_List();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MailBox_Req_CheckReceiver : public ISigDispatcher
{
public:
    Inter_MailBox_Req_CheckReceiver() {}
    virtual ~Inter_MailBox_Req_CheckReceiver();
    virtual int dispatch_sig(CUser* user, char* data, int len);

    // ORIG T 0x084cff18 / 0x084d008a（InterWave1 批；定义属主未落地，U 引用合规）
    // b28c-l23 SIGFIX：int→bool（ORIG __PRETTY_FUNCTION__ 串 @0x08c8ce00 =
    // "bool Inter_MailBox_Req_CheckReceiver::SendDBThread_forchina(...)"，P174）。
    bool SendDBThread_forchina(CUser* user, SIG_MAILBOX_CHECK_RECEIVER* sig);
    void SendErrorPacket(CUser* user, ENUM_CMDPACKET cmd, unsigned short err);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MailBox_Req_Send : public ISigDispatcher
{
public:
    Inter_MailBox_Req_Send() {}
    virtual ~Inter_MailBox_Req_Send();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_Monitor_Notify_New_Mail : public ISigDispatcher
{
public:
    Inter_Monitor_Notify_New_Mail() {}
    virtual ~Inter_Monitor_Notify_New_Mail();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_InsertCreatureItemResult : public ISigDispatcher
{
public:
    Inter_InsertCreatureItemResult() {}
    virtual ~Inter_InsertCreatureItemResult();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_UpdateCreatureItemResult : public ISigDispatcher
{
public:
    Inter_UpdateCreatureItemResult() {}
    virtual ~Inter_UpdateCreatureItemResult();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorNoticeMessage : public ISigDispatcher
{
public:
    Inter_MonitorNoticeMessage() {}
    virtual ~Inter_MonitorNoticeMessage();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LastMsgResponse : public ISigDispatcher
{
public:
    Inter_LastMsgResponse() {}
    virtual ~Inter_LastMsgResponse();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorGuildPointDel : public ISigDispatcher
{
public:
    Inter_MonitorGuildPointDel() {}
    virtual ~Inter_MonitorGuildPointDel();
    virtual int dispatch_sig(CUser* user, char* data, int len);
    // ORIG 静态成员（与 dispatch 域同类：nm 同 mangled 类名实证）：
    //   getLastDeleteTime W 0x82a5a4c / setLastDeleteTime W 0x084eb100 /
    //   g_lastDeleteTime B 0x943dd88（定义 Inter_MonitorGuildPointDel.cpp）。
    static int getLastDeleteTime();
    static void setLastDeleteTime();
    static int g_lastDeleteTime;

private:
    // ORIG T 0x084ce6d2：_makeDefaultTime（月初 05:00 基准时间，7 月特例 6 日）。
    time_t _makeDefaultTime();
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorGuildWarInfo : public ISigDispatcher
{
public:
    Inter_MonitorGuildWarInfo() {}
    virtual ~Inter_MonitorGuildWarInfo();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_CodeChecksumRange : public ISigDispatcher
{
public:
    Inter_CodeChecksumRange() {}
    virtual ~Inter_CodeChecksumRange();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorForbidChat : public ISigDispatcher
{
public:
    Inter_MonitorForbidChat() {}
    virtual ~Inter_MonitorForbidChat();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MouseRegister : public ISigDispatcher
{
public:
    Inter_MouseRegister() {}
    virtual ~Inter_MouseRegister();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_Monitor_MegaphoneMsg : public ISigDispatcher
{
public:
    Inter_Monitor_MegaphoneMsg() {}
    virtual ~Inter_Monitor_MegaphoneMsg();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_Monitor_RegisterBlackListResult : public ISigDispatcher
{
public:
    Inter_Monitor_RegisterBlackListResult() {}
    virtual ~Inter_Monitor_RegisterBlackListResult();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_Monitor_DeleteBlackListResult : public ISigDispatcher
{
public:
    Inter_Monitor_DeleteBlackListResult() {}
    virtual ~Inter_Monitor_DeleteBlackListResult();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_Monitor_RequestBlackListResult : public ISigDispatcher
{
public:
    Inter_Monitor_RequestBlackListResult() {}
    virtual ~Inter_Monitor_RequestBlackListResult();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_DB_RequestBlackCount : public ISigDispatcher
{
public:
    Inter_DB_RequestBlackCount() {}
    virtual ~Inter_DB_RequestBlackCount();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorNoticeBlackList : public ISigDispatcher
{
public:
    Inter_MonitorNoticeBlackList() {}
    virtual ~Inter_MonitorNoticeBlackList();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_WebNoticeSingle : public ISigDispatcher
{
public:
    Inter_WebNoticeSingle() {}
    virtual ~Inter_WebNoticeSingle();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MemberExpLevelUp : public ISigDispatcher
{
public:
    Inter_MemberExpLevelUp() {}
    virtual ~Inter_MemberExpLevelUp();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorAddBuddy : public ISigDispatcher
{
public:
    Inter_MonitorAddBuddy() {}
    virtual ~Inter_MonitorAddBuddy();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorDelBuddy : public ISigDispatcher
{
public:
    Inter_MonitorDelBuddy() {}
    virtual ~Inter_MonitorDelBuddy();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_ChangeLetterStat : public ISigDispatcher
{
public:
    Inter_ChangeLetterStat() {}
    virtual ~Inter_ChangeLetterStat();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_SaveClientHackLog : public ISigDispatcher
{
public:
    Inter_SaveClientHackLog() {}
    virtual ~Inter_SaveClientHackLog();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_ReqChangeCharacName : public ISigDispatcher
{
public:
    Inter_ReqChangeCharacName() {}
    virtual ~Inter_ReqChangeCharacName();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_ReqQueryCharacInfo : public ISigDispatcher
{
public:
    Inter_ReqQueryCharacInfo() {}
    virtual ~Inter_ReqQueryCharacInfo();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_ReportMannerless : public ISigDispatcher
{
public:
    Inter_ReportMannerless() {}
    virtual ~Inter_ReportMannerless();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_ChagneUserHandicap : public ISigDispatcher
{
public:
    Inter_ChagneUserHandicap() {}
    virtual ~Inter_ChagneUserHandicap();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_GMRequestMid : public ISigDispatcher
{
public:
    Inter_GMRequestMid() {}
    virtual ~Inter_GMRequestMid();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_UDP_Incoming_Capcha_Data : public ISigDispatcher
{
public:
    Inter_UDP_Incoming_Capcha_Data() {}
    virtual ~Inter_UDP_Incoming_Capcha_Data();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_Add_Slang : public ISigDispatcher
{
public:
    Inter_Add_Slang() {}
    virtual ~Inter_Add_Slang();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_ReplyGuildLetter : public ISigDispatcher
{
public:
    Inter_ReplyGuildLetter() {}
    virtual ~Inter_ReplyGuildLetter();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_CallGuildInviteToCaller : public ISigDispatcher
{
public:
    Inter_CallGuildInviteToCaller() {}
    virtual ~Inter_CallGuildInviteToCaller();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_CallGuildInviteToInvited : public ISigDispatcher
{
public:
    Inter_CallGuildInviteToInvited() {}
    virtual ~Inter_CallGuildInviteToInvited();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_ReplyGuildInviteToInvited : public ISigDispatcher
{
public:
    Inter_ReplyGuildInviteToInvited() {}
    virtual ~Inter_ReplyGuildInviteToInvited();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_ReplyGuildInviteToCaller : public ISigDispatcher
{
public:
    Inter_ReplyGuildInviteToCaller() {}
    virtual ~Inter_ReplyGuildInviteToCaller();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadDeathTowerBestRecord : public ISigDispatcher
{
public:
    Inter_LoadDeathTowerBestRecord() {}
    virtual ~Inter_LoadDeathTowerBestRecord();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_DeathTowerLoadTopRanker : public ISigDispatcher
{
public:
    Inter_DeathTowerLoadTopRanker() {}
    virtual ~Inter_DeathTowerLoadTopRanker();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorDeathTowerRanking : public ISigDispatcher
{
public:
    Inter_MonitorDeathTowerRanking() {}
    virtual ~Inter_MonitorDeathTowerRanking();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorTowerReloadTopRanker : public ISigDispatcher
{
public:
    Inter_MonitorTowerReloadTopRanker() {}
    virtual ~Inter_MonitorTowerReloadTopRanker();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_ReplyGuildSecede : public ISigDispatcher
{
public:
    Inter_ReplyGuildSecede() {}
    virtual ~Inter_ReplyGuildSecede();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_NoticeGuildMasterDelegate : public ISigDispatcher
{
public:
    Inter_NoticeGuildMasterDelegate() {}
    virtual ~Inter_NoticeGuildMasterDelegate();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_GuildReplyGuildMasterDelegate : public ISigDispatcher
{
public:
    Inter_GuildReplyGuildMasterDelegate() {}
    virtual ~Inter_GuildReplyGuildMasterDelegate();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_NoticeHasBeenGuildMaster : public ISigDispatcher
{
public:
    Inter_NoticeHasBeenGuildMaster() {}
    virtual ~Inter_NoticeHasBeenGuildMaster();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_DBCheckDoubleGuildName : public ISigDispatcher
{
public:
    Inter_DBCheckDoubleGuildName() {}
    virtual ~Inter_DBCheckDoubleGuildName();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_DBCheckDoubleGuildAddress : public ISigDispatcher
{
public:
    Inter_DBCheckDoubleGuildAddress() {}
    virtual ~Inter_DBCheckDoubleGuildAddress();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_ReplyGuildCreate : public ISigDispatcher
{
public:
    Inter_ReplyGuildCreate() {}
    virtual ~Inter_ReplyGuildCreate();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_NoticeGuildNameOnGuildCreate : public ISigDispatcher
{
public:
    Inter_NoticeGuildNameOnGuildCreate() {}
    virtual ~Inter_NoticeGuildNameOnGuildCreate();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_SecurityCardIssue : public ISigDispatcher
{
public:
    Inter_SecurityCardIssue() {}
    virtual ~Inter_SecurityCardIssue();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_SecurityCardActivate : public ISigDispatcher
{
public:
    Inter_SecurityCardActivate() {}
    virtual ~Inter_SecurityCardActivate();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_SecurityCardRetransfer : public ISigDispatcher
{
public:
    Inter_SecurityCardRetransfer() {}
    virtual ~Inter_SecurityCardRetransfer();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadDnfHackLog : public ISigDispatcher
{
public:
    Inter_LoadDnfHackLog() {}
    virtual ~Inter_LoadDnfHackLog();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_AutoPunishRuleBlackIP : public ISigDispatcher
{
public:
    Inter_AutoPunishRuleBlackIP() {}
    virtual ~Inter_AutoPunishRuleBlackIP();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_AutoPunishRuleHackType : public ISigDispatcher
{
public:
    Inter_AutoPunishRuleHackType() {}
    virtual ~Inter_AutoPunishRuleHackType();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_ReplyPowerWarScore : public ISigDispatcher
{
public:
    Inter_ReplyPowerWarScore() {}
    virtual ~Inter_ReplyPowerWarScore();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_ReplyJoinPower : public ISigDispatcher
{
public:
    Inter_ReplyJoinPower() {}
    virtual ~Inter_ReplyJoinPower();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_SecedePower : public ISigDispatcher
{
public:
    Inter_SecedePower() {}
    virtual ~Inter_SecedePower();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_NoticePowerWarStartTime : public ISigDispatcher
{
public:
    Inter_NoticePowerWarStartTime() {}
    virtual ~Inter_NoticePowerWarStartTime();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_NoticePowerWarEndTime : public ISigDispatcher
{
public:
    Inter_NoticePowerWarEndTime() {}
    virtual ~Inter_NoticePowerWarEndTime();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_NoticePowerWarInfo : public ISigDispatcher
{
public:
    Inter_NoticePowerWarInfo() {}
    virtual ~Inter_NoticePowerWarInfo();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_NoticePowerWarProlong : public ISigDispatcher
{
public:
    Inter_NoticePowerWarProlong() {}
    virtual ~Inter_NoticePowerWarProlong();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_ChangeGuildName : public ISigDispatcher
{
public:
    Inter_ChangeGuildName() {}
    virtual ~Inter_ChangeGuildName();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_NoticeChangeGuildNameToMembers : public ISigDispatcher
{
public:
    Inter_NoticeChangeGuildNameToMembers() {}
    virtual ~Inter_NoticeChangeGuildNameToMembers();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_SendAllUserInfoMiniumForGuildSystem : public ISigDispatcher
{
public:
    Inter_SendAllUserInfoMiniumForGuildSystem() {}
    virtual ~Inter_SendAllUserInfoMiniumForGuildSystem();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadCleanPadPointReq : public ISigDispatcher
{
public:
    Inter_LoadCleanPadPointReq() {}
    virtual ~Inter_LoadCleanPadPointReq();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadCleanPadPoint : public ISigDispatcher
{
public:
    Inter_LoadCleanPadPoint() {}
    virtual ~Inter_LoadCleanPadPoint();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_NotifyAuctionMail : public ISigDispatcher
{
public:
    Inter_NotifyAuctionMail() {}
    virtual ~Inter_NotifyAuctionMail();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_AuctionResultAskPrice : public ISigDispatcher
{
public:
    Inter_AuctionResultAskPrice() {}
    virtual ~Inter_AuctionResultAskPrice();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_AuctionResultAskRegistedItemNum : public ISigDispatcher
{
public:
    Inter_AuctionResultAskRegistedItemNum() {}
    virtual ~Inter_AuctionResultAskRegistedItemNum();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_AuctionResultRegist : public ISigDispatcher
{
public:
    Inter_AuctionResultRegist() {}
    virtual ~Inter_AuctionResultRegist();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_AuctionResultBidding : public ISigDispatcher
{
public:
    Inter_AuctionResultBidding() {}
    virtual ~Inter_AuctionResultBidding();
    virtual int dispatch_sig(CUser* user, char* data, int len);
    // ORIG T 0x084d6cda _ZN26Inter_AuctionResultBidding19dispatch_sig_taiwanEP5CUserPci
    //   （台湾平台实现体归台湾批；声明 InterWave1 批）
    int dispatch_sig_taiwan(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_AuctionResultRegistCancel : public ISigDispatcher
{
public:
    Inter_AuctionResultRegistCancel() {}
    virtual ~Inter_AuctionResultRegistCancel();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_AuctionResultItemList : public ISigDispatcher
{
public:
    Inter_AuctionResultItemList() {}
    virtual ~Inter_AuctionResultItemList();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_AuctionResultMyRegistedItems : public ISigDispatcher
{
public:
    Inter_AuctionResultMyRegistedItems() {}
    virtual ~Inter_AuctionResultMyRegistedItems();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_AuctionResultMyBidding : public ISigDispatcher
{
public:
    Inter_AuctionResultMyBidding() {}
    virtual ~Inter_AuctionResultMyBidding();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_AuctionLogMessage : public ISigDispatcher
{
public:
    Inter_AuctionLogMessage() {}
    virtual ~Inter_AuctionLogMessage();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_AuctionNotifyAuctionService : public ISigDispatcher
{
public:
    Inter_AuctionNotifyAuctionService() {}
    virtual ~Inter_AuctionNotifyAuctionService();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_PointAuctionLogMessage : public ISigDispatcher
{
public:
    Inter_PointAuctionLogMessage() {}
    virtual ~Inter_PointAuctionLogMessage();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_PointAuctionChargeCeraPoint : public ISigDispatcher
{
public:
    Inter_PointAuctionChargeCeraPoint() {}
    virtual ~Inter_PointAuctionChargeCeraPoint();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_AuctionResultBuyItemApiece : public ISigDispatcher
{
public:
    Inter_AuctionResultBuyItemApiece() {}
    virtual ~Inter_AuctionResultBuyItemApiece();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadGameOption : public ISigDispatcher
{
public:
    Inter_LoadGameOption() {}
    virtual ~Inter_LoadGameOption();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_PvPChannelInfo : public ISigDispatcher
{
public:
    Inter_PvPChannelInfo() {}
    virtual ~Inter_PvPChannelInfo();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_PvPChannelUserCount : public ISigDispatcher
{
public:
    Inter_PvPChannelUserCount() {}
    virtual ~Inter_PvPChannelUserCount();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadBloodDungeonBestRecord : public ISigDispatcher
{
public:
    Inter_LoadBloodDungeonBestRecord() {}
    virtual ~Inter_LoadBloodDungeonBestRecord();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadBreakAwayPreventSystem : public ISigDispatcher
{
public:
    Inter_LoadBreakAwayPreventSystem() {}
    virtual ~Inter_LoadBreakAwayPreventSystem();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadBreakAwayPreventMessage : public ISigDispatcher
{
public:
    Inter_LoadBreakAwayPreventMessage() {}
    virtual ~Inter_LoadBreakAwayPreventMessage();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadDimensionInout : public ISigDispatcher
{
public:
    Inter_LoadDimensionInout() {}
    virtual ~Inter_LoadDimensionInout();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_BlackIPMonitorReq : public ISigDispatcher
{
public:
    Inter_BlackIPMonitorReq() {}
    virtual ~Inter_BlackIPMonitorReq();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_BlackIPMonitor : public ISigDispatcher
{
public:
    Inter_BlackIPMonitor() {}
    virtual ~Inter_BlackIPMonitor();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_BlackIPMonitorPartLoadReq : public ISigDispatcher
{
public:
    Inter_BlackIPMonitorPartLoadReq() {}
    virtual ~Inter_BlackIPMonitorPartLoadReq();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_BlackIPMonitorDeleteIP : public ISigDispatcher
{
public:
    Inter_BlackIPMonitorDeleteIP() {}
    virtual ~Inter_BlackIPMonitorDeleteIP();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadAutoMarketConditionsControl : public ISigDispatcher
{
public:
    Inter_LoadAutoMarketConditionsControl() {}
    virtual ~Inter_LoadAutoMarketConditionsControl();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_UpdateOneDayLethe : public ISigDispatcher
{
public:
    Inter_UpdateOneDayLethe() {}
    virtual ~Inter_UpdateOneDayLethe();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_DisconnectMonitorServer : public ISigDispatcher
{
public:
    Inter_DisconnectMonitorServer() {}
    virtual ~Inter_DisconnectMonitorServer();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_ResposncePCRoomPlyerList : public ISigDispatcher
{
public:
    Inter_ResposncePCRoomPlyerList() {}
    virtual ~Inter_ResposncePCRoomPlyerList();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_ResposncePCRoomPlyerCount : public ISigDispatcher
{
public:
    Inter_ResposncePCRoomPlyerCount() {}
    virtual ~Inter_ResposncePCRoomPlyerCount();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_NoticePCRoomPlayerLoginOut : public ISigDispatcher
{
public:
    Inter_NoticePCRoomPlayerLoginOut() {}
    virtual ~Inter_NoticePCRoomPlayerLoginOut();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_NoticePowerWarRank : public ISigDispatcher
{
public:
    Inter_NoticePowerWarRank() {}
    virtual ~Inter_NoticePowerWarRank();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadPowerWarStatueRanker : public ISigDispatcher
{
public:
    Inter_LoadPowerWarStatueRanker() {}
    virtual ~Inter_LoadPowerWarStatueRanker();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadPowerWarStatueInfo : public ISigDispatcher
{
public:
    Inter_LoadPowerWarStatueInfo() {}
    virtual ~Inter_LoadPowerWarStatueInfo();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_UpdateServerMessage : public ISigDispatcher
{
public:
    Inter_UpdateServerMessage() {}
    virtual ~Inter_UpdateServerMessage();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadServerMessage : public ISigDispatcher
{
public:
    Inter_LoadServerMessage() {}
    virtual ~Inter_LoadServerMessage();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_NoticeServerMessageInfo : public ISigDispatcher
{
public:
    Inter_NoticeServerMessageInfo() {}
    virtual ~Inter_NoticeServerMessageInfo();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_NoticeGuildMemo : public ISigDispatcher
{
public:
    Inter_NoticeGuildMemo() {}
    virtual ~Inter_NoticeGuildMemo();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_PowerWarProcessInfo : public ISigDispatcher
{
public:
    Inter_PowerWarProcessInfo() {}
    virtual ~Inter_PowerWarProcessInfo();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_PowerWarEndInfo : public ISigDispatcher
{
public:
    Inter_PowerWarEndInfo() {}
    virtual ~Inter_PowerWarEndInfo();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_NoticeCreateGuildAgit : public ISigDispatcher
{
public:
    Inter_NoticeCreateGuildAgit() {}
    virtual ~Inter_NoticeCreateGuildAgit();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_NoticeDeleteGuildAgit : public ISigDispatcher
{
public:
    Inter_NoticeDeleteGuildAgit() {}
    virtual ~Inter_NoticeDeleteGuildAgit();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_NoticeLoadGuildAgit : public ISigDispatcher
{
public:
    Inter_NoticeLoadGuildAgit() {}
    virtual ~Inter_NoticeLoadGuildAgit();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadExpertJob : public ISigDispatcher
{
public:
    Inter_LoadExpertJob() {}
    virtual ~Inter_LoadExpertJob();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorPunishUserReq : public ISigDispatcher
{
public:
    Inter_MonitorPunishUserReq() {}
    virtual ~Inter_MonitorPunishUserReq();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_PcroomResponse : public ISigDispatcher
{
public:
    Inter_PcroomResponse() {}
    virtual ~Inter_PcroomResponse();
    virtual int dispatch_sig(CUser* user, char* data, int len);
    bool IsShutdonTimeOverLogin();

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_ItemLimitEdition_Sell_Start : public ISigDispatcher
{
public:
    Inter_ItemLimitEdition_Sell_Start() {}
    virtual ~Inter_ItemLimitEdition_Sell_Start();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_ItemLimitEdition_Sell_End : public ISigDispatcher
{
public:
    Inter_ItemLimitEdition_Sell_End() {}
    virtual ~Inter_ItemLimitEdition_Sell_End();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_ItemLimitEdition_Update : public ISigDispatcher
{
public:
    Inter_ItemLimitEdition_Update() {}
    virtual ~Inter_ItemLimitEdition_Update();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_ItemLimitEdition_Reply : public ISigDispatcher
{
public:
    Inter_ItemLimitEdition_Reply() {}
    virtual ~Inter_ItemLimitEdition_Reply();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_FindFactoryHub : public ISigDispatcher
{
public:
    Inter_FindFactoryHub() {}
    virtual ~Inter_FindFactoryHub();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_GuildExpLimit : public ISigDispatcher
{
public:
    Inter_GuildExpLimit() {}
    virtual ~Inter_GuildExpLimit();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_SetIPCount : public ISigDispatcher
{
public:
    Inter_SetIPCount() {}
    virtual ~Inter_SetIPCount();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_SetFullIPCount : public ISigDispatcher
{
public:
    Inter_SetFullIPCount() {}
    virtual ~Inter_SetFullIPCount();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_CancelRestingUser : public ISigDispatcher
{
public:
    Inter_CancelRestingUser() {}
    virtual ~Inter_CancelRestingUser();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_ApplyTempPowerSide : public ISigDispatcher
{
public:
    Inter_ApplyTempPowerSide() {}
    virtual ~Inter_ApplyTempPowerSide();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_ReloadPowerWarRanker : public ISigDispatcher
{
public:
    Inter_ReloadPowerWarRanker() {}
    virtual ~Inter_ReloadPowerWarRanker();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadPowerWarInfo : public ISigDispatcher
{
public:
    Inter_LoadPowerWarInfo() {}
    virtual ~Inter_LoadPowerWarInfo();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_SetFactoryHubUser : public ISigDispatcher
{
public:
    Inter_SetFactoryHubUser() {}
    virtual ~Inter_SetFactoryHubUser();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_SetFactoryHubUserNotConn : public ISigDispatcher
{
public:
    Inter_SetFactoryHubUserNotConn() {}
    virtual ~Inter_SetFactoryHubUserNotConn();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_Set_CleanPad_Point : public ISigDispatcher
{
public:
    Inter_Set_CleanPad_Point() {}
    virtual ~Inter_Set_CleanPad_Point();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadNoCacheData : public ISigDispatcher
{
public:
    Inter_LoadNoCacheData() {}
    virtual ~Inter_LoadNoCacheData();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_GuildCargo : public ISigDispatcher
{
public:
    Inter_GuildCargo() {}
    virtual ~Inter_GuildCargo();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_NoticeGuildCargo : public ISigDispatcher
{
public:
    Inter_NoticeGuildCargo() {}
    virtual ~Inter_NoticeGuildCargo();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_GuildCargoHistory : public ISigDispatcher
{
public:
    Inter_GuildCargoHistory() {}
    virtual ~Inter_GuildCargoHistory();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_GuildCargoCheckPushItem : public ISigDispatcher
{
public:
    Inter_GuildCargoCheckPushItem() {}
    virtual ~Inter_GuildCargoCheckPushItem();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_GuildCargoPushItem : public ISigDispatcher
{
public:
    Inter_GuildCargoPushItem() {}
    virtual ~Inter_GuildCargoPushItem();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_GuildCargoPopItem : public ISigDispatcher
{
public:
    Inter_GuildCargoPopItem() {}
    virtual ~Inter_GuildCargoPopItem();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_GuildCargoMoveItem : public ISigDispatcher
{
public:
    Inter_GuildCargoMoveItem() {}
    virtual ~Inter_GuildCargoMoveItem();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_GuildCargoUpgrade : public ISigDispatcher
{
public:
    Inter_GuildCargoUpgrade() {}
    virtual ~Inter_GuildCargoUpgrade();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_GuildBoardData : public ISigDispatcher
{
public:
    Inter_GuildBoardData() {}
    virtual ~Inter_GuildBoardData();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_GuildBoardDelete : public ISigDispatcher
{
public:
    Inter_GuildBoardDelete() {}
    virtual ~Inter_GuildBoardDelete();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_TakeScreenShot : public ISigDispatcher
{
public:
    Inter_TakeScreenShot() {}
    virtual ~Inter_TakeScreenShot();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_VillageAttackedStart : public ISigDispatcher
{
public:
    Inter_VillageAttackedStart() {}
    virtual ~Inter_VillageAttackedStart();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_VillageAttackedEnd : public ISigDispatcher
{
public:
    Inter_VillageAttackedEnd() {}
    virtual ~Inter_VillageAttackedEnd();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_VillageAttackedCountdown : public ISigDispatcher
{
public:
    Inter_VillageAttackedCountdown() {}
    virtual ~Inter_VillageAttackedCountdown();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_VillageAttackedUpdate : public ISigDispatcher
{
public:
    Inter_VillageAttackedUpdate() {}
    virtual ~Inter_VillageAttackedUpdate();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_VillageAttackedScore : public ISigDispatcher
{
public:
    Inter_VillageAttackedScore() {}
    virtual ~Inter_VillageAttackedScore();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_VillageAttackedReward : public ISigDispatcher
{
public:
    Inter_VillageAttackedReward() {}
    virtual ~Inter_VillageAttackedReward();
    virtual int dispatch_sig(CUser* user, char* data, int len);

    // ORIG T 0x084e0390 _ZN27Inter_VillageAttackedReward17_send_reward_itemEP5CUsermii
    //   （m=itemIdx，i1=count→create_item param，i2=win/rank；InterWave1 W0 批）
    void _send_reward_item(CUser* user, unsigned long itemIdx, int count, int rank);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_VillageAttackedRewardServer : public ISigDispatcher
{
public:
    Inter_VillageAttackedRewardServer() {}
    virtual ~Inter_VillageAttackedRewardServer();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_FullLevelBroadCast : public ISigDispatcher
{
public:
    Inter_FullLevelBroadCast() {}
    virtual ~Inter_FullLevelBroadCast();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_Hack_CheckMyAccount : public ISigDispatcher
{
public:
    Inter_Hack_CheckMyAccount() {}
    virtual ~Inter_Hack_CheckMyAccount();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_SetSecounUserData : public ISigDispatcher
{
public:
    Inter_SetSecounUserData() {}
    virtual ~Inter_SetSecounUserData();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_SetARSInfo : public ISigDispatcher
{
public:
    Inter_SetARSInfo() {}
    virtual ~Inter_SetARSInfo();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_OnTimeEventRewardStart : public ISigDispatcher
{
public:
    Inter_OnTimeEventRewardStart() {}
    virtual ~Inter_OnTimeEventRewardStart();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_OnTimeEventRewardEnd : public ISigDispatcher
{
public:
    Inter_OnTimeEventRewardEnd() {}
    virtual ~Inter_OnTimeEventRewardEnd();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadPvPBuddy : public ISigDispatcher
{
public:
    Inter_LoadPvPBuddy() {}
    virtual ~Inter_LoadPvPBuddy();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_PvPBuddyConnList : public ISigDispatcher
{
public:
    Inter_PvPBuddyConnList() {}
    virtual ~Inter_PvPBuddyConnList();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_ReqAddPvPBuddy : public ISigDispatcher
{
public:
    Inter_ReqAddPvPBuddy() {}
    virtual ~Inter_ReqAddPvPBuddy();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_AddPvPBuddyResult : public ISigDispatcher
{
public:
    Inter_AddPvPBuddyResult() {}
    virtual ~Inter_AddPvPBuddyResult();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_RemovePvPBuddyResult : public ISigDispatcher
{
public:
    Inter_RemovePvPBuddyResult() {}
    virtual ~Inter_RemovePvPBuddyResult();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_NoticePvPBuddyInOut : public ISigDispatcher
{
public:
    Inter_NoticePvPBuddyInOut() {}
    virtual ~Inter_NoticePvPBuddyInOut();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadPvPLiveEventData : public ISigDispatcher
{
public:
    Inter_LoadPvPLiveEventData() {}
    virtual ~Inter_LoadPvPLiveEventData();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_Antibot_SendBufferClient : public ISigDispatcher
{
public:
    Inter_Antibot_SendBufferClient() {}
    virtual ~Inter_Antibot_SendBufferClient();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_Antibot_PunishClient : public ISigDispatcher
{
public:
    Inter_Antibot_PunishClient() {}
    virtual ~Inter_Antibot_PunishClient();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_Antibot_ProcessProtectMsg : public ISigDispatcher
{
public:
    Inter_Antibot_ProcessProtectMsg() {}
    virtual ~Inter_Antibot_ProcessProtectMsg();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadOnlinePreliminaryMember : public ISigDispatcher
{
public:
    Inter_LoadOnlinePreliminaryMember() {}
    virtual ~Inter_LoadOnlinePreliminaryMember();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadOnlinePreliminaryTeam : public ISigDispatcher
{
public:
    Inter_LoadOnlinePreliminaryTeam() {}
    virtual ~Inter_LoadOnlinePreliminaryTeam();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadOnlinePreliminaryMemberMatchList : public ISigDispatcher
{
public:
    Inter_LoadOnlinePreliminaryMemberMatchList() {}
    virtual ~Inter_LoadOnlinePreliminaryMemberMatchList();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadOnlinePreliminaryTeamMatchList : public ISigDispatcher
{
public:
    Inter_LoadOnlinePreliminaryTeamMatchList() {}
    virtual ~Inter_LoadOnlinePreliminaryTeamMatchList();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadCharacLinkBonus : public ISigDispatcher
{
public:
    Inter_LoadCharacLinkBonus() {}
    virtual ~Inter_LoadCharacLinkBonus();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_CreateAccountCargo : public ISigDispatcher
{
public:
    Inter_CreateAccountCargo() {}
    virtual ~Inter_CreateAccountCargo();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_UpgradeAccountCargo : public ISigDispatcher
{
public:
    Inter_UpgradeAccountCargo() {}
    virtual ~Inter_UpgradeAccountCargo();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadAccountCargo : public ISigDispatcher
{
public:
    Inter_LoadAccountCargo() {}
    virtual ~Inter_LoadAccountCargo();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MultiMailBoxReqSend : public ISigDispatcher
{
public:
    Inter_MultiMailBoxReqSend() {}
    virtual ~Inter_MultiMailBoxReqSend();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_ReqQueryCharacInfoMailbox : public ISigDispatcher
{
public:
    Inter_ReqQueryCharacInfoMailbox() {}
    virtual ~Inter_ReqQueryCharacInfoMailbox();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_ReqGameServerInfo : public ISigDispatcher
{
public:
    Inter_ReqGameServerInfo() {}
    virtual ~Inter_ReqGameServerInfo();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorRegistGameServerAck : public ISigDispatcher
{
public:
    Inter_MonitorRegistGameServerAck() {}
    virtual ~Inter_MonitorRegistGameServerAck();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadMileage : public ISigDispatcher
{
public:
    Inter_LoadMileage() {}
    virtual ~Inter_LoadMileage();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_GuildExpBookDelete : public ISigDispatcher
{
public:
    Inter_GuildExpBookDelete() {}
    virtual ~Inter_GuildExpBookDelete();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_NoCache : public ISigDispatcher
{
public:
    Inter_NoCache() {}
    virtual ~Inter_NoCache();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_Send_Charac_no_from_monitor : public ISigDispatcher
{
public:
    Inter_Send_Charac_no_from_monitor() {}
    virtual ~Inter_Send_Charac_no_from_monitor();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_SendPeriodicMessage : public ISigDispatcher
{
public:
    Inter_SendPeriodicMessage() {}
    virtual ~Inter_SendPeriodicMessage();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_SendTimeSync : public ISigDispatcher
{
public:
    Inter_SendTimeSync() {}
    virtual ~Inter_SendTimeSync();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_SendTimeSyncLogin : public ISigDispatcher
{
public:
    Inter_SendTimeSyncLogin() {}
    virtual ~Inter_SendTimeSyncLogin();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_AddGuildFundResult : public ISigDispatcher
{
public:
    Inter_AddGuildFundResult() {}
    virtual ~Inter_AddGuildFundResult();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_TodayGuildMember : public ISigDispatcher
{
public:
    Inter_TodayGuildMember() {}
    virtual ~Inter_TodayGuildMember();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_CheckJoinGuild : public ISigDispatcher
{
public:
    Inter_CheckJoinGuild() {}
    virtual ~Inter_CheckJoinGuild();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_RequestJoinGuild : public ISigDispatcher
{
public:
    Inter_RequestJoinGuild() {}
    virtual ~Inter_RequestJoinGuild();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_JoinGuildInfo : public ISigDispatcher
{
public:
    Inter_JoinGuildInfo() {}
    virtual ~Inter_JoinGuildInfo();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_ApproveJoinGuild : public ISigDispatcher
{
public:
    Inter_ApproveJoinGuild() {}
    virtual ~Inter_ApproveJoinGuild();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_GuildJoinList : public ISigDispatcher
{
public:
    Inter_GuildJoinList() {}
    virtual ~Inter_GuildJoinList();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_GuildAttendanceInfo : public ISigDispatcher
{
public:
    Inter_GuildAttendanceInfo() {}
    virtual ~Inter_GuildAttendanceInfo();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_AchieveGuildAttendance : public ISigDispatcher
{
public:
    Inter_AchieveGuildAttendance() {}
    virtual ~Inter_AchieveGuildAttendance();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_UpdatePurchaseCashItemBonus : public ISigDispatcher
{
public:
    Inter_UpdatePurchaseCashItemBonus() {}
    virtual ~Inter_UpdatePurchaseCashItemBonus();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_HadesDisConnect : public ISigDispatcher
{
public:
    Inter_HadesDisConnect() {}
    virtual ~Inter_HadesDisConnect();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_GoblinPadChagePwdReward : public ISigDispatcher
{
public:
    Inter_GoblinPadChagePwdReward() {}
    virtual ~Inter_GoblinPadChagePwdReward();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_GoblinPadSamePwdCheck : public ISigDispatcher
{
public:
    Inter_GoblinPadSamePwdCheck() {}
    virtual ~Inter_GoblinPadSamePwdCheck();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_PunishCancel : public ISigDispatcher
{
public:
    Inter_PunishCancel() {}
    virtual ~Inter_PunishCancel();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_BroadcastMsg : public ISigDispatcher
{
public:
    Inter_BroadcastMsg() {}
    virtual ~Inter_BroadcastMsg();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_TOD_ReturnToMonitor_APCSettingComplete : public ISigDispatcher
{
public:
    Inter_TOD_ReturnToMonitor_APCSettingComplete() {}
    virtual ~Inter_TOD_ReturnToMonitor_APCSettingComplete();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_TOD_Reload_APC : public ISigDispatcher
{
public:
    Inter_TOD_Reload_APC() {}
    virtual ~Inter_TOD_Reload_APC();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_UpdateSecuServiceState : public ISigDispatcher
{
public:
    Inter_UpdateSecuServiceState() {}
    virtual ~Inter_UpdateSecuServiceState();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadKillMonsterInfo : public ISigDispatcher
{
public:
    Inter_LoadKillMonsterInfo() {}
    virtual ~Inter_LoadKillMonsterInfo();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LevelUpReward : public ISigDispatcher
{
public:
    Inter_LevelUpReward() {}
    virtual ~Inter_LevelUpReward();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_GetUserRegdate : public ISigDispatcher
{
public:
    Inter_GetUserRegdate() {}
    virtual ~Inter_GetUserRegdate();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_GoldControlMoneyActionCancel : public ISigDispatcher
{
public:
    Inter_GoldControlMoneyActionCancel() {}
    virtual ~Inter_GoldControlMoneyActionCancel();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorNoticeGuildChatMsgHyperLink : public ISigDispatcher
{
public:
    Inter_MonitorNoticeGuildChatMsgHyperLink() {}
    virtual ~Inter_MonitorNoticeGuildChatMsgHyperLink();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorNoticeMemberChatMsgHyperLink : public ISigDispatcher
{
public:
    Inter_MonitorNoticeMemberChatMsgHyperLink() {}
    virtual ~Inter_MonitorNoticeMemberChatMsgHyperLink();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorNoticeOtherChannelChatHyperLink : public ISigDispatcher
{
public:
    Inter_MonitorNoticeOtherChannelChatHyperLink() {}
    virtual ~Inter_MonitorNoticeOtherChannelChatHyperLink();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_Monitor_MegaphoneMsgHyperLink : public ISigDispatcher
{
public:
    Inter_Monitor_MegaphoneMsgHyperLink() {}
    virtual ~Inter_Monitor_MegaphoneMsgHyperLink();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_SendSecuReward : public ISigDispatcher
{
public:
    Inter_SendSecuReward() {}
    virtual ~Inter_SendSecuReward();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_HadesAction : public ISigDispatcher
{
public:
    Inter_HadesAction() {}
    virtual ~Inter_HadesAction();
    virtual int dispatch_sig(CUser* user, char* data, int len);

    // ORIG W 0x084e5e62 _ZN17Inter_HadesAction10send_alertEP5CUserj（InterWave1 W5 批）
    void send_alert(CUser* user, unsigned int code);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_Check3rdPartyConcent : public ISigDispatcher
{
public:
    Inter_Check3rdPartyConcent() {}
    virtual ~Inter_Check3rdPartyConcent();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_SelectCharacMid : public ISigDispatcher
{
public:
    Inter_SelectCharacMid() {}
    virtual ~Inter_SelectCharacMid();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

// ORIG 对象尺寸 0x98（ctor 处 operator new(0x98)，0x84bbafa+0xb50）。
// 布局（ORIG ctor/reset/checkError 汇编逐字段）：vptr@0、m_error@4、
// m_needItem pair<int,int>@8（pair::pair 由 ORIG ctor 显式调用）、
// m_param1@0x10=-1、m_param2@0x14=-1、m_itemRef@0x18、m_hasData@0x1c、
// 两成员 Inven_Item@0x1d/0x5a（Inven_Item 为 pack(1)，对齐 1 → 自然落位）。
class Inven_Item;


class Inter_NpcLimitBuyItemInfo : public ISigDispatcher
{
public:
    Inter_NpcLimitBuyItemInfo();
    virtual ~Inter_NpcLimitBuyItemInfo();
    virtual int dispatch_sig(CUser* user, char* data, int len);
    int checkError(CUser* user, LimitNpcBuyItemInfo* info);   // ORIG 返回 int 被 dispatch 消费
    // [b28c-32] P216：ORIG C1 0x084e6502 无 EH landing（reset 体仅标量赋值 + 两个
    // Inven_Item::reset）；4.4 对可见 reset() 判可能抛而布 ~ISigDispatcher cleanup。
    // reset 声明+定义加 nothrow 后 4.4 判 nothrow ⇒ cleanup 链与 ORIG 同形。
    void reset() throw();
    void sendPacket(CUser* user);

private:
    int m_error;                       // +0x04
    std::pair<int, int> m_needItem;    // +0x08 (need item idx / need count)
    int m_param1;                      // +0x10 reset 写 -1
    int m_param2;                      // +0x14 reset 写 -1
    int m_itemRef;                     // +0x18 CItem*（find_item 结果）
    unsigned char m_data1c;            // +0x1c
    Inven_Item m_invenItem1;           // +0x1d
    Inven_Item m_invenItem2;           // +0x5a
};


class Inter_NpcLimitBuyItemInfoAll : public ISigDispatcher
{
public:
    Inter_NpcLimitBuyItemInfoAll() {}
    virtual ~Inter_NpcLimitBuyItemInfoAll();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_NpcLimitBuyItemChangeInfo : public ISigDispatcher
{
public:
    Inter_NpcLimitBuyItemChangeInfo() {}
    virtual ~Inter_NpcLimitBuyItemChangeInfo();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_NpcLimitBuyItemInfoRenew : public ISigDispatcher
{
public:
    Inter_NpcLimitBuyItemInfoRenew() {}
    virtual ~Inter_NpcLimitBuyItemInfoRenew();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_MonitorReplyCharacInfo : public ISigDispatcher
{
public:
    Inter_MonitorReplyCharacInfo() {}
    virtual ~Inter_MonitorReplyCharacInfo();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_CeraShopPurchaseCount : public ISigDispatcher
{
public:
    Inter_CeraShopPurchaseCount() {}
    virtual ~Inter_CeraShopPurchaseCount();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_OwnerisVIP : public ISigDispatcher
{
public:
    Inter_OwnerisVIP() {}
    virtual ~Inter_OwnerisVIP();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadPuUserCheck : public ISigDispatcher
{
public:
    Inter_LoadPuUserCheck() {}
    virtual ~Inter_LoadPuUserCheck();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_CollectItemsResult : public ISigDispatcher
{
public:
    Inter_CollectItemsResult() {}
    virtual ~Inter_CollectItemsResult();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_CollectItemsReward : public ISigDispatcher
{
public:
    Inter_CollectItemsReward() {}
    virtual ~Inter_CollectItemsReward();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_CollectItemsRewardBroadcast : public ISigDispatcher
{
public:
    Inter_CollectItemsRewardBroadcast() {}
    virtual ~Inter_CollectItemsRewardBroadcast();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_RequestCharacSkillInfo : public ISigDispatcher
{
public:
    Inter_RequestCharacSkillInfo() {}
    virtual ~Inter_RequestCharacSkillInfo();
    virtual int dispatch_sig(CUser* user, char* data, int len);

    // ORIG T 0x084e7820 _ZN28Inter_RequestCharacSkillInfo21_PutSkillInfoToCharacEcPK15_Mastered_skillP9SkillSlot
    //   （InterWave1 W5 批）
    void _PutSkillInfoToCharac(char slotFlag, const _Mastered_skill* skill,
                               SkillSlot* skillSlot);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_RequestTagCharacInfo : public ISigDispatcher
{
public:
    Inter_RequestTagCharacInfo() {}
    virtual ~Inter_RequestTagCharacInfo();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_EmergencyPatchMessage : public ISigDispatcher
{
public:
    Inter_EmergencyPatchMessage() {}
    virtual ~Inter_EmergencyPatchMessage();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_AuctionSubMoney : public ISigDispatcher
{
public:
    Inter_AuctionSubMoney() {}
    virtual ~Inter_AuctionSubMoney();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_UpdateServerSnapShot : public ISigDispatcher
{
public:
    Inter_UpdateServerSnapShot() {}
    virtual ~Inter_UpdateServerSnapShot();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadCharacTypeHotKey : public ISigDispatcher
{
public:
    Inter_LoadCharacTypeHotKey() {}
    virtual ~Inter_LoadCharacTypeHotKey();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_UpdateMiniCraneSeed : public ISigDispatcher
{
public:
    Inter_UpdateMiniCraneSeed() {}
    virtual ~Inter_UpdateMiniCraneSeed();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_GetPremiumSelectedCubeInfo : public ISigDispatcher
{
public:
    Inter_GetPremiumSelectedCubeInfo() {}
    virtual ~Inter_GetPremiumSelectedCubeInfo();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_CheckAddFriend : public ISigDispatcher
{
public:
    Inter_CheckAddFriend() {}
    virtual ~Inter_CheckAddFriend();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadCerashopAddRestrictType : public ISigDispatcher
{
public:
    Inter_LoadCerashopAddRestrictType() {}
    virtual ~Inter_LoadCerashopAddRestrictType();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

// ORIG ctor 调用点为「operator new 后 movl $0x0,(%eax) 再 call C1」的值初始化
// 形态（0x84bbafa+0x2d32/+0x2d3e）：类无用户声明构造/析构，new X() 触发
// 零初始化 + 隐式 C1（仅置 vptr）。故此处不得声明显式 ctor/dtor。
class Inter_LoadEventLevelupSupport : public ISigDispatcher
{
public:
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_SelectAllowProxyUser : public ISigDispatcher
{
public:
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

namespace APSystem
{
class Inter_LoadActionPoint : public ISigDispatcher
{
public:
    Inter_LoadActionPoint() {}
    virtual ~Inter_LoadActionPoint();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_EmptyActionPoint : public ISigDispatcher
{
public:
    Inter_EmptyActionPoint() {}
    virtual ~Inter_EmptyActionPoint();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

class Inter_LoadRewardMedal : public ISigDispatcher
{
public:
    Inter_LoadRewardMedal() {}
    virtual ~Inter_LoadRewardMedal();
    virtual int dispatch_sig(CUser* user, char* data, int len);
    bool SendMail(CUser& user, unsigned int medalCount);   // ORIG T 0x08124ea0

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

}  // namespace APSystem

namespace advancealtar
{
class Inter_Load : public ISigDispatcher
{
public:
    Inter_Load() {}
    virtual ~Inter_Load();
    virtual int dispatch_sig(CUser* user, char* data, int len);

private:
    // ORIG 对象尺寸 4：仅 vptr，无数据成员。
};

}  // namespace advancealtar

class InterDispatcher
{
public:
    InterDispatcher();
    ~InterDispatcher();
    ISigDispatcher* get_dispatcher(int code);              // ORIG T 0x84be906
    void register_dispatcher(int code, ISigDispatcher* d); // ORIG T 0x84be95e
    bool dispatch(int code, int userId, char* data, int len);  // ORIG T 0x84be970

private:
    // 注册表：下标即分发码；ORIG 构造按槽位直接写入。
    ISigDispatcher* m_dispatchers[852];
};

static_assert(sizeof(InterDispatcher) == 0xd50, "InterDispatcher layout mismatch");

#endif  // DNF_GAME_INTER_DISPATCHER_H_
