// ORIG T 0x08174aec
bool ConfirmQuery::setQuery(MySQL& db)
{
    return db.set_query("call usp_confirm(%s, @out_code)",
                        NumberToString(m_trans.m_tranNo, 0));
}