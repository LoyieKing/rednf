bool isDbcsLead(char c)
{
    return (c & 0x80) != 0;
