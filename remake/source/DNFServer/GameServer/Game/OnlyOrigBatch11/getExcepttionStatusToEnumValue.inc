// ORIG T 0x089bcde0："silence"→1、"disease"→3、"mindcontrol"→2、"all"→5、
// 默认 -1。
int getExcepttionStatusToEnumValue(std::string& s)
{
    if (s == "silence")
        return 1;
    if (s == "disease")
        return 3;
    if (s == "mindcontrol")
        return 2;
    if (s == "all")
        return 5;
    return -1;
}