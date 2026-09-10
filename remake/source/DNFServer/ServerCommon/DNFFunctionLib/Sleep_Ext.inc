void DNFFLib::Sleep_Ext(int sec, int usec) {
    timeval tv;
    tv.tv_sec = sec;
    tv.tv_usec = usec;
    select(0, NULL, NULL, NULL, &tv);
}
