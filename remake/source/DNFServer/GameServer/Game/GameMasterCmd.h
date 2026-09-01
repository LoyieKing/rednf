// GameMasterCmd.h — game_master 命令体系还原（df_game_r ORIG T 符号族）。
//
// 布局事实（objdump/DWARF 验证）：
//   IGameMaster          = vptr+0, m_next+4（责任链下一环）
//   CCommand             = vptr+0, CParser*+4；虚表[0]=execute(纯虚), [1]=parse(空实现),
//                          [2]=HasJuniorPermission, [3]=HasSeniorPermission（基类均返回 1）
//   CParser              = vptr+0, CUser*+4, string cmd+8, string args+0xc,
//                          string line+0x10, pos+0x14, id+0x18, char buf[0x100]+0x1c,
//                          vector<string>+0x11c（sizeof=0x128，无虚析构）
//   CCommandMap          = map<int,CCommand*>+0x00（size 0x18),
//                          unordered_map<string,CCommand*>+0x18（ctor bucket=10）
//   CGameMasterMgr       = senior*+0, junior*+4, CCommandMap+8
// 分发链：Mgr::HandleRequest → junior->HandleRequest → (权限校验通过 ? executeTemplate
//         : 基类 HandleRequest 转发 m_next)；Senior 为链尾（校验失败即丢弃）。
#ifndef GAME_GAME_MASTER_CMD_H_
#define GAME_GAME_MASTER_CMD_H_

#include <map>
#include <set>
#include <string>
#include <vector>
#include <unordered_map>

class CUser;
class IDispatcher;

// ---- GM 测试角色状态注入（ORIG 全局命名空间类，CItemDropRateCmd 用） ----
// vtable 单槽（sendServerState），无虚析构、无数据成员（sizeof=4）。
class CGameMasterCharacter
{
public:
    CGameMasterCharacter();                              // ORIG W 0x084b4426
    virtual void sendServerState(CUser* user, char const* msg);  // ORIG W 0x084b42b6（空实现）
};

class CGameMasterCharacterItemDropRate : public CGameMasterCharacter
{
public:
    CGameMasterCharacterItemDropRate();                  // ORIG W 0x084b4434
    virtual void sendServerState(CUser* user, char const* msg);  // ORIG W 0x084b4330
};

namespace game_master
{

class CCommand;
class CParser;   // ORIG: game_master::CParser（本命名空间内）

// ---- 责任链基类 ----
class IGameMaster
{
public:
    explicit IGameMaster(IGameMaster* next);              // 0x082a7f04
    virtual void HandleRequest(CCommand* cmd);            // 0x082a7ed6：转发 m_next

protected:
    IGameMaster* m_next;                                  // +0x04
};

class CSeniorGameMaster : public IGameMaster
{
public:
    explicit CSeniorGameMaster(IGameMaster* next);        // 0x082a7f40（覆盖 vptr 0x8c81cb8）
    virtual void HandleRequest(CCommand* cmd);            // 0x084a8c44
};

class CJuniorGameMaster : public IGameMaster
{
public:
    explicit CJuniorGameMaster(IGameMaster* next);        // 0x082a7f1c（覆盖 vptr 0x8c81cc8）
    virtual void HandleRequest(CCommand* cmd);            // 0x084a8c06
};

// ---- 命令基类 ----
class CCommand
{
public:
    CCommand();                                           // 0x082a7ff0（仅设 vptr）

    void SetParser(CParser* parser);                      // 0x08234972
    CUser* GetUser();                                     // 0x084b3fba：parser->user
    void executeTemplate();                               // 0x084b3f90：parse()+execute()

    virtual void execute() = 0;                           // 虚表[0]（基类 __cxa_pure_virtual）
    virtual void parse() {}                               // 虚表[1]（基类空实现 0x082a7c5c）
    virtual bool HasJuniorPermission() { return true; }      // 虚表[2] 0x082a7c62
    virtual bool HasSeniorPermission() { return true; }      // 虚表[3] 0x082a7c6c

protected:
    CParser* m_parser;                                    // +0x04（由 GM 入口 SetParser 注入）
};

// ---- 命令行解析器 ----
class CParser
{
public:
    CParser();                                            // 0x082353a4
    ~CParser();                                           // 0x08235446（非虚）
    virtual void parse();                                 // 0x084a8ab6

