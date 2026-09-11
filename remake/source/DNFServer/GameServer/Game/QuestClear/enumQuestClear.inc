void CQuestClear::enumQuestClear(bool (*cb)(unsigned int, char*), ...) const
{
    // ORIG 0x0808bb88：va_start 后对 i=1..0x752f 调 cb(i,(char*)ap)；setle 有符号比较（P2）。
    va_list ap;
    va_start(ap, cb);
    for (int i = 1; i <= 0x752f; ++i)
        cb(i, (char*)ap);
    va_end(ap);
}