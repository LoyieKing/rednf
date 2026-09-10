int getStackableActionUsableServerType(std::string& type)
{
    if (type == "[general server]")
    {
        return 1;
    }
    if (type == "[event server]")
    {
        return 2;
    }
    if (type == "[starter server]")
    {
        return 3;
    }
    return 1;
