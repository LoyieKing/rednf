int CUser::get_SoloPlay_mode()
{
    CGameOption* opt = GetGameOptionRef();
    short v = opt->get_etc_option((ENUM_OPTION_ETC)0x1b);
    return (int)v;
