#include <stdint.h>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <deque>
#include <map>
#include <unordered_set>
#include <algorithm>
#include <utility>


using edge = std::pair<int, int>;  
enum Turns { u, f, r, d, b, l, id, x, x2, xi, y, y2, yi, xz, xz2, xzi, x2y,
             x2y2, x2yi, xiz, xiz2, xizi, z, zx, zx2, zxi, zi, zix, zix2, zixi };
struct graph {
    std::unordered_set<uint64_t>* verts;
    std::vector<edge>* edges;
    std::map<edge, char>* edgelabels;
    //std::map<uint64_t, int> cube2int {{initcube, 0}};
    //std::map<int, uint64_t> int2cube {{0, initcube}};
};


uint64_t turn(Turns face, uint64_t cube);


graph explore(uint64_t initcube, std::map<Turns, uint64_t>* blockers)
{
    std::deque<uint64_t> to_visit(1, initcube);
    auto verts = new std::unordered_set<uint64_t>;
    int counter = 0;
    auto edges = new std::vector<edge>;
    std::map<uint64_t, int> cube2int {{initcube, 0}};
    std::map<int, uint64_t> int2cube {{0, initcube}};
    auto edgelabels = new std::map<edge, char>;

    while (!to_visit.empty()) {
        uint64_t cube = to_visit.front();
        to_visit.pop_front();
        verts->insert(cube);
        for(Turns face: std::vector<Turns> { u, f, r, d, b, l }) {
            if ((cube & (*blockers)[face]) == 0) {
                uint64_t newcube = turn(face, cube);
                if (verts->find(newcube) == verts->end()
                    && std::find(to_visit.begin(),
                                 to_visit.end(),
                                 newcube) == to_visit.end()) {
                    to_visit.push_back(newcube);
                    counter++;
                    cube2int[newcube] = counter;
                    int2cube[counter] = newcube;
                }
                edge newedge = std::make_pair(cube2int[cube], cube2int[newcube]);
                edges->push_back(newedge);
                (*edgelabels)[newedge] = face;
            }
        }
    }
    return graph { verts, edges, edgelabels };
}


void explore_fast(uint64_t initcube, std::map<Turns, uint64_t>* blockers,
                  std::unordered_set<uint64_t>* cubes)
{
    std::deque<uint64_t> to_visit(1, initcube);
    auto verts = new std::unordered_set<uint64_t>;

    while (!to_visit.empty()) {
        uint64_t cube = to_visit.front();
        to_visit.pop_front();
        verts->insert(cube);
        for(Turns rot: { id, x, x2, xi, y, y2, yi, xz, xz2, xzi, x2y, x2y2,
            x2yi, xiz, xiz2, xizi, z, zx, zx2, zxi, zi, zix, zix2, zixi }) {
            uint64_t cube_rot = turn(rot, cube);
            auto pos = cubes->find(cube_rot);
            if (pos != cubes->end()) {
                cubes->erase(pos);
            }
        }
        for(Turns face: std::vector<Turns> { u, f, r, d, b, l }) {
            if ((cube & (*blockers)[face]) == 0) {
                uint64_t newcube = turn(face, cube);
                if (verts->find(newcube) == verts->end()
                    && std::find(to_visit.begin(),
                                 to_visit.end(),
                                 newcube) == to_visit.end()) {
                    to_visit.push_back(newcube);
                }
            }
        }
    }
    delete verts;
}


void explore_all(std::unordered_set<uint64_t>* cubes,
                 std::map<Turns, uint64_t>* blockers)
{
    int cnt = 0;
    auto res = new std::vector<uint64_t>;
    while (!cubes->empty()) {
        uint64_t cube = *cubes->begin();
        cubes->erase(cubes->begin());
        res->push_back(cube);
        explore_fast(cube, blockers, cubes);
        if (cnt % 1000 == 0) {
            std::cout << cnt << std::endl;
        }
        cnt++;
    }
    std::cout << "Remaining cubes:" << res->size() << std::endl;
    delete res;
}


std::unordered_set<uint64_t>* load_cubes(std::string path)
{
    auto cubes = new std::unordered_set<uint64_t>;
    std::ifstream file(path);
    uint64_t c;
    std::cout << "Gonna load from file..." << path << std::endl;
    if (!file) {
        std::cout << "Trouble with file..." << std::endl;
    }
    while (file >> c) {
        //std::cout << c << std::endl;
        cubes->insert(c);
    }
    file.close();
    return cubes;
}


