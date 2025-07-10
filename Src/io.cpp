#include "io.h"
#include <iostream>

void read_input() {
    int N, M, a, b;
    std::cin >> N;
    server_types.resize(N);
    for (int i = 0; i < N; ++i) {
        std::cin >> server_types[i].g
                 >> server_types[i].k
                 >> server_types[i].m;
        server_types[i].id = i + 1;
    }

    std::cin >> M;
    users.resize(M);
    solutions.resize(M);
    for (int i = 0; i < M; ++i) {
        users[i].id = i;
        std::cin >> users[i].s
                 >> users[i].e
                 >> users[i].cnt;
    }

    latency.assign(N, std::vector<int>(M));
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < M; ++j)
            std::cin >> latency[i][j];

    std::cin >> a >> b;
}

void write_output() {
    for (size_t i = 0; i < users.size(); ++i) {
        std::cout << solutions[i].size() << "\n";
        if (!solutions[i].empty()) {
            for (size_t j = 0; j < solutions[i].size(); ++j) {
                const auto& r = solutions[i][j];
                std::cout << r.time << " "
                          << r.server << " "
                          << r.npu    << " "
                          << r.B      << (j + 1 == solutions[i].size() ? "" : " ");
            }
        }
        std::cout << "\n";
    }
}
