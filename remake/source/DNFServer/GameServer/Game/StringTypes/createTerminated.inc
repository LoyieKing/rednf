CharStringData* CharStringData::createTerminated(int length)
{
    CharStringData* data = create(length);
    data->getBuffer()[length] = '\0';
    return data;
}