void save_cubes(std::vector<uint64_t>* cubes, std::string path)
{
    std::ofstream file(path);
    for(auto cube: *cubes) {
        file << cube << std::endl;
    };
    file.close();
}


void save_graph(graph results, std::string path)
{
    std::ofstream file(path);
    for(auto e: *results.edges) {
        file << e.first << ","
             << e.second << ","
             << (*results.edgelabels)[e] << std::endl;
    };
    file.close();
}


main(int argc, char const *argv[])
{
    std::cout << "Started program..." << std::endl;

    std::map<Turns, uint64_t> blockers {
        {u, UINT64_C(9296555530816457730)},
        {f, UINT64_C(567902088462465)},
        {r, UINT64_C(2305675885281284)},
        {d, UINT64_C(62008590624)},
        {b, UINT64_C(213305257967640)},
        {l, UINT64_C(1153212188068937792)}
    };

    uint64_t bicube_fuse = UINT64_C(1153354739914719560);

    graph results = explore(bicube_fuse, &blockers);
    std::cout << "Found shapes: " << results.verts->size() << std::endl;
    std::cout << "Found edges: " << results.edges->size() << std::endl;
    save_graph(results, "C:\\temp\\graph_cpp.csv");
    delete results.verts;
    delete results.edges;
    delete results.edgelabels;

    std::unordered_set<uint64_t>* cubes = load_cubes("C:\\temp\\cpp\\all_cubes.txt");
    std::cout << "Cubes loaded from file: " << cubes->size() << std::endl;
    explore_all(cubes, &blockers);
    delete cubes;
    std::cin.ignore();
    return 0;
}


// generated code for face turns and cube rotations
uint64_t turn_u(uint64_t cube)
{
    return 
        ((cube & UINT64_C(92358987743253)) << 2) |
        ((cube & UINT64_C(281475010265152)) >> 6) |
        (cube & UINT64_C(18446370239711543210));
}


uint64_t turn_f(uint64_t cube)
{
    return 
        ((cube & UINT64_C(281483566907392)) << 15) |
        ((cube & UINT64_C(9223372036854775808)) >> 45) |
        ((cube & UINT64_C(806882304)) << 9) |
        ((cube & UINT64_C(412316860416)) >> 27) |
        (cube & UINT64_C(9223090140164125695));
}


uint64_t turn_r(uint64_t cube)
{
    return 
        ((cube & UINT64_C(567382630219776)) << 14) |
        ((cube & UINT64_C(9295429630892703744)) >> 42) |
        ((cube & UINT64_C(42)) << 2) |
        ((cube & UINT64_C(128)) >> 6) |
        (cube & UINT64_C(9150747060186627925));
}


uint64_t turn_d(uint64_t cube)
{
    return 
        ((cube & UINT64_C(187604175962112)) << 4) |
        ((cube & UINT64_C(2814749834215424)) >> 12) |
        (cube & UINT64_C(18443741719699374079));
}


uint64_t turn_l(uint64_t cube)
{
    return 
        ((cube & UINT64_C(8640463104)) << 8) |
        ((cube & UINT64_C(2203318222848)) >> 24) |
        ((cube & UINT64_C(17184064512)) << 12) |
        ((cube & UINT64_C(70368744177664)) >> 36) |
        (cube & UINT64_C(18446671475822623487));
}


uint64_t turn_b(uint64_t cube)
{
    return 
        ((cube & UINT64_C(34426978304)) << 9) |
        ((cube & UINT64_C(17592186044416)) >> 27) |
        ((cube & UINT64_C(35201560346624)) << 11) |
        ((cube & UINT64_C(72057594037927936)) >> 33) |
        ((cube & UINT64_C(1127000492212224)) << 10) |
        ((cube & UINT64_C(1152921504606846976)) >> 30) |
        (cube & UINT64_C(17220585146399195135));
}

