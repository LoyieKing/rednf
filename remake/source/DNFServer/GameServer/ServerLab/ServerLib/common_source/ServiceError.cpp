// nsl::ServiceError.cpp
#include <stdio.h>
#include <map>
#include <string>

#include "ServiceError.h"
#include "TraceLog.h"

namespace nsl {

std::map<int, std::string> ___errstr_map___;
char ___errstr_tmp___[2048];

void InitServiceErrorStr()
{
    sprintf(___errstr_tmp___, "[%d:%s]", ERROR_NOT_USE, "***GetErrorStr() failed***");
    ___errstr_map___.insert(std::make_pair<COMMON_ERROR_LIST, char(&)[2048]>(COMMON_ERROR_LIST(ERROR_NOT_USE), ___errstr_tmp___));
    sprintf(___errstr_tmp___, "[%d:%s]", ERROR_NONE_, "ERROR_NONE_");
    ___errstr_map___.insert(std::make_pair<COMMON_ERROR_LIST, char(&)[2048]>(COMMON_ERROR_LIST(ERROR_NONE_), ___errstr_tmp___));
    sprintf(___errstr_tmp___, "[%d:%s]", ERROR_UNKNOWN_FAIL_, "ERROR_UNKNOWN_FAIL_");
    ___errstr_map___.insert(std::make_pair<COMMON_ERROR_LIST, char(&)[2048]>(COMMON_ERROR_LIST(ERROR_UNKNOWN_FAIL_), ___errstr_tmp___));
    sprintf(___errstr_tmp___, "[%d:%s]", ERROR_DB_CONNECTION_LOST, "ERROR_DB_CONNECTION_LOST");
    ___errstr_map___.insert(std::make_pair<COMMON_ERROR_LIST, char(&)[2048]>(COMMON_ERROR_LIST(ERROR_DB_CONNECTION_LOST), ___errstr_tmp___));
    sprintf(___errstr_tmp___, "[%d:%s]", ERROR_STOP_CALLED_, "ERROR_STOP_CALLED_");
    ___errstr_map___.insert(std::make_pair<COMMON_ERROR_LIST, char(&)[2048]>(COMMON_ERROR_LIST(ERROR_STOP_CALLED_), ___errstr_tmp___));
}

const char* GetErrorStr(int err)
{
    std::map<int, std::string>::iterator it = ___errstr_map___.find(err);
    if (it == ___errstr_map___.end())
    {
        G_TraceLog()->sysLog(7, "GetErrorStr() failed. error_no : %d\n", err);
        sprintf(___errstr_tmp___, "[%d:%s]", -1, "***GetErrorStr() failed***");
        return ___errstr_tmp___;
    }
    return it->second.c_str();
}

} // namespace nsl

using namespace nsl;

