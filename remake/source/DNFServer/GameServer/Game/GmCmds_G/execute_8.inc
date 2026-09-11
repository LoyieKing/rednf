// 称号簿(扩展数据 0xe)：_gm_clear 后重发列表（ORIG 每次 GetUser 重取）。
void CTitleBookReset::execute()
{
    ((CTitleBook*)GetUser()->GetCharacExpandData(
        (ENUM_CHARAC_EXPAND_TYPE)0xe))->_gm_clear();
    ((CTitleBook*)GetUser()->GetCharacExpandData(
        (ENUM_CHARAC_EXPAND_TYPE)0xe))->sendList();
}