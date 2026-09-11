bool BlueMarble::checkMoveFirstZone(int seat, BlueMarbleTileScript::Type type)
{
    switch (type)
    {
    case (BlueMarbleTileScript::Type)11:
    case (BlueMarbleTileScript::Type)12:
    case (BlueMarbleTileScript::Type)13:
        return 0;
    case (BlueMarbleTileScript::Type)9:
    case (BlueMarbleTileScript::Type)10:
        if (getMovePosition(seat) != 1)
        {
            return 0;
        }
        break;
    default:
        break;
    }
    return 1;
}