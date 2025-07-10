#include "scheduler.h"
#include <algorithm>
#include <cmath>
#include <set>

const double DEADLINE_URGENCY_WEIGHT = 100.0;
const double THROUGHPUT_WEIGHT       = 1.0;
const double MIGRATION_PENALTY       = 2000.0;
const double SLACK_USER_THRESHOLD    = 0.8;
const double SLACK_NPU_TAX           = 0.129;
const long long INF_LL               = std::numeric_limits<long long>::max();
const int MAX_REQUESTS_PER_USER      = 300;

std::vector<ServerType> server_types;
std::vector<User> users;
std::vector<std::vector<int>> latency;
std::vector<NPU> all_npus;
std::vector<std::vector<Result>> solutions;

void initialize() {
    // 标记高性能
    std::vector<double> all_ks;
    for (auto& s : server_types) all_ks.push_back(s.k);
    std::sort(all_ks.rbegin(), all_ks.rend());
    double perf_threshold = all_ks.size() ? all_ks[all_ks.size() / 3] : 0.0;
    for (auto& s : server_types) s.is_high_perf = (s.k >= perf_threshold);

    // 计算 max_b 并填充 all_npus
    int current_global_id = 0;
    for (size_t i = 0; i < server_types.size(); ++i) {
        auto& s = server_types[i];
        s.max_b = std::max(0, std::min(1000, (s.m - b) / a));
        for (int j = 0; j < s.g; ++j) {
            all_npus.push_back({current_global_id++, int(i + 1), j + 1, s.k, 0});
        }
    }

    // 初始化用户状态
    for (auto& u : users) {
        u.rem_cnt = u.cnt;
        u.next_available_send_time = u.s;
        u.last_npu_global_id = -1;
    }
}

std::tuple<double, int, int> find_best_decision_for_npu(const NPU& npu, long long current_time) {
    double best_score = -1.0;
    int best_user = -1, best_B = -1;
    int server_idx = npu.server_type_idx - 1;

    for (size_t i = 0; i < users.size(); ++i) {
        auto& u = users[i];
        if (u.rem_cnt <= 0
         || solutions[i].size() >= MAX_REQUESTS_PER_USER
         || u.next_available_send_time > current_time)
            continue;

        int B = std::min(server_types[server_idx].max_b, 
                         static_cast<int>(u.rem_cnt));
        if (B <= 0) continue;

        int proc = std::max(1,
            int(std::ceil(B / (npu.k * std::sqrt(B))));
        double score = B / double(proc) * THROUGHPUT_WEIGHT;
        double urg = 1.0 + DEADLINE_URGENCY_WEIGHT /
                     std::max(1.0, double(u.e - current_time) / u.rem_cnt);
        score *= urg;

        double remain_ratio = u.e > u.s ?
            double(u.e - current_time) / (u.e - u.s) : 0.0;
        if (remain_ratio > SLACK_USER_THRESHOLD && server_types[server_idx].is_high_perf)
            score *= SLACK_NPU_TAX;

        if (u.last_npu_global_id != -1 && u.last_npu_global_id != npu.global_id)
            score -= MIGRATION_PENALTY;

        if (score > best_score) {
            best_score = score;
            best_user  = int(i);
            best_B     = B;
        }
    }
    return {best_score, best_user, best_B};
}
