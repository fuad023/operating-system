/* 
 * Non pre-emptive priority-scheduling algorithm
 * Created at [2026-07-13] authored by [Fuad]
 */

#include <cstdint>
#include <iostream>
#include <queue>
#include <unordered_map>
using namespace std;

struct Process
{
    uint32_t pid;
    uint64_t arrival_time;
    uint32_t cpu_time;
    uint32_t priority;

    Process(uint32_t pid, uint64_t at, uint32_t ct, uint32_t priority)
    : pid(pid), arrival_time(at), cpu_time(ct), priority(priority) {}
};

// if (CompareProcess{}(a, b))
// cout << "`a` has lower priority than `b`";
struct CompareProcess
{
    bool operator()(const Process& a, const Process& b) const
    {
        return a.priority != b.priority ? a.priority > b.priority
            : a.cpu_time != b.cpu_time ? a.cpu_time > b.cpu_time
            : a.arrival_time != b.arrival_time ? a.arrival_time > b.arrival_time
            : a.pid > b.pid;
    }
};
using ReadyQueue = priority_queue<Process, vector<Process>, CompareProcess>;

struct Debug_Process
{
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
    double total_tt = 0, total_wt = 0, total_rt = 0;
    for (auto& [pid, p] : dp_map)
    {
        total_tt += p.turnaround_time;
        total_wt += p.waiting_time;
        total_rt += p.response_time;
        printf("\nP#%d: TT %2d, WT %2d, RT %2d", pid, p.turnaround_time, p.waiting_time, p.response_time);
    }

    printf("\nAverage TT %.3lf", total_tt / n);
    printf("\nAverage WT %.3lf", total_wt / n);
    printf("\nAverage RT %.3lf\n", total_rt / n);
}

void enq_process(queue<Process>& process_q, uint32_t n)
{
    // default processes, MUST be sorted by arrival time
    // Process{ pid, arrival_time, cpu_time, priority }
    uint32_t p[n][4] = { { 2,0,7,2 }, { 3,2,9,1 }, { 1,4,5,0 } };
    for (uint32_t i = 0; i < n; ++i)
    {
        process_q.emplace(p[i][0], p[i][1], p[i][2], p[i][3]);
    }
}

uint64_t pace_up(Process& p, uint64_t clock)
{
    uint32_t duration = 0;
    if (clock < p.arrival_time)
    {
        duration = p.arrival_time - clock;
        gantt_chart(clock, duration, '?');
    }

    return duration;
}

uint64_t cpu(const Process& p, uint64_t clock)
{
    uint32_t duration = p.cpu_time;

    // debug info
    Debug_Process& dp = dp_map[p.pid];
    dp.waiting_time = clock - p.arrival_time;
    dp.response_time = duration;
    dp.turnaround_time = dp.waiting_time + dp.response_time;
    gantt_chart(clock, duration, p.pid + '0');

    return duration;
}

void enq_ready(queue<Process>& process_q, ReadyQueue& ready_q, uint64_t clock)
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

int main()
{
    uint32_t n = 3;
    queue<Process> process_q;
    enq_process(process_q, n);

    uint64_t clock = 0;
    ReadyQueue ready_q;
    while (not process_q.empty() | not ready_q.empty())
    {
        // if there is any time gap betw end of a process
        // and arrival of another then pace up the time
        if (ready_q.empty())
        {
            Process& p = process_q.front();
            clock += pace_up(p, clock);
        }
        else
        {
            // process on cpu
            const Process& p = ready_q.top();
            clock += cpu(p, clock);
            ready_q.pop();
        }

        // enqueue ready
        enq_ready(process_q, ready_q, clock);
    }

    print_debug_info(n);
    return 0;
}

/*
    [Gantt chart] 0-------P2:7-----P1:12---------P3:21
    P#3: TT 19, WT 10, RT  9
    P#1: TT  8, WT  3, RT  5
    P#2: TT  7, WT  0, RT  7
    Average TT 11.333
    Average WT 4.333
    Average RT 7.000
*/
