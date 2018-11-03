""" Find mappings of adjacent cubie pairs into 64-bit register positions
minimizing number of instructions needed for face turn permutations. """
import numpy as np


def search(cycles, maps, **kwargs):
    """ Search for best mappings of adjacent cubie pairs into bit offsets. """
    facediff = {f: False for f in "ufrdlb"}
    place_cycle(cycles, {"max": 0, "min": 0}, set(), facediff, maps, **kwargs)


def update_width(mapping, pos):
    """ Keep track of mapping width for pruning. """
    for p in pos:
        if p < mapping["min"]:
            mapping["min"] = p
        elif p > mapping["max"]:
            mapping["max"] = p
    return mapping["max"] - mapping["min"]


def place_cycle(cycles_left, currmap, nonfree, facediff, maps, max_diff=17):
    """
    Take partial assignment of bit offsets (integers 0..63) and try to place
    pairs of a next cycle. If all pairs are placed reasonably well, add mapping
    to results list.
    :param currmap:      current partial mapping of pairs to bit offsets
    :param cycles_left:  list of 4-cycles of pairs remaining to be placed
    :param nonfree:      set of occupied bit offsets
    :param maps:         reference to results list
    :param facediff:     dict of {face: int}. Ideally, the three 4-cycles of a
                         face should map into arithmetic sequences with the same
                         difference. In practice we can't achieve this for each
                         face so we relax on requiring this criterion somewhat.
    :param max_diff:     maximum arithmetic difference of bit offsets in a cycle
    :return              none, mutates maps object
    """
    if len(maps) > 100:
        return
    if not cycles_left:
        if currmap["max"] - currmap["min"] < 64:
            if check_map(currmap, cycles) < 10:
                maps.append(currmap)
        return
    cycle = cycles_left[0]
    placed = [c for c in enumerate(cycle) if c[1] in currmap]
    toplace = [c for c in enumerate(cycle) if c[1] not in currmap]
    toplace_d = dict(toplace)
    face = (set(cycle[0]) & set(cycle[1]) & set(cycle[2]) & set(cycle[3])).pop()

    if len(placed) == 4:
        (i1, p1), (i2, p2), (i3, p3), (i4, p4) = sorted(
            [(i, currmap[p]) for i, p in placed], key=lambda x: x[1])
        if p2 - p1 == p3 - p2 == p4 - p3:  # diffs check
            if 1 == (i2 - i1) % 2 == (i3 - i2) % 2:  # order check
                #if not facediff[face] or (facediff[face] == p2 - p1):
                # cross face compat check
                place_cycle(cycles_left[1:], currmap, nonfree, facediff, maps)

    elif len(placed) == 3:
        fst, snd, trd = sorted([currmap[p[1]] for p in placed])
        if snd - fst != trd - snd:
            if snd - fst == 2*(trd - snd):  # gap to be filled
                #and (not facediff[face] or (facediff[face] == trd - snd)):
                pos = [fst + trd - snd]
                newfd = {**facediff, face: trd - snd}
            elif 2*(snd - fst) == trd - snd and\
                    (not facediff[face] or (facediff[face] == snd - fst)):
                pos = [snd + snd - fst]
                newfd = {**facediff, face: snd - fst}
            else:
                return
            if any(p in nonfree for p in pos):
                return
            newm = {**currmap, toplace[0][1]: pos[0]}
            newnf = nonfree.copy()
            newnf.update(pos)
            place_cycle(cycles_left[1:], newm, newnf, newfd, maps)

        for pos in [[trd + trd - snd], [fst - (trd - snd)]]:  # fill around
            if any(p in nonfree for p in pos):
                continue
            if not facediff[face] or (facediff[face] == trd - snd):
                newm = {**currmap, toplace[0][1]: pos[0]}
                newnf = nonfree.copy()
                newnf.update(pos)
                newfd = {**facediff, face: trd - snd}
                place_cycle(cycles_left[1:], newm, newnf, newfd, maps)

    elif len(placed) == 2:
        (i1, fst), (i2, snd) = sorted([(i, currmap[p]) for i, p in placed],
                                      key=lambda x: x[1])
        d = snd - fst

        if (i1 - i2) % 2 == 1:  # adjacent pairs placed
            if i1 - i2 in {-3, 1}:  # direction is to the left
                i3, i4 = (i2 - 1) % 4, (i2 - 2) % 4
            else:  # direction is to the right
                i3, i4 = (i2 + 1) % 4, (i2 + 2) % 4

            if (d % 3 == 0  # squeeze remaining two inbetween
                and (not facediff[face] or (facediff[face] == d // 3))):
                pos = [fst + d // 3, fst + 2 * (d // 3)]
                if not any(p in nonfree for p in pos):
                    newm = {**currmap, toplace_d[i3]: pos[0],
                            toplace_d[i4]: pos[1]}
                    newnf = nonfree.copy()
                    newnf.update(pos)
                    newfd = {**facediff, face: d // 3}
                    place_cycle(cycles_left[1:], newm, newnf, newfd, maps)

            if not facediff[face] or (facediff[face] == d):
                newfd = {**facediff, face: d}
                for pos in [[snd + d, snd + 2 * d], [snd + d, fst - d],
                            [fst - 2 * d, fst - d]]:  # place remaining two around
                    if any(p in nonfree for p in pos):
                        continue
                    newm = {**currmap, toplace_d[i3]: pos[0], toplace_d[i4]: pos[1]}
                    width = update_width(newm, pos)
                    if width > 63:
                        continue
                    newnf = nonfree.copy()
                    newnf.update(pos)
                    place_cycle(cycles_left[1:], newm, newnf, newfd, maps)

        else:  # opposing pairs placed
            if d % 2 == 1:
                return
            if facediff[face] and facediff[face] != d // 2:
                return
            newfd = {**facediff, face: d // 2}
            for pos in [[fst + d // 2, snd + d // 2],
                        [fst + d // 2, fst - d // 2]]:
                if any(p in nonfree for p in pos):
                    continue
                newnf = nonfree.copy()
                newnf.update(pos)
                for newm in (  # two directions to fill the slots
                    {**currmap, toplace[0][1]: pos[0], toplace[1][1]: pos[1]},
                    {**currmap, toplace[0][1]: pos[1], toplace[1][1]: pos[0]}):
                    width = update_width(newm, pos)
                    if width > 63:
                        continue
                    place_cycle(cycles_left[1:], newm, newnf, newfd, maps)

    elif len(placed) == 1:
        p1 = currmap[placed[0][1]]
        i1 = placed[0][0]
        ds = range(1, max_diff + 1) if not facediff[face] else [facediff[face]]
        for d in ds:
            for pos in [[p1 - 3 * d, p1 - 2 * d, p1 - d],
                        [p1 + d, p1 - 2 * d, p1 - d],
                        [p1 + d, p1 + 2 * d, p1 - d],
                        [p1 + d, p1 + 2 * d, p1 + 3 * d]]:
                if any(p in nonfree for p in pos):
                    continue
                for reverse in (0, 1):  # two directions to fill the slots
                    if reverse == 0:
                        newm = {**currmap, toplace_d[(i1 + 1) % 4]: pos[0],
                                toplace_d[(i1 + 2) % 4]: pos[1],
                                toplace_d[(i1 + 3) % 4]: pos[2]}
                    else:
                        newm = {**currmap, toplace_d[(i1 + 1) % 4]: pos[2],
                                toplace_d[(i1 + 2) % 4]: pos[1],
                                toplace_d[(i1 + 3) % 4]: pos[0]}
                    width = update_width(newm, pos)
                    if width > 63:
                        continue
                    newnf = nonfree.copy()
                    newnf.update(pos)
                    newfd = {**facediff, face: d}
                    place_cycle(cycles_left[1:], newm, newnf, newfd, maps)

    elif len(placed) == 0:
        if len(cycle[0]) == 3:  # first cycle
            for d in range(1, max_diff + 1):
                newm = {**currmap, **{toplace[i][1]: i * d for i in range(4)}}
                newm["max"] = 3 * d
                newnf = nonfree.copy()
                newnf.update([0, d, 2 * d, 3 * d])
                newfd = {**facediff, face: d}
                place_cycle(cycles_left[1:], newm, newnf, newfd, maps)
        else:  # the isolated cycles
            start = min(nonfree) - max_diff
            for d in [facediff[face], *list(range(1, max_diff + 1))]:
                for left in range(start, start + 60):
                    pos = [left, left + d, left + 2 * d, left + 3 * d]
                    if any(p in nonfree for p in pos):
                        continue
                    newm = {**currmap, **{toplace[i][1]: pos[i] for i in range(4)}}
                    width = update_width(newm, [left, left + 3 * d])
                    if width > 63:
                        continue
                    newnf = nonfree.copy()
                    newnf.update(pos)
                    place_cycle(cycles_left[1:], newm, newnf, facediff, maps)


def check_map(mapping, cycles, printout=False):
    """ Score mapping and optionally print it out in a comprehensive format. """
    m = min(mapping.values())
    score = 0
    for f in "ufrdlb":
        pdirs, diffs = [], set()
        for c in [c for c in cycles if all(f in pair for pair in c)]:
            inds = [mapping[p] for p in c]
            inds = np.array([i - m for i in inds])
            pdir = len([i for i in np.roll(inds, 1) - inds if i > 0])
            sind = sorted(inds)
            diff = sind[1] - sind[0]
            diffs.add(diff)
            if printout:
                print(c, inds, 'dir:', pdir, 'diff:', diff)
            pdirs.append(pdir)
        score += len(diffs)
    return score


# calculation of optimal placements
maps = []
cycles = [['uFr', 'ubR', 'uBl', 'ufL'], ['uFr', 'Ubr', 'dBr', 'Dfr'],
          ['ufL', 'Dfl', 'dfR', 'Ufr'], ['dFl', 'dbL', 'dBr', 'dfR'],
          ['ubR', 'Dbr', 'dbL', 'Ubl'], ['uFl', 'Ubl', 'dBl', 'Dfl'],
          ['dFr', 'Dbr', 'uBr', 'Ufr'], ['dFr', 'dbR', 'dBl', 'dfL'],
          ['ubL', 'Dbl', 'dbR', 'Ubr'], ['dFl', 'Dbl', 'uBl', 'Ufl'],
          ['uFl', 'ubL', 'uBr', 'ufR'], ['ufR', 'Dfr', 'dfL', 'Ufl'],
          ['uf', 'ur', 'ub', 'ul'], ['fu', 'fr', 'fd', 'fl'],
          ['ru', 'rb', 'rd', 'rf'], ['df', 'dr', 'db', 'dl'],
          ['bu', 'br', 'bd', 'bl'], ['lu', 'lb', 'ld', 'lf']]
search(cycles, maps, max_diff=21)
print("Min score:", min([check_map(m, cycles) for m in maps]))
check_map(maps[0], cycles, printout=True)