    CUser* m_user;                                        // +0x04（GM 发起者）
    std::string m_cmdName;                                // +0x08（首 token，trim 后 atoi→id）
    std::string m_args;                                   // +0x0c（其余部分）
    std::string m_line;                                   // +0x10（工作副本）
    int m_pos;                                            // +0x14（find(" ") 结果）
    int m_id;                                             // +0x18（atoi(cmdName)）
    char m_buf[0x100];                                    // +0x1c（原始行缓冲，外部写入）
    std::vector<std::string> m_extra;                     // +0x11c
};                                                        // sizeof = 0x128

// ---- 双索引注册表 ----
class CCommandMap
{
public:
    CCommandMap();                                        // 0x082a7f64
    ~CCommandMap();                                       // 0x082a7d88（delete 全部命令对象）
    void add(int id, CCommand* cmd);                      // 0x082a7c76
    void add(std::string name, CCommand* cmd);            // 0x082a7cdc
    CCommand* get(int id);                                // 0x084b3fc8
    CCommand* get(std::string& name);                     // 0x084b402c

private:
    std::map<int, CCommand*> m_byId;                      // +0x00
    std::unordered_map<std::string, CCommand*> m_byName;  // +0x18
};

// ---- 宏录制/回放 ----
struct stMacroData                                       // sizeof = 12
{
    stMacroData();                                       // 0x084b3f6a
    ~stMacroData();                                      // 0x084b5db4

    unsigned char m_cmd;                                 // +0x00 包类型
    unsigned char m_sub;                                 // +0x01 子类型
    char* m_data;                                        // +0x04（new[] 拷贝的包体）
    int m_len;                                           // +0x08
};

class CMacro
{
public:
    void Reset();                                        // 0x084a85b4
    void StartMacro();                                   // 0x084a865c
    void StopMacro();                                    // 0x084a8678
    void RudMacro();                                     // 0x084a8686（置 running 并重放全部）
    void NextCmd();                                      // 0x084a86fe
    stMacroData* GetCurCmd();                            // 0x084a87e2
    void Recording(int type, int sub, char* data, int len);  // 0x084a8802
    void OnRecv(int type, int sub, char* data, int len);     // 0x084a8870
    void OnSend(int type, int sub);                          // 0x084a88aa
    void SendPacket(char const* data, int len);              // 0x084a8924

private:
    int m_state;                                         // +0x00：0 stop / 1 recording / 2 running
    std::vector<stMacroData> m_data;                     // +0x04
    unsigned int m_curIdx;                               // +0x10
    CUser* m_user;                                       // +0x14（回放目标）
};

// ---- 管理器（责任链 + 注册表）----
class CGameMasterMgr
{
public:
    CGameMasterMgr();                                 // 0x082a8a28（注册链，GmRegister TU）
    static IDispatcher* GetDispatcher(int code);      // ORIG static（code@+0x8；0x084a8a9a）
    void HandleRequest(CCommand* cmd);                // 0x084a8c6e
    CCommand* GetCommand(int id);                     // 0x084a8c92
    CCommand* GetCommand(std::string& name);          // 0x084a8cb0

private:
    CSeniorGameMaster* m_senior;                      // +0x00（链尾）
    CJuniorGameMaster* m_junior;                      // +0x04（入口）
    CCommandMap m_cmdMap;                             // +0x08
};


// =====================================================================
// GM 具体命令类（87 个，按 ORIG Mgr ctor 注册顺序声明）。
// 字段布局：sizeof 由 ORIG operator new 尺寸实证；0xc 起的 string@+0x08
// 为通用 parse 结果槽（CMoveCharacCmd 实证），其余字段语义由各实现 TU
// 对照 docs/class_func_reports/game_master__<Class>/ 具名化。
// 权限虚函数全部继承基类默认（ORIG 各类虚表 [2]/[3] 槽均为基类地址）。
// =====================================================================
class CMoveCharacCmd : public CCommand
{
public:
    CMoveCharacCmd();                // ORIG W 0x082a7ffe（sizeof=0xc）
    virtual void parse();                            // ORIG T 0x084a8cce
    virtual void execute();                          // ORIG T 0x084a8d02

