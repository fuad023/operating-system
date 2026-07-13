/* 
 * Pre-emptive round-robin (RR) scheduling algorithm
 * Created at [2026-07-08] authored by [Fuad]
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

    Process(uint32_t pid, uint64_t at, uint32_t ct)
    : pid(pid), arrival_time(at), cpu_time(ct) {}
};

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
    // Process{ pid, arrival_time, cpu_time }
    uint32_t p[n][3] = { { 1,0,11 }, { 2,2,9 }, { 3,3,6 }, { 4,6,3 }, { 5,15,11 }, { 6,21,4 }, { 7,31,2 } };
    for (uint32_t i = 0; i < n; ++i)
    {
        process_q.emplace(p[i][0], p[i][1], p[i][2]);
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

uint64_t cpu(Process& p, uint32_t qt, uint64_t clock)
{
    uint32_t duration = min(p.cpu_time, qt);
    p.cpu_time -= duration;

    // debug info
    Debug_Process& dp = dp_map[p.pid];
    dp.response_time += duration;
    gantt_chart(clock, duration, p.pid + '0');

    return duration;
}

void enq_ready(queue<Process>& process_q, queue<Process>& ready_q, uint64_t clock)
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

void deq_ready(queue<Process>& ready_q, bool q_back, uint64_t clock)
{
    if (q_back)
    {
        Process p = ready_q.front(); // if no debug block, create reference instead
        ready_q.pop();
        if (p.cpu_time > 0)
        {
            ready_q.push(p);
        }

        // debug block, goes into else block when a process ends
        else
        {
            Debug_Process& dp = dp_map[p.pid];
            dp.turnaround_time = clock - p.arrival_time; // total time of the process
            dp.waiting_time = dp.turnaround_time - dp.response_time; // wt = tt - ct
        }
    }
}

int main()
{
    uint32_t n = 7;
    queue<Process> process_q;
    enq_process(process_q, n);

    uint64_t clock = 0;
    uint32_t qt = 3;
    queue<Process> ready_q;
    while (not process_q.empty() | not ready_q.empty())
    {
        bool q_back = false;

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
            Process& p = ready_q.front();
            clock += cpu(p, qt, clock);
            q_back = true;
        }

        // enqueue ready
        enq_ready(process_q, ready_q, clock);
        deq_ready(ready_q, q_back, clock);
    }

    print_debug_info(n);
    return 0;
}

/*
    Gantt chart: 0---P1:3---P2:6---P3:9---P1:12---P4:15---P2:18---P3:21---P1:24---P5:27---P2:30---P6:33--P1:35---P5:38--P7:40-P6:41---P5:44--P5:46
    P#7: TT  9, WT  7, RT  2
    P#6: TT 20, WT 16, RT  4
    P#5: TT 31, WT 20, RT 11
    P#4: TT  9, WT  6, RT  3
    P#3: TT 18, WT 12, RT  6
    P#2: TT 28, WT 19, RT  9
    P#1: TT 35, WT 24, RT 11
    Average TT 21.43
    Average WT 14.86
    Average RT 6.57
*/
