// ----------------------------------------------------------------------------
// ORIG 0x0868bf14 CUser::select_charac_set_visible_values(char value) void
void CUser::select_charac_set_visible_values(char value)
{
    set_charac_visible_values(value);
    SetVisible();
    if (get_charac_visible_grow_avatar())
        GetGameOptionRef()->set_etc_option((ENUM_OPTION_ETC)1, 1);
    else
        GetGameOptionRef()->set_etc_option((ENUM_OPTION_ETC)1, 0);
}