    std::string m_str0;                              // +0x08
};
class CPvPStartCmd : public CCommand
{
public:
    CPvPStartCmd();                  // ORIG W 0x082a8028（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084a8f6a
};
class CCirculationStatisticCmd : public CCommand
{
public:
    CCirculationStatisticCmd();      // ORIG W 0x082a8044（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084a8ff6
};
class CAutoMarketConditionsControlCmd : public CCommand
{
public:
    CAutoMarketConditionsControlCmd(); // ORIG W 0x082a8060（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084a8ffc
};
class CItemDropRateCmd : public CCommand
{
public:
    CItemDropRateCmd();              // ORIG W 0x082a807c（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084a9002
};
class CCharacStatCmd : public CCommand
{
public:
    CCharacStatCmd();                // ORIG W 0x082a8098（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084a905e
};
class CSetTutorialFlagCmd : public CCommand
{
public:
    CSetTutorialFlagCmd();           // ORIG W 0x082a80b4（sizeof=0xc）
    virtual void parse();                            // ORIG T 0x084a921a
    virtual void execute();                          // ORIG T 0x084a9252

    unsigned int m_flag;                             // +0x08（ORIG parse atoi）
};
class CViewTutorialFlagCmd : public CCommand
{
public:
    CViewTutorialFlagCmd();          // ORIG W 0x082a80d0（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084a927e
};
class CResetTutorialFlagCmd : public CCommand
{
public:
    CResetTutorialFlagCmd();         // ORIG W 0x082a80ec（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084a94c6
};
class CCreateDisjointStoreCmd : public CCommand
{
public:
    CCreateDisjointStoreCmd();       // ORIG W 0x082a8108（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084a9720
};
class CRequestDisjointCmd : public CCommand
{
public:
    CRequestDisjointCmd();           // ORIG W 0x082a8124（sizeof=0xc）
    virtual void parse();                            // ORIG T 0x084a97c0
    virtual void execute();                          // ORIG T 0x084a97f4

    std::string m_str0;                              // +0x08
};
class CTeleportCmd : public CCommand
{
public:
    CTeleportCmd();                  // ORIG W 0x082a814e（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084a986e
};
class CChangeExpertJobCmd : public CCommand
{
public:
    CChangeExpertJobCmd();           // ORIG W 0x082a816a（sizeof=0xc）
    virtual void parse();                            // ORIG T 0x084a98c8
    virtual void execute();                          // ORIG T 0x084a9900

    int m_jobType;                                   // +0x08
};
class CChangeExpertJobExpCmd : public CCommand
{
public:
    CChangeExpertJobExpCmd();        // ORIG W 0x082a8186（sizeof=0xc）
    virtual void parse();                            // ORIG T 0x084a9b98
    virtual void execute();                          // ORIG T 0x084a9bd0

    int m_exp;                                       // +0x08
};
class CStartMacroCmd : public CCommand
{
public:
    CStartMacroCmd();                // ORIG W 0x082a81a2（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084a9bfa
};
class CStopMacroCmd : public CCommand
{
public:
    CStopMacroCmd();                 // ORIG W 0x082a81be（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084a9c00
};
class CRunMacroCmd : public CCommand
{
public:
    CRunMacroCmd();                  // ORIG W 0x082a81da（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084a9c06
};
class CTestDisjointCmd : public CCommand
{
public:
    CTestDisjointCmd();              // ORIG W 0x082a81f6（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084a9c0c
};
class CAssaultAbuseUserCmd : public CCommand
{
public:
    CAssaultAbuseUserCmd();          // ORIG W 0x082a8212（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084a9daa
};
class CPcroomPremiumCmd : public CCommand
{
public:
    CPcroomPremiumCmd();             // ORIG W 0x082a822e（sizeof=0xc）
    virtual void parse();                            // ORIG T 0x084a9db0
    virtual void execute();                          // ORIG T 0x084a9de8

    int m_duration;                                  // +0x08（时长秒；atoi(args)）
};
class CBloodMonsterListCmd : public CCommand
{
public:
    CBloodMonsterListCmd();          // ORIG W 0x082a824a（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084a9e34
};
class CVillageAttackedStartCmd : public CCommand
{
public:
    CVillageAttackedStartCmd();      // ORIG W 0x082a8266（sizeof=0xc）
    virtual void parse();                            // ORIG T 0x084aa10c
    virtual void execute();                          // ORIG T 0x084aa144

    int m_villageNo;                            // +0x08（ORIG POD：村庄号）
};
class CVillageAttackedStopCmd : public CCommand
{
public:
    CVillageAttackedStopCmd();       // ORIG W 0x082a8282（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084aa298
};
class CVillageAttackedFightCmd : public CCommand
{
public:
    CVillageAttackedFightCmd();      // ORIG W 0x082a829e（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084aa1dc
};
class CItemInfoCmd : public CCommand
{
public:
    CItemInfoCmd();                  // ORIG W 0x082a82ba（sizeof=0xc）
    virtual void parse();                            // ORIG T 0x084aa29e
    virtual void execute();                          // ORIG T 0x084aa2d2

