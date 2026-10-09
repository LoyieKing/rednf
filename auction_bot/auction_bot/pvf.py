# -*- coding: utf-8 -*-
"""
Script.pvf 只读解析：道具 ID -> 名称（简体中文）/ 是否堆叠品 / 类别。
算法与 /mnt/d/tmp_grok/pvfblood/lib.py 相同（头部 + 文件表 XOR 0x81A79011^crc + 循环右移 6 位），
这里自带实现，不依赖外部 pickle 索引。结果缓存为 JSON（按 pvf 的 mtime/size 失效）。
"""
import json
import os
import struct

KEY = 0x81A79011


def _decrypt(raw, crc):
    n = len(raw) // 4
    if n == 0:
        return b""
    words = struct.unpack("<%dI" % n, raw[: n * 4])
    k = KEY ^ crc
    out = [(((w ^ k) >> 6) | ((w ^ k) << 26)) & 0xFFFFFFFF for w in words]
    return struct.pack("<%dI" % n, *out)


def _dec_text(bs):
    for e in ("utf-8", "cp950", "gbk", "cp949"):
        try:
            return bs.decode(e)
        except UnicodeDecodeError:
            pass
    return bs.decode("latin-1", "replace")


class PVF:
    def __init__(self, path):
        self.path = path
        self.f = open(path, "rb")
        f = self.f
        gl = struct.unpack("<I", f.read(4))[0]
        f.read(gl)
        _ver, tl, tcrc, fc = struct.unpack("<IIII", f.read(16))
        t = _decrypt(f.read(tl), tcrc)
        self.base = 4 + gl + 16 + tl
        self.files = {}
        p = 0
        for _ in range(fc):
            _fn, pl = struct.unpack_from("<II", t, p)
            p += 8
            name = t[p: p + pl].decode("cp949", "replace").lower()
            p += pl
            ln, crc, off = struct.unpack_from("<III", t, p)
            p += 12
            self.files[name] = (ln, crc, off)
        st = self.read("stringtable.bin")
        cnt = struct.unpack_from("<I", st, 0)[0]
        offs = struct.unpack_from("<%dI" % (cnt + 1), st, 4)
        self.strings = [_dec_text(st[4 + offs[i]: 4 + offs[i + 1]]) for i in range(cnt)]
        ns = self.read("n_string.lst")
        self.nstring_files = []
        for i in range(2, len(ns) - 9, 10):
            t2 = ns[i + 5]
            v2 = struct.unpack_from("<i", ns, i + 6)[0]
            self.nstring_files.append(self.strings[v2] if t2 == 7 and 0 <= v2 < len(self.strings) else None)
        self._nstr_cache = {}

    def read(self, path):
        ln, crc, off = self.files[path.lower()]
        self.f.seek(self.base + off)
        raw = self.f.read((ln + 3) & ~3)
        return _decrypt(raw, crc)[:ln]

    def _s(self, v):
        return self.strings[v] if 0 <= v < len(self.strings) else ""

    def nstr(self, fid, key):
        if fid is None or fid >= len(self.nstring_files) or not self.nstring_files[fid]:
            return None
        p = self.nstring_files[fid]
        if p not in self._nstr_cache:
            m = {}
            try:
                txt = _dec_text(self.read(p))
                for line in txt.splitlines():
                    if ">" in line:
                        k, v = line.split(">", 1)
                        m[k.strip()] = v.strip()
            except KeyError:
                pass
            self._nstr_cache[p] = m
        return self._nstr_cache[p].get(key)

    def units(self, data):
        us = []
        last9 = None
        for i in range(2, len(data) - 4, 5):
            t = data[i]
            v = struct.unpack_from("<i", data, i + 1)[0]
            if t == 4:
                v = struct.unpack_from("<f", data, i + 1)[0]
                us.append((t, v))
            elif t in (5, 6, 7, 8):
                us.append((t, self._s(v)))
            elif t == 9:
                last9 = v
            elif t == 10:
                us.append((t, self.nstr(last9, self._s(v)) or ""))
            else:
                us.append((t, v))
        return us

    def lst(self, path):
        data = self.read(path)
        m = {}
        for i in range(2, len(data) - 9, 10):
            idx = struct.unpack_from("<i", data, i + 1)[0]
            si = struct.unpack_from("<i", data, i + 6)[0]
            m[idx] = self._s(si)
        return m

    def item_fields(self, path, wanted=("[name]", "[stackable type]", "[grade]", "[price]", "[stack limit]",
                                        "[minimum level]", "[rarity]", "[equipment type]", "[attach type]",
                                        "[durability]", "[random option]")):
        try:
            us = self.units(self.read(path))
        except KeyError:
            return {}
        out = {}
        cur = None
        for t, v in us:
            if t == 5:
                cur = v.lower() if isinstance(v, str) else None
                if cur:
                    out.setdefault("_tags", set()).add(cur)
                continue
            if cur in wanted and cur not in out:
                out[cur] = v
        return out


def build_item_index(pvf_path):
    p = PVF(pvf_path)
    items = {}
    for lst_path, base, kind in (("stackable/stackable.lst", "stackable/", "stackable"),
                                 ("equipment/equipment.lst", "equipment/", "equipment")):
        try:
            m = p.lst(lst_path)
        except KeyError:
            continue
        for iid, rel in m.items():
            fields = p.item_fields(base + rel.lower())
            name = fields.get("[name]") or ""
            et = fields.get("[equipment type]")
            items[int(iid)] = {
                "name": name if isinstance(name, str) else str(name),
                "kind": kind,
                "path": base + rel.lower(),
                "stackable_type": fields.get("[stackable type]") if kind == "stackable" else None,
                "equipment_type": et if isinstance(et, str) else None,
                "stack_limit": fields.get("[stack limit]") if isinstance(fields.get("[stack limit]"), int) else None,
                "rarity": fields.get("[rarity]") if isinstance(fields.get("[rarity]"), int) else None,
                "level": fields.get("[minimum level]") if isinstance(fields.get("[minimum level]"), int) else None,
                "attach": fields.get("[attach type]") if isinstance(fields.get("[attach type]"), str) else None,
                # 装备：[durability] 满耐久（首饰等没有这个字段 → None）；[random option] 非 0 = 随机属性装备（假紫）
                "durability": fields.get("[durability]") if isinstance(fields.get("[durability]"), int) else None,
                "random_option": (kind == "equipment" and (bool(fields.get("[random option]"))
                                                            or ("[random option]" in fields.get("_tags", ()) and "[random option]" not in fields))),
            }
    return items


def load_item_index(pvf_path, cache_path):
    st = os.stat(pvf_path)
    sig = "v2:%d:%d" % (st.st_size, int(st.st_mtime))   # v2：增加 durability / random_option 字段
    if os.path.exists(cache_path):
        try:
            with open(cache_path, "r", encoding="utf-8") as fh:
                c = json.load(fh)
            if c.get("sig") == sig:
                return {int(k): v for k, v in c["items"].items()}
        except Exception:
            pass
    items = build_item_index(pvf_path)
    tmp = cache_path + ".tmp"
    with open(tmp, "w", encoding="utf-8") as fh:
        json.dump({"sig": sig, "items": items}, fh, ensure_ascii=False)
    os.replace(tmp, cache_path)
    return items


if __name__ == "__main__":
    import sys
    idx = build_item_index(sys.argv[1] if len(sys.argv) > 1 else "/opt/dnf1031/data/Script.pvf")
    print(len(idx))
    for i in [int(x) for x in sys.argv[2:]] or [3187, 3205, 1047, 3255]:
        print(i, idx.get(i))
