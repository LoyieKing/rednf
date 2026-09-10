void CTitleBookAdjust::execute()
{
    ((CTitleBook*)GetUser()->GetCharacExpandData(
        (ENUM_CHARAC_EXPAND_TYPE)0xe))->_gm_clear();
    ((CAchievement*)GetUser()->GetCharacExpandData(
        (ENUM_CHARAC_EXPAND_TYPE)0xf))->_gm_init();
    ((CAchievement*)GetUser()->GetCharacExpandData(
        (ENUM_CHARAC_EXPAND_TYPE)0xf))->adjust();
    ((CTitleBook*)GetUser()->GetCharacExpandData(
        (ENUM_CHARAC_EXPAND_TYPE)0xe))->sendList();
    ((CAchievement*)GetUser()->GetCharacExpandData(
        (ENUM_CHARAC_EXPAND_TYPE)0xf))->sendList();
