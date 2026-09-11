// ORIG 0x088d4856：与 getAvatarColorValue(0)（默认色）相同则不可启用。
bool isEnAbleColorKey(colorRGB& color)
{
    colorRGB defaultColor = getAvatarColorValue(0);
    if (color == defaultColor)
        return false;
    return true;
}