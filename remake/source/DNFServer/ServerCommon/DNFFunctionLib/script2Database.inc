bool CodePage::script2Database(char* src, char* dst) {
    if (gDatabaseEncoding.compare(gScriptEncoding) == 0) {
        return 1;
    }
    return DNFFLib::CharacSetSwitch(gScriptEncoding.c_str(), gDatabaseEncoding.c_str(), src, dst);
}