uint64_t turn_x(uint64_t cube)
{
    return 
        ((cube & UINT64_C(567382630219776)) << 14) |
        ((cube & UINT64_C(9295429630892703744)) >> 42) |
        ((cube & UINT64_C(42)) << 2) |
        ((cube & UINT64_C(128)) >> 6) |
        ((cube & UINT64_C(2211958554624)) >> 8) |
        ((cube & UINT64_C(131328)) << 24) |
        ((cube & UINT64_C(70385928241152)) >> 12) |
        ((cube & UINT64_C(1024)) << 36) |
        ((cube & UINT64_C(2048)) >> 7) |
        ((cube & UINT64_C(16)) << 26) |
        ((cube & UINT64_C(1073741824)) << 9) |
        ((cube & UINT64_C(549755813888)) >> 28) |
        ((cube & UINT64_C(536870912)) >> 29) |
        ((cube & UINT64_C(1)) << 50) |
        ((cube & UINT64_C(1125899906842624)) >> 3) |
        ((cube & UINT64_C(140737489403904)) >> 18) |
        ((cube & UINT64_C(274877906944)) >> 32) |
        ((cube & UINT64_C(64)) << 54) |
        ((cube & UINT64_C(1152921504606846976)) >> 17) |
        ((cube & UINT64_C(8796093022208)) >> 5) |
        ((cube & UINT64_C(4)) << 38) |
        ((cube & UINT64_C(1237017690112)) << 11) |
        ((cube & UINT64_C(2251799813685248)) >> 31) |
        ((cube & UINT64_C(281474976710656)) >> 25) |
        ((cube & UINT64_C(8388608)) << 3) |
        ((cube & UINT64_C(524288)) << 25) |
        ((cube & UINT64_C(17592186306560)) << 1) |
        ((cube & UINT64_C(35184372088832)) >> 27);
}


uint64_t turn_y(uint64_t cube)
{
    return 
        ((cube & UINT64_C(806882304)) << 9) |
        ((cube & UINT64_C(563362270281728)) >> 27) |
        ((cube & UINT64_C(281483566907392)) << 15) |
        ((cube & UINT64_C(9223372036854775808)) >> 45) |
        ((cube & UINT64_C(17626612891648)) >> 9) |
        ((cube & UINT64_C(147456)) << 27) |
        ((cube & UINT64_C(8388608)) << 33) |
        ((cube & UINT64_C(72092795589885952)) >> 11) |
        ((cube & UINT64_C(1073741824)) << 30) |
        ((cube & UINT64_C(1154048504025317376)) >> 10) |
        ((cube & UINT64_C(64)) >> 5) |
        ((cube & UINT64_C(2)) << 50) |
        ((cube & UINT64_C(2251799813685248)) >> 43) |
        ((cube & UINT64_C(256)) >> 2) |
        ((cube & UINT64_C(4194308)) << 3) |
        ((cube & UINT64_C(32)) << 38) |
        ((cube & UINT64_C(8796093022208)) >> 19) |
        ((cube & UINT64_C(16777216)) >> 22) |
        ((cube & UINT64_C(4398046511105)) << 7) |
        ((cube & UINT64_C(128)) << 32) |
        ((cube & UINT64_C(549757911040)) >> 7) |
        ((cube & UINT64_C(4294967296)) >> 32) |
        ((cube & UINT64_C(16)) >> 1) |
        ((cube & UINT64_C(8)) << 44) |
        ((cube & UINT64_C(140737488355328)) >> 31) |
        ((cube & UINT64_C(65536)) >> 12) |
        ((cube & UINT64_C(33554432)) << 17) |
        ((cube & UINT64_C(70368744177664)) >> 25) |
        ((cube & UINT64_C(2199023255552)) << 5);
}


uint64_t turn_z(uint64_t cube)
{
    return 
        ((cube & UINT64_C(92358987743253)) << 2) |
        ((cube & UINT64_C(281475010265152)) >> 6) |
        ((cube & UINT64_C(687194783744)) << 12) |
        ((cube & UINT64_C(3001666815393792)) >> 4) |
        ((cube & UINT64_C(274877906944)) >> 31) |
        ((cube & UINT64_C(128)) << 33) |
        ((cube & UINT64_C(1100048498688)) >> 24) |
        ((cube & UINT64_C(65536)) << 22) |
        ((cube & UINT64_C(1048576)) >> 17) |
        ((cube & UINT64_C(8)) << 57) |
        ((cube & UINT64_C(1152921504606846976)) >> 28) |
        ((cube & UINT64_C(4294967296)) >> 12) |
        ((cube & UINT64_C(2048)) >> 10) |
        ((cube & UINT64_C(2)) << 49) |
        ((cube & UINT64_C(1125899906842624)) >> 26) |
        ((cube & UINT64_C(16777216)) >> 13) |
        ((cube & UINT64_C(32)) << 25) |
        ((cube & UINT64_C(1073741824)) >> 22) |
        ((cube & UINT64_C(256)) << 21) |
        ((cube & UINT64_C(1024)) << 53) |
        ((cube & UINT64_C(9223372036854906880)) >> 7) |
        ((cube & UINT64_C(72057594037927936)) >> 39) |
        ((cube & UINT64_C(8589934592)) >> 5) |
        ((cube & UINT64_C(268435456)) << 7) |
        ((cube & UINT64_C(51539607552)) >> 1);
}


