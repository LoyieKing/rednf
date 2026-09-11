// ORIG 0x0812006e：SetEventFlag(false) + cMyTrace(__FUNCTION__,0x1e,0)。
// ORIG W 0x0812011c：恒返 1（该事件常驻生效）。
bool LevelUpBefore70LvEvent::IsEventing(CUser* /*user*/) const
{
    return 1;
}