// 称号簿清空 + 成就(0xf)_gm_init/adjust + 双列表重发（顺序照 ORIG）。
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
}