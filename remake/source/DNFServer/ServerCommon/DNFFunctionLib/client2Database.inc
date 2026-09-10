bool CodePage::client2Database(char* src, char* dst) {
    if (gDatabaseEncoding.compare(gClientEncoding) == 0) {
        return 1;
    }
    return DNFFLib::CharacSetSwitch(gClientEncoding.c_str(), gDatabaseEncoding.c_str(), src, dst);