    std::string m_str0;                              // +0x08
};
class CAddItemPollutionCmd : public CCommand
{
public:
    CAddItemPollutionCmd();          // ORIG W 0x082a82e4（sizeof=0xc）
    virtual void parse();                            // ORIG T 0x084aa9c8
    virtual void execute();                          // ORIG T 0x084aaa00

    int m_itemIdx;                              // +0x08（ORIG POD：道具 idx）
};
class CChangeNPCMaxDayCmd : public CCommand
{
public:
    CChangeNPCMaxDayCmd();           // ORIG W 0x082a8300（sizeof=0x10）
    virtual void parse();                            // ORIG T 0x084aacb8
    virtual void execute();                          // ORIG T 0x084aadd2

    int m_npcId;                                     // +0x08
    int m_maxDay;                                    // +0x0c
};
class CChangeNPCFavorCmd : public CCommand
{
public:
    CChangeNPCFavorCmd();            // ORIG W 0x082a831c（sizeof=0x10）
    virtual void parse();                            // ORIG T 0x084aadd8
    virtual void execute();                          // ORIG T 0x084aaef2

    int m_npcId;                                     // +0x08
    int m_favor;                                     // +0x0c
};
class CClearInventoryCmd : public CCommand
{
public:
    CClearInventoryCmd();            // ORIG W 0x082a8338（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084aaef8
};
class CClearAvatarInvenCmd : public CCommand
{
public:
    CClearAvatarInvenCmd();          // ORIG W 0x082a8354（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084aafb0
};
class CReselectDailyQuestCmd : public CCommand
{
public:
    CReselectDailyQuestCmd();        // ORIG W 0x082a8370（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084ab4f2
};
class CResetTrainingQuestCmd : public CCommand
{
public:
    CResetTrainingQuestCmd();        // ORIG W 0x082a838c（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084ab508
};
class CInvalidGold : public CCommand
{
public:
    CInvalidGold();                  // ORIG W 0x082a83a8（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084ab16e
};
class CPowerWarStraightVictory : public CCommand
{
public:
    CPowerWarStraightVictory();      // ORIG W 0x082a83c4（sizeof=0xc）
    virtual void parse();                            // ORIG T 0x084ab202
    virtual void execute();                          // ORIG T 0x084ab23e

    short m_straight;                                // +0x08
    char m_pad[2];
};
class CDeleteGuild : public CCommand
{
public:
    CDeleteGuild();                  // ORIG W 0x082a83e0（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084ab29c
};
class CEnduranceCmd : public CCommand
{
public:
    CEnduranceCmd();                 // ORIG W 0x082a83fc（sizeof=0x10）
    virtual void parse();                            // ORIG T 0x084aa73a
    virtual void execute();                          // ORIG T 0x084aa858

    int m_slot;                                      // +0x08
    unsigned short m_endurance;                      // +0x0c
};
class CChangeCharacName : public CCommand
{
public:
    CChangeCharacName();             // ORIG W 0x082a8418（sizeof=0xc）
    virtual void parse();                            // ORIG T 0x084ab524
    virtual void execute();                          // ORIG T 0x084ab558

    std::string m_str0;                              // +0x08
};
class CStartEvent : public CCommand
{
public:
    CStartEvent();                   // ORIG W 0x082a8442（sizeof=0x14）
    virtual void parse();                            // ORIG T 0x084ab5b6
    virtual void execute();                          // ORIG T 0x084ab886
    void getParam(std::string param);                // ORIG T 0x084ab64e

    int m_eventCount;                                // +0x08
    int m_eventId;                                   // +0x0c
    unsigned short m_param1;                         // +0x10
    unsigned short m_param2;                         // +0x12
};
class CEndEvent : public CCommand
{
public:
    CEndEvent();                     // ORIG W 0x082a845e（sizeof=0xc）
    virtual void parse();                            // ORIG T 0x084ab914
    virtual void execute();                          // ORIG T 0x084ab94c

    int m_eventCode;                                 // +0x08
};
class CSecretShopCheckLottery : public CCommand
{
public:
    CSecretShopCheckLottery();       // ORIG W 0x082a847a（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084ab9c4
};
class CSecretShopNpc : public CCommand
{
public:
    CSecretShopNpc();                // ORIG W 0x082a8496（sizeof=0xc）
    virtual void parse();                            // ORIG T 0x084ab9da
    virtual void execute();                          // ORIG T 0x084aba12

