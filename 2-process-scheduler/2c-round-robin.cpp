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

void gantt_chart(int clock, int time, char pid)
{
    if (time == 0) return;

    if(clock == 0) cout << "Gantt chart: 0";
    int i = time;
    while (i--) cout << '-'; // # of dashes is equal to `elapsed_time`
    printf("P%c:%d", pid, clock + time);
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

void init_process(queue<Process>& p_list, int n)
{
    // default processes, MUST be sorted by arrival time
    // Process{ pid, arrival_time, cpu_time }
    int p[n][3] = { { 1,0,11 }, { 2,2,9 }, { 3,3,6 }, { 4,6,3 }, { 5,15,11 }, { 6,21,4 }, { 7,31,2 } };
    for (int i = 0; i < n; ++i)
    {
        p_list.emplace(p[i][0], p[i][1], p[i][2]);
    }
}

int process_at_work(Process& p, int qt, int clock)
{
    int elapsed_time = min(p.cpu_time, qt);
    p.cpu_time -= elapsed_time;

    // debug info
    dp_map[p.pid].response_time += elapsed_time; // accumulates cpu time
    gantt_chart(clock, elapsed_time, p.pid + '0');

    return elapsed_time;
}

void enqueue_process(queue<Process>& p_list, queue<Process>& p_que, int clock)
{
    while (not p_list.empty())
    {
        Process& p = p_list.front();
        if (p.arrival_time <= clock)
        {
            p_que.push(p);
            p_list.pop();
        }
        else break;
    }
}

void dequeue_process(queue<Process>& p_que, bool q_back, int clock)
{
    if (q_back)
    {
        Process p = p_que.front();
        p_que.pop();
        if (p.cpu_time > 0)
        {
            p_que.push(p);
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
    int n = 7;
    queue<Process> p_list;
    init_process(p_list, n);

    int clock = 0, qt = 3;
    queue<Process> p_que;
    while (not p_list.empty() | not p_que.empty())
    {
        bool q_back = false;

        // if there is any time gap betw end of a process
        // and arrival of another then pace up the time
        if (p_que.empty())
        {
            int elapsed_time = p_list.front().arrival_time - clock;
            gantt_chart(clock, elapsed_time, '?');
            clock += elapsed_time;
        }
        else
        {
            Process& p = p_que.front();
            clock += process_at_work(p, qt, clock);
            q_back = true;
        }

        enqueue_process(p_list, p_que, clock);
        dequeue_process(p_que, q_back, clock);
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
