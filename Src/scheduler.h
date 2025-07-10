#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "types.h"

// 初始化全局状态（计算 max_b、标记高性能服务器、生成 all_npus、设置用户 rem_cnt 等）
void initialize();

// 对单个 NPU 在当前时刻选出最优任务：返回（得分，用户 id，batch 大小）
std::tuple<double, int, int> find_best_decision_for_npu(const NPU& npu, long long current_time);

#endif // SCHEDULER_H
