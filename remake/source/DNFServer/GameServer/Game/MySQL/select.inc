int MySQL::select()
{
    if (!set_query())
    {
        return 3;
    }
    if (!exec(true))
    {
        return 1;
    }
    return 0;
}