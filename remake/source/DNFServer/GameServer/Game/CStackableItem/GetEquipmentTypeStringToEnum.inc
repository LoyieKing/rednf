int GetEquipmentTypeStringToEnum(std::string& type)
{
    int result = 0x16;
    if (type == "[hat avatar]")
    {
        result = 0;
    }
    else if (type == "[hair avatar]")
    {
        result = 1;
    }
    else if (type == "[face avatar]")
    {
        result = 2;
    }
    else if (type == "[coat avatar]")
    {
        result = 3;
    }
    else if (type == "[pants avatar]")
    {
        result = 4;
    }
    else if (type == "[shoes avatar]")
    {
        result = 5;
    }
    else if (type == "[breast avatar]")
    {
        result = 6;
    }
    else if (type == "[waist avatar]")
    {
        result = 7;
    }
    else if (type == "[skin avatar]")
    {
        result = 8;
    }
    else if (type == "[aurora avatar]")
    {
        result = 9;
    }
    else if (type == "[weapon]")
    {
        result = 10;
    }
    else if (type == "[title name]")
    {
        result = 11;
    }
    else if (type == "[coat]")
    {
        result = 12;
    }
    else if (type == "[shoulder]")
    {
        result = 13;
    }
    else if (type == "[pants]")
    {
        result = 14;
    }
    else if (type == "[shoes]")
    {
        result = 15;
    }
    else if (type == "[waist]")
    {
        result = 16;
    }
    else if (type == "[amulet]")
    {
        result = 17;
    }
    else if (type == "[wrist]")
    {
        result = 18;
    }
    else if (type == "[ring]")
    {
        result = 19;
    }
    else if (type == "[support]")
    {
        result = 20;
    }
    else if (type == "[magic stone]")
    {
        result = 21;
    }
    else if (type == "[creature]")
    {
        result = 22;
    }
    else if (type == "[artifact red]")
    {
        result = 23;
    }
    else if (type == "[artifact blue]")
    {
        result = 24;
    }
    else if (type == "[artifact green]")
    {
        result = 25;
    }
    return result;
