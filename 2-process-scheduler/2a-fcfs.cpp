#include <iostream>
#include <queue>
#include <unordered_map>
using namespace std;

struct Process
{
    int pid;
    int arrival_time;
    int cpu_time;

    Process(int pid, int at, int ct) : pid(pid), arrival_time(at), cpu_time(ct) {}
};

struct Debug_Process
{
    int turnaround_time = 0;
    int waiting_time = 0;
    int response_time = 0;
};
unordered_map<int, Debug_Process> dp_map;

void init_process(queue<Process>& p_list, int n)
{
    // default processes, MUST be sorted by arrival time
    // Process{ pid, arrival_time, cpu_time }
    int p[n][3] = { { 0,0,5 }, { 1,1,3 }, { 2,2,8 }, { 3,3,6 } };
    for (int i = 0; i < n; ++i)
    {
        p_list.emplace(p[i][0], p[i][1], p[i][2]);
    }
}

int process_at_work(Process& p, int clock)
{
    int elapsed_time = p.cpu_time;

    Debug_Process& dp = dp_map[p.pid];
    dp.waiting_time = clock - p.arrival_time;
    dp.response_time = elapsed_time;
    dp.turnaround_time = dp.waiting_time + dp.response_time;

    // gantt chart
    if(clock == 0) cout << "Gantt chart: 0";
    int i = elapsed_time;
    while (i--) cout << '-'; // # of dashes is equal to `elapsed_time`
    printf("P%d:%d", p.pid, clock + elapsed_time);

    return elapsed_time;
}

void print_debug_info(int n)
{
    double total_tt = 0, total_wt = 0, total_rt = 0;
    for (auto& [pid, p] : dp_map)
    {
        total_tt += p.turnaround_time;
        total_wt += p.waiting_time;
        total_rt += p.response_time;
        printf("\nP#%d: TT %2d, WT %2d, RT %2d", pid, p.turnaround_time, p.waiting_time, p.response_time);
    }

    printf("\nAverage TT %.2lf", total_tt / n);
    printf("\nAverage WT %.2lf", total_wt / n);
    printf("\nAverage RT %.2lf", total_rt / n);
}

int main()
{
    int n = 4;
    queue<Process> p_list;
    init_process(p_list, n);

    int clock = 0;
    while (not p_list.empty())
    {
        Process& p = p_list.front();

        // if there is any time gap betw end of a process
        // and arrival of another then pace up the time
        clock = max(clock, p.arrival_time);

        clock += process_at_work(p, clock);
        p_list.pop();
    }

    print_debug_info(n);
    return 0;
}

/*
    Gantt chart: 0-----P0:5---P1:8--------P2:16------P3:22
    P#3: TT 19, WT 13, RT  6
    P#2: TT 14, WT  6, RT  8
    P#1: TT  7, WT  4, RT  3
    P#0: TT  5, WT  0, RT  5
    Average TT 11.25
    Average WT 5.75
    Average RT 5.50
*/
