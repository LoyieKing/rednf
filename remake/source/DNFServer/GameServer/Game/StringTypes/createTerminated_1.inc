WideStringData* WideStringData::createTerminated(int length)
{
    WideStringData* data = create(length);
    data->getBuffer()[length] = 0;
    return data;
