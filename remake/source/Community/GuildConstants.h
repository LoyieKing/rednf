//
// 公会/成员常量定义（原始 .rodata 逆向提取，2026-08-07）
// 原始为头文件内 static const 常量，被 7 个 TU 包含（LOGIN_FLAG/MAX_GUILD_CNT 等）。
//

#ifndef REDNF_GUILD_CONSTANTS_H_
#define REDNF_GUILD_CONSTANTS_H_

static const char LOGIN_FLAG = 1;
static const char LOGOUT_FLAG = 0;
static const char MEMBER_BOTH = 2;
static const char MEMBER_NONE = 3;
static const int REASON_BIND = 0x0;
static const int COIN_ITEM_NO = 0x1;
static const char MAX_CHAT_MSG = 255;
static const int MAX_PEER_CNT = 0x3e8;
static const char MEMBER_ENTER = 1;
static const char MEMBER_LOWER = 2;
static const char MEMBER_UPPER = 1;
static const char DB_QUERY_FAIL = 0;
static const int DIFF_CONST_10 = 0x32;
static const int DIFF_CONST_20 = 0x64;
static const int DIFF_CONST_30 = 0xc8;
static const int DIFF_CONST_50 = 0x1f4;
static const char GUILD_LEVEL_1 = 0;
static const char GUILD_LEVEL_2 = 1;
static const char GUILD_LEVEL_3 = 2;
static const short MAX_GUILD_CNT = 10000;
static const char MAX_ID_LENGTH = 19;
static const char MAX_IP_LENGTH = 15;
static const int MAX_TOWER_NUM = 0x5;
static const char MEMBER_SECEDE = 2;
static const int DIFF_CONST_100 = 0x3e8;
static const int DIFF_CONST_200 = 0x1388;
static const char FILEOPEN_ERROR = 255;
static const int MAX_GUILD_FUND = 0x1312d00;
static const short MAX_MEMBER_CNT = 20000;
static const short MAX_PACKET_CNT = 10000;
static const int MAX_SLANG_SIZE = 0x32;
static const int MAX_SPEC_COUNT = 0xff;
static const char MEMBER_JUSTONE = 1;
static const int RANDOMBOX_KIND = 0x5;
static const char HEART_BEAT_TIME = 20;
static const short MAX_WORD_NUMBER = 65535;
static const char MEMBER_ENTER_OK = 1;
static const char OVER_LINE_PARSE = 254;
static const int REASON_DISJOINT = 0x1;
static const char DB_QUERY_SUCCESS = 1;
static const int MAX_DWORD_NUMBER = 0xffffffff;
static const int STRING_LENGTH_20 = 0x14;
static const short DB_FLAG_MAX_STATE = 4096;
static const char GUILD_INVITE_TIME = 1;
static const char GUILD_LEVEL_UP_OK = 0;
static const int MAX_AGENCY_NO_LEN = 0x18;
static const char MAX_DB_ACC_LENGTH = 20;
static const char MAX_DB_PWD_LENGTH = 20;
static const short MAX_GUILD_MEM_CNT = 300;
static const char STATE_USER_LOGOUT = 0;
static const int STATISTIC_MAX_AGE = 0x64;
static const short DB_FLAG_LOAD_STATE = 4;
static const short DB_FLAG_NULL_STATE = 0;
static const char DB_QUERY_GET_ERROR = 3;
static const char DB_QUERY_NOT_EXIST = 2;
static const int MAX_BLACK_USER_CNT = 0x186a0;
static const char MAX_COMMENT_LENGTH = 255;
static const char MAX_DB_NAME_LENGTH = 30;
static const short MAX_SEND_USER_INFO = 75;
static const int MAX_TOWER_RANK_NUM = 0x9c40;
static const char BLACKLIST_CNT_LIMIT = 10;
static const short DB_FLAG_QUERY_STATE = 2;
static const char MAX_GUILD_JOIN_LIST = 50;
static const int MAX_PACKET_SIZE_TCP = 0x186a0;
static const int MAX_PACKET_SIZE_UDP = 0x1800;
static const int MAX_POWER_WAR_POINT = 0x5f5e0ff;
static const int MAX_RESTRICT_STRING = 0x2d;
static const char MEMBER_ENTER_REJECT = 2;
static const char TIME_LOGOUT_WAIT_DB = 10;
static const int kMaxNpcBuyLimitItem = 0x1e;
static const char MAX_LEN_OF_IP_STRING = 15;
static const short MAX_LOWER_MEMBER_CNT = 10;
static const short MAX_MONITOR_USER_CNT = 28000;
static const short MAX_READ_LINE_LENGTH = 1024;
static const int MAX_TOWER_MEMBER_NUM = 0x4;
static const char HEART_BEAT_MISS_COUNT = 20;
static const int MAX_AVATAR_PART_COUNT = 0x9;
static const char MAX_C_CLASS_IP_LENGTH = 12;
static const char MAX_GUILD_NAME_LENGTH = 22;
static const short MAX_PROHIBIT_USER_CNT = 10000;
static const int MAX_RESTRICT_LIST_NUM = 0x64;
static const int MEMBER_DB_LOAD_BEFORE = 0x0;
static const char WAIT_TIME_ALL_MONITOR = 2;
static const int kNpcBuyLimitItemCount = 0x1;
static const char GUILD_WAR_CHANNEL_TYPE = 6;
static const int IS_NOT_THERE_MEMBER_ST = 0xfffffffe;
static const int MAX_AVATAR_GRADE_COUNT = 0x3;
static const int MAX_GUILD_FUND_FOR_LV17 = 0x989680;
static const char MAX_MAP_USERS_ARRAY_CNT = 10;
static const short MAX_MEMBER_EXP_READ_CNT = 11;
static const int MAX_POWER_WAR_USER_RANK = 0x1f4;
static const short MAX_TCP_RECV_PACKET_CNT = 1000;
static const short MAX_TCP_SEND_PACKET_CNT = 1000;
static const char DISPLAY_GUILD_RANK_LIMIT = 100;
static const char GUILD_LEVEL_ERR_NO_MONEY = 1;
static const char GUILD_MEMBER_SAVE_LOGOUT = 1;
static const int MAX_DBMW_SAVE_SPEC_COUNT = 0x1b4;
static const char MAX_DB_PWD_HEXSTR_LENGTH = 60;
static const char MAX_GUILD_ADDRESS_LENGTH = 12;
static const int MAX_POWER_WAR_GUILD_RANK = 0x64;
static const int MAX_POWER_WAR_POINT_INFO = 0xfa;
static const int MAX_PVP_CHANNEL_INFO_NUM = 0x17e;
static const int MAX_SEND_TCP_PACKET_SIZE = 0x96000;
static const char MEMBER_ENTER_REJECT_SOLO = 3;
static const short RECV_QUEUE_CNT_LIMIT_LOG = 100;
static const char CALL_GUILD_INFO_CNT_LIMIT = 95;
static const char GUILD_LEVEL_ERR_END_LEVEL = 3;
static const int GUILD_LEVEL_EVENT_ITEM_NO = 0x65d3;
static const int IS_NOT_THERE_UPPER_MEMBER = 0xffffffff;
static const int MAX_DISJOINT_AVATAR_COUNT = 0x2;
static const int MAX_POWER_WAR_BONUS_COUNT = 0xfa;
static const int MAX_SECRET_SHOP_STATISTIC = 0xc8;
static const char STATE_USER_LOGOUT_WAIT_DB = 1;
static const short SYNC_DB_SCRIPT_CHANNEL_NO = 1;
static const char GUILD_LEVEL_ERR_MEMBER_CNT = 4;
static const char GUILD_LEVEL_ERR_NOT_MASTER = 2;
static const short GUILD_WAR_POINT_INIT_VALUE = 1000;
static const int MAX_AVATAR_DISJOINT_REASON = 0x2;
static const char MAX_GUILD_CALL_NAME_LENGTH = 12;
static const char MAX_GUILD_JOIN_MEMO_LENGTH = 25;
static const short MAX_MEMBER_CONFIG_READ_CNT = 10;
static const int MAX_PACKET_SIZE_UDP_DOUBLE = 0x200;
static const int MAX_TOWER_RANK_ELEMENT_NUM = 0x3c;
static const char WEB_NOTICE_SERVER_NO_LIMIT = 20;
static const int MAX_PACKET_SIZE_AUCTION_TCP = 0x5000;
static const char STATE_USER_REACH_GAME_WORLD = 3;
static const int HALF_MAX_POWER_WAR_USER_RANK = 0xfa;
static const int MAX_DBMW_QUERY_STRING_LENGTH = 0x1000;
static const short MAX_GUILD_MEMBER_CNT_1_LEVEL = 300;
static const short MAX_GUILD_MEMBER_CNT_2_LEVEL = 300;
static const short MAX_GUILD_MEMBER_CNT_3_LEVEL = 300;
static const char MAX_GUILD_MEMBER_NAME_LENGTH = 29;
static const char MAX_GUILD_WAR_COCACOLRA_RANK = 10;
static const char MAX_GUILD_WAR_ENTERABLE_RANK = 10;
static const int MAX_TOWER_RANK_BY_MEMBER_NUM = 0x2710;
static const char STATE_USER_SELECT_CHAR_SCREEN = 2;
static const char GUILD_MEMBER_SAVE_CHANGE_GRADE = 3;
static const int MAX_DBMW_SAVE_ERROR_LINE_COUNT = 0x264;
static const int MAX_DOUBLE_CONN_CHECK_USER_CNT = 0x30d40;
static const char MAX_GUILD_MEMBERS_ONE_SEND_CNT = 96;
static const int MAX_POWER_WAR_STATUE_RANK_USER = 0x3;
static const char UPPER_MEMBER_ENTER_LEVEL_LIMIT = 18;
static const char GUILD_MEMBER_SAVE_GUILD_PROCESS = 2;
static const char GUILD_WAR_RANK_PROCESS_INTERVAL = 2;
static const char MAX_PCROOM_MEMBERS_ONE_SEND_CNT = 40;
static const char MEMBER_ENTER_REJECT_INVITE_DENY = 4;
static const int STATISTIC_PROXY_KEY_VALUE_LENGTH = 0x38;
static const int STATISTIC_PROXY_FIELD_NAME_LENGTH = 0x20;
static const int STATISTIC_PROXY_TABLE_NAME_LENGTH = 0x20;
static const char GUILD_WAR_DB_SAVE_PROCESS_INTERVAL = 1;
static const int MAX_DBMW_QUERY_STRING_SHORT_LENGTH = 0x800;
static const int GUILD_WAR_FIRST_RANK_REWARD_ITEM_NO = 0x65b2;
static const char MAX_DBMW_GUILD_MEMBERS_ONE_SEND_CNT = 93;
static const int MAX_DBMW_SAVE_CUBE_STATISTICS_COUNT = 0x1d6;
static const int MAX_DBMW_SAVE_STATISTIC_PARTY_COUNT = 0x64;
static const char FATIGUE_APPLY_LEVEL_FOR_UPPER_MEMBER = 20;
static const int MAX_DBMW_SAVE_HELLPARTY_ITEM_STATISTIC = 0xa8;
static const int GUILD_WAR_COCACOLRA_RANK_REWARD_ITEM_NO = 0x65b3;
static const int MAX_DBMW_SAVE_STATISTIC_PARTY_JOB_COUNT = 0xf3;
static const int MAX_DBMW_SAVE_TING_USER_TIMECHECK_COUNT = 0x2fe;
static const int MAX_DBMW_SAVE_STATISTIC_PARTY_CHARAC_COUNT = 0x59;
static const int MAX_DBMW_SAVE_STATISTICS_POWERWAR_LAG_COUNT = 0x17;
static const int MAX_DBMW_SAVE_STATISTIC_ASSERT_MANAGER_COUNT = 0x9;
static const int MAX_DBMW_SAVE_STATISTIC_DEATHTOWER_VALUE_COUNT = 0x197;
static const int MAX_DBMW_SAVE_STATISTICS_POWERWAR_LOADING_COUNT = 0x17;
static const int MAX_DBMW_SAVE_STATISTIC_USER_TING_TIMECHECK_COUNT = 0x2fe;
static const int MAX_DBMW_SAVE_STATISTIC_DEATHTOWER_PLAYDATA_JOB_COUNT = 0x17e;
static const int MAX_DBMW_SAVE_STATISTIC_DEATHTOWER_PLAYDATA_PARTY_COUNT = 0x264;
static const char MEMBER_NO = 3;


