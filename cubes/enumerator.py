import numpy as np

# mapping found by backtracking optimizer
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
SLOTS = ["ubl", "ub", "ubr", "ul", "u", "ur", "ufl", "uf", "ufr",
         "bl", "b", "br", "l", "c", "r", "fl", "f", "fr",
         "dbl", "db", "dbr", "dl", "d", "dr", "dfl", "df", "dfr"]
CUBIES = {c: i for i, c in enumerate(SLOTS)}

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
    With a few other tweaks to be spotted below, how many distinct bandage
    shapes are there?
    """
    p1 = 1
    p2 = 1 + p1**2
    p3 = 1 + 2*p2*p1 - 1
    p22 = 1 + 2*p2**2 - 1
    p32 = 1 + 2*p22*p2 + p3**2 - p2**3 - 2*p2**2 + 1
    p33 = p32*p3
    p222 = 1 + 3*p22**2 - 3*p2**4 + 1
    p322 = 1 + 2*p32**2 + 2*p222*p22 - p3**4 - 4*p2**2*p22**2 - p22**3 + 2*p2**4 + 2*p2**6 - 1
    p332 = p33**2 + p322*p32 - p32**2*p3**2
    p333 = 1 + p332*p33
    return p333


def get_cubies(pair):
    """ Return the two cubies belonging to a pair. """
    c1 = "".join(c for c in pair if c.islower())  # edge
    c1 = c1 if c1 in CUBIES.keys() else c1[::-1]
    if len(pair) == 3:
        c2 = pair.lower()  # corner
    else:
        c2 = pair[0]  # face center
    return c1, c2


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
        c1, c2 = get_cubies(pair)
        if cubelist[CUBIES[c1]] == cubelist[CUBIES[c2]] > 0:
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
        c1, c2 = get_cubies(pair)
        code.append(cond.format(CUBIES[c1], CUBIES[c2], 2 ** mapping[pair]))
    code.append("    return np.uint64(res)")
    return "".join(code)

# define into_bitarray_fast function whose body is just lot of magic constants
exec(into_bitarray_gencode(MAPPING))