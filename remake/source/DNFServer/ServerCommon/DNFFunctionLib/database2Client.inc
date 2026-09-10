bool CodePage::database2Client(char* src, char* dst) {
    if (gDatabaseEncoding.compare(gClientEncoding) == 0) {
        return 1;
    }
    return DNFFLib::CharacSetSwitch(gDatabaseEncoding.c_str(), gClientEncoding.c_str(), src, dst);
}
