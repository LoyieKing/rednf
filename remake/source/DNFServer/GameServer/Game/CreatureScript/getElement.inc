// ORIG 0x88bfe5b：字符串→ENUM_ELEMENT（fire/water/dark/light，默认 4）。
int getElement(const char* s)
{
    if (strcmp(s, "[fire element]") == 0) return 0;
    if (strcmp(s, "[water element]") == 0) return 1;
    if (strcmp(s, "[dark element]") == 0) return 2;
    if (strcmp(s, "[light element]") == 0) return 3;
    return 4;
}