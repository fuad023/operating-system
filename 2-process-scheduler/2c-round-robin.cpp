/* 
 * Pre-emptive round-robin (RR) scheduling algorithm
 * Created at [2026-07-08] authored by [Fuad]
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
    uint32_t p[n][3] = { { 1,0,11 }, { 2,2,9 }, { 3,3,6 }, { 4,6,3 }, { 5,15,11 }, { 6,21,4 }, { 7,31,2 } };
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

uint64_t dispatch_process(Process& p, uint32_t qt, uint64_t clock)
{
    uint32_t duration = min(p.cpu_time, qt);
    p.cpu_time -= duration;

    // debug info
    Debug_Process& dp = dp_map[p.pid];
    dp.response_time += duration;
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
            clock += dispatch_process(running_p, qt, clock);
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
    [Gantt chart] 0---P1:3---P2:6---P3:9---P1:12---P4:15---P2:18---P3:21---P1:24---P5:27---P2:30---P6:33--P1:35---P5:38--P7:40-P6:41---P5:44--P5:46
    PID   TT   WT   RT
     P7    9    7    2
     P6   20   16    4
     P5   31   20   11
     P4    9    6    3
     P3   18   12    6
     P2   28   19    9
     P1   35   24   11
    Average TT 21.429
    Average WT 14.857
    Average RT 6.571
*/
