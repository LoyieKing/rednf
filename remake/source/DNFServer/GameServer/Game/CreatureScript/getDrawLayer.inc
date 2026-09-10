int getDrawLayer(const char* s)
{
    if (strcmp(s, "[close]") == 0) return 0;
    if (strcmp(s, "[normal]") == 0) return 1;
    if (strcmp(s, "[bottom]") == 0) return 2;
    if (strcmp(s, "[closeback]") == 0) return 3;
    if (strcmp(s, "[middleback]") == 0) return 4;
    if (strcmp(s, "[distantback]") == 0) return 5;
    if (strcmp(s, "[cover]") == 0) return 6;
    return 7;
