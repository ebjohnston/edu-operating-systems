#include <iostream>
#include <list>
#include <iterator>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
    Assignment 03 - CS 3600
    Ethan Johnston
*/

#define NUM_SECONDS 20

// make sure the asserts work
#undef NDEBUG
#include <assert.h>

#define EBUG
#ifdef EBUG
#define dmess(a) cout << "in " << __FILE__ << \
    " at " << __LINE__ << " " << a << endl;

#define dprint(a) cout << "in " << __FILE__ << \
    " at " << __LINE__ << " " << (#a) << " = " << a << endl;

#define dprintt(a,b) cout << "in " << __FILE__ << \
    " at " << __LINE__ << " " << a << " " << (#b) << " = " \
    << b << endl
#else
#define dprint(a)
#endif /* EBUG */

using namespace std;

enum STATE { NEW, RUNNING, WAITING, READY, TERMINATED };

/*
** a signal handler for those signals delivered to this process, but
** not already handled.
*/
void grab (int signum) { dprint (signum); }

// c++decl> declare ISV as array 32 of pointer to function (int) returning
// void
void (*ISV[32])(int) = {
/*        00    01    02    03    04    05    06    07    08    09 */
/*  0 */ grab, grab, grab, grab, grab, grab, grab, grab, grab, grab,
/* 10 */ grab, grab, grab, grab, grab, grab, grab, grab, grab, grab,
/* 20 */ grab, grab, grab, grab, grab, grab, grab, grab, grab, grab,
/* 30 */ grab, grab
};

struct PCB
{
    STATE state;
    const char* name;   // name of the executable
    int pid;            // process id from fork();
    int ppid;           // parent process id
    int interrupts;     // number of times interrupted
    int switches;       // may be < interrupts
    int started;        // the time this process started
};

/*
** an overloaded output operator that prints a PCB
*/
ostream& operator << (ostream &os, struct PCB *pcb)
{
    os << "state:        " << pcb->state << endl;
    os << "name:         " << pcb->name << endl;
    os << "pid:          " << pcb->pid << endl;
    os << "ppid:         " << pcb->ppid << endl;
    os << "interrupts:   " << pcb->interrupts << endl;
    os << "switches:     " << pcb->switches << endl;
    os << "started:      " << pcb->started << endl;
    return (os);
}

/*
** an overloaded output operator that prints a list of PCBs
*/
ostream& operator << (ostream &os, list<PCB*> which)
{
    list<PCB*>::iterator PCB_iter;
    for (PCB_iter = which.begin(); PCB_iter != which.end(); PCB_iter++)
    {
        os << (*PCB_iter);
    }
    return (os);
}

PCB* running;
PCB* idle;

// http://www.cplusplus.com/reference/list/list/
list<PCB*> new_list;
list<PCB*> processes;

int sys_time;

/*
**  send signal to process pid every interval for number of times.
*/
void send_signals (int signal, int pid, int interval, int number)
{
    dprintt ("at beginning of send_signals", getpid ());

    for (int i = 1; i <= number; i++)
    {
        sleep (interval);

        dprintt ("sending", signal);
        dprintt ("to", pid);

        if (kill (pid, signal) == -1)
        {
            perror ("kill");
            return;
        }
    }
    dmess ("at end of send_signals");
}

struct sigaction *create_handler (int signum, void (*handler)(int))
{
    struct sigaction *action = new (struct sigaction);

    action->sa_handler = handler;
/*
**  SA_NOCLDSTOP
**  If  signum  is  SIGCHLD, do not receive notification when
**  child processes stop (i.e., when child processes  receive
**  one of SIGSTOP, SIGTSTP, SIGTTIN or SIGTTOU).
*/
    if (signum == SIGCHLD)
    {
        action->sa_flags = SA_NOCLDSTOP;
    }
    else
    {
        action->sa_flags = 0;
    }
    sigemptyset (&(action->sa_mask));

    assert (sigaction (signum, action, NULL) == 0);
    return (action);
}

PCB* choose_process ()
{
    list<PCB*>::iterator process_iter;
    for (process_iter = processes.begin(); process_iter != processes.end(); process_iter++)
    {
        PCB* pending = *process_iter;

        if (pending->state == READY)
        {
            if (pending != running)
            {
                running->switches++;
            }
            // move chosen element to back to cycle round robin
            processes.splice(processes.end(), processes, process_iter);

            PCB* selected = processes.back();
            running = selected;
            return selected;
        }
    }

    // if no process is ready, idle
    return idle;
}

void scheduler (int signum)
{
    assert (signum == SIGALRM);
    sys_time++;

    running->interrupts++;
    running->state = READY;

    if (new_list.size() > 0)
    {
        PCB* activating = new_list.front();

        activating->state = RUNNING;
        activating->started = sys_time;

        int newpid = fork();
        assert(newpid >= 0);

        if (newpid > 0) { // parent process
            activating->pid = newpid;
        }
        else // newpid == 0, child process
        {
            activating->pid = getpid();

            execl(activating->name, activating->name + 2, (char*) NULL);
            assert(errno == 0);
        }

        processes.push_back(activating);
        new_list.pop_front();
    }

    PCB* tocont = choose_process();

    dprintt ("continuing", tocont->pid);
    if (kill (tocont->pid, SIGCONT) == -1)
    {
        perror ("kill");
        return;
    }
}

void process_done (int signum)
{
    assert (signum == SIGCHLD);

    running->interrupts++;

    int status, cpid;

    cpid = waitpid (-1, &status, WNOHANG);

    dprintt ("in process_done", cpid);

    if  (cpid == -1)
    {
        perror ("waitpid");
    }
    else if (cpid == 0)
    {
        if (errno == EINTR) { return; }
        perror ("no children");
    }
    else
    {
        list<PCB*>::iterator process_iter;
        for (process_iter = processes.begin(); process_iter != processes.end(); process_iter++)
        {
            PCB* child = *process_iter;

            cout << "comparing cpid: " << cpid << " against: " << child->pid << "\n";

            if (child->pid == cpid)
            {
                child->state = TERMINATED;
                dprintt ("for terminated child", child->interrupts);
                dprintt ("for terminated child", child->switches);
                cout << "total system time for child process " << cpid << " was: " << sys_time - child->started << "\n";
            }
        }

        dprint (WEXITSTATUS (status));

        if (kill (idle->pid, SIGCONT) == -1)
        {
            perror ("kill");
            return;
        }
    }
}

/*
** stop the running process and index into the ISV to call the ISR
*/
void ISR (int signum)
{
    if (kill (running->pid, SIGSTOP) == -1)
    {
        perror ("kill");
        return;
    }
    dprintt ("stopped", running->pid);

    ISV[signum](signum);
}

/*
** set up the "hardware"
*/
void boot (int pid)
{
    ISV[SIGALRM] = scheduler;       create_handler (SIGALRM, ISR);
    ISV[SIGCHLD] = process_done;    create_handler (SIGCHLD, ISR);

    // start up clock interrupt
    int ret;
    if ((ret = fork ()) == 0)
    {
        // signal this process once a second for three times
        send_signals (SIGALRM, pid, 1, NUM_SECONDS);

        // once that's done, really kill everything...
        kill (0, SIGTERM);
    }

    if (ret < 0)
    {
        perror ("fork");
    }
}

void create_idle ()
{
    int idlepid;

    if ((idlepid = fork ()) == 0)
    {
        dprintt ("idle", getpid ());

        // the pause might be interrupted, so we need to
        // repeat it forever.
        for (;;)
        {
            dmess ("going to sleep");
            pause ();
            if (errno == EINTR)
            {
                dmess ("waking up");
                continue;
            }
            perror ("pause");
        }
    }
    idle = new (PCB);
    idle->state = RUNNING;
    idle->name = "IDLE";
    idle->pid = idlepid;
    idle->ppid = 0;
    idle->interrupts = 0;
    idle->switches = 0;
    idle->started = sys_time;
}

int main (int argc, char **argv)
{
    int pid = getpid();
    dprintt ("main", pid);

    // modification of code by kendra lamb posted on slack
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            cout << "Adding argument " << argv[i] << " to new_list...";

            // note: pid and started are initialized in choose_process
            PCB* process = new (PCB);
            process->state = NEW;
            process->name = argv[i];
            process->ppid = 0;
            process->interrupts = 0;
            process->switches = 0;

            cout << process;
            new_list.push_back(process);
        }
        //cout << new_list;
    }

    sys_time = 0;
    boot (pid);

    // create a process to soak up cycles
    create_idle ();
    running = idle;

    cout << running;

    // we keep this process around so that the children don't die and
    // to keep the IRQs in place.
    for (;;)
    {
        pause();
        if (errno == EINTR) { continue; }
        perror ("pause");
    }
}
