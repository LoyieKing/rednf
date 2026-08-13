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
    bool UpdateQueryCount(unsigned int idx, int count, int time);
    char SelectTest();
    bool QueryGuildWarPointList(int guildWarPoint, CGuildManager* gm);
    bool AwardGuildTitleByMail(int guildId, unsigned int characNo, unsigned int itemId,
                               char* guildName, unsigned int item);
    bool RegisterToBlackList(unsigned int m_id, unsigned int characNo, char* characName);
    bool GuildSecede(Packet_DB_Request_Guild_Secede* req, unsigned int& characNo,
                     unsigned int& m_id, unsigned int& result);
    bool QueryGuildCreate(Packet_DBMW_Request_Guild_Create* req,
                          unsigned int& guildId, unsigned int& result);
    bool QueryMember(unsigned int characNo, Packet_DB_Reply_Query_Member& reply);
    int FindCharProxyInArray(ST_MemberProxy* proxies, unsigned int characNo,
                             unsigned char maxIdx);
    bool QueryGuildMemberProxy(unsigned int guildId, unsigned int characNo,
                               STGuildMemberProxy& proxy);
    bool QueryGuildAllMembersProxy(unsigned int guildId, STGuildMemberProxy* proxies,
                                   unsigned short& count);
    bool GuildJoin(STGuildJoinInfo* info, unsigned int& result);
    char SaveMemberInsert(unsigned int masterNo, unsigned int characNo,
                          unsigned char type);
    char SaveMemberDelete(unsigned int characNo, unsigned int masterNo,
                          unsigned char type);
    bool OnGuildJoinByListApprove(unsigned int guildId, char serverId,
                                  unsigned int m_id, unsigned int characNo,
                                  STGuildJoinInfo& joinInfo, unsigned int& result);
    char QueryPartyStatisticCreate(Packet_DBMW_Dungeon_Statistic_Party* packet);
    char QueryPartyJobStatisticCreate(Packet_DBMW_Dungeon_Statistic_Party_Job* packet);
    char QueryPartyCharacStatisticCreate(
        Packet_DBMW_Dungeon_Statistic_Party_Charac* packet);
    bool SaveGuildInfo(unsigned char serverGroup, unsigned int guildId,
                       STGuildDBInfoOnly& info);
    bool onItemLimitEditionLoadData(
        const Packet_Item_Limit_Edition_Load_Data_Req* req,
        Packet_Item_Limit_Edition_Load_Data_Rpy* rpy);
    bool onItemLimitEditionUpdateData(
        const Packet_Item_Limit_Edition_Update* packet);
    char SaveServerQueueLoadStatistic(unsigned char type, int kind, int qCnt);
    char UpdateGuildWarPointList(int serverId, int rank);
    char UpdateResetGuildPoint(int serverId);
    char UpdateAccumulateGuildPoint(int serverId);
    bool QueryGuild(unsigned char serverGroup, unsigned int guildId,
                    Packet_DB_Reply_Query_Guild& reply);
    char ChangeCharName(Packet_DBMW_Change_Char_Name* packet);
    char ChangePvPBuddyName(Packet_DBMW_Change_Char_Name* packet);
    bool DeleteJoinListByInvite(unsigned int guildId, unsigned int characNo);
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
    bool QueryTodayGuildMember(unsigned int guildId,
                               Packet_Reply_Today_Guild_Member& reply);
    char QueryHWspecCreate(Packet_DBMW_Save_Client_Spec_Statistic* packet);
    char OnLoadGuildCargoHistory(unsigned int guildId,
                                 Packet_Guild_Load_Guild_Cargo_History& reply);
    bool DeleteToBlackList(unsigned int m_id, unsigned int characNo);
    bool OnLoadGuildBoard(int guildId, int& count, STGuildBoardDBInfo* boards);
    bool selectCollectItems(unsigned char serverInfo, int& curCount,
                            int& totalCount, unsigned int& changeFlag,
                            unsigned char& fullTime);
    bool updateNexonPinPcRoomPlayTimeEvent(unsigned char serverInfo,
                                          unsigned int m_id,
                                          unsigned int& pinNo, char* nexonPin,
                                          unsigned int len);
    char OnSaveAssertManagerInfoWrite(
        Packet_DBMW_Assert_Manager_Info_Write_Query* packet);
    char QueryCubeStatisticCreate(Packet_DBMW_Cube_Statistic* packet);
    char SaveUnchangableGuildInfo(Packet_UnChangable_GuildInfo_Save* packet);
    bool InsertLetter(unsigned int characNo, unsigned int sendCharacNo,
                      const char* subject, const char* content, int& letterNo,
                      long expiry);
    char AddBuddy(unsigned int characNo, char* name, STBuddyDBInfo& info,
                  int& result);
    bool QueryIPCounter(unsigned char serverGroup,
                        std::vector<st_ip_counter_list>& ipList,
                        std::vector<st_full_ip_counter_list>& fullIpList);
    char QueryDeathTowerPlayDataJobStatisticCreate(
        Packet_DBMW_DeathTower_Statistic_Playdata_Job* packet);
    char QueryDeathTowerValueStatisticCreate(
        Packet_DBMW_DeathTower_Statistic_Value* packet);
    bool queryTowerFullRank(unsigned int towerIndex,
                            std::vector<stTowerRank_t>& ranks,
                            unsigned int limit);
    bool UpdateDisjointAvatarStatistic(
        Packet_Avater_Disjoint_Statistic_DB* packet);
    char QueryDeathTowerPlayDataPartyStatisticCreate(
        Packet_DBMW_DeathTower_Statistic_Playdata_Party* packet);
    bool AwardGuildCoinByMail(int guildId, unsigned int serverGroup,
                              unsigned int itemId, unsigned int endurance,
                              int addInfo, int upgrade,
                              std::vector<int>& characNos);
    bool SendGuildCoinByMail(int guildId, unsigned int serverGroup,
                             unsigned int itemId, unsigned int endurance,
                             int addInfo, char* subject, char* content);
    bool InsertPostal(unsigned int receiveCharacNo, unsigned int sendCharacNo,
                      int sealFlag, unsigned int itemId, int addInfo,
                      unsigned int endurance, int upgrade, char* name,
                      long occTime, int letterId);
    bool QueryLoadARSInfo(std::vector<st_ars_info_list>& arsList);
    bool QuerySubGuildMaster(unsigned char serverGroup, unsigned int guildId,
                             Packet_DB_Reply_Query_Guild& reply);
    bool QueryGuildNotiMessage(unsigned char serverGroup, unsigned int guildId,
                               Packet_DB_Reply_Query_Guild& reply);
    bool QueryGuildSkill(unsigned char serverGroup, unsigned int guildId,
                         Packet_DB_Reply_Query_Guild& reply);
    char QueryOnTimeEventIdxUpdate(Packet_Req_Ontime_Event_Idx_Update* packet);
    char QueryOnTimeEventItem(Packet_Result_Ontime_Event_Item& reply);
    bool QueryBuddyInfo(unsigned int characNo, STBuddyDBInfo* buddies,
                        unsigned char& count);
    bool GetCoinEventPerDay(int serverId, int add, int& out1, int& out2);
    bool QueryCharacNoByName(char* name, unsigned int& characNo, int* result);
    bool updateCompatibilityIndex(Packet_Stat_Compatibility_Index* packet);
    char OnSecretShopStatistic(Packet_Secret_Shop_Statistic* packet);
    bool loadLimitNpcBuyItemInfo(LimitNpcBuyItemRequestInfo* req,
                                 LimitNpcBuyItemResultInfo* result);
    bool updateLimitNpcBuyItemInfo(LimitNpcBuyItemUpdate* update);
    bool QueryGuildMemberGradeByName(unsigned char serverId,
                                     unsigned int guildId, char* name,
                                     unsigned char& grade,
                                     unsigned int& m_id,
                                     unsigned int& result);
    bool ChangeGuildMemberGrade(unsigned char serverId,
                                unsigned int guildId,
                                unsigned char grade, char* name);
    bool ChangeGuildMemberGrade(unsigned char serverId,
                                unsigned int guildId,
                                unsigned char grade,
                                unsigned int characNo);
    bool ChangeGuildNotifyMessage(int guildId, unsigned int m_id, char* msg);
    bool GuildMasterDelegate(int serverId, unsigned int guildId,
                             unsigned int oldMasterNo,
                             unsigned int newMasterMId,
                             unsigned int newMasterNo, char* newMasterName);
    bool SendGuildLetter(int serverId, unsigned int guildId, char* msg);
    bool OnWriteGuildBoard(
        Packet_DB_Load_Request_Guild_Board_Write* req,
        STGuildBoardDBInfo* info);
    bool OnWriteWebGuildBoard(
        Packet_DB_Load_Request_Web_Guild_Board_Write* req,
        STGuildBoardDBInfo* info);
    bool OnDeleteGuildBoard(unsigned int no);
    bool OnLoadGuildAgit(Packet_DB_Load_Guild_Agit* req,
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
    bool DeleteToBlackListOnly(unsigned int m_id, char* name);
    bool QueryBlackList(unsigned int m_id, STBlackUserDBType* list);
    char SaveGuildSkill(unsigned char serverGroup, unsigned int guildId,
                        STGuildDBInfoOnly& info);
    char SaveGuildMember(unsigned char serverGroup, unsigned int guildId,
                         STGuildMemerDBInfo& info, unsigned int flag,
                         unsigned char type);
    bool SaveGuildWarPointList(int serverId, unsigned int* guildIds,
                               unsigned int* points);
    bool OnSavePowerWarBonusPoint(
        Packet_DB_Save_Power_War_Bonus_Point* packet);
    char SavePowerWarPoint(Packet_DB_Save_Power_War_Point* packet);
    char OnSavePowerWarStatueRanker(
        Packet_DB_Save_Power_War_Statue_Ranker* packet);
    bool OnSavePowerWarPointReward(
        Packet_DB_Save_Power_War_Point_Reward* packet);
    bool InsertMail(unsigned int characNo, char* subject, char* content,
                    unsigned int hE, unsigned int h12, int h16, int h17);
    bool OnLoadPeriodicMessage(
        Packet_Load_Periodic_Message* req,
        Packet_Result_Loading_Periodic_Message* reply);
    bool QueryGuildMember(unsigned char serverId, unsigned int guildId,
                          Packet_DB_Reply_Query_Guild_Member& reply);
    char OnSavePowerWarUserRank(
        Packet_DB_Save_Power_War_User_Rank* packet);
    char OnSavePowerWarGuildRank(
        Packet_DB_Save_Power_War_Guild_Rank* packet);
    bool InsertUdpCharacteristic(Packet_Udp_Characteristic* packet);
    bool DelBuddy(unsigned int m_id, unsigned int characNo);
    bool insertServerGameEvent(Packet_StartGameEventFromServer* packet);
    bool updateServerGameEvent(Packet_StopGameEventFromServer* packet);
    char UpdateGuildRank(int serverId, CGuildManager* gm);
    char QueryGuildPointList(int serverId, CGuildManager* gm);
    bool QueryP2PStatistics(Packet_P2P_Statistics* packet);
    char OnGoldcardEventStatistic(
        Packet_Goldcard_Event_Statistic_STD* packet);
    bool QueryUpdateChannelOccNum(Packet_User_Count_Statistic* packet);
    char OnMemberDeleteAsCharDelete(unsigned int characNo);
    bool UpdateMemberKeyInCharacInfo(unsigned char serverId,
                                     unsigned int guildId);
    char QueryGuildBooting(Packet_DB_Query_Reply_On_Guild_Booting& reply,
                           int serverId);
    char QueryHellPartyStatisticItemCreate(
        Packet_DBMW_HellParty_Statistic_Item* packet);
    char OnSavePacketOverflowWrite(
        Packet_DBMW_Packet_Overflow_Statistic* packet);
    char QueryErrorLineStatisticCreate(
        Packet_DBMW_Save_Error_Line_Statistic* packet);
    bool QueryTowerOfDespairStatistic(
        Packet_TowerOfDespair_Statistic_STD* packet);
    bool GetVillageAttackedRank(Packet_DB_VillageAttackedRank* packet,
                                bool& flag, int& a, int& b);
    int GetMinTimeServerGroup(int serverId);
    int GetMaxHuntingPointServerGroup(int serverId);
    bool updateCollectItems(unsigned char a, int b, unsigned int c,
                            unsigned char d);
    char updateCollectItemsGm(unsigned char a, int b, int c, unsigned int d);
    bool insertHolePunchingResult(
        Packet_GameServer2Statisctics2DBServer* packet);
    bool UpdateRandomboxStatistic(Packet_Randombox_statistic_DB* packet);
    char SaveMemberExp(unsigned int characNo, unsigned int exp,
                       unsigned int lev);
    char UpdatePowerSecedeTime(unsigned char serverId,
                               unsigned int secedeTime);
    bool QueryMsg(Packet_DBMW_Query_Msg* packet);
    bool GetDBMWStatistic(Packet_DBMW_Query_String* packet);
    bool UpdateCreateEmblemStatistic(
        Packet_Emblem_Create_Statistic_DB* packet);
    char OnWriteGuildMemberMemo(
        Packet_DB_Write_Guild_Member_Memo* packet);
    char OnServerMatchData(Packet_Server_Match_data_DBMW* packet);
    char OnManagerEventTriggerAck(Packet_Manager_Event_Trigger_Ack* packet);
    char OnSaveLoadingTimeReport(Packet_DBMW_Loading_Time_Report* packet);
    char OnSaveFatigueBattery(
        Packet_DBMW_Fatigue_Battery_Money_Statistic* packet);
    bool QueryInsertUpdate(PacketInsertUpdate* packet);
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
