bool isEnAbleColorKey(colorRGB& color)
{
    colorRGB defaultColor = getAvatarColorValue(0);
    if (color == defaultColor)
        return false;
    return true;
