// ORIG 0x081dd7fe（报告 Dispatcher_ItemDictionary/process.md）：
// err>0 直接 0；err<0 记 LineFunc(0x4e52)；否则 0。
// b28c-L20: AE — 具名 stItemDictionary* 局部（-0x10 槽）+ 三次重读 p->m_4；
//   >0 早返 0 / <0 return LineFunc / ==0 落 msg 死槽后 return 0（P0 槽往返判读）。
int Dispatcher_ItemDictionary::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stItemDictionary* p = static_cast<stItemDictionary*>(&param);
    p->m_4 = check_error(user, msg);
    if (p->m_4 > 0)
        return 0;
    if (p->m_4 < 0)
        return LineFunc(0x4e52,
                        __PRETTY_FUNCTION__,
                        p->m_4, 0);
    MSG_BASE* pm = &msg;
    (void)pm;
    return 0;
}