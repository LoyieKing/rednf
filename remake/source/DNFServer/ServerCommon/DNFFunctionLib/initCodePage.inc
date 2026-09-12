void CodePage::initCodePage() {
    gClientEncoding = "UTF-8";
#if defined(DNF_SVC_AUCTION) || defined(DNF_SVC_POINT) || defined(DNF_SVC_GUILD) || defined(DNF_SVC_GAME)
    // auction/point/guild/game ORIG 实测 script 编码为 GBK
    // （game=df_game_r：0x8108da2 `movl $0x8b446d1,0x4(%esp)`，raw@0x8b446d1 = 47 42 4b）。
    gScriptEncoding = "GBK";
#else
    // manager/monitor/statics/community/dbmw 等其余服务 ORIG 实测为 BIG5。
    gScriptEncoding = "BIG5";
#endif
    gDatabaseEncoding = "UTF-8";
}
