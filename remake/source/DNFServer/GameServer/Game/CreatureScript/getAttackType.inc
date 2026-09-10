int AttackInfo::getAttackType(std::string& tag)
{
    int type = 7;
    if (tag == "attack type physical")
        type = 0;
    else if (tag == "attack type magical")
        type = 1;
    else if (tag == "attack type absolute")
        type = 2;
    return type;
