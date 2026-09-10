int getStackableActionUsablePlace(std::string& place)
{
    if (place == "[village]")
    {
        return 1;
    }
    if (place == "[dungeon]")
    {
        return 2;
    }
    if (place == "[pvp]")
    {
        return 3;
    }
    if (place == "[seria room]")
    {
        return 4;
    }
    if (place == "[dont use assult]")
    {
        return 5;
    }
    return 1;