uint64_t turn_xi(uint64_t cube)
{
    return 
        ((cube & UINT64_C(9295997013520809984)) >> 14) |
        ((cube & UINT64_C(2113536)) << 42) |
        ((cube & UINT64_C(168)) >> 2) |
        ((cube & UINT64_C(2)) << 6) |
        ((cube & UINT64_C(8640463104)) << 8) |
        ((cube & UINT64_C(2203318222848)) >> 24) |
        ((cube & UINT64_C(70368744177664)) >> 36) |
        ((cube & UINT64_C(17184064512)) << 12) |
        ((cube & UINT64_C(549755813888)) >> 9) |
        ((cube & UINT64_C(1073741824)) >> 26) |
        ((cube & UINT64_C(16)) << 7) |
        ((cube & UINT64_C(2048)) << 28) |
        ((cube & UINT64_C(140737488355328)) << 3) |
        ((cube & UINT64_C(1125899906842624)) >> 50) |
        ((cube & UINT64_C(1)) << 29) |
        ((cube & UINT64_C(536870916)) << 18) |
        ((cube & UINT64_C(8796093022208)) << 17) |
        ((cube & UINT64_C(1152921504606846976)) >> 54) |
        ((cube & UINT64_C(64)) << 32) |
        ((cube & UINT64_C(274877906944)) << 5) |
        ((cube & UINT64_C(2533412229349376)) >> 11) |
        ((cube & UINT64_C(1099511627776)) >> 38) |
        ((cube & UINT64_C(1048576)) << 31) |
        ((cube & UINT64_C(67108864)) >> 3) |
        ((cube & UINT64_C(8388608)) << 25) |
        ((cube & UINT64_C(262144)) << 27) |
        ((cube & UINT64_C(35184372613120)) >> 1) |
        ((cube & UINT64_C(17592186044416)) >> 25);
}


uint64_t turn_yi(uint64_t cube)
{
    return 
        ((cube & UINT64_C(413123739648)) >> 9) |
        ((cube & UINT64_C(4197376)) << 27) |
        ((cube & UINT64_C(9223653520421421056)) >> 15) |
        ((cube & UINT64_C(262144)) << 45) |
        ((cube & UINT64_C(19791209299968)) >> 27) |
        ((cube & UINT64_C(34426978304)) << 9) |
        ((cube & UINT64_C(35201560346624)) << 11) |
        ((cube & UINT64_C(72057594037927936)) >> 33) |
        ((cube & UINT64_C(1127000492212224)) << 10) |
        ((cube & UINT64_C(1152921504606846976)) >> 30) |
        ((cube & UINT64_C(256)) << 43) |
        ((cube & UINT64_C(2251799813685248)) >> 50) |
        ((cube & UINT64_C(2)) << 5) |
        ((cube & UINT64_C(64)) << 2) |
        ((cube & UINT64_C(16777216)) << 19) |
        ((cube & UINT64_C(8796093022208)) >> 38) |
        ((cube & UINT64_C(33554464)) >> 3) |
        ((cube & UINT64_C(4)) << 22) |
        ((cube & UINT64_C(4294983680)) << 7) |
        ((cube & UINT64_C(549755813888)) >> 32) |
        ((cube & UINT64_C(562949953421440)) >> 7) |
        ((cube & UINT64_C(1)) << 32) |
        ((cube & UINT64_C(65536)) << 31) |
        ((cube & UINT64_C(140737488355328)) >> 44) |
        ((cube & UINT64_C(8)) << 1) |
        ((cube & UINT64_C(16)) << 12) |
        ((cube & UINT64_C(4398046511104)) >> 17) |
        ((cube & UINT64_C(2097152)) << 25) |
        ((cube & UINT64_C(70368744177664)) >> 5);
}