    int m_npcIdx;                               // +0x08（ORIG POD：NPC idx）
};
class CItemDropInDungeon : public CCommand
{
public:
    CItemDropInDungeon();            // ORIG W 0x082a84b2（sizeof=0xc）
    virtual void parse();                            // ORIG T 0x084abcb0
    virtual void execute();                          // ORIG T 0x084abce8

    int m_itemIdx;                              // +0x08（ORIG POD：道具 idx）
};
class CDeleteAccountCargo : public CCommand
{
public:
    CDeleteAccountCargo();           // ORIG W 0x082a84ce（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084abdb4
};
class CLinkSystem : public CCommand
{
public:
    CLinkSystem();                   // ORIG W 0x082a84ea（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084abdd0
};
class CGuildExpBookDelete : public CCommand
{
public:
    CGuildExpBookDelete();           // ORIG W 0x082a8506（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084abf2c
};
class CUserPremiumDelete : public CCommand
{
public:
    CUserPremiumDelete();            // ORIG W 0x082a8522（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084abf4e
};
class CAddItemByMail : public CCommand
{
public:
    CAddItemByMail();                // ORIG W 0x082a853e（sizeof=0xc）
    virtual void parse();                            // ORIG T 0x084ac2aa
    virtual void execute();                          // ORIG T 0x084ac2e2

    int m_itemIdx;                              // +0x08（ORIG POD：道具 idx）
};
class CAdjustCharacStat : public CCommand
{
public:
    CAdjustCharacStat();             // ORIG W 0x082a855a（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084ac58c
};
class CSetGuildFund : public CCommand
{
public:
    CSetGuildFund();                 // ORIG W 0x082a8576（sizeof=0xc）
    virtual void parse();                            // ORIG T 0x084ac668
    virtual void execute();                          // ORIG T 0x084ac6a0

    int m_fund;                                 // +0x08（ORIG POD：资金额）
};
class CGuildDebug : public CCommand
{
public:
    CGuildDebug();                   // ORIG W 0x082a8592（sizeof=0x14）
    virtual void parse();                            // ORIG T 0x084ac720
    virtual void execute();                          // ORIG T 0x084ac76c

    int m_param1;                                    // +0x08
    int m_param2;                                    // +0x0c
    unsigned int m_f1;                               // +0x10（字段语义见报告）
};
class CDropItem : public CCommand
{
public:
    CDropItem();                     // ORIG W 0x082a85ae（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084aca8c
};
class CGetCashInven : public CCommand
{
public:
    CGetCashInven();                 // ORIG W 0x082a85ca（sizeof=0xc）
    virtual void parse();                            // ORIG T 0x084ac80a
    virtual void execute();                          // ORIG T 0x084ac846

    short m_cashType;                           // +0x08（ORIG POD：short）
};
class CPickup : public CCommand
{
public:
    CPickup();                       // ORIG W 0x082a85e6（sizeof=0x10）
    virtual void parse();                            // ORIG T 0x084ac84c
    virtual void execute();                          // ORIG T 0x084ac966

    int m_param1;                                    // +0x08
    int m_param2;                                    // +0x0c
};
class CRefund : public CCommand
{
public:
    CRefund();                       // ORIG W 0x082a8602（sizeof=0x10）
    virtual void parse();                            // ORIG T 0x084ac96c
    virtual void execute();                          // ORIG T 0x084aca86

    int m_param1;                                    // +0x08
    int m_param2;                                    // +0x0c
};
class CMonsterDropTest : public CCommand
{
public:
    CMonsterDropTest();              // ORIG W 0x082a861e（sizeof=0x10）
    virtual void parse();                            // ORIG T 0x084ace8a
    virtual void execute();                          // ORIG T 0x084acf98
    void checkCommandExecuteError();                                       // 辅助（ORIG T 0x084ad82a，签名见报告）
    void clear();                                       // 辅助（ORIG T 0x084b4096，签名见报告）
    void getDungeonDropResult();                                       // 辅助（ORIG T 0x084ae0e2，签名见报告）
    void makeFileGoldContents();                                       // 辅助（ORIG T 0x084ad6d6，签名见报告）
    void makeGeneralItemDropList();                                       // 辅助（ORIG T 0x084ae734，签名见报告）
    void makeGoldItemDropList();                                       // 辅助（ORIG T 0x084ae944，签名见报告）
    void makeUpgradeDropList();                                       // 辅助（ORIG T 0x084ae14c，签名见报告）
    void passedMapMonsterKill();                                       // 辅助（ORIG T 0x084adaf8，签名见报告）
    void processDungeonDrop();                                       // 辅助（ORIG T 0x084ad8dc，签名见报告）
    void processKillMonster();                                       // 辅助（ORIG T 0x084add20，签名见报告）
    void singleMapMonsterKill();                                       // 辅助（ORIG T 0x084ad93a，签名见报告）

