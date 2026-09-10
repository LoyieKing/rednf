bool CodePage::database2Script(char* src, char* dst) {
    if (gDatabaseEncoding.compare(gScriptEncoding) == 0) {
        return 1;
    }
    return DNFFLib::CharacSetSwitch(gDatabaseEncoding.c_str(), gScriptEncoding.c_str(), src, dst);
}