void SetAuctionServiceErrorStr()
{
    nsl::InitServiceErrorStr();
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 4, "ERROR_AUCTION_FAIL_ID_CREATE_");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(4), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 5, "ERROR_AUCTION_CONNECTION_LOST_");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(5), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 3, "ERROR_STOP_CALLED_");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(3), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 2, "ERROR_DB_CONNECTION_LOST");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(2), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 6, "ERROR_AUCTION_CONNECT_GA_FAIL_");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(6), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 7, "ERROR_AUCTION_REGIST_GA_FAIL_ID_DUPLICATE_");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(7), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 8, "ERROR_AUCTION_STOP_CALLED_");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(8), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 9, "ERROR_AUCTION_FAIL_MEMORY_ALLOC_");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(9), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 10, "ERROR_AUCTION_FAIL_REGIST_ITEM_");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(10), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 11, "ERROR_AUCTION_FAIL_UNREGIST_ITEM_");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(11), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 12, "ERROR_AUCTION_FAIL_INSERT_CHARACTER_");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(12), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 13, "ERROR_AUCTION_FAIL_INSERT_AVERAGE_PRICE_");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(13), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 14, "ERROR_AUCTION_FAIL_INSERT_BY_ITEM_ID");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(14), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 15, "ERROR_AUCTION_FAIL_INSERT_BY_CATEGORY_RARITY_UPGRADE_LV");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(15), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 16, "ERROR_AUCTION_FAIL_INSERT_BY_CATEGORY_RARITY_UPGRADE");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(16), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 17, "ERROR_AUCTION_FAIL_INSERT_BY_CATEGORY_UPGRADE_LV");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(17), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 18, "ERROR_AUCTION_FAIL_INSERT_BY_CATEGORY_UPGRADE");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(18), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 19, "ERROR_CANNOT_FIND_AUCTION_DATA_TO_DELETE_FROM_SEARCH_MODULE");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(19), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 20, "ERROR_AUCTION_FAIL_SEARCH_BY_ITEM_ID");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(20), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 21, "ERROR_AUCTION_FAIL_SEARCH_BY_CATEGORY_RARITY_UPGRADE_LV");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(21), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 22, "ERROR_AUCTION_FAIL_SEARCH_BY_CATEGORY_RARITY_UPGRADE");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(22), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 23, "ERROR_AUCTION_FAIL_SEARCH_BY_CATEGORY_UPGRADE_LV");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(23), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 24, "ERROR_AUCTION_FAIL_SEARCH_BY_CATEGORY_UPGRADE");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(24), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 25, "ERROR_INVALID_CATEGORY_ID");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(25), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 26, "ERROR_INVALID_RARITY");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(26), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 27, "ERROR_INVALID_LV");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(27), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 28, "ERROR_INVALID_UPGRADE");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(28), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 29, "ERROR_INVALID_ITEM_COUNT");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(29), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 30, "ERROR_AUCTION_EXCEED_REGISTED_ITEM_NUM_");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(30), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 31, "ERROR_AUCTION_EXCEED_INSTANT_PRICE_");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(31), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 32, "ERROR_AUCTION_WRONG_ITEM_UPGRADE_VALUE_");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(32), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 33, "ERROR_AUCTION_WRONG_AVERAGE_PRICE_");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(33), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 34, "ERROR_AUCTION_INVALID_CHARACTER_ID_");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(34), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 35, "ERROR_AUCTION_INVALID_ITEM_ID_");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(35), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 36, "ERROR_AUCTION_INVALID_AUCTION_ID_");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(36), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 37, "ERROR_AUCTION_INVALID_BIDDING_PRICE_");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(37), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 38, "ERROR_AUCTION_INVALID_PRICE_");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(38), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 39, "ERROR_AUCTION_INVALID_ITEM_INFO_");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(39), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 40, "ERROR_AUCTION_INVALID_EXPIRED_ITEM_");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(40), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 41, "ERROR_AUCTION_INVALID_ARGUMENT_");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(41), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 42, "ERROR_AUCTION_REGISTER_UNAUTHORIZE_");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(42), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 43, "ERROR_AUCTION_UNREGISTER_UNAUTHORIZE_");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(43), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 44, "ERROR_AUCTION_NOT_EXIST_REGISTED_ITEM_");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(44), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 45, "ERROR_AUCTION_BIDDING_MY_ITEM");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(45), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 46, "ERROR_AUCTION_BIDDING_ALREADY");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(46), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 47, "ERROR_AUCTION_BIDDING_MONEY_SHORTAGE");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(47), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 48, "ERROR_AUCTION_NO_PRIVATE_STORE_OPEN_");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(48), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 49, "ERROR_AUCTION_BUSY");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(49), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 50, "ERROR_AUCTION_INVALID_AUCTION_TYPE");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(50), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 51, "ERROR_AUCTION_NOT_READY");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(51), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 52, "ERROR_AUCTION_REGIST_GP_FAIL_ID_DUPLICATE_");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(52), nsl::___errstr_tmp___));
    sprintf(nsl::___errstr_tmp___, "[%d:%s]", 54, "ERROR_AUCTION_END_");
    nsl::___errstr_map___.insert(std::make_pair<AUCTION_ERROR_LIST, char(&)[2048]>(AUCTION_ERROR_LIST(54), nsl::___errstr_tmp___));
}
