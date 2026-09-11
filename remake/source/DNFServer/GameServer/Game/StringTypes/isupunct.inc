bool WideString::isupunct(wchar_t ch)
{
    // ORIG 0x08adcb50：条件为「不是 Unicode 标点」，命中返回 false，否则 true。
    // 极性反演（P196）：ORIG true 常量岛在前（mov $1;jmp 在 mov $0 之前），
    // 需 if 条件 = 「是标点」命中 → return true。
    if (!(((((((((((ch < 0x21) || (0x23 < ch)) && ((ch < 0x25 || (0x2a < ch)))) &&
            ((((((ch < 0x2c || (0x2f < ch)) && (ch != 0x3a)) && ((ch != 0x3b && (ch != 0x3f)))) &&
              (ch != 0x40)) &&
             (((ch < 0x5b || (0x5d < ch)) &&
              ((ch != 0x5f && (((ch != 0x7b && (ch != 0x7d)) && (ch != 0xa1)))))))))) &&
           (((ch != 0xab && (ch != 0xad)) && ((ch != 0xb7 && ((ch != 0xbb && (ch != 0xbf)))))))) &&
          (ch != 0x37e)) && ((ch != 0x387 && (((ch < 0x55a || (0x55f < ch)) && (ch != 0x589)))))) &&
        (((ch != 0x58a && (ch != 0x5be)) &&
         ((ch != 0x5c0 &&
          ((((ch != 0x5c3 && (ch != 0x5c6)) &&
            ((ch != 0x5f3 &&
             ((((ch != 0x5f4 && (ch != 0x60c)) && (ch != 0x60d)) && ((ch != 0x61b && (ch != 0x61e)))
              ))))) && (ch != 0x61f)))))))) &&
       ((((((((ch < 0x66a || (0x66d < ch)) && ((ch != 0x6d4 && ((ch < 0x700 || (0x70d < ch)))))) &&
            ((ch < 0x7f7 || (0x7f9 < ch)))) &&
           ((((((ch != 0x964 && (ch != 0x965)) && (ch != 0x970)) &&
              ((ch != 0xdf4 && ((ch < 0xe4f || (0xe5b < ch)))))) && ((ch < 0xf04 || (0xf12 < ch))))
            && (((ch < 0xf3a || (0xf3d < ch)) && ((ch != 0xf85 && ((ch != 0xfd0 && (ch != 0xfd1)))))
                ))))) && ((ch < 0x104a || (0x104f < ch)))) &&
         (((((ch != 0x10fb && (((ch < 0x1361 || (0x1368 < ch)) && (ch != 0x166d)))) &&
            ((ch != 0x166e && (ch != 0x169b)))) && (ch != 0x169c)) &&
          (((((((ch < 0x16eb || (0x16ed < ch)) && ((ch != 0x1735 && (ch != 0x1736)))) &&
              ((ch < 0x17d4 || (0x17d6 < ch)))) &&
             (((ch < 0x17d8 || (0x17da < ch)) &&
              (((((ch < 0x1800 || (0x180a < ch)) && (ch != 0x1944)) &&
                ((ch != 0x1945 && (ch != 0x19de)))) &&
               ((ch != 0x19df && ((ch != 0x1a1e && (ch != 0x1a1f)))))))))) &&
            ((ch < 0x1b5a || (0x1b60 < ch)))) &&
           (((((ch < 0x2010 || (0x2027 < ch)) && ((ch < 0x2030 || (0x2043 < ch)))) &&
             ((ch < 0x2045 || (0x2051 < ch)))) && ((ch < 0x2053 || (0x205e < ch)))))))))) &&
        ((((ch != 0x207d && (ch != 0x207e)) && (ch != 0x208d)) &&
         (((ch != 0x208e && (ch != 0x2329)) && ((ch != 0x232a && ((ch < 0x2768 || (0x2775 < ch))))))
         )))))) && ((ch < 0x27c5 || (0x27c6 < ch)))) &&
     (((((((ch < 0x27e6 || (0x27eb < ch)) && ((ch < 0x2983 || (0x2998 < ch)))) &&
         (((((((ch < 0x29d8 || (0x29db < ch)) && (ch != 0x29fc)) && (ch != 0x29fd)) &&
            (((ch < 0x2cf9 || (0x2cfc < ch)) && ((ch != 0x2cfe && (ch != 0x2cff)))))) &&
           ((ch < 0x2e00 || (0x2e17 < ch)))) &&
          ((ch != 0x2e1c && ((ch != 0x2e1d && ((ch < 0x3001 || (0x3003 < ch)))))))))) &&
        ((ch < 0x3008 || (0x3011 < ch)))) &&
       ((((((((ch < 0x3014 || (0x301f < ch)) && (ch != 0x3030)) &&
            ((ch != 0x303d && (ch != 0x30a0)))) && (ch != 0x30fb)) &&
          (((ch < 0xa874 || (0xa877 < ch)) && ((ch != 0xfd3e && (ch != 0xfd3f)))))) &&
         ((ch < 0xfe10 || (0xfe19 < ch)))) && ((ch < 0xfe30 || (0xfe52 < ch)))))) &&
      (((((ch < 0xfe54 || (0xfe61 < ch)) && (ch != 0xfe63)) &&
        (((ch != 0xfe68 && (ch != 0xfe6a)) && ((ch != 0xfe6b && ((ch < 0xff01 || (0xff03 < ch)))))))
        ) && (((ch < 0xff05 || (0xff0a < ch)) &&
              (((((ch < 0xff0c || (0xff0f < ch)) && (ch != 0xff1a)) &&
                (((ch != 0xff1b && (ch != 0xff1f)) &&
                 (((ch != 0xff20 && ((ch < 0xff3b || (0xff3d < ch)))) && (ch != 0xff3f)))))) &&
               (((ch != 0xff5b && (ch != 0xff5d)) && ((ch < 0xff5f || (0xff65 < ch))))))))))))))) {
    return true;
    }
        return false;
}