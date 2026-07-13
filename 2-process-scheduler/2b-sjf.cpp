#include <cstdint>
#include <forward_list>
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
    uint32_t p[n][3] = { { 0,0,8 }, { 1,1,4 }, { 2,2,9 }, { 3,3,5 } };
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

pair<
    forward_list<Process>::iterator,
    forward_list<Process>::iterator
> find_sj(forward_list<Process>& process_q)
{
    // find shortest job as `sj_p`
    auto sj_prev = process_q.before_begin();
    auto prev = process_q.before_begin();
    auto sj_p = process_q.begin();
    for (auto p = process_q.begin(); p != process_q.end(); ++p)
    {
        if (p->cpu_time < sj_p->cpu_time)
        {
            sj_p = p;
            sj_prev = prev;
        }
        ++prev;
    }

    return { sj_prev, sj_p };
}

uint64_t cpu(forward_list<Process>::iterator& sj_p, uint64_t clock)
{
    uint32_t duration = sj_p->cpu_time;

    // debug info
    Debug_Process& dp = dp_map[sj_p->pid];
    dp.waiting_time = clock - sj_p->arrival_time;
    dp.response_time = duration;
    dp.turnaround_time = dp.waiting_time + dp.response_time;
    gantt_chart(clock, duration, sj_p->pid + '0');

    return duration;
}

void enq_ready(queue<Process>& process_q, forward_list<Process>& ready_q, uint64_t clock)
{
    while (not process_q.empty())
    {
        Process& p = process_q.front();
        if (p.arrival_time <= clock)
        {
            ready_q.push_front(p);
            process_q.pop();
        }
        else break;
    }
}

int main()
{
    uint32_t n = 4;
    queue<Process> process_q;
    enq_process(process_q, n);

    uint64_t clock = 0;
    forward_list<Process> ready_q;
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
            auto [sj_prev, sj_p] = find_sj(ready_q);
            clock += cpu(sj_p, clock);
            ready_q.erase_after(sj_prev);
        }

        // enqueue ready
        enq_ready(process_q, ready_q, clock);
    }

    print_debug_info(n);
    return 0;
}

/*
    Gantt chart: 0--------P0:8----P1:12-----P3:17---------P2:26
    P#2: TT 24, WT 15, RT  9
    P#3: TT 14, WT  9, RT  5
    P#1: TT 11, WT  7, RT  4
    P#0: TT  8, WT  0, RT  8
    Average TT 14.25
    Average WT 7.75
    Average RT 6.50
*/
