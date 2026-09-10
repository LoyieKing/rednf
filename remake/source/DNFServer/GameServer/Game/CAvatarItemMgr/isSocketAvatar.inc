bool CAvatarItemMgr::isSocketAvatar(int key)   // ORIG 非 const
{
    const char* jewelData = getJewelSocketData(key);
    if (*(short*)jewelData == 0)
        return false;
    return true;
