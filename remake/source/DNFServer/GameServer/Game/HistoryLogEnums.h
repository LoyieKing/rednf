#ifndef GAME_HISTORY_LOG_ENUMS_H_
#define GAME_HISTORY_LOG_ENUMS_H_

// ============================================================================
// df_game_r 还原 —— cUserHistoryLog 域全局枚举 / 枚举包装类。
//
// ORIG 依据：df_game_r mangled 符号直接引用以下类型名（无 namespace 前缀 =
// 全局作用域；N..1TE 形式 = struct 包 enum T 惯例）。仓库此前未建模，由
// TaiwanLog 批次建立（Wave5 仅ORIG 清零批）。枚举成员值除特别注明外按调用点
// 已知比较值/0 起始建模【推断：值不敏感——所有使用点均以 %d 透传】。
// ============================================================================

// ORIG _ZN15cUserHistoryLog9GuildCargoE31ENUM_GUILD_CARGO_HISTORY_REASONi 等
enum ENUM_GUILD_CARGO_HISTORY_REASON
{
    ENUM_GUILD_CARGO_HISTORY_REASON_0 = 0
};

// ORIG cUserHistoryLog::SecuService(ENUM_SECU_SERVICE_TYPE, ENUM_SECU_SERVICE_ACTION)
// ORIG 0x0868479c 仅判 type==0("s_gob")/type==1("s_card")。
enum ENUM_SECU_SERVICE_TYPE
{
    ENUM_SECU_SERVICE_TYPE_GOBLIN = 0,   // s_gob
    ENUM_SECU_SERVICE_TYPE_CARD   = 1    // s_card
};

enum ENUM_SECU_SERVICE_ACTION
{
    ENUM_SECU_SERVICE_ACTION_0 = 0
};

// ORIG _ZN15cUserHistoryLog12CashInvenAddE19eCashInvenAddReasonjjm 等（透传 %d）
enum eCashInvenAddReason { eCashInvenAddReason_0 = 0 };
enum eCashInvenDelReason { eCashInvenDelReason_0 = 0 };

// ORIG _ZN15cUserHistoryLog18ItemMakingSkillAddE21ENUM_ITEM_MAKING_TYPEiis（透传）
enum ENUM_ITEM_MAKING_TYPE { ENUM_ITEM_MAKING_TYPE_0 = 0 };

// ORIG _ZN15cUserHistoryLog9CeraPointE23ENUM_CERAPOINT_LOG_TYPEj（透传 %d）
enum ENUM_CERAPOINT_LOG_TYPE { ENUM_CERAPOINT_LOG_TYPE_0 = 0 };

// ORIG struct 包 enum T：_ZN15cUserHistoryLog16AntibotPacketLogEN20AntibotPacketLogType1TE
struct AntibotPacketLogType
{
    enum T
    {
        T_0 = 0
    };
};

// ORIG struct 包 enum T：BuyCashShopItem(... N19CeraShopPaymentType1TE y)
struct CeraShopPaymentType
{
    enum T
    {
        T_0 = 0
    };
};

#endif  // GAME_HISTORY_LOG_ENUMS_H_
