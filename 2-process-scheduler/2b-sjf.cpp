/* 
 * Non pre-emptive shortest-job-first (SJF) scheduling algorithm
 * Created at [2026-07-10] authored by [Fuad]
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

    Process() {}
    Process(uint32_t pid, uint64_t at, uint32_t ct)
    : pid(pid), arrival_time(at), cpu_time(ct) {}
};

// if (CompareProcess{}(a, b))
// cout << "`a` has lower priority than `b`";
struct CompareProcess
{
    bool operator()(const Process& a, const Process& b) const
    {
        return a.cpu_time != b.cpu_time ? a.cpu_time > b.cpu_time
            : a.arrival_time != b.arrival_time ? a.arrival_time > b.arrival_time
            : a.pid > b.pid;
    }
};
using ReadyQueue = std::priority_queue<Process, std::vector<Process>, CompareProcess>;

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
    cout << "\n PID   TT   WT   RT\n";
    double total_tt = 0, total_wt = 0, total_rt = 0;
    for (auto& [pid, p] : dp_map)
    {
        total_tt += p.turnaround_time;
        total_wt += p.waiting_time;
        total_rt += p.response_time;
        cout << "  P" << pid
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
    // Process{ pid, arrival_time, cpu_time }
    uint32_t p[n][3] = { { 0,0,8 }, { 1,1,4 }, { 2,2,9 }, { 3,3,5 } };
    for (uint32_t i = 0; i < n; ++i)
    {
        process_q.emplace(p[i][0], p[i][1], p[i][2]);
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
    uint32_t duration = p.cpu_time;

    // debug info
    Debug_Process& dp = dp_map[p.pid];
    dp.waiting_time = clock - p.arrival_time;
    dp.response_time = duration;
    dp.turnaround_time = dp.waiting_time + dp.response_time;
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

int main()
{
    uint32_t n = 4;
    queue<Process> process_q;
    enq_process(process_q, n);

    uint64_t clock = 0;
    ReadyQueue ready_q;
    while (not process_q.empty() | not ready_q.empty())
    {
        Process running_p;

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
            clock += dispatch_process(running_p, clock);
        }

        // admit newly arrived processes into ready queue
        admit_processes(process_q, ready_q, clock);
    }

    print_debug_info(n);
    return 0;
}

/*
    [Gantt chart] 0--------P0:8----P1:12-----P3:17---------P2:26
    PID   TT   WT   RT
     P2   24   15    9
     P3   14    9    5
     P1   11    7    4
     P0    8    0    8
    Average TT 14.250
    Average WT 7.750
    Average RT 6.500
*/
