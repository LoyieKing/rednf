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
