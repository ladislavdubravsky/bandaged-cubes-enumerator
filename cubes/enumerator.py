import numpy as np
import csv

# mapping of pairs to bitarray positions found by backtracking optimizer
MAPPING = {"uFr": 42, "ubR": 44, "uBl": 46, "ufL": 48,
           "ufR": 19, "uBr": 21, "ubL": 23, "uFl": 25,
           "uf": 0, "ur": 2, "ub": 4, "ul": 6,
           "Ufl": 10, "ufR": 19, "Dfr": 28, "dfL": 37,
           "dfR": 18, "Dfl": 33, "ufL": 48, "Ufr": 63,
           "fu": 11, "fr": 20, "fd": 29, "fl": 38,
           "dBr": 14, "Dfr": 28, "uFr": 42, "Ubr": 56,
           "uBr": 21, "Dbr": 35, "dFr": 49, "Ufr": 63,
           "ru": 1, "rb": 3, "rd": 5, "rf": 7,
           "dfL": 37, "dBl": 41, "dbR": 45, "dFr": 49,
           "dBr": 14, "dfR": 18, "dFl": 22, "dbL": 26,
           "df": 39, "dr": 43, "db": 47, "dl": 51,
           "Ubl": 17, "uFl": 25, "Dfl": 33, "dBl": 41,
           "Ufl": 10, "dFl": 22, "Dbl": 34, "uBl": 46,
           "lu": 8, "lb": 16, "ld": 24, "lf": 32,
           "Ubl": 17, "dbL": 26, "Dbr": 35, "ubR": 44,
           "ubL": 23, "Dbl": 34, "dbR": 45, "Ubr": 56,
           "bu": 30, "br": 40, "bd": 50, "bl": 60}

# bit masks for checking face turnability
TURNABLE = {}
for _face in 'UFRDBL':
    TURNABLE[_face] = np.uint64(sum(2**v for k, v in MAPPING.items()
        if _face in k or (len(k) == 2 and _face.lower() == k[1])))

# single cubies with number codes for cubelist representation of bandage shape
CUBIES = ["ubl", "ub", "ubr", "ul", "u", "ur", "ufl", "uf", "ufr",
          "bl", "b", "br", "l", "c", "r", "fl", "f", "fr",
          "dbl", "db", "dbr", "dl", "d", "dr", "dfl", "df", "dfr"]
SLOTS = {c: i for i, c in enumerate(CUBIES)}

# cycles and pairs
CYCLES = [['uFr', 'ubR', 'uBl', 'ufL'], ['uFr', 'Ubr', 'dBr', 'Dfr'],
          ['Ufr', 'dfR', 'Dfl', 'ufL'], ['dfR', 'dBr', 'dbL', 'dFl'],
          ['ubR', 'Dbr', 'dbL', 'Ubl'], ['uFl', 'Ubl', 'dBl', 'Dfl'],
          ['Ufr', 'uBr', 'Dbr', 'dFr'], ['dFr', 'dbR', 'dBl', 'dfL'],
          ['Ubr', 'dbR', 'Dbl', 'ubL'], ['Ufl', 'uBl', 'Dbl', 'dFl'],
          ['ufR', 'uBr', 'ubL', 'uFl'], ['ufR', 'Dfr', 'dfL', 'Ufl'],
          ['uf', 'ur', 'ub', 'ul'], ['fu', 'fr', 'fd', 'fl'],
          ['ru', 'rb', 'rd', 'rf'], ['df', 'dr', 'db', 'dl'],
          ['bu', 'br', 'bd', 'bl'], ['lu', 'lb', 'ld', 'lf']]
PAIRS = set(p for c in CYCLES for p in c)
assert PAIRS == MAPPING.keys()


