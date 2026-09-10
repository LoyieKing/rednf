void CTitleBookReset::execute()
{
    ((CTitleBook*)GetUser()->GetCharacExpandData(
        (ENUM_CHARAC_EXPAND_TYPE)0xe))->_gm_clear();
    ((CTitleBook*)GetUser()->GetCharacExpandData(
        (ENUM_CHARAC_EXPAND_TYPE)0xe))->sendList();
