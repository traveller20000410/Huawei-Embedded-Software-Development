#include <iostream>
#include "io.h"
#include "scheduler.h"

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    read_input();
    initialize();

    long long total_rem = 0;
    for (auto& u : users) total_rem += u.cnt;

    long long current_time = 0;
    while (total_rem > 0) {
        // 计算下一个事件时间
        long long next_time = INF_LL;
        bool has_task = false;
        for (auto& u : users) {
            if (u.rem_cnt > 0) {
                has_task = true;
                next_time = std::min(next_time, u.next_available_send_time);
            }
        }
        if (!has_task) break;
        for (auto& npu : all_npus) {
            if (npu.free_at > current_time)
                next_time = std::min(next_time, npu.free_at);
        }
        if (next_time == INF_LL) break;
        current_time = std::max(current_time, next_time);

        // 收集所有可执行决策
        std::vector<std::tuple<double,int,int,int>> decs;
        for (auto& npu : all_npus) {
            if (npu.free_at <= current_time) {
                auto [score, uid, B] = find_best_decision_for_npu(npu, current_time);
                if (uid != -1) decs.emplace_back(score, npu.global_id, uid, B);
            }
        }

        if (!decs.empty()) {
            std::sort(decs.rbegin(), decs.rend());
            std::set<int> used;
            for (auto& [_, nid, uid, B] : decs) {
                if (used.count(uid)) continue;
                auto& u = users[uid];
                long long t0 = u.next_available_send_time;

                solutions[uid].push_back({t0,
                    all_npus[nid].server_type_idx,
                    all_npus[nid].npu_in_server_idx,
                    B
                });

                long long arr = t0 + latency[all_npus[nid].server_type_idx-1][uid];
                long long start = std::max(arr, all_npus[nid].free_at);
                int proc = std::max(1,
                   int(std::ceil(B / (all_npus[nid].k * std::sqrt(B))))
                );

                all_npus[nid].free_at = start + proc;
                u.rem_cnt     -= B;
                u.last_npu_global_id = nid;
                u.next_available_send_time = arr + 1;
                total_rem    -= B;
                used.insert(uid);
            }
        } else {
            // 若无决策，则时间推进
            ++current_time;
        }
    }

    write_output();
    return 0;
}