def enumerate_analytic():
    """
    A face plane is a plane separating a cube face (9 cubies) from
    the rest of the cube (18 cubies). Now:
    1. Start with set S containing one element, the fully bandaged 3x3x3 block
       (i.e., puzzle isomorphic to 1x1x1 cube)
    2. Pick any element from S and split in two it by cutting it by a face
       plane. Replace the original block in S by the two new blocks.
    3. Either end the process or return to step 2.
    We'll call any partition of 3x3x3 cube into blocks resulting from the
    process above a bandage shape.
    With several important tweaks to be spotted below, how many distinct bandage
    shapes are there?
    """
    p1 = 1
    p2 = 1 + p1**2
    p3 = 1 + 2*p2*p1 - 1
    p22 = 1 + 2*p2**2 - 1
    p32 = 1 + 2*p22*p2 + p3**2 - p2**3 - 2*p2**2 + 1
    p33 = 1 + p32*p3
    p22c = 1 + 2*p2 - 1
    p222 = 1 + 3*p22*p22c - 3*p2**3 + 1
    p3c = 1
    p32c = 1 + 2*p22c*p2 + p3*p3c - p2**2 - 2*p2**1 + 1
    p322 = 1 + 2*p32*p32c + 2*p222*p22 - p3**3 - 4*p2**2*p22c*p22 - p22**2*p22c + 2*p2**3 + 2*p2**5 - 1
    p33c = 1 + p32c*p3
    p332 = 1 + p33*p33c + p322*p32 - p32*p32c*p3**2
    p333 = 1 + p332*p33
    return p333  # 6473251


def get_slots(pair):
    """ Return slots of the two cubies belonging to a pair. """
    c1 = "".join(c for c in pair if c.islower())  # edge
    c1 = c1 if c1 in SLOTS.keys() else c1[::-1]
    if len(pair) == 3:
        c2 = pair.lower()  # corner
    else:
        c2 = pair[0]  # face center
    return SLOTS[c1], SLOTS[c2]


def into_bitarray(cubelist, mapping):
    """
    Changes cubelist representation of a bandage shape into bitarray
    representation using given mapping of cubie pairs into bit offsets.
    Example:
        [  0, 0, 0,
         0, 0, 2,
        0, 1, 2,
          0, 0, 0,
         0, 0, 0,
        0, 1, 0,
          0, 0, 0,
         0, 0, 0,
        0, 0, 0]
    should turn into
        2**mapping['uFr'] + 2**mapping['fu']
    since uFr and fu are the only two glued pairs which should have their bit
    set to one.
    """
    rep = 0
    for pair in PAIRS:
        s1, s2 = get_slots(pair)
        if cubelist[s1] == cubelist[s2] > 0:
            rep += 2 ** mapping[pair]
    return np.uint64(rep)


def into_bitarray_gencode(mapping):
    """
    Generate code for faster version of into_bitarray.
    Cubelist numbering scheme:
          00 01 02
         03 04 05
        06 07 08
          09 10 11
         12 13 14
        15 16 17
          18 19 20
         21 22 23
        24 25 26
    """
    code = ["def into_bitarray_fast(c):\n    res=0\n"]
    cond = "    if c[{0}] == c[{1}]:\n        res += {2}\n"
    for pair in PAIRS:
        c1, c2 = get_slots(pair)
        code.append(cond.format(c1, c2, 2 ** mapping[pair]))
    code.append("    return np.uint64(res)")
    return "".join(code)


# define into_bitarray_fast function whose body is just lot of magic constants
exec(into_bitarray_gencode(MAPPING))


