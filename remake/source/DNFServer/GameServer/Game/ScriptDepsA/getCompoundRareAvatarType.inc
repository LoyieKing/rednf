int getCompoundRareAvatarType(const std::string& token)
{
    if (token == "[hat rare avatar]") return 0;
    if (token == "[hair rare avatar]") return 1;
    if (token == "[face rare avatar]") return 2;
    if (token == "[neck rare avatar]") return 6;
    if (token == "[coat rare avatar]") return 3;
    if (token == "[pants rare avatar]") return 4;
    if (token == "[belt rare avatar]") return 7;
    if (token == "[shoes rare avatar]") return 5;
    if (token == "[skin rare avatar]") return 8;
    if (token == "[aurora rare avatar]") return 9;
    return 0x16;
