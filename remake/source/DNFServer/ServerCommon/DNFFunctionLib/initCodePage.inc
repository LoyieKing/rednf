void CodePage::initCodePage() {
    gClientEncoding = "UTF-8";
#if defined(DNF_SVC_AUCTION) || defined(DNF_SVC_POINT) || defined(DNF_SVC_GUILD)
    // auction/point/guild ORIG 实测 script 编码为 GBK。
    gScriptEncoding = "GBK";
#else
    // manager/monitor/statics/community/dbmw 等其余服务 ORIG 实测为 BIG5。
    gScriptEncoding = "BIG5";
#endif
    gDatabaseEncoding = "UTF-8";
}