    int m_mode;                                      // +0x08（0=single/1=passed 保存/2=passed 统计/3=gold 统计）
    int m_count;                                     // +0x0c（统计次数，0 视作 10000）
};
class CBoosterTest : public CCommand
{
public:
    CBoosterTest();                  // ORIG W 0x082a863a（sizeof=0x10）
    virtual void parse();                            // ORIG T 0x084aeb5e
    virtual void execute();                          // ORIG T 0x084aed04
    void SendChatMsg();                                       // 辅助（ORIG T 0x084aeac6，签名见报告）
    void _selectJewelTypeIndex();                                       // 辅助（ORIG T 0x084af3b2，签名见报告）
    void clear();                                       // 辅助（ORIG T 0x084b40b0，签名见报告）
    void getBoosterResult();                                       // 辅助（ORIG T 0x084af9d6，签名见报告）
    void getCompoundEmblemResult();                                       // 辅助（ORIG T 0x084af0d6，签名见报告）
    void getJarResult();                                       // 辅助（ORIG T 0x084afba8，签名见报告）
    void getRandomboxResult();                                       // 辅助（ORIG T 0x084afe3a，签名见报告）
    void getRarityStr();                                       // 辅助（ORIG T 0x084aea80，签名见报告）
    void get_gold_lottery_result();                                       // 辅助（ORIG T 0x084afd1c，签名见报告）
    void is_gold_lottery_item();                                       // 辅助（ORIG T 0x084afccc，签名见报告）
    void makeFileContents();                                       // 辅助（ORIG T 0x084af504，签名见报告）
    void make_file_gold_contents();                                       // 辅助（ORIG T 0x084af7fc，签名见报告）
    static void printClient(CUser* user, bool more, char const* name, char const* contents);
    static void printServer(char const* path, char const* contents);

    int m_mode;                                  // +0x08（ORIG POD：sscanf "%d%d" 首值）
    int m_count;                                 // +0x0c（ORIG POD：sscanf 次值）
};
class CRandomObjectTest : public CCommand
{
public:
    CRandomObjectTest();             // ORIG W 0x082a8656（sizeof=0x10）
    virtual void parse();                            // ORIG T 0x084b0032
    virtual void execute();                          // ORIG T 0x084b0082
    void checkCommandExecuteError();                                       // 辅助（ORIG T 0x084b02be，签名见报告）
    void clear();                                       // 辅助（ORIG T 0x084b40ca，签名见报告）
    void getVendingMachineInfo();                                       // 辅助（ORIG T 0x084b0444，签名见报告）
    void getVendingMachineItem();                                       // 辅助（ORIG T 0x084b02e6，签名见报告）
    void processVendingMachineItemLottery();                                       // 辅助（ORIG T 0x084b0550，签名见报告）

    int m_mode;                                      // +0x08
    int m_count;                                     // +0x0c
};
class CBindCubeTest : public CCommand
{
public:
    CBindCubeTest();                 // ORIG W 0x082a8672（sizeof=0xc）
    virtual void parse();                            // ORIG T 0x084acbe4
    virtual void execute();                          // ORIG T 0x084acc1e

    char m_flag;                                // +0x08（ORIG POD：byte 标志）
};
class COneADayItemShop : public CCommand
{
public:
    COneADayItemShop();              // ORIG W 0x082a868e（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084b0610
};
class CCraneNeedMaterial : public CCommand
{
public:
    CCraneNeedMaterial();            // ORIG W 0x082a86aa（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084b0626
};
class CTitleBookReset : public CCommand
{
public:
    CTitleBookReset();               // ORIG W 0x082a86c6（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084b063c
};
class CTitleBookAdjust : public CCommand
{
public:
    CTitleBookAdjust();              // ORIG W 0x082a86e2（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084b068a
};
class CTitleBookNoClear : public CCommand
{
public:
    CTitleBookNoClear();             // ORIG W 0x082a86fe（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084b0742
};
class CAchievementReset : public CCommand
{
public:
    CAchievementReset();             // ORIG W 0x082a871a（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084b076a
};
class CAchievementReport : public CCommand
{
public:
    CAchievementReport();            // ORIG W 0x082a8736（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084b07fe
};
class CAchievementClear : public CCommand
{
public:
    CAchievementClear();             // ORIG W 0x082a8752（sizeof=0xc）
    virtual void parse();                            // ORIG T 0x084b08f0
    virtual void execute();                          // ORIG T 0x084b092a