def from_bitarray(bitarray, mapping, pprint=True):
    """ Changes bitarray representation of bandage shape into human readable
    cubelist representation. """
    # retrieve list of glued pairs
    glued_pairs = []
    inv = {v: k for k, v in mapping.items()}
    for i in range(64):
        if np.bitwise_and(bitarray, np.uint64(2**i)) != 0:
            glued_pairs.append(inv[i])

    # merge into blocks
    cubelist = np.array(range(1, 28))
    for pair in glued_pairs:
        s1, s2 = get_slots(pair)
        block1 = np.where(cubelist == cubelist[s1])[0]
        cubelist[block1] = cubelist[s2]

    # re-number blocks in reading order
    blockno = 1
    renumber = {}
    for v in cubelist:
        if v in renumber:
            continue
        else:
            renumber[v] = blockno
            blockno += 1
    cubelist = [renumber[b] for b in cubelist]

    if pprint:  # pretty print
        for i, block in enumerate(cubelist):
            if i % 3 == 0:
                print("\n", ((8 - i // 3) % 3)*" ", end="")
            print(str(block).zfill(2), end=" ")
    return cubelist


def split(clist, res, cmax):
    """
    Keep recursively splitting fully bandaged (isomorphic to 1x1) 3x3 cube by
    planes to enumerate all interesting bandaged cubes.
    :param clist: cubelist representation of bandage shape
    :param res: reference to set with enumerated cubelists in bitarray
        representation
    :param cmax: number of blocks in current cubelist
    """
    def split_to_blocks(block1, clist, res, cmax):
        """ Try to add result to hash set and continue splitting. """
        newclist = np.array(clist)
        newclist[block1] = cmax + 1
        newcl_b = into_bitarray_fast(newclist)
        if newcl_b not in res:
            res.add(newcl_b)
            split(newclist, res, cmax + 1)

    if len(res) % 100000 == 0:
        print(len(res))
    for i in range(1, cmax + 1):
        block = np.where(clist == i)[0]
        lb = block.size
        if lb == 27:  # 333 starting block
            # into 33 and 332
            newb1 = block[:9]
            split_to_blocks(newb1, clist, res, cmax)
        elif lb == 18:  # 332 block
            # into 33s
            newb1 = block[:9]
            split_to_blocks(newb1, clist, res, cmax)
            # into 32 and 322
            newb1 = block[::3]
            split_to_blocks(newb1, clist, res, cmax)
        elif lb == 12:  # 322 block
            # into 32s
            newb1 = block[::2]
            split_to_blocks(newb1, clist, res, cmax)
            newb1 = block[:6]
            split_to_blocks(newb1, clist, res, cmax)
            # into 22 and 222
            newb1 = block[[4, 5, 10, 11]]
            split_to_blocks(newb1, clist, res, cmax)
            newb1 = block[[0, 1, 6, 7]]
            split_to_blocks(newb1, clist, res, cmax)
        elif lb == 9:  # 33 block
            # into 3 and 32
            newb1 = block[::3]  # aligned with the 332 vertical cut
            split_to_blocks(newb1, clist, res, cmax)
        elif lb == 8:  # 222 block
            # into 22s
            newb1 = block[:4]
            split_to_blocks(newb1, clist, res, cmax)
            newb1 = block[::2]
            split_to_blocks(newb1, clist, res, cmax)
            newb1 = block[[0, 1, 4, 5]]
            split_to_blocks(newb1, clist, res, cmax)
        elif lb == 6:  # 32 block
            if (block[0] + 2 == block[1] + 1 == block[2] or  # orientation 1
                    block[0] + 6 == block[1] + 3 == block[2]):  # orientation 2
                # into 2 and 22
                newb1 = block[[0, 3]]
                split_to_blocks(newb1, clist, res, cmax)
                newb1 = block[[2, 5]]
                split_to_blocks(newb1, clist, res, cmax)
                # into 3s
                newb1 = block[:3]
                split_to_blocks(newb1, clist, res, cmax)
            elif block[1] + 2 == block[2]:  # orientation 3
                # into 2 and 22
                newb1 = block[:2]
                split_to_blocks(newb1, clist, res, cmax)
                newb1 = block[:4]
                split_to_blocks(newb1, clist, res, cmax)
                # into 3s
                newb1 = block[::2]
                split_to_blocks(newb1, clist, res, cmax)
            else:
                raise Exception("Unexpected 32 block orientation!")
        elif lb == 4:  # 22 block
            # into 2s
            newb1 = block[:2]
            split_to_blocks(newb1, clist, res, cmax)
            newb1 = block[::2]
            split_to_blocks(newb1, clist, res, cmax)
        elif lb == 3:  # 3 block
            # into 1 and 2
            newb1 = block[:2]
            split_to_blocks(newb1, clist, res, cmax)
            newb1 = block[1:]
            split_to_blocks(newb1, clist, res, cmax)
        elif lb == 2:  # 2 block
            # into 1 and 1
            newb1 = block[[0]]
            split_to_blocks(newb1, clist, res, cmax)


res = set()
split(np.ones(27, dtype=np.uint8), res, 1)
print(len(res))