uint64_t turn_zi(uint64_t cube)
{
    return 
        ((cube & UINT64_C(369435950973012)) >> 2) |
        ((cube & UINT64_C(4398047035393)) << 6) |
        ((cube & UINT64_C(187604175962112)) << 4) |
        ((cube & UINT64_C(2814749834215424)) >> 12) |
        ((cube & UINT64_C(65568)) << 24) |
        ((cube & UINT64_C(1099511627776)) >> 33) |
        ((cube & UINT64_C(128)) << 31) |
        ((cube & UINT64_C(274877906944)) >> 22) |
        ((cube & UINT64_C(4294967296)) << 28) |
        ((cube & UINT64_C(1152921504606846976)) >> 57) |
        ((cube & UINT64_C(8)) << 17) |
        ((cube & UINT64_C(1048576)) << 12) |
        ((cube & UINT64_C(16777216)) << 26) |
        ((cube & UINT64_C(1125899906842624)) >> 49) |
        ((cube & UINT64_C(2)) << 10) |
        ((cube & UINT64_C(2048)) << 13) |
        ((cube & UINT64_C(256)) << 22) |
        ((cube & UINT64_C(1073741824)) >> 25) |
        ((cube & UINT64_C(536870912)) >> 21) |
        ((cube & UINT64_C(131072)) << 39) |
        ((cube & UINT64_C(72057594037928960)) << 7) |
        ((cube & UINT64_C(9223372036854775808)) >> 53) |
        ((cube & UINT64_C(25769803776)) << 1) |
        ((cube & UINT64_C(34359738368)) >> 7) |
        ((cube & UINT64_C(268435456)) << 5);
}


uint64_t turn_x2(uint64_t cube)
{
    return 
        ((cube & UINT64_C(34630287360)) << 28) |
        ((cube & UINT64_C(9295996978892636160)) >> 28) |
        ((cube & UINT64_C(10)) << 4) |
        ((cube & UINT64_C(160)) >> 4) |
        ((cube & UINT64_C(33751296)) << 16) |
        ((cube & UINT64_C(2211924934656)) >> 16) |
        ((cube & UINT64_C(70385924046848)) >> 24) |
        ((cube & UINT64_C(4195328)) << 24) |
        ((cube & UINT64_C(2048)) << 19) |
        ((cube & UINT64_C(1073741824)) >> 19) |
        ((cube & UINT64_C(16)) << 35) |
        ((cube & UINT64_C(549755813888)) >> 35) |
        ((cube & UINT64_C(536870912)) << 21) |
        ((cube & UINT64_C(1125899906842624)) >> 21) |
        ((cube & UINT64_C(1)) << 47) |
        ((cube & UINT64_C(140737488355328)) >> 47) |
        ((cube & UINT64_C(274945015808)) << 22) |
        ((cube & UINT64_C(1153202979583557632)) >> 22) |
        ((cube & UINT64_C(64)) << 37) |
        ((cube & UINT64_C(8796093022208)) >> 37) |
        ((cube & UINT64_C(1048576)) << 20) |
        ((cube & UINT64_C(1099511627776)) >> 20) |
        ((cube & UINT64_C(4)) << 49) |
        ((cube & UINT64_C(2251799813685248)) >> 49) |
        ((cube & UINT64_C(8388608)) << 14) |
        ((cube & UINT64_C(137438953472)) >> 14) |
        ((cube & UINT64_C(786432)) << 26) |
        ((cube & UINT64_C(52776558133248)) >> 26);
}


