int saveGreyImage(const GreyImage& img, const char* path)
{
    FILE* f = fopen(path, "wb");
    if (!f)
        return 0;
    TgaHeader hdr;
    hdr.idLength = 0;
    hdr.colorMapType = 0;
    hdr.imageType = 2;
    hdr.colorMapFirst = 0;
    hdr.colorMapLength = 0;
    hdr.colorMapEntrySize = 0;
    hdr.xOrigin = 0;
    hdr.yOrigin = 0;
    hdr.width = static_cast<unsigned short>(img.getWidth());
    hdr.height = static_cast<unsigned short>(img.getHeight());
    hdr.depth = 0x20;
    hdr.descriptor = 8;
    fwrite(&hdr, 1, 0x12, f);

    unsigned char* buf = new unsigned char[img.getWidth() * 4];
    int y = img.getHeight() - 1;
    while (y >= 0)
    {
        const unsigned char* row =
            static_cast<const unsigned char*>(img.getImage()) + img.getWidth() * y;
        for (int x = 0; x < img.getWidth(); ++x)
        {
            unsigned char v = static_cast<unsigned char>(~row[x]);
            *reinterpret_cast<unsigned int*>(buf + x * 4) =
                (static_cast<unsigned int>(v) << 16) |
                (static_cast<unsigned int>(v) << 8) |
                static_cast<unsigned int>(v) | 0xff000000u;
        }
        fwrite(buf, 1, img.getWidth() * 4, f);
        --y;
    }
    delete[] buf;
    fclose(f);
    return 1;
