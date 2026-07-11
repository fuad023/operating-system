#include <forward_list>
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
    int p[n][3] = { { 0,0,8 }, { 1,1,4 }, { 2,2,9 }, { 3,3,5 } };
    for (int i = 0; i < n; ++i)
    {
        p_list.emplace(p[i][0], p[i][1], p[i][2]);
    }
}

pair<
    forward_list<Process>::iterator,
    forward_list<Process>::iterator
> find_sj(forward_list<Process>& p_list)
{
    // find shortest job as `sj_p`
    auto sj_prev = p_list.before_begin();
    auto prev = p_list.before_begin();
    auto sj_p = p_list.begin();
    for (auto p = p_list.begin(); p != p_list.end(); ++p)
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

int process_at_work(forward_list<Process>::iterator& sj_p, int clock)
{
    int elapsed_time = sj_p->cpu_time;
    Debug_Process& dp = dp_map[sj_p->pid];

    // debug info
    dp.waiting_time = clock - sj_p->arrival_time;
    dp.response_time = elapsed_time;
    dp.turnaround_time = dp.waiting_time + dp.response_time;
    gantt_chart(clock, elapsed_time, sj_p->pid + '0');

    return elapsed_time;
}

void enqueue_process(queue<Process>& p_list, forward_list<Process>& p_que, int clock)
{
    while (not p_list.empty())
    {
        Process& p = p_list.front();
        if (p.arrival_time <= clock)
        {
            p_que.push_front(p);
            p_list.pop();
        }
        else break;
    }
}

int main()
{
    int n = 4;
    queue<Process> p_list;
    init_process(p_list, n);

    int clock = 0;
    forward_list<Process> p_que;
    while (not p_list.empty() | not p_que.empty())
    {
        if (not p_que.empty())
        {
            auto [sj_prev, sj_p] = find_sj(p_que);
            clock += process_at_work(sj_p, clock);
            p_que.erase_after(sj_prev);
        }

        enqueue_process(p_list, p_que, clock);

        // if there is any time gap betw end of a process
        // and arrival of another then pace up the time
        if (p_que.empty())
        {
            int elapsed_time = p_list.front().arrival_time;
            gantt_chart(clock, elapsed_time, '?');
            clock += elapsed_time;
        }
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
