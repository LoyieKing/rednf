int BlueMarbleTileScript::toType(std::string type)
{
    if (type == "empty") return 0;
    if (type == "start") return 1;
    if (type == "end") return 2;
    if (type == "gold") return 3;
    if (type == "exp") return 4;
    if (type == "drop") return 5;
    if (type == "item") return 6;
    if (type == "unique item") return 7;
    if (type == "boss unique") return 8;
    if (type == "move") return 9;
    if (type == "jump") return 0xa;
    if (type == "gold goblin") return 0xb;
    if (type == "rope") return 0xc;
    if (type == "kick") return 0xd;
    return 0;