uint64_t turn_y2(uint64_t cube)
{
    return 
        ((cube & UINT64_C(68815872)) << 18) |
        ((cube & UINT64_C(18039667949568)) >> 18) |
        ((cube & UINT64_C(8590196736)) << 30) |
        ((cube & UINT64_C(9223653511831486464)) >> 30) |
        ((cube & UINT64_C(17188257792)) << 22) |
        ((cube & UINT64_C(72092778410016768)) >> 22) |
        ((cube & UINT64_C(1100591661056)) << 20) |
        ((cube & UINT64_C(1154054001583456256)) >> 20) |
        ((cube & UINT64_C(64)) << 45) |
        ((cube & UINT64_C(2251799813685248)) >> 45) |
        ((cube & UINT64_C(2)) << 7) |
        ((cube & UINT64_C(256)) >> 7) |
        ((cube & UINT64_C(4)) << 41) |
        ((cube & UINT64_C(8796093022208)) >> 41) |
        ((cube & UINT64_C(32)) << 19) |
        ((cube & UINT64_C(16777216)) >> 19) |
        ((cube & UINT64_C(1)) << 39) |
        ((cube & UINT64_C(549755813888)) >> 39) |
        ((cube & UINT64_C(128)) << 25) |
        ((cube & UINT64_C(4294967296)) >> 25) |
        ((cube & UINT64_C(16)) << 43) |
        ((cube & UINT64_C(140737488355328)) >> 43) |
        ((cube & UINT64_C(8)) << 13) |
        ((cube & UINT64_C(65536)) >> 13) |
        ((cube & UINT64_C(33554432)) << 24) |
        ((cube & UINT64_C(562949953421312)) >> 24) |
        ((cube & UINT64_C(70368744177664)) >> 32) |
        ((cube & UINT64_C(16384)) << 32);
}


uint64_t turn_z2(uint64_t cube)
{
    return 
        ((cube & UINT64_C(21990235176965)) << 4) |
        ((cube & UINT64_C(351843762831440)) >> 4) |
        ((cube & UINT64_C(11682311323648)) << 8) |
        ((cube & UINT64_C(2990671698853888)) >> 8) |
        ((cube & UINT64_C(283467841536)) << 2) |
        ((cube & UINT64_C(1133871366144)) >> 2) |
        ((cube & UINT64_C(128)) << 9) |
        ((cube & UINT64_C(65536)) >> 9) |
        ((cube & UINT64_C(1048576)) << 40) |
        ((cube & UINT64_C(1152921504606846976)) >> 40) |
        ((cube & UINT64_C(8)) << 29) |
        ((cube & UINT64_C(4294967296)) >> 29) |
        ((cube & UINT64_C(2048)) << 39) |
        ((cube & UINT64_C(1125899906842624)) >> 39) |
        ((cube & UINT64_C(2)) << 23) |
        ((cube & UINT64_C(16777216)) >> 23) |
        ((cube & UINT64_C(536870912)) << 1) |
        ((cube & UINT64_C(1073741824)) >> 1) |
        ((cube & UINT64_C(32)) << 3) |
        ((cube & UINT64_C(256)) >> 3) |
        ((cube & UINT64_C(132096)) << 46) |
        ((cube & UINT64_C(9295429630892703744)) >> 46) |
        ((cube & UINT64_C(268435456)) << 6) |
        ((cube & UINT64_C(17179869184)) >> 6);
}


uint64_t turn(Turns face, uint64_t cube)
{
    switch (face) {
        case u: return turn_u(cube);
        case f: return turn_f(cube);
        case r: return turn_r(cube);
        case d: return turn_d(cube);
        case l: return turn_l(cube);
        case b: return turn_b(cube);
        case id: return cube;
        case x: return turn_x(cube);
        case x2: return turn_x2(cube);
        case xi: return turn_xi(cube);
        case y: return turn_y(cube);
        case y2: return turn_y2(cube);
        case yi: return turn_yi(cube);
        case xz: return turn_z(turn_x(cube));
        case xz2: return turn_z2(turn_x(cube));
        case xzi: return turn_zi(turn_x(cube));
        case x2y: return turn_y(turn_x2(cube));
        case x2y2: return turn_y2(turn_x2(cube));
        case x2yi: return turn_yi(turn_x2(cube));
        case xiz: return turn_z(turn_xi(cube));
        case xiz2: return turn_z2(turn_xi(cube));
        case xizi: return turn_zi(turn_xi(cube));
        case z: return turn_z(cube);
        case zx: return turn_x(turn_z(cube));
        case zx2: return turn_x2(turn_z(cube));
        case zxi: return turn_xi(turn_z(cube));
        case zi: return turn_zi(cube);
        case zix: return turn_x(turn_zi(cube));
        case zix2: return turn_x2(turn_zi(cube));
        case zixi: return turn_xi(turn_zi(cube));
    }
}