static const int guild_att_phase[] = {
    5, 10, 20, 35, 60, 100, 150, 220, 300,
};


static const int guild_att_exp[] = {
    1, 3, 4, 8, 10, 18, 29, 46, 70, 77, 112, 158,
    218, 236, 307, 386, 0, 1, 4, 4, 9, 11, 18, 29,
    46, 70, 78, 113, 159, 219, 236, 307, 387, 0, 2, 6,
    8, 17, 21, 36, 59, 91, 140, 155, 224, 316, 437, 472,
    614, 773, 0, 2, 10, 12, 25, 31, 53, 88, 138, 209,
    232, 337, 475, 655, 708, 920, 1159, 0, 5, 16, 21, 42,
    52, 90, 146, 229, 348, 387, 561, 792, 1093, 1179, 1534, 1933,
    0, 7, 26, 32, 67, 83, 143, 234, 367, 557, 620, 899,
    1266, 1748, 1888, 2454, 3092, 0, 9, 33, 41, 84, 104, 179,
    292, 458, 697, 774, 1122, 1583, 2184, 2360, 3067, 3865, 0, 13,
    46, 56, 117, 145, 251, 409, 642, 975, 1084, 1572, 2217, 3059,
    3303, 4295, 5411, 0, 14, 52, 65, 135, 166, 286, 468, 734,
    1115, 1239, 1797, 2533, 3496, 3775, 4907, 6184, 0,
};

#endif  // REDNF_GUILD_CONSTANTS_H_
