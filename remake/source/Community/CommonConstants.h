//
// 公共常量定义（来自原始二进制的 .rodata 逆向提取，2026-08-07）
// 原始为头文件内 static const 常量，被 18 个 TU 包含后各生成一份本地副本（_ZL 本地符号）。
// 值取自原始 ELF .rodata 首个实例（小端）。类成员常量（EmoticonType 等）未包含。
//

#ifndef REDNF_COMMON_CONSTANTS_H_
#define REDNF_COMMON_CONSTANTS_H_

static const int kURLLength = 0xff;
static const char MAX_STAMINA = 100;
static const int kMaxBingoNo = 0x32;
static const int kMaxUserAge = 0x96;
static const int SFP_UP_LEVEL = 0x32;
static const int kBingoBaseNo = 0x5;
static const int MAX_ATT_PHASE = 0x9;
static const int MAX_DROP_GOLD = 0x3e8;
static const int MAX_LEVLE_GAP = 0x4;
static const int kMaxMatchLine = 0x3;
static const int BILLION_NUMBER = 0x3b9aca00;
static const int MAX_LUCK_POINT = 0x1869f;
static const int MAX_PVP_PLAYER = 0x8;
static const int kFairPvpJobMax = 0x14;
static const short MAX_BADGE_COUNT = 10;
static const char MAX_CHAOS_POINT = 100;
static const int MAX_ELIXIR_ITEM = 0x8;
static const char MAX_GUILD_LEVEL = 17;
static const int MAX_TEAM_PLAYER = 0x4;
static const int QUEST_RECV_SLOT = 0x14;
static const int DELAY_FOR_WINNER = 0x3e8;
static const int DOUBT_TRADE_GOLD = 0x989680;
static const int kItemGainFlagMax = 0x1388;
static const int LEVEL_UP_ITEM_MAX = 0x3b;
static const int LEVEL_UP_ITEM_MIN = 0x13;
static const char MAX_GUILD_MESSAGE = 100;
static const int kLotteryCountItem = 0x989763;
static const int kMaxBuySkillCount = 0x46;
static const int kMaxRentItemCount = 0x1;
static const char CHANGE_HP_INTERVAL = 3;
static const int GHOST_REVIVAL_TIME = 0x258;
static const int MAX_FAVOR_KIND_MAX = 0x3;
static const int MAX_ITEM_HYPERLINK = 0x3;
static const int MAX_LIMIT_OF_STACK = 0x98967f;
static const int SCHOOL_NAME_LENGTH = 0x28;
static const int WEAPON_SUBTYPE_MAX = 0x6;
static const char ASSAULT_DIE_STAMINA = 30;
static const int CLEANPAD_IMAGE_SIZE = 0x3390;
static const char EXP_PER_CHAOS_POINT = 1;
static const int MAX_ITEM_LOCK_COUNT = 0x64;
static const char ONETIME_CHAOS_POINT = 10;
static const char UPGRAGE_CHECK_LIMIT = 10;
static const int CLEANPAD_IMAGE_WIDTH = 0xdc;
static const int EVENT_071218_MONEY_1 = 0x249f0;
static const int EVENT_071218_MONEY_2 = 0x7a120;
static const int EVENT_071218_MONEY_3 = 0xf4240;
static const int EXPERT_JOB_MAX_LEVEL = 0xb;
static const int FIRST_JOB_CHANGE_LEV = 0xf;
static const char MAX_CHAR_NAME_LENGTH = 29;
static const int MAX_DEATHTOWER_STAGE = 0xc8;
static const int MAX_GUILD_CARGO_SLOT = 0x78;
static const int MAX_POWER_JOIN_COUNT = 0x80;
static const int PVP_RELAY_COUNT_TIME = 0x1220;
static const int PVP_ROOM_NAME_LENGTH = 0x1e;
static const int ROOM_PASSWORD_LENGTH = 0x9;
static const int SSO_EXPIRE_TIME_TERM = 0xb4;
static const int TITLE_ITEM_BASE_TIME = 0x44a54a80;
static const int WINE_SHOP_AREA_INDEX = 0x4;
static const int YUSHA_EVENT_END_TIME = 0x47b208d0;
static const int BASE_GUILD_CARGO_SLOT = 0x8;
static const int CLEANPAD_IMAGE_HEIGHT = 0x3c;
static const int EVENT_070726_END_TIME = 0x46e1bbd0;
static const int EVENT_071218_END_TIME = 0x478e7050;
static const char EXP_GUILD_SKILL_INDEX = 201;
static const int GUILD_SKILL_END_INDEX = 0xd1;
static const short MAX_LEARN_GUILD_SKILL = 16;
static const int SECOND_JOB_CHANGE_LEV = 0x32;
static const char ASSAULT_DISTANCE_LIMIT = 100;
static const int AVATAR_NULL_ITEM_INDEX = 0x98967f;
static const int CLEANPAD_STRING_LENGTH = 0x5;
static const int DEFAULT_LOGIN_PASSWORD = 0x31;
static const char DISCONNECT_DETECT_TERM = 10;
static const int HUMAN_CERTIFY_TIME_OUT = 0x3c;
static const int LEVEL_TRADE_GOLD_LIMIT = 0x2710;
static const int MAX_BLOODDUNGEON_ROUND = 0xc8;
static const int SPF_ADD_UNIT_PER_LEVEL = 0x1;
static const int YUSHA_EVENT_START_TIME = 0x47686d10;
static const int kReturnUserRewardMoney = 0x186a0;
static const int AVATARVARIATIONCOLORMAX = 0x2;
static const short CONST_REDUCE_RATE_CARGO = 10;
static const short CONST_REDUCE_RATE_EQUIP = 20;
static const char DUNGEON_HELP_PASS_LEVEL = 7;
static const int EVENT_070726_START_TIME = 0x46a7b9d0;
static const int GUILD_SKILL_START_INDEX = 0xc8;
static const int HUMAN_CERTIFY_WRONG_MAX = 0x3;
static const char MAX_GUILD_LEVEL_DUNGEON = 10;
static const int MAX_MOBILE_PHONE_LENGTH = 0xc;
static const char MAX_RANDOM_ENCHANT_RATE = 100;
static const int POWER_WAR_DUNGEON_INDEX = 0x64;
static const int RevivalItemContinueTime = 0x4b0;
static const int SSO_SELECT_CHANNEL_TIME = 0x124f80;
static const int AMPLIFY_INVEST_MIN_LEVEL = 0x37;
static const short CONST_REDUCE_RATE_AVATAR = 10;
static const int HENDONMYRE_VILLAGE_INDEX = 0x2;
static const int INGAME_EVENT_HISTORY_MAX = 0x1e;
static const int MAX_GOBLIN_PAD_CRYPT_KEY = 0x1388;
static const int MAX_GOBLIN_VALIDITY_TIME = 0xb4;
static const int MAX_SIGNED_INTEGER_VALUE = 0x7fffffff;
static const char MAX_SUB_GUILD_MASTER_CNT = 5;
static const int MAX_USER_POWER_WAR_POINT = 0x270f;
static const int PREMIUM_SKILL_DOWN_GRADE = 0x5;
static const int PVP_DM_BATTLE_COUNT_TIME = 0x1220;
static const int PVP_RELAY_CANDIDATE_TIME = 0x1388;
static const int SECURITY_CARD_POS_BUFFER = 0x5;
static const int TOWN_SEND_MOVE_INFO_TERM = 0x3e8;
static const int kCollectItemsRewardCount = 0x14;
static const int BONNYBUNNY_CREATURE_INDEX = 0xf68a;
static const int FADE_IN_TIME_RELAY_BATTLE = 0xa8c;
static const char TIMEOUT_DISCONNECT_DETECT = 30;
static const int CONST_GROW_CHARACTER_LEVEL = 0x14;
static const short CONST_MAX_AUTO_CLIENT_RATE = 100;
static const char CONST_MOVE_AREA_AREA_INDEX = 5;
static const int FADE_IN_TIME_NORMAL_BATTLE = 0x6a4;
static const int FADE_OUT_TIME_RELAY_BATTLE = 0x7d0;
static const int MAX_ACTIVE_INGAMEADV_COUNT = 0xc8;
static const int MAX_SECURITY_CARD_CERT_KEY = 0x1869f;
static const int PREMIUM_LEVEL_REDUCE_VALUE = 0x5;
static const int kLevelAchieveOfJobEventMax = 0x4;
static const short CONST_REDUCE_RATE_PVP_GRADE = 20;
static const int FADE_OUT_TIME_NORMAL_BATTLE = 0x3e8;
static const int FAIRPVP_EQUIPSLOT_GET_LEVEL = 0x2d;
static const int MAX_ENC_MOBILE_PHONE_LENGTH = 0x21;
static const int MaxExpertJobRecipeArraySize = 0x1e;
static const char TimePCRoomForbiddenIPClient = 5;
static const char TimePCRoomForbiddenIPServer = 120;
static const int kMultiBoxLotteryFailOverCnt = 0x3;
static const int EXPERT_JOB_MAXIMUM_LEVEL_GAP = 0x2;
static const int MAX_HASH_MOBILE_PHONE_LENGTH = 0x29;
static const int PVP_NORMAL_BATTLE_COUNT_TIME = 0x1220;
static const int RevisionDisjointMachineGrade = 0x1;
static const char kLimitStartServerCharacLevel = 50;
static const int kPcroomDailyRewardMaxCountTw = 0x4;
static const int APC_SCRIPT_ORDER_RANDOM_BEGIN = 0x40;
static const char CONST_MOVE_AREA_VILLAGE_INDEX = 4;
static const short CONST_REDUCE_RATE_EQUIP_BASIS = 9;
static const char DEFAULT_COLLISION_GROUP_INDEX = 0;
static const int HUMAN_CERTIFY_TOTAL_WRONG_MAX = 0xa;
static const int MAX_SECURITY_CARD_DATA_LENGTH = 0x27;
static const int YUSHA_EVENT_PACKAGE_SELL_TIME = 0x479fcc10;
static const char kLimitStartServerCreateCharac = 3;
static const int CONST_ASSAULT_EVADE_LIMIT_TIME = 0x5;
static const short CONST_REDUCE_RATE_AVATAR_FIRST = 40;
static const int EVENT_070726_PACKAGE_SELL_TIME = 0x46d5de50;
static const int MAX_SECURITY_CARD_MAX_FAIL_CNT = 0x2;
static const int MOUSE_REGISTER_MAX_ERROR_COUNT = 0x3;
static const int APC_SCRIPT_ORDER_SPECIFIC_BEGIN = 0x0;
static const short CONST_ASSAULT_MINIMUM_LEVEL_GAP = 8;
static const int INIT_EXPERT_JOB_STACKABLE_INDEX = 0xffffffff;
static const int MAX_SECURITY_CARD_VALIDITY_TIME = 0xb4;
static const int MOUSE_REGISTER_PASSWORD_MAX_LEN = 0x6;
static const int MOUSE_REGISTER_PASSWORD_MIN_LEN = 0x4;
static const int kInGameAdvertisementTitleLength = 0x32;
static const int ESCALADE_RETURN_TO_VILLAGE_COUNT = 0xea60;
static const int ESCALADE_TUTORIAL_BIT_FLAG_COUNT = 0x80;
static const int ESCALADE_TUTORIAL_SKIPABLE_LEVEL = 0xa;
static const char ESCALADE_TUTORIA_FLAG_BYTE_COUNT = 16;
static const int MAX_REPORT_MANNERLESS_USER_COUNT = 0x2;
static const int MONSTER_SCRIPT_ORDER_EVENT_BEGIN = 0x2710;
static const int MAX_SECURITY_CARD_AUTH_CANCEL_CNT = 0xa;
static const int MAX_SECURITY_CARD_CERT_KEY_LENGTH = 0x6;
static const int MONSTER_SCRIPT_ORDER_NORMAL_BEGIN = 0x0;
static const char CONST_INTEGRATED_PVP_UPGRADE_COUNT = 10;
static const int MAX_SECURITY_CARD_RE_ISSUE_PER_DAY = 0x3;
static const int EVENT_KEEP_GOING_MINIMUM_JOIN_COUNT = 0x7;
static const int MAX_SECURITY_CARD_SERVER_KEY_LENGTH = 0x21;
static const int MOUSE_REGISTER_PASSWORD_ENCRYPT_LEN = 0x8;
static const int MOUSE_REGISTER_PASSWORD_STRING_LENG = 0x7;
static const int CONST_GIVEUP_PENALTY_CHARACTER_LEVEL = 0x12;
static const int CONST_PARTY_MAP_LOADING_WAIT_TIMEOUT = 0x5a;
static const int POWER_WAR_DUNGEON_WAITING_AREA_INDEX = 0x7;
static const int POWER_WAR_DUNGEON_WAITING_TOWN_INDEX = 0x2;
static const short CONST_ASSAULT_FATIGUE_PER_CHAOS_POINT = 2;
static const int CONST_SINGLE_MAP_LOADING_WAIT_TIMEOUT = 0x78;
static const int MAX_SECURITY_CARD_SAME_PHONE_VALIDITY = 0x4;
static const int MAX_REPORT_MANNERLESS_USER_TEXT_LENGTH = 0xff;
static const int RESTRICTIVE_COMMERCIAL_TRANSACTION_ZONE_X = 0x50;
static const int RESTRICTIVE_COMMERCIAL_TRANSACTION_ZONE_Y = 0x96;
static const short CONST_ASSAULT_GOLD_PANALTY_CHAOS_POINT_MAX = 100;
static const short CONST_ASSAULT_GOLD_PANALTY_CHAOS_POINT_MIN = 1;
static const int MAX_ONLINE_PRELIMINARY_PVP_TEAM_NAME_LENGTH = 0xf;
static const int MOUSE_REGISTER_PASSWORD_ENCRYPT_STRING_LENG = 0x9;
static const int BONNYBUNNY_CREATURE_OVER_SKILL_CONTINUE_TIME = 0x14;
static const int BONNYBUNNY_CREATURE_OVER_SKILL_REVISION_TIME = 0x2;


static const int TITLE_BOOK_MAX[] = {
    70, 200, 120, 60,
};


static const int elixir_item_id[] = {
    1039, 1040, 1041, 1042, 1043, 1044, 1045, 1046,
};


static const int _S_CHARAC_JOB_MASTARY[] = {
    3, 2, 0, 3, 4, 2, 0, 2, 3, 2, 1, 1,
    3, 0, 1, 0, 0, 0, 1, 1, 3, 4, 2, 0,
    3, 1, 1, 3, 0, 1, 1, 1, 2, 1, 1, 2,
    0, 2, 3, 2, 0, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
};

#ifdef ENABLE_REDNF_LOG
#define REDNF_LOG(fmt, ...) ArchiveLog(fmt, ##__VA_ARGS__)
#else
#define REDNF_LOG(fmt, ...)
#endif

#endif  // REDNF_COMMON_CONSTANTS_H_
