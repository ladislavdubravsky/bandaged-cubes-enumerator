#include <stdint.h>
#include <iostream>
#include <deque>
#include <map>
#include <unordered_set>


std::string FACES = "UFRDLB";


uint64_t turn_u(uint64_t cube)
{
    return ((cube & 11010069) << 2) |
           ((cube & 33554496) >> 6) |
           ((cube & 4398046511104) << 6) |
           ((cube & 369435906932736) >> 2) |
           (cube & 18446370239711543210);
}


uint64_t turn_f(uint64_t cube)
{
    return ((cube & 16843008) << 8) |
           ((cube & 4294967296) >> 24) |
           ((cube & 131072) << 24) |
           ((cube & 2207646744576) >> 8) |
           ((cube & 17184064512) << 12) |
           ((cube & 70368744177664) >> 36) |
           (cube & 18446671475822623487);
}


uint64_t turn_r(uint64_t cube)
{
    return ((cube & 806882304) << 9) |
           ((cube & 412316860416) >> 27) |
           ((cube & 131072) << 27) |
           ((cube & 17626612891648) >> 9) |
           (cube & 18446726033972786175);
}


uint64_t turn_d(uint64_t cube)
{
    return 1;
    /*return np.bitwise_or.reduce([np.left_shift(np.bitwise_and(cube, np.uint64(4473378)), np.uint64(4)),
                                 np.right_shift(np.bitwise_and(cube, np.uint64(67117056)), np.uint64(12)),
                                 np.left_shift(np.bitwise_and(cube, np.uint64(137438953472)), np.uint64(12)),
                                 np.right_shift(np.bitwise_and(cube, np.uint64(600333348765696)), np.uint64(4)),
                                 np.bitwise_and(cube, np.uint64(18446143602850242013))])*/
}


uint64_t turn_l(uint64_t cube)
{
    return 1;
    /*return np.bitwise_or.reduce([np.left_shift(np.bitwise_and(cube, np.uint64(1127000492212224)), np.uint64(10)),
                                 np.right_shift(np.bitwise_and(cube, np.uint64(1152921504606846976)), np.uint64(30)),
                                 np.left_shift(np.bitwise_and(cube, np.uint64(281483566907392)), np.uint64(15)),
                                 np.right_shift(np.bitwise_and(cube, np.uint64(9223372036854775808)), np.uint64(45)),
                                 np.left_shift(np.bitwise_and(cube, np.uint64(8388608)), np.uint64(33)),
                                 np.right_shift(np.bitwise_and(cube, np.uint64(72092795589885952)), np.uint64(11)),
                                 np.bitwise_and(cube, np.uint64(7996949252590534655))])*/
}


uint64_t turn_b(uint64_t cube)
{
    return 1;
    /*return np.bitwise_or.reduce([np.left_shift(np.bitwise_and(cube, np.uint64(150083337191424)), np.uint64(4)),
                                 np.right_shift(np.bitwise_and(cube, np.uint64(2251799813685248)), np.uint64(12)),
                                 np.left_shift(np.bitwise_and(cube, np.uint64(562984315256832)), np.uint64(14)),
                                 np.right_shift(np.bitwise_and(cube, np.uint64(9223372036854775808)), np.uint64(42)),
                                 np.left_shift(np.bitwise_and(cube, np.uint64(16384)), np.uint64(42)),
                                 np.right_shift(np.bitwise_and(cube, np.uint64(72061992352874496)), np.uint64(14)),
                                 np.bitwise_and(cube, np.uint64(9148345177035751423))])*/
}



uint64_t turn(char face, uint64_t cube)
{
    switch (face) {
        case 'U': return turn_u(cube);
        case 'F': return turn_f(cube);
        case 'R': return turn_r(cube);
        case 'D': return turn_d(cube);
        case 'L': return turn_l(cube);
        case 'B': return turn_b(cube);
    }
}


std::unordered_set<uint64_t> explore(uint64_t initcube,
                                     std::map<char, uint64_t> blockers)
{
    std::deque<uint64_t> to_visit;
    to_visit.push_back(initcube);
    std::unordered_set<uint64_t> verts;

    while (!to_visit.empty()) {
        uint64_t cube = to_visit.front();
        to_visit.pop_front();
        verts.insert(cube);
        for(char& face : FACES) {
            //std::cout << f << "\n";
            if ((cube & blockers[face]) == 0) {
                //std::cout << f << " is turnable\n";
                uint64_t new_cube = turn(face, cube);
                if (verts.find(new_cube) == verts.end() /*&&
                    std::find(to_visit.begin(), to_visit.end(), new_cube) == to_visit.end()*/) {
                    to_visit.push_back(new_cube);
                }
            }
        }
    }
    return verts;
}


main(int argc, char const *argv[])
{
    std::cout << "Started program...\n";

    std::map<char, uint64_t> blockers;
    blockers['U'] = 9223372605132769536;
    blockers['F'] = 1153238438901186563;
    blockers['R'] = 81363862634532;
    blockers['D'] = 73324274936448000;
    blockers['B'] = 18829146325520;
    blockers['L'] = 2819152146341952;

    uint64_t alcatraz = 76144522270352976;
    uint64_t bicube_fuse = 76162217024955968;
    std::unordered_set<uint64_t> verts = explore(alcatraz, blockers);
    std::cout << "Found shapes: " << verts.size();
    std::cin.ignore();
    return 0;
}
