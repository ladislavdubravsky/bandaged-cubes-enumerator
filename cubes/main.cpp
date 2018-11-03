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


std::string FACES = "UFRDLB";


struct graph {
    std::unordered_set<uint64_t>* verts;
    std::vector<edge>* edges;
    std::map<edge, char>* edgelabels;
    //std::map<uint64_t, int> cube2int {{initcube, 0}};
    //std::map<int, uint64_t> int2cube {{0, initcube}};
};


uint64_t turn_u(uint64_t cube)
{
    return ((cube & UINT64_C(11010069)) << 2) |
           ((cube & UINT64_C(33554496)) >> 6) |
           ((cube & UINT64_C(4398046511104)) << 6) |
           ((cube & UINT64_C(369435906932736)) >> 2) |
           (cube & UINT64_C(18446370239711543210));
}


uint64_t turn_f(uint64_t cube)
{
    return ((cube & UINT64_C(16843008)) << UINT64_C(8)) |
           ((cube & UINT64_C(4294967296)) >> UINT64_C(24)) |
           ((cube & UINT64_C(131072)) << UINT64_C(24)) |
           ((cube & UINT64_C(2207646744576)) >> UINT64_C(8)) |
           ((cube & UINT64_C(17184064512)) << UINT64_C(12)) |
           ((cube & UINT64_C(70368744177664)) >> UINT64_C(36)) |
           (cube & UINT64_C(18446671475822623487));
}


uint64_t turn_r(uint64_t cube)
{
    return ((cube & UINT64_C(806882304)) << 9) |
           ((cube & UINT64_C(412316860416)) >> 27) |
           ((cube & UINT64_C(131072)) << 27) |
           ((cube & UINT64_C(17626612891648)) >> 9) |
           (cube & UINT64_C(18446726033972786175));
}


uint64_t turn_d(uint64_t cube)
{
    return ((cube & UINT64_C(4473378)) << 4) |
           ((cube & UINT64_C(67117056)) >> 12) |
           ((cube & UINT64_C(137438953472)) << 12) |
           ((cube & UINT64_C(600333348765696)) >> 4) |
           (cube & UINT64_C(18446143602850242013));
}


uint64_t turn_l(uint64_t cube)
{
    return ((cube & UINT64_C(1127000492212224)) << 10) |
           ((cube & UINT64_C(1152921504606846976)) >> 30) |
           ((cube & UINT64_C(281483566907392)) << 15) |
           ((cube & UINT64_C(9223372036854775808)) >> 45) |
           ((cube & UINT64_C(8388608)) << 33) |
           ((cube & UINT64_C(72092795589885952)) >> 11) |
           (cube & UINT64_C(7996949252590534655));
}


uint64_t turn_b(uint64_t cube)
{
    return ((cube & UINT64_C(150083337191424)) << 4) |
           ((cube & UINT64_C(2251799813685248)) >> 12) |
           ((cube & UINT64_C(562984315256832)) << 14) |
           ((cube & UINT64_C(9223372036854775808)) >> 42) |
           ((cube & UINT64_C(16384)) << 42) |
           ((cube & UINT64_C(72061992352874496)) >> 14) |
           (cube & UINT64_C(9148345177035751423));
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


graph explore(uint64_t initcube, std::map<char, uint64_t>* blockers)
{
    std::deque<uint64_t> to_visit(1, initcube);
    std::unordered_set<uint64_t>* verts = new std::unordered_set<uint64_t>;
    int counter = 0;
    std::vector<edge>* edges = new std::vector<edge>;
    std::map<uint64_t, int> cube2int {{initcube, 0}};
    std::map<int, uint64_t> int2cube {{0, initcube}};
    std::map<edge, char>* edgelabels = new std::map<edge, char>;

    while (!to_visit.empty()) {
        uint64_t cube = to_visit.front();
        to_visit.pop_front();
        verts->insert(cube);
        for(char& face : FACES) {
            if ((cube & (*blockers)[face]) == 0) {
                uint64_t newcube = turn(face, cube);
                if (verts->find(newcube) == verts->end()
                    && std::find(to_visit.begin(),
                                 to_visit.end(),
                                 newcube) == to_visit.end())
                {
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


void graph_to_file(graph results, std::string path)
{
    std::ofstream file;
    file.open(path);
    for(auto e : *results.edges) {
        file << e.first << ","
             << e.second << ","
             << (*results.edgelabels)[e] << std::endl;
    };
    file.close();
}


main(int argc, char const *argv[])
{
    std::cout << "Started program..." << std::endl;

    std::map<char, uint64_t> blockers {
        {'U', UINT64_C(9223372605132769536)},
        {'F', UINT64_C(1153238438901186563)},
        {'R', UINT64_C(81363862634532)},
        {'D', UINT64_C(73324274936448000)},
        {'B', UINT64_C(18829146325520)},
        {'L', UINT64_C(2819152146341952)}
    };

    uint64_t alcatraz = UINT64_C(76144522270352976);
    uint64_t bicube_fuse = UINT64_C(76162217024955968);
    uint64_t bicube_fuse_rot = UINT64_C(73423368414642736);
    uint64_t most_shapes = UINT64_C(73324292116317184);

    graph results = explore(bicube_fuse, &blockers);
    std::cout << "Found shapes: " << results.verts->size() << std::endl;
    std::cout << "Found edges: " << results.edges->size() << std::endl;
    //for(auto e : *results.verts) 
    //    std::cout << e << std::endl;
    graph_to_file(results, "C:\\temp\\graph.csv");

    delete results.verts;
    delete results.edges;
    delete results.edgelabels;
    std::cin.ignore();
    return 0;
}