    unsigned int m_achievementId;                    // +0x08（ORIG parse atoi 结果，setTrigger id）
};
class CAchievementAllClear : public CCommand
{
public:
    CAchievementAllClear();          // ORIG W 0x082a876e（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084b097c
};
class CAchievementNoClear : public CCommand
{
public:
    CAchievementNoClear();           // ORIG W 0x082a878a（sizeof=0x8）
    virtual void execute();                          // ORIG T 0x084b09a8
};
class CInGameEventSetDay : public CCommand
{
public:
    CInGameEventSetDay();            // ORIG W 0x082a87a6（sizeof=0xc）
    virtual void parse();                            // ORIG T 0x084b09d0
    virtual void execute();                          // ORIG T 0x084b0a08

    int m_day;                                  // +0x08（ORIG POD：天数）
};
class CChoiceItem : public CCommand
{
public:
    CChoiceItem();                   // ORIG W 0x082a87c2（sizeof=0xc）
    virtual void parse();                            // ORIG T 0x084b0a34
    virtual void execute();                          // ORIG T 0x084b0a6e

    int m_itemIdx;                                   // +0x08
};
class CIndependentDrop : public CCommand
{
public:
    CIndependentDrop();              // ORIG W 0x082a87de（sizeof=0x3c）
    virtual void parse();                            // ORIG T 0x084b0b08
    virtual void execute();                          // ORIG T 0x084b0b4e
    void PrintItemDropList();                                       // 辅助（ORIG T 0x084b1756，签名见报告）
    void ResultToStr();                                       // 辅助（ORIG T 0x084b165c，签名见报告）
    void SendChatMsg();                                       // 辅助（ORIG T 0x084b15c4，签名见报告）
    void printClient();                                       // 辅助（ORIG T 0x084b14b8，签名见报告）

    bool m_all;                                   // +0x08（ORIG POD：all 标志）
    char m_pad09[3];                              // +0x09..0x0b 对齐
    std::set<int> m_mons;                         // +0x0c（ORIG C1 构造，0x18）
    std::set<int> m_ais;                          // +0x24（ORIG C1 构造，0x18）
};
class CUpgradeCardDebug : public CCommand
{
public:
    CUpgradeCardDebug();             // ORIG W 0x082a883e（sizeof=0xc）
    virtual void parse();                            // ORIG T 0x084b1b5e
    virtual void execute();                          // ORIG T 0x084b1b96
    void SendChatMsg();                                       // 辅助（ORIG T 0x084b1e8a，签名见报告）

    int m_itemIdx;                              // +0x08（ORIG POD：道具 idx）
};
class NpcLimitBuyItem : public CCommand
{
public:
    NpcLimitBuyItem();               // ORIG W 0x082a885a（sizeof=0x8）
    virtual void parse();                            // ORIG T 0x084b1f22
    virtual void execute();                          // ORIG T 0x084b1f28
};
class CIndependentDropByForce : public CCommand
{
public:
    CIndependentDropByForce();       // ORIG W 0x082a8876（sizeof=0xc）
    virtual void parse();                            // ORIG T 0x084b1f8c
    virtual void execute();                          // ORIG T 0x084b1fda

    char m_flag;                                // +0x08（ORIG POD：byte 标志）
};
class DungeonDifficulty : public CCommand
{
public:
    DungeonDifficulty();             // ORIG W 0x082a8892（sizeof=0xc）
    virtual void parse();                            // ORIG T 0x084b2110
    virtual void execute();                          // ORIG T 0x084b2152

    unsigned char m_difficulty;                      // +0x08
    char m_pad[3];
};
class BlueMarbleSet : public CCommand
{
public:
    BlueMarbleSet();                 // ORIG W 0x082a88ae（sizeof=0x10）
    virtual void parse();                            // ORIG T 0x084b22ca
    virtual void execute();                          // ORIG T 0x084b25a4
    void getParam();                                       // 辅助（ORIG T 0x084b235c，签名见报告）

