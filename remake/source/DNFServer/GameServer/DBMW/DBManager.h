#ifndef DBMW_DBMANAGER_H_
#define DBMW_DBMANAGER_H_

#include "DBMWCommon.h"


class CDBManager
{
public:
    CDBManager();
    ~CDBManager();
    char Open(ENUM_DB_HANDLE_IDX idx, const char* host, unsigned int port,
              const char* user, const char* pass, const char* db);
    void Init(ENUM_DB_KIND kind, CApplication* app);
    void Close();
    CDBHandle* GetDBHandle(ENUM_DB_HANDLE_IDX idx);
    char UpdateQueryCount(unsigned int idx, int count, int time);
    char SelectTest();
    char QueryGuildWarPointList(int guildWarPoint, CGuildManager* gm);
    char AwardGuildTitleByMail(int guildId, unsigned int characNo, unsigned int itemId,
                               char* guildName, unsigned int item);
    char RegisterToBlackList(unsigned int m_id, unsigned int characNo, char* characName);
    char GuildSecede(Packet_DB_Request_Guild_Secede* req, unsigned int& characNo,
                     unsigned int& m_id, unsigned int& result);
    char QueryGuildCreate(Packet_DBMW_Request_Guild_Create* req,
                          unsigned int& guildId, unsigned int& result);
    char QueryMember(unsigned int characNo, Packet_DB_Reply_Query_Member& reply);
    int FindCharProxyInArray(ST_MemberProxy* proxies, unsigned int characNo,
                             unsigned char maxIdx);
    char QueryGuildMemberProxy(unsigned int guildId, unsigned int characNo,
                               STGuildMemberProxy& proxy);
    char QueryGuildAllMembersProxy(unsigned int guildId, STGuildMemberProxy* proxies,
                                   unsigned short& count);
    char GuildJoin(STGuildJoinInfo* info, unsigned int& result);
    char SaveMemberInsert(unsigned int masterNo, unsigned int characNo,
                          unsigned char type);
    char SaveMemberDelete(unsigned int characNo, unsigned int masterNo,
                          unsigned char type);
    char OnGuildJoinByListApprove(unsigned int guildId, char serverId,
                                  unsigned int m_id, unsigned int characNo,
                                  STGuildJoinInfo& joinInfo, unsigned int& result);
    char QueryPartyStatisticCreate(Packet_DBMW_Dungeon_Statistic_Party* packet);
    char QueryPartyJobStatisticCreate(Packet_DBMW_Dungeon_Statistic_Party_Job* packet);
    char QueryPartyCharacStatisticCreate(
        Packet_DBMW_Dungeon_Statistic_Party_Charac* packet);
    char SaveGuildInfo(unsigned char serverGroup, unsigned int guildId,
                       STGuildDBInfoOnly& info);
    char onItemLimitEditionLoadData(
        const Packet_Item_Limit_Edition_Load_Data_Req* req,
        Packet_Item_Limit_Edition_Load_Data_Rpy* rpy);
    char onItemLimitEditionUpdateData(
        const Packet_Item_Limit_Edition_Update* packet);
    char SaveServerQueueLoadStatistic(unsigned char type, int kind, int qCnt);
    char UpdateGuildWarPointList(int serverId, int rank);
    char UpdateResetGuildPoint(int serverId);
    char UpdateAccumulateGuildPoint(int serverId);
    char QueryGuild(unsigned char serverGroup, unsigned int guildId,
                    Packet_DB_Reply_Query_Guild& reply);
    char ChangeCharName(Packet_DBMW_Change_Char_Name* packet);
    char ChangePvPBuddyName(Packet_DBMW_Change_Char_Name* packet);
    char DeleteJoinListByInvite(unsigned int guildId, unsigned int characNo);
    char OnUpgradeGuildCargo(Packet_DB_Guild_Cargo_Upgrade* packet);
    char OnUpdateGuildCargo(Packet_DB_Update_Guild_Cargo* packet);
    char OnStatisticNumOfOccupations(Packet_DBMW_Statistic_Login_Logout* packet);
    char OnStatisticLoginLogout(Packet_DBMW_Statistic_Login_Logout* packet);
    char QueryOnTimeEventIdx(Packet_Result_OnTimeEvent_Idx& rpy);
    char QueryReloadSpecDb(Packet_Frame_Lag_Statistic_Reload_Spec* req,
                           CStatisticsServer* stats);
    char InsertFrameLagStatistics(
        Packet_Frame_Lag_Statistic_Write_Lag_Index* packet,
        CStatisticsServer* stats);
    char QueryFirstLoadSpecDb(Packet_Frame_Lag_Statistic_Load_Spec* req,
                              CStatisticsServer* stats);
    char QueryTodayGuildMember(unsigned int guildId,
                               Packet_Reply_Today_Guild_Member& reply);
    char QueryHWspecCreate(Packet_DBMW_Save_Client_Spec_Statistic* packet);
    char OnLoadGuildCargoHistory(unsigned int guildId,
                                 Packet_Guild_Load_Guild_Cargo_History& reply);
    char DeleteToBlackList(unsigned int m_id, unsigned int characNo);
    char OnLoadGuildBoard(int guildId, int& count, STGuildBoardDBInfo* boards);
    char selectCollectItems(unsigned char serverInfo, int& curCount,
                            int& totalCount, unsigned int& changeFlag,
                            unsigned char& fullTime);
    char updateNexonPinPcRoomPlayTimeEvent(unsigned char serverInfo,
                                          unsigned int m_id,
                                          unsigned int& pinNo, char* nexonPin,
                                          unsigned int len);
    char OnSaveAssertManagerInfoWrite(
        Packet_DBMW_Assert_Manager_Info_Write_Query* packet);
    char QueryCubeStatisticCreate(Packet_DBMW_Cube_Statistic* packet);
    char SaveUnchangableGuildInfo(Packet_UnChangable_GuildInfo_Save* packet);
    char InsertLetter(unsigned int characNo, unsigned int sendCharacNo,
                      const char* subject, const char* content, int& letterNo,
                      long expiry);
    char AddBuddy(unsigned int characNo, char* name, STBuddyDBInfo& info,
                  int& result);
    char QueryIPCounter(unsigned char serverGroup,
                        std::vector<st_ip_counter_list>& ipList,
                        std::vector<st_full_ip_counter_list>& fullIpList);
    char QueryDeathTowerPlayDataJobStatisticCreate(
        Packet_DBMW_DeathTower_Statistic_Playdata_Job* packet);
    char QueryDeathTowerValueStatisticCreate(
        Packet_DBMW_DeathTower_Statistic_Value* packet);
    char queryTowerFullRank(unsigned int towerIndex,
                            std::vector<stTowerRank_t>& ranks,
                            unsigned int limit);
    char UpdateDisjointAvatarStatistic(
        Packet_Avater_Disjoint_Statistic_DB* packet);
    char QueryDeathTowerPlayDataPartyStatisticCreate(
        Packet_DBMW_DeathTower_Statistic_Playdata_Party* packet);
    char AwardGuildCoinByMail(int guildId, unsigned int serverGroup,
                              unsigned int itemId, unsigned int endurance,
                              int addInfo, int upgrade,
                              std::vector<int>& characNos);
    char SendGuildCoinByMail(int guildId, unsigned int serverGroup,
                             unsigned int itemId, unsigned int endurance,
                             int addInfo, char* subject, char* content);
    char InsertPostal(unsigned int receiveCharacNo, unsigned int sendCharacNo,
                      int sealFlag, unsigned int itemId, int addInfo,
                      unsigned int endurance, int upgrade, char* name,
                      long occTime, int letterId);
    char QueryLoadARSInfo(std::vector<st_ars_info_list>& arsList);
    char QuerySubGuildMaster(unsigned char serverGroup, unsigned int guildId,
                             Packet_DB_Reply_Query_Guild& reply);
    char QueryGuildNotiMessage(unsigned char serverGroup, unsigned int guildId,
                               Packet_DB_Reply_Query_Guild& reply);
    char QueryGuildSkill(unsigned char serverGroup, unsigned int guildId,
                         Packet_DB_Reply_Query_Guild& reply);
    char QueryOnTimeEventIdxUpdate(Packet_Req_Ontime_Event_Idx_Update* packet);
    char QueryOnTimeEventItem(Packet_Result_Ontime_Event_Item& reply);
    char QueryBuddyInfo(unsigned int characNo, STBuddyDBInfo* buddies,
                        unsigned char& count);
    char GetCoinEventPerDay(int serverId, int add, int& out1, int& out2);
    char QueryCharacNoByName(char* name, unsigned int& characNo, int* result);
    char updateCompatibilityIndex(Packet_Stat_Compatibility_Index* packet);
    char OnSecretShopStatistic(Packet_Secret_Shop_Statistic* packet);
    char loadLimitNpcBuyItemInfo(LimitNpcBuyItemRequestInfo* req,
                                 LimitNpcBuyItemResultInfo* result);
    char updateLimitNpcBuyItemInfo(LimitNpcBuyItemUpdate* update);
    char QueryGuildMemberGradeByName(unsigned char serverId,
                                     unsigned int guildId, char* name,
                                     unsigned char& grade,
                                     unsigned int& m_id,
                                     unsigned int& result);
    char ChangeGuildMemberGrade(unsigned char serverId,
                                unsigned int guildId,
                                unsigned char grade, char* name);
    char ChangeGuildMemberGrade(unsigned char serverId,
                                unsigned int guildId,
                                unsigned char grade,
                                unsigned int characNo);
    char ChangeGuildNotifyMessage(int guildId, unsigned int m_id, char* msg);
    char GuildMasterDelegate(int serverId, unsigned int guildId,
                             unsigned int oldMasterNo,
                             unsigned int newMasterMId,
                             unsigned int newMasterNo, char* newMasterName);
    char SendGuildLetter(int serverId, unsigned int guildId, char* msg);
    char OnWriteGuildBoard(
        Packet_DB_Load_Request_Guild_Board_Write* req,
        STGuildBoardDBInfo* info);
    char OnWriteWebGuildBoard(
        Packet_DB_Load_Request_Web_Guild_Board_Write* req,
        STGuildBoardDBInfo* info);
    char OnDeleteGuildBoard(unsigned int no);
    char OnLoadGuildAgit(Packet_DB_Load_Guild_Agit* req,
                         Packet_Guild_Load_Guild_Agit& reply);
    char OnLoadGuildCargo(unsigned int guildId,
                          Packet_Guild_Load_Guild_Cargo& reply);
    char OnCreateGuildAgit(Packet_DB_Create_Guild_Agit* req,
                           Packet_DB_Create_Guild_Agit_Reply& reply);
    char OnDeleteGuildAgit(Packet_DB_Delete_Guild_Agit* req,
                           Packet_DB_Delete_Guild_Agit_Reply& reply);
    char OnUpgradeGuildAgit(Packet_DB_Upgrade_Guild_Agit* req,
                            Packet_DB_Upgrade_Guild_Agit_Reply& reply);
    char OnInsertGuildCargoHistory(
        Packet_DB_Insert_Guild_Cargo_History* req);
    char DeleteToBlackListOnly(unsigned int m_id, char* name);
    char QueryBlackList(unsigned int m_id, STBlackUserDBType* list);
    char SaveGuildSkill(unsigned char serverGroup, unsigned int guildId,
                        STGuildDBInfoOnly& info);
    char SaveGuildMember(unsigned char serverGroup, unsigned int guildId,
                         STGuildMemerDBInfo& info, unsigned int flag,
                         unsigned char type);
    char SaveGuildWarPointList(int serverId, unsigned int* guildIds,
                               unsigned int* points);
    char OnSavePowerWarBonusPoint(
        Packet_DB_Save_Power_War_Bonus_Point* packet);
    char SavePowerWarPoint(Packet_DB_Save_Power_War_Point* packet);
    char OnSavePowerWarStatueRanker(
        Packet_DB_Save_Power_War_Statue_Ranker* packet);
    char OnSavePowerWarPointReward(
        Packet_DB_Save_Power_War_Point_Reward* packet);
    char InsertMail(unsigned int characNo, char* subject, char* content,
                    unsigned int hE, unsigned int h12, int h16, int h17);
    char OnLoadPeriodicMessage(
        Packet_Load_Periodic_Message* req,
        Packet_Result_Loading_Periodic_Message* reply);
    char QueryGuildMember(unsigned char serverId, unsigned int guildId,
                          Packet_DB_Reply_Query_Guild_Member& reply);
    char OnSavePowerWarUserRank(
        Packet_DB_Save_Power_War_User_Rank* packet);
    char OnSavePowerWarGuildRank(
        Packet_DB_Save_Power_War_Guild_Rank* packet);
    char InsertUdpCharacteristic(Packet_Udp_Characteristic* packet);
    char DelBuddy(unsigned int m_id, unsigned int characNo);
    char insertServerGameEvent(Packet_StartGameEventFromServer* packet);
    char updateServerGameEvent(Packet_StopGameEventFromServer* packet);
    char UpdateGuildRank(int serverId, CGuildManager* gm);
    char QueryGuildPointList(int serverId, CGuildManager* gm);
    char QueryP2PStatistics(Packet_P2P_Statistics* packet);
    char OnGoldcardEventStatistic(
        Packet_Goldcard_Event_Statistic_STD* packet);
    char QueryUpdateChannelOccNum(Packet_User_Count_Statistic* packet);
    char OnMemberDeleteAsCharDelete(unsigned int characNo);
    char UpdateMemberKeyInCharacInfo(unsigned char serverId,
                                     unsigned int guildId);
    char QueryGuildBooting(Packet_DB_Query_Reply_On_Guild_Booting& reply,
                           int serverId);
    char QueryHellPartyStatisticItemCreate(
        Packet_DBMW_HellParty_Statistic_Item* packet);
    char OnSavePacketOverflowWrite(
        Packet_DBMW_Packet_Overflow_Statistic* packet);
    char QueryErrorLineStatisticCreate(
        Packet_DBMW_Save_Error_Line_Statistic* packet);
    char QueryTowerOfDespairStatistic(
        Packet_TowerOfDespair_Statistic_STD* packet);
    char GetVillageAttackedRank(Packet_DB_VillageAttackedRank* packet,
                                bool& flag, int& a, int& b);
    int GetMinTimeServerGroup(int serverId);
    int GetMaxHuntingPointServerGroup(int serverId);
    char updateCollectItems(unsigned char a, int b, unsigned int c,
                            unsigned char d);
    char updateCollectItemsGm(unsigned char a, int b, int c, unsigned int d);
    char insertHolePunchingResult(
        Packet_GameServer2Statisctics2DBServer* packet);
    char UpdateRandomboxStatistic(Packet_Randombox_statistic_DB* packet);
    char SaveMemberExp(unsigned int characNo, unsigned int exp,
                       unsigned int lev);
    char UpdatePowerSecedeTime(unsigned char serverId,
                               unsigned int secedeTime);
    char QueryMsg(Packet_DBMW_Query_Msg* packet);
    char GetDBMWStatistic(Packet_DBMW_Query_String* packet);
    char UpdateCreateEmblemStatistic(
        Packet_Emblem_Create_Statistic_DB* packet);
    char OnWriteGuildMemberMemo(
        Packet_DB_Write_Guild_Member_Memo* packet);
    char OnServerMatchData(Packet_Server_Match_data_DBMW* packet);
    char OnManagerEventTriggerAck(Packet_Manager_Event_Trigger_Ack* packet);
    char OnSaveLoadingTimeReport(Packet_DBMW_Loading_Time_Report* packet);
    char OnSaveFatigueBattery(
        Packet_DBMW_Fatigue_Battery_Money_Statistic* packet);
    char QueryInsertUpdate(PacketInsertUpdate* packet);
    char InsertDailyBadSpecStatistics(
        Packet_Frame_Lag_Statistic_Write_Daily_Bad_Spec* packet);
    char RegisterQueryIdTable(int queryId, const char* query);
    char LoadQueryIdTable();
    int FindCharIdInArray(unsigned int* arr, unsigned int characNo,
                          unsigned char maxIdx);
    char OnSaveTingUserAccount(
        Packet_DBMW_Ting_User_TimeCheck_Write_Query* packet);
    char OnSavePowerwarLagReport(Packet_DBMW_Powerwar_Lag_Report* packet);
    char OnSaveUsedMemoryWriteQuery(
        Packet_Frame_Lag_Used_Memory_Write_Query* packet);
    char OnReasonCrashDownQueryWrite(
        Packet_DBMW_Reason_Crash_Down_Query* packet);
    char OnSavePowerwarLoadingReport(
        Packet_DBMW_Powerwar_Loading_Time_Report* packet);
    char OnSaveUserTingTimeCheckWrite(
        Packet_DBMW_User_Ting_TimeCheck_Write_Query* packet);
    char OnTechnicalReportCommonQuery(
        Packet_DBMW_TechnicalReport_Common_Query* packet);
    char SunAhWriteQuery(Packet_Frame_Lag_Statistic_Write_Query* packet);
    char Open(ENUM_DB_HANDLE_IDX idx, const char* host, const char* user,
              const char* pass, const char* db);
    unsigned int GetIdentity(CDBHandle* h);
    CDBHandle* m_handles[0x11];  // +0（0x44 字节，17 个槽）
    CApplication* m_app;  // +0x44
};

#endif  // DBMW_DBMANAGER_H_
