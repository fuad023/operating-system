/* 
 * Pre-emptive weighted-round-robin (WRR) scheduling algorithm
 * Created at [2026-07-17] authored by [Fuad]
 */

#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <queue>
#include <unordered_map>
using namespace std;

struct Process
{
    uint32_t pid;
    uint64_t arrival_time;
    uint32_t cpu_time;
    uint32_t v_runtime;

    Process() {}
    Process(uint32_t pid, uint64_t at, uint32_t ct, uint32_t v_rt)
    : pid(pid), arrival_time(at), cpu_time(ct), v_runtime(v_rt) {}
};

struct Debug_Process
{
    uint64_t arrival_time;
    uint32_t cpu_time;

    uint32_t starting_time = -1;
    uint32_t ending_time = -1;

    uint32_t turnaround_time = 0;
    uint32_t waiting_time = 0;
    uint32_t response_time = 0;
};
unordered_map<uint32_t, Debug_Process> dp_map;

void gantt_chart(uint64_t clock, uint32_t duration, char pid)
{
    if (clock == 0)
    {
        cout << "[REPRESENTATION] a---b:c\n"
             << "`a`: starting time\n" << "`b`: ending time\n"
             << "`c`: pid\n" << "`?`: no process\n"
             << "`-`: # of dashes = duration\n\n"
             << "[Gantt chart] 0";
    }

    uint32_t i = duration;
    while (i--) cout << '-';
    printf("P%c:%d", pid, clock + duration);
}

void print_debug_info(uint32_t n)
{
    cout << "\n PID   AT   CT   ST   FT   TT   WT   RT\n";
    double total_tt = 0, total_wt = 0, total_rt = 0;
    for (auto& [pid, p] : dp_map)
    {
        total_tt += p.turnaround_time;
        total_wt += p.waiting_time;
        total_rt += p.response_time;
        cout << "  P" << pid
             << setw(5) << p.arrival_time
             << setw(5) << p.cpu_time
             << setw(5) << p.starting_time
             << setw(5) << p.ending_time
             << setw(5) << p.turnaround_time
             << setw(5) << p.waiting_time
             << setw(5) << p.response_time << '\n';
    }

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Average TT " << total_tt / n << '\n';
    std::cout << "Average WT " << total_wt / n << '\n';
    std::cout << "Average RT " << total_rt / n << '\n';
    std::cout << std::defaultfloat;
}

void enq_process(queue<Process>& process_q, uint32_t n, uint32_t w_mul, uint32_t s_factor)
{
    // default processes, MUST be sorted by arrival time
    // Process{ pid, arrival_time, cpu_time, priority }
    uint32_t p[n][4] = { { 1,0,20,8 }, { 2,6,20,9 }, { 3,15,30,6 }, { 4,17,20,6 }, { 5,22,34,10 } };
    uint32_t w_arr[n];

    uint32_t total_w = 0;
    for (uint32_t i = 0; i < n; ++i)
    {
        w_arr[i] = p[i][3] * w_mul;
        total_w += w_arr[i];
    }

    uint32_t i = 0;
    for (auto& [pid, at, ct, priority] : p)
    {
        uint32_t v_runtime = round(((double) w_arr[i++] / total_w) * s_factor);
        process_q.emplace(pid, at, ct, v_runtime);
        dp_map[pid].arrival_time = at;
        dp_map[pid].cpu_time = ct;
    }
}

uint64_t skip_idle_cpu(Process& p, uint64_t clock)
{
    uint32_t duration = 0;
    if (clock < p.arrival_time)
    {
        duration = p.arrival_time - clock;
        gantt_chart(clock, duration, '?');
    }

    return duration;
}

uint64_t dispatch_process(Process& p, uint64_t clock)
{
    uint32_t duration = min(p.cpu_time, p.v_runtime);
    p.cpu_time -= duration;

    // debug info
    Debug_Process& dp = dp_map[p.pid];
    if (dp.starting_time == -1)
    {
        dp.starting_time = clock;
        dp.response_time = dp.starting_time - dp.arrival_time;
    }
    gantt_chart(clock, duration, p.pid + '0');

    return duration;
}

void admit_processes(queue<Process>& process_q, queue<Process>& ready_q, uint64_t clock)
{
    while (not process_q.empty())
    {
        Process& p = process_q.front();
        if (p.arrival_time <= clock)
        {
            ready_q.push(p);
            process_q.pop();
        }
        else break;
    }
}

void preempt_process(queue<Process>& ready_q, Process& p, bool preempt, uint64_t clock)
{
    if (preempt)
    {
        if (p.cpu_time > 0)
        {
            ready_q.push(p);
        }

        // debug block, goes into else block when a process ends
        else
        {
            Debug_Process& dp = dp_map[p.pid];
            dp.ending_time = clock;
            dp.turnaround_time = dp.ending_time - dp.arrival_time;
            dp.waiting_time = dp.turnaround_time - dp.cpu_time;
        }
    }
}

int main()
{
    uint32_t n = 5, w_mul = 15, s_factor = 50;
    queue<Process> process_q;
    enq_process(process_q, n, w_mul, s_factor);

    uint64_t clock = 0;
    queue<Process> ready_q;
    while (not process_q.empty() | not ready_q.empty())
    {
        Process running_p;
        bool preempt = false;

        // cpu sits idle within time gap betw
        // end of a process and arrival of another
        if (ready_q.empty())
        {
            Process& p = process_q.front();
            clock += skip_idle_cpu(p, clock);
        }
        else
        {
            // dispatch next ready process to cpu
            running_p = ready_q.front(); ready_q.pop();
            clock += dispatch_process(running_p, clock);
            preempt = true;
        }

        // admit newly arrived processes into ready queue
        admit_processes(process_q, ready_q, clock);

        // pre-empt running process
        preempt_process(ready_q, running_p, preempt, clock);
    }

    print_debug_info(n);
    return 0;
}

/*
    [Gantt chart] 0----------P1:10------------P2:22----------P1:32--------P3:40--------P4:48-------------P5:61--------P2:69--------P3:77--------P4:85-------------P5:98--------P3:106----P4:110--------P5:118------P3:124
    PID   AT   CT   ST   FT   TT   WT   RT
     P5   22   34   48  118   96   62   26
     P4   17   20   40  110   93   73   23
     P3   15   30   32  124  109   79   17
     P2    6   20   10   69   63   43    4
     P1    0   20    0   32   32   12    0
    Average TT 78.600
    Average WT 53.800
    Average RT 14.000
*/
