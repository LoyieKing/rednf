// ----------------------------------------------------------------------------
// ORIG T 0x0868dd44 _ZN5CUser17get_SoloPlay_modeEv：
// GetGameOptionRef()→get_etc_option(0x1b) short→int 符号扩展返回。
// ----------------------------------------------------------------------------
int CUser::get_SoloPlay_mode()
{
    CGameOption* opt = GetGameOptionRef();
    short v = opt->get_etc_option((ENUM_OPTION_ETC)0x1b);
    return (int)v;
}