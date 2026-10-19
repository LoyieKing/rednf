#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Shared DWARF/ELF model extraction for the TSS SDK layout audit.

One ELF is reduced to a JSON-able model:

  types      qualified type name -> memory layout
             (byte size, members, base classes, virtual functions, CU)
  funcs      mangled symbol name -> signature
             (return/parameter types, size, CU, decl position)
  vars       absolute address -> global variable DIEs (name/type/size/CU)
  objects    ELF OBJECT/FUNC symbols with a non-zero size
  conflicts  aggregate names that appear with diverging layouts inside one ELF

The extractor is deliberately independent of the reconstruction sources: it
only reads the ELF that was produced. ORIG and the reconstructed library are
then compared with the same code path (diff_model.py).
"""
from __future__ import print_function

import json
import re
import sys

from elftools.elf.elffile import ELFFile

AGG_TAGS = ("DW_TAG_structure_type", "DW_TAG_class_type",
            "DW_TAG_union_type")
QUAL_TAGS = ("DW_TAG_const_type", "DW_TAG_volatile_type",
             "DW_TAG_restrict_type")
TAGSHORT = {"DW_TAG_structure_type": "struct",
            "DW_TAG_class_type": "class",
            "DW_TAG_union_type": "union"}

# ABI-sensitive aggregates whose layout is selected per TU by feature macros
# (e.g. _FILE_OFFSET_BITS).  `types` keeps only the richest definition, so
# these names need a separate per-CU record: a 96-byte and an 88-byte `stat`
# can both be legitimate in ORIG, but a reconstructed TU must match the ORIG
# TU it replaces.
ABI_TYPE_NAMES = ("stat", "stat64")

# DW_ATE_* encoding -> layout class, used so that typedef spelling differences
# do not hide a real size/representation difference.
ENC_CLASS = {
    1: "ptr",        # DW_ATE_address
    2: "bool",       # DW_ATE_boolean
    3: "cplx",       # DW_ATE_complex_float
    4: "float",      # DW_ATE_float
    5: "int",        # DW_ATE_signed
    6: "int",        # DW_ATE_signed_char
    7: "uint",       # DW_ATE_unsigned
    8: "uint",       # DW_ATE_unsigned_char
    9: "imag",       # DW_ATE_imaginary_float
    13: "int",       # DW_ATE_signed_fixed
    14: "uint",      # DW_ATE_unsigned_fixed
    15: "float",     # DW_ATE_decimal_float
    16: "char",      # DW_ATE_UTF
    17: "char",      # DW_ATE_UCS
    18: "char",      # DW_ATE_ASCII
}


def decoded(value):
    if isinstance(value, (bytes, bytearray)):
        return value.decode("utf-8", "replace")
    return value


def astr(die, key):
    a = die.attributes.get(key)
    if a is None:
        return None
    return decoded(a.value)


def aint(die, key):
    a = die.attributes.get(key)
    if a is None:
        return None
    v = a.value
    if isinstance(v, int):
        return v
    if isinstance(v, (bytes, bytearray, list)):
        return int.from_bytes(bytes(v), "little")
    return None


def uleb(data, pos=0):
    shift = 0
    out = 0
    for b in data[pos:]:
        out |= (b & 0x7F) << shift
        if not (b & 0x80):
            return out, pos + 1
        shift += 7
    return out, pos + 1


class Context(object):
    def __init__(self, path):
        self.path = path
        self.f = open(path, "rb")
        self.elf = ELFFile(self.f)
        self.dwarf = self.elf.get_dwarf_info()
        self.cu = None
        self.cu_name = ""
        self.types = {}
        self.type_cus = {}
        self.conflicts = []
        self.funcs = {}
        self.vars = {}
        self.objects = {}
        self.names = {}
        self.variants = {}
        self.abi_types = {}
        self._type_cache = {}

    # -- references -------------------------------------------------------
    def ref(self, die, key):
        a = die.attributes.get(key)
        if a is None:
            return None
        form = a.form
        if form == "DW_FORM_ref_addr":
            try:
                return self.dwarf.get_DIE_from_refaddr(a.value, self.cu)
            except Exception:
                return None
        candidates = []
        if self.cu is not None:
            candidates.append(self.cu.cu_offset + a.value)
            candidates.append(a.value)
        for off in candidates:
            try:
                return self.cu.get_DIE_from_refaddr(off)
            except Exception:
                continue
        return None

    def resolve(self, die, key, depth=6):
        cur = die
        for _ in range(depth):
            v = astr(cur, key)
            if v is not None:
                return v
            nxt = self.ref(cur, "DW_AT_specification") or \
                self.ref(cur, "DW_AT_abstract_origin")
            if nxt is None:
                return None
            cur = nxt
        return None

    def resolve_die(self, die, depth=6):
        """Follow specification/abstract_origin to the declaring DIE."""
        cur = die
        for _ in range(depth):
            nxt = self.ref(cur, "DW_AT_specification") or \
                self.ref(cur, "DW_AT_abstract_origin")
            if nxt is None:
                return cur
            cur = nxt
        return cur

    def agg_name(self, die):
        """Declared name of an aggregate DIE, following DW_AT_specification.

        GCC 4.1 emits a type *definition* as an unnamed DIE tied to its named
        declaration through DW_AT_specification (verified on the tag-394
        image: CIpcChannel 0xbd281 -> 0xbd22f 'CIpcChannel', std::vector<...>
        instantiations, the ZEN_LIB::_shm_memory_base base of shm_dequechunk).
        GCC 13 names the definition DIE directly.  Without this lookup the
        extractor saw the GCC 4.1 definition as an anonymous struct, flattened
        it into the enclosing type and reported pure DWARF-shape differences
        as layout conflicts.  `._NN` names are still synthetic per-CU counters
        and are left to the anonymous path.
        """
        nm = astr(die, "DW_AT_name")
        if nm and not nm.startswith("._"):
            return nm
        spec = self.ref(die, "DW_AT_specification")
        if spec is not None:
            sn = astr(spec, "DW_AT_name")
            if sn and not sn.startswith("._"):
                return self.names.get(spec.offset) or sn
        return nm

    # -- type spelling / layout key --------------------------------------
    def type_info(self, die, depth=0):
        """Return {lkey, skey, size} for a type DIE."""
        if die is None:
            return {"lkey": "void", "skey": "void", "size": 0}
        if depth > 12:
            return {"lkey": "?", "skey": "?", "size": 0}
        tag = die.tag
        if tag == "DW_TAG_typedef":
            inner = self.type_info(self.ref(die, "DW_AT_type"), depth + 1)
            nm = astr(die, "DW_AT_name") or inner["skey"]
            return {"lkey": inner["lkey"], "skey": nm, "size": inner["size"]}
        if tag in QUAL_TAGS:
            prefix = {"DW_TAG_const_type": "const ",
                      "DW_TAG_volatile_type": "volatile ",
                      "DW_TAG_restrict_type": "restrict "}[tag]
            inner = self.type_info(self.ref(die, "DW_AT_type"), depth + 1)
            return {"lkey": inner["lkey"], "skey": prefix + inner["skey"],
                    "size": inner["size"]}
        if tag == "DW_TAG_pointer_type":
            inner = self.type_info(self.ref(die, "DW_AT_type"), depth + 1)
            return {"lkey": "ptr", "skey": inner["skey"] + "*", "size": 4}
        if tag == "DW_TAG_reference_type":
            inner = self.type_info(self.ref(die, "DW_AT_type"), depth + 1)
            return {"lkey": "ref", "skey": inner["skey"] + "&", "size": 4}
        if tag == "DW_TAG_rvalue_reference_type":
            inner = self.type_info(self.ref(die, "DW_AT_type"), depth + 1)
            return {"lkey": "rref", "skey": inner["skey"] + "&&", "size": 4}
        if tag == "DW_TAG_array_type":
            inner = self.type_info(self.ref(die, "DW_AT_type"), depth + 1)
            dims = []
            for c in die.iter_children():
                if c.tag != "DW_TAG_subrange_type":
                    continue
                cnt = aint(c, "DW_AT_count")
                up = aint(c, "DW_AT_upper_bound")
                dims.append(cnt if cnt is not None
                            else (up + 1 if up is not None else 0))
            total = inner["size"]
            for d in dims:
                total *= d
            return {"lkey": "array(%s;%s)" % (",".join(map(str, dims)),
                                              inner["lkey"]),
                    "skey": inner["skey"] + "".join("[%d]" % d for d in dims),
                    "size": total}
        if tag == "DW_TAG_base_type":
            enc = aint(die, "DW_AT_encoding") or 0
            size = aint(die, "DW_AT_byte_size") or 0
            cls = ENC_CLASS.get(enc, "base")
            return {"lkey": "%s%d" % (cls, size),
                    "skey": astr(die, "DW_AT_name") or "%s%d" % (cls, size),
                    "size": size}
        if tag == "DW_TAG_enumeration_type":
            size = aint(die, "DW_AT_byte_size") or 4
            nm = astr(die, "DW_AT_name") or "<anon enum>"
            return {"lkey": "enum%d" % size, "skey": "enum " + nm,
                    "size": size}
        if tag in AGG_TAGS:
            size = aint(die, "DW_AT_byte_size") or 0
            nm = self.agg_name(die)
            if nm is None or nm.startswith("._"):
                # GCC's synthetic names are per-CU counters; compare by size.
                return {"lkey": "anon%d" % size, "skey": "<anon>",
                        "size": size}
            qual = self.names.get(die.offset, nm)
            # Strip anonymous-namespace components ("._NN"): they are CU-local
            # and do not name the same entity across builds.
            parts = [p for p in qual.split("::")
                     if not re.match(r"^\._\d+$", p)]
            qual = "::".join(parts) if parts else nm
            return {"lkey": "agg:" + qual, "skey": qual, "size": size}
        if tag == "DW_TAG_subroutine_type":
            return {"lkey": "func", "skey": "func", "size": 4}
        return {"lkey": tag, "skey": tag, "size": aint(die, "DW_AT_byte_size")
                or 0}

    def qual_name(self, die, name):
        # DWARF does not store the full qualification on the DIE; callers pass
        # the scope-resolved name when available. This is the fallback.
        return name

    def scan_names(self, die, scope):
        """First pass: map DIE offset -> qualified name.

        GCC 4.1 emits class definitions as unnamed DIEs that reference the
        named declaration through DW_AT_specification, and out-of-line member
        definitions use the same pattern. The map lets the second pass resolve
        those without parent links.
        """
        for c in die.iter_children():
            tag = c.tag
            nm = astr(c, "DW_AT_name")
            if tag == "DW_TAG_namespace":
                q = "::".join(scope + ([nm] if nm else []))
                self.names[c.offset] = q
                self.scan_names(c, scope + ([nm] if nm else []))
            elif tag in AGG_TAGS:
                q = "::".join(scope + [nm]) if nm else None
                if q:
                    self.names[c.offset] = q
                self.scan_names(c, scope + ([nm] if nm else []))
            elif tag == "DW_TAG_subprogram":
                if nm:
                    self.names[c.offset] = "::".join(scope + [nm])
            elif tag in ("DW_TAG_lexical_block", "DW_TAG_inlined_subroutine"):
                continue
            else:
                self.scan_names(c, scope)

    # -- member offsets ---------------------------------------------------
    @staticmethod
    def member_offset(die):
        a = die.attributes.get("DW_AT_data_member_location")
        if a is None:
            return 0
        v = a.value
        if isinstance(v, int):
            return v
        data = bytes(v)
        if data and data[0] == 0x23:                # DW_OP_plus_uconst
            return uleb(data, 1)[0]
        if data and data[0] == 0x22:                # DW_OP_plus
            return 0
        return int.from_bytes(data, "little")

    # -- registration -----------------------------------------------------
    def flat_layout(self, die, out, prefix, depth):
        """Flatten members, descending anonymous aggregates."""
        if depth > 8:
            return
        index = 0
        for child in die.iter_children():
            tag = child.tag
            if tag == "DW_TAG_member":
                nm = astr(child, "DW_AT_name")
                if nm and nm.startswith("_vptr"):
                    # GCC 4.1 models the vptr as a member named
                    # "_vptr.<class>"; reconstruction models name it
                    # differently. Both are the Itanium vptr slot at offset 0.
                    nm = "<vptr>"
                off = self.member_offset(child)
                tdie = self.ref(child, "DW_AT_type")
                tinfo = self.type_info(tdie, 0)
                path = prefix + (nm if nm else "@%d" % index)
                index += 1
                bit_size = aint(child, "DW_AT_bit_size")
                bit_off = aint(child, "DW_AT_data_bit_offset")
                if bit_off is None and bit_size is not None:
                    v2off = aint(child, "DW_AT_bit_offset")
                    if v2off is not None:
                        base = tinfo["size"] * 8
                        bit_off = off * 8 + (base - v2off - bit_size)
                entry = {"path": path, "offset": off, "size": tinfo["size"],
                         "lkey": tinfo["lkey"], "skey": tinfo["skey"]}
                if bit_size is not None:
                    entry["bit_size"] = bit_size
                    entry["bit_offset"] = bit_off
                # Anonymous aggregate (unnamed member, or a member whose type
                # is one of GCC's synthetic "._NNN" aggregates): expand in
                # place. Synthetic names are per-CU counters, so a name-based
                # comparison would report false layout conflicts.
                tdie_nm = (self.agg_name(tdie) if tdie is not None and
                           tdie.tag in AGG_TAGS else None)
                anon_type = (tdie is not None and tdie.tag in AGG_TAGS and
                             (tdie_nm is None or tdie_nm.startswith("._")))
                if ((nm is None or anon_type) and tdie is not None and
                        tdie.tag in AGG_TAGS and tinfo["size"]):
                    sub = []
                    self.flat_layout(tdie, sub, path + ".", depth + 1)
                    for s in sub:
                        s = dict(s)
                        s["offset"] += off
                        out.append(s)
                    continue
                out.append(entry)
            elif tag == "DW_TAG_inheritance":
                tdie = self.ref(child, "DW_AT_type")
                out.append({"path": "<base>", "offset": self.member_offset(child),
                            "size": (self.type_info(tdie)["size"]),
                            "lkey": self.type_info(tdie)["lkey"],
                            "skey": self.type_info(tdie)["skey"],
                            "base": True,
                            "virtual": bool(aint(child, "DW_AT_virtuality"))})

    def register_type(self, die, qual):
        if not qual or qual.rsplit("::", 1)[-1].startswith("._"):
            return
        size = aint(die, "DW_AT_byte_size")
        if not size:
            return
        members = []
        self.flat_layout(die, members, "", 0)
        bases = [m for m in members if m.get("base")]
        members = [m for m in members if not m.get("base")]
        vfuncs = []
        for c in die.iter_children():
            if c.tag != "DW_TAG_subprogram":
                continue
            if aint(c, "DW_AT_virtuality") == 2:
                vfuncs.append(astr(c, "DW_AT_name") or "?")
        rec = {"name": qual, "tag": TAGSHORT.get(die.tag, die.tag),
               "size": size, "align": aint(die, "DW_AT_alignment"),
               "members": members, "bases": bases, "vfuncs": vfuncs,
               "cu": self.cu_name}
        if qual in ABI_TYPE_NAMES:
            self.register_abi_type(rec)
        sig = (size, tuple((m.get("path"), m.get("offset"), m.get("size"),
                            m.get("lkey"), m.get("bit_size"),
                            m.get("bit_offset")) for m in members),
               tuple((b.get("offset"), b.get("lkey"), b.get("virtual"))
                     for b in bases))
        bucket = self.variants.setdefault(qual, [])
        if not any(v["sig"] == sig for v in bucket):
            bucket.append({"sig": sig, "size": size, "cu": self.cu_name,
                           "members": members, "bases": bases,
                           "vfuncs": vfuncs})
        here = self.types.get(qual)
        if here is None:
            self.types[qual] = rec
            self.type_cus[qual] = [self.cu_name]
            return
        self.type_cus[qual].append(self.cu_name)
        # GCC also emits "thin" completion DIEs (byte size, no members) in CUs
        # that only need the size. Keep the richest definition and do not
        # report a thin DIE as a layout conflict by itself.
        a, b = here, rec
        if len(b["members"]) > len(a["members"]):
            self.types[qual] = b
            a, b = b, a
        if a["size"] != b["size"]:
            self.conflicts.append({"name": qual, "a_cu": a["cu"],
                                   "b_cu": b["cu"],
                                   "a_size": a["size"],
                                   "b_size": b["size"],
                                   "a_members": a["members"],
                                   "b_members": b["members"]})
            return
        if not a["members"] or not b["members"]:
            return
        if self.layout_equal(a, b):
            return
        self.conflicts.append({"name": qual, "a_cu": a["cu"],
                               "b_cu": b["cu"], "a_size": a["size"],
                               "b_size": b["size"], "a_members": a["members"],
                               "b_members": b["members"]})

    def register_abi_type(self, rec):
        """Keep one richest ABI layout per (type, CU).

        `self.types` intentionally keeps the richest definition and can lose
        the 88-byte `stat` variant; this map preserves every CU's definition.
        """
        qual = rec["name"]
        bucket = self.abi_types.setdefault(qual, {})
        prev = bucket.get(rec["cu"])
        if prev is None or len(rec["members"]) > len(prev["members"]):
            bucket[rec["cu"]] = {
                "name": qual, "cu": rec["cu"], "tag": rec["tag"],
                "size": rec["size"], "align": rec["align"],
                "members": rec["members"], "bases": rec["bases"],
            }

    @staticmethod
    def layout_equal(a, b):
        if a["size"] != b["size"] or a["tag"] != b["tag"]:
            return False
        if len(a["members"]) != len(b["members"]):
            return False
        for x, y in zip(a["members"], b["members"]):
            for k in ("path", "offset", "size", "lkey", "bit_size",
                      "bit_offset"):
                if x.get(k) != y.get(k):
                    return False
        if len(a["bases"]) != len(b["bases"]):
            return False
        for x, y in zip(a["bases"], b["bases"]):
            if (x.get("offset"), x.get("lkey"), x.get("virtual")) != \
                    (y.get("offset"), y.get("lkey"), y.get("virtual")):
                return False
        return True

    def register_func(self, die, scope):
        low = aint(die, "DW_AT_low_pc")
        span_size = None
        if low is None:
            ra = die.attributes.get("DW_AT_ranges")
            if ra is not None:
                spans = []
                try:
                    rl = self.dwarf.range_lists().get_range_list_at_offset(
                        ra.value, self.cu)
                    for e in rl:
                        b = getattr(e, "begin_offset", None)
                        en = getattr(e, "end_offset", None)
                        if b is not None and en is not None:
                            spans.append((b, en))
                except Exception:
                    spans = []
                if spans:
                    low = min(b for b, _ in spans)
                    span_size = sum(en - b for b, en in spans)
        if low is None:
            return
        decl = self.resolve_die(die)
        name = astr(decl, "DW_AT_name") or astr(die, "DW_AT_name")
        link = (astr(decl, "DW_AT_linkage_name") or
                astr(decl, "DW_AT_MIPS_linkage_name") or
                astr(die, "DW_AT_linkage_name") or
                astr(die, "DW_AT_MIPS_linkage_name"))
        if link is None:
            return
        rdie = self.ref(die, "DW_AT_type") or self.ref(decl, "DW_AT_type")
        ret = self.type_info(rdie)

        def collect_params(owner):
            out = []
            for c in owner.iter_children():
                if c.tag != "DW_TAG_formal_parameter":
                    continue
                tdie = self.ref(c, "DW_AT_type")
                if tdie is None:
                    origin = (self.ref(c, "DW_AT_abstract_origin") or
                              self.ref(c, "DW_AT_specification"))
                    if origin is not None:
                        tdie = self.ref(origin, "DW_AT_type")
                if tdie is None:
                    # "f(void)" marker, or a parameter only described by an
                    # abstract origin we cannot resolve: not part of the ABI.
                    continue
                pinfo = self.type_info(tdie)
                pinfo["artificial"] = bool(aint(c, "DW_AT_artificial"))
                out.append(pinfo)
            return out

        # The declaration carries DW_AT_artificial on the implicit `this`
        # parameter; a GCC 13 out-of-line definition may omit it. Prefer the
        # declaration so `this` is excluded from the explicit parameter list.
        params = collect_params(decl)
        if not params:
            params = collect_params(die)
        high = die.attributes.get("DW_AT_high_pc")
        size = span_size
        if high is not None:
            size = high.value if high.form != "DW_FORM_addr" else \
                high.value - low
        virt = aint(decl, "DW_AT_virtuality")
        file_index = aint(decl, "DW_AT_decl_file")
        pretty = self.names.get(decl.offset) or self.names.get(die.offset) or             ("::".join(scope + [name]) if name else link)
        real = [p for p in params if not p["artificial"]]
        self_params = [p for p in params if p["artificial"]]
        rec = {"mangled": link,
               "pretty": pretty,
               "ret": ret["skey"], "ret_lkey": ret["lkey"],
               "params": [p["skey"] for p in real],
               "param_lkeys": [p["lkey"] for p in real],
               "this_type": self_params[0]["skey"] if self_params else None,
               "size": size, "addr": low, "cu": self.cu_name,
               "virtual": virt == 2,
               "decl_file": file_index,
               "decl_line": aint(decl, "DW_AT_decl_line")}
        old = self.funcs.get(link)
        if old is None or (old.get("size") or 0) < (size or 0):
            self.funcs[link] = rec

    def register_var(self, die, scope):
        a = die.attributes.get("DW_AT_location")
        if a is None:
            return
        data = None
        if isinstance(a.value, int):
            return
        data = bytes(a.value)
        addr = None
        if data and data[0] == 0x03 and len(data) >= 5:
            addr = int.from_bytes(data[1:5], "little")
        if addr is None:
            return
        decl = self.resolve_die(die)
        name = astr(decl, "DW_AT_name") or astr(die, "DW_AT_name")
        link = (astr(decl, "DW_AT_linkage_name") or
                astr(decl, "DW_AT_MIPS_linkage_name") or
                astr(die, "DW_AT_linkage_name") or
                astr(die, "DW_AT_MIPS_linkage_name"))
        tinfo = self.type_info(self.ref(die, "DW_AT_type") or
                               self.ref(decl, "DW_AT_type"))
        self.vars.setdefault(addr, []).append(
            {"name": link or name, "pretty": name, "type": tinfo["skey"],
             "lkey": tinfo["lkey"], "size": tinfo["size"],
             "cu": self.cu_name})

    # -- symbol tables ----------------------------------------------------
    def read_symbols(self):
        for secname in (".symtab", ".dynsym"):
            sec = self.elf.get_section_by_name(secname)
            if sec is None:
                continue
            for sym in sec.iter_symbols():
                info = sym["st_info"]
                typ = info["type"]
                if typ not in ("STT_OBJECT", "STT_FUNC"):
                    continue
                if sym["st_shndx"] == "SHN_UNDEF" or not sym["st_size"]:
                    continue
                name = sym.name
                if not name:
                    continue
                rec = {"name": name, "type": typ, "size": sym["st_size"],
                       "bind": info["bind"], "addr": sym["st_value"],
                       "section": sym["st_shndx"]}
                old = self.objects.get(name)
                if old is None or old["size"] < rec["size"]:
                    self.objects[name] = rec

    # -- driver -----------------------------------------------------------
    def walk(self, die, scope):
        for c in die.iter_children():
            tag = c.tag
            if tag == "DW_TAG_namespace":
                nm = astr(c, "DW_AT_name")
                self.walk(c, scope + ([nm] if nm else []))
            elif tag in AGG_TAGS:
                nm = astr(c, "DW_AT_name")
                qual = self.names.get(c.offset)
                if qual is None and nm is None:
                    spec = self.ref(c, "DW_AT_specification")
                    if spec is not None:
                        qual = self.names.get(spec.offset)
                if qual is None and nm:
                    qual = "::".join(scope + [nm])
                if qual:
                    self.register_type(c, qual)
                self.walk(c, scope + ([nm] if nm else []))
            elif tag == "DW_TAG_subprogram":
                self.register_func(c, scope)
                # do not descend: local DIEs are not part of the ABI surface
            elif tag == "DW_TAG_variable":
                self.register_var(c, scope)
            elif tag in ("DW_TAG_lexical_block", "DW_TAG_inlined_subroutine"):
                continue
            else:
                self.walk(c, scope)

    def build(self):
        cus = list(self.dwarf.iter_CUs())
        for cu in cus:
            self.scan_names(cu.get_top_DIE(), [])
        for cu in cus:
            self.cu = cu
            top = cu.get_top_DIE()
            self.cu_name = astr(top, "DW_AT_name") or ""
            self.walk(top, [])
        self.read_symbols()
        return self

    def to_json(self, include_orig_only=True):
        return {"elf": self.path,
                "types": self.types,
                "type_cus": self.type_cus,
                "variants": self.variants,
                "abi_types": {
                    name: sorted(by_cu.values(), key=lambda r: r["cu"])
                    for name, by_cu in self.abi_types.items()},
                "conflicts": self.conflicts,
                "funcs": self.funcs,
                "vars": {str(k): v for k, v in self.vars.items()},
                "objects": self.objects}


def main(argv):
    if len(argv) < 3:
        print("usage: dwarf_model.py <elf> <out.json> [--no-vars]", file=sys.stderr)
        return 2
    path, out = argv[1], argv[2]
    ctx = Context(path).build()
    data = ctx.to_json()
    if "--no-vars" in argv:
        data.pop("vars", None)
    with open(out, "w") as f:
        json.dump(data, f, indent=1, sort_keys=True)
    print("types=%d funcs=%d vars=%d objects=%d conflicts=%d" %
          (len(data["types"]), len(data["funcs"]), len(data.get("vars", {})),
           len(data["objects"]), len(data["conflicts"])))
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
