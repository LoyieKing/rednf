int getCharacterJobToEnumValue(std::string& job)
{
    if (job == "swordman")
        return 0;
    if (job == "fighter")
        return 1;
    if (job == "gunner")
        return 2;
    if (job == "mage")
        return 3;
    if (job == "priest")
        return 4;
    if (job == "at gunner")
        return 5;
    if (job == "thief")
        return 6;
    if (job == "at fighter")
        return 7;
    if (job == "at mage")
        return 8;
    if (job == "demonic swordman")
        return 9;
    if (job == "creator mage")
        return 10;
    if (job == "common")
        return 11;
    return -1;
