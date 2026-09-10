int GetEquipmentType(const char* name)
{
    static const char* const kNames[26] = {
        "[hat avatar]",   "[hair avatar]",  "[face avatar]",  "[coat avatar]",
        "[pants avatar]", "[shoes avatar]", "[breast avatar]", "[waist avatar]",
        "[skin avatar]",  "[aurora avatar]", "[weapon]",      "[title name]",
        "[coat]",         "[shoulder]",     "[pants]",        "[shoes]",
        "[waist]",        "[amulet]",       "[wrist]",        "[ring]",
        "[support]",      "[magic stone]",  "[creature]",     "[artifact red]",
        "[artifact blue]", "[artifact green]",
    };
    for (int i = 0; i < 26; ++i)
    {
        if (strcmp(name, kNames[i]) == 0)
            return i;
    }
    return 26;