    char m_flag0;                                 // +0x08（ORIG POD）
    char m_flag1;                                 // +0x09
    char m_flag2;                                 // +0x0a
    int m_param;                                  // +0x0c
};
class CTestTradeRestrict : public CCommand
{
public:
    CTestTradeRestrict();            // ORIG W 0x082a88ca（sizeof=0x8）
    virtual void parse();                            // ORIG T 0x084b263a
    virtual void execute();                          // ORIG T 0x084b2640
};
class CGrowhWeaponEvent : public CCommand
{
public:
    CGrowhWeaponEvent();             // ORIG W 0x082a88e6（sizeof=0x10）
    virtual void parse();                            // ORIG T 0x084b28d6
    virtual void execute();                          // ORIG T 0x084b293c

    unsigned char m_resetFlag;                       // +0x08
    char m_pad[3];
    int m_timepiece;                                 // +0x0c
};
class CClearRewardTest : public CCommand
{
public:
    CClearRewardTest();              // ORIG W 0x082a8902（sizeof=0x8）
    virtual void parse();                            // ORIG T 0x084b2a02
    virtual void execute();                          // ORIG T 0x084b2a08
    void printClient(CUser* user, char const* fileName, char const* content);
};
class CBingoGmTest : public CCommand
{
public:
    CBingoGmTest();                  // ORIG W 0x082a893a（sizeof=0xc）
    virtual void parse();                            // ORIG T 0x084b35c4
    virtual void execute();                          // ORIG T 0x084b35fc
    void reqValue();                                 // ORIG T 0x084b368a
    void resetData();                                // ORIG T 0x084b363e

    int m_subCmd;                               // +0x08（ORIG POD：子命令码）
};

class CSetMultiboxFailCnt : public CCommand
{
public:
    CSetMultiboxFailCnt();           // ORIG W 0x082a891e（sizeof=0xc）
    virtual void parse();                            // ORIG T 0x084b34a4
    virtual void execute();                          // ORIG T 0x084b34f8

    int m_failCnt;                              // +0x08（ORIG POD：失败次数）
};
class CLevelUp : public CCommand
{
public:
    CLevelUp();                      // ORIG W 0x082a8956（sizeof=0xc）
    virtual void parse();                            // ORIG T 0x084b388c
    virtual void execute();                          // ORIG T 0x084b38e2
    void getLevel();                                       // 辅助（ORIG T 0x084b398e，签名见报告）
    void setLevel();                                       // 辅助（ORIG T 0x084b395c，签名见报告）

    int m_level;                                // +0x08（ORIG POD：等级）
};
class CRecipeSuccess : public CCommand
{
public:
    CRecipeSuccess();                // ORIG W 0x082a8972（sizeof=0xc）
    virtual void parse();                            // ORIG T 0x084b399a
    virtual void execute();                          // ORIG T 0x084b3a34

    int m_result;                                    // +0x08（1=success/2=fail/0=auto）
};
class CBillingRechargeCommand : public CCommand
{
public:
    CBillingRechargeCommand();       // ORIG W 0x082a898e（sizeof=0x10）
    virtual void parse();                            // ORIG T 0x084b3a60
    virtual void execute();                          // ORIG T 0x084b3ad4

    std::string m_str0;                              // +0x08
};
class CMobileRestrictCommand : public CCommand
{
public:
    CMobileRestrictCommand();        // ORIG W 0x082a89b8（sizeof=0x10）
    virtual void parse();                            // ORIG T 0x084b3bdc
    virtual void execute();                          // ORIG T 0x084b3c60

    std::string m_str0;                              // +0x08（目标限制域："mobile"/"character"）
    unsigned int m_restrictSwitch;                   // +0x0c
};
class CPcroomRewardReset : public CCommand
{
public:
    CPcroomRewardReset();            // ORIG W 0x082a89e2（sizeof=0x8）
    virtual void parse();                            // ORIG T 0x084b3d12
    virtual void execute();                          // ORIG T 0x084b3d20
};
class CHeroMissionCommand : public CCommand
{
public:
    CHeroMissionCommand();           // ORIG W 0x082a89fe（sizeof=0x14）
    virtual void parse();                            // ORIG T 0x084b3db2
    virtual void execute();                          // ORIG T 0x084b3e5e

    std::string m_str0;                              // +0x08
    unsigned int m_f1;                               // +0x10（字段语义见报告）
};


} // namespace game_master

#endif  // GAME_GAME_MASTER_CMD_H_
