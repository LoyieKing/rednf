// ---------------------------------------------------------------------------
// DB_InsertQueryCounterLogdb::dispatch —— ORIG 0x08425cde
// ---------------------------------------------------------------------------
// ORIG：qid=0/len=0 → memset(buf,0,0x6001) → >>qid → >>len →
// get_binary(buf,len) → GetDBHandle(4,0) → GetQueryCounter() →
// RegisterQuery(buf,qid,0)；返回 RegisterQuery==1（xor $1/test 形态）。
// [b28d-solo-3] AE：P0/P52 链式 GetQueryCounter（无 counter 槽）+ P101 早返
// （if(!RegisterQuery)return 0; return 1;）+ 声明序 qid/len 前置 + `char
// buf[0x6001]={'\0'}`（P312 复现 ORIG lea;mov $0x6001,%edx;rep stos 慢路）。
// ORIG 08425cde。
bool DB_InsertQueryCounterLogdb::dispatch(int userIdx, int slot, Stream* stream)
{
    int qid = 0;
    int len = 0;
    char buf[0x6001] = { '\0' };
    *stream >> qid;
    *stream >> len;
    stream->get_binary(buf, len);
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    // P0/P52：ORIG 无 counter 槽往返（GetQueryCounter 返回值直作 this）⇒ 链式。
    if (!db->GetQueryCounter()->RegisterQuery(buf, qid, 0))
        return 0;
    return 1;
}