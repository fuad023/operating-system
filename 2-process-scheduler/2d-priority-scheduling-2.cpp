/* 
 * Pre-emptive priority-scheduling algorithm
 * Created at [2026-07-13] authored by [Fuad]
 */

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
    uint32_t priority;

    Process() {}
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

void enq_process(queue<Process>& process_q, uint32_t n)
{
    // default processes, MUST be sorted by arrival time
    // Process{ pid, arrival_time, cpu_time, priority }
    uint32_t p[n][4] = { { 2,0,7,2 }, { 3,2,9,1 }, { 1,4,5,0 } };
    for (auto& [pid, at, ct, priority] : p)
    {
        process_q.emplace(pid, at, ct, priority);
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

uint32_t calc_run_duration(queue<Process>& process_q, Process& p, uint64_t clock)
{
    if (process_q.empty()) return p.cpu_time;

    const Process& next = process_q.front();
    uint32_t duration = p.cpu_time;
    if (next.arrival_time < clock + p.cpu_time && CompareProcess{}(p, next))
    {
        duration = next.arrival_time - clock;
    }

    return duration;
}

uint64_t dispatch_process(Process& p, uint32_t duration, uint64_t clock)
{
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

void admit_processes(queue<Process>& process_q, ReadyQueue& ready_q, uint64_t clock)
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

void preempt_process(ReadyQueue& ready_q, Process& p, bool preempt, uint64_t clock)
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
    uint32_t n = 3;
    queue<Process> process_q;
    enq_process(process_q, n);

    uint64_t clock = 0;
    ReadyQueue ready_q;
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
            running_p = ready_q.top(); ready_q.pop();
            uint32_t duration = calc_run_duration(process_q, running_p, clock);
            clock += dispatch_process(running_p, duration, clock);
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
    [Gantt chart] 0--P2:2--P3:4-----P1:9-------P3:16-----P2:21
    PID   AT   CT   ST   FT   TT   WT   RT
     P1    4    5    4    9    5    0    0
     P3    2    9    9   16   14    5    7
     P2    0    7   16   21   21   14   16
    Average TT 13.333
    Average WT 6.333
    Average RT 7.667
*/
