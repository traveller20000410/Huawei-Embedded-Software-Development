#ifndef TYPES_H
#define TYPES_H

#include <vector>
#include <tuple>
#include <limits>

// --- 全局常量 ---
extern const double DEADLINE_URGENCY_WEIGHT;
extern const double THROUGHPUT_WEIGHT;
extern const double MIGRATION_PENALTY;
extern const double SLACK_USER_THRESHOLD;
extern const double SLACK_NPU_TAX;
extern const long long INF_LL;
extern const int MAX_REQUESTS_PER_USER;

// --- 数据结构 ---
struct ServerType {
    int id;
    int g;
    double k;
    int m;
    int max_b;
    bool is_high_perf;
};

struct User {
    int id;
    long long s;
    long long e;
    long long cnt;
    long long rem_cnt;
    long long next_available_send_time;
    int last_npu_global_id;
};

struct NPU {
    int global_id;
    int server_type_idx;
    int npu_in_server_idx;
    double k;
    long long free_at;
};

struct Result {
    long long time;
    int server;
    int npu;
    int B;
};

// 全局容器
extern std::vector<ServerType> server_types;
extern std::vector<User> users;
extern std::vector<std::vector<int>> latency;
extern std::vector<NPU> all_npus;
extern std::vector<std::vector<Result>> solutions;

// 函数声明
void read_input();
void initialize();
std::tuple<double, int, int> find_best_decision_for_npu(const NPU& npu, long long current_time);

#endif // TYPES_H
