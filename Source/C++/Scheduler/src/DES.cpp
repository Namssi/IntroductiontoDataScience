//============================================================================
// Name        : DES.cpp
// Author      : Namssi
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
//#include <ctype.h>
//#include <stdio.h>
//#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <map>
#include "Scheduler.h"
#include "Process.h"
#include "Event.h"

using namespace std;

#define BUF_SIZE 1024
#define MAX_STATIC_PRIORITY 4

ifstream g_f;

char* g_scheduler_info;

int g_total_rnd_num;
int g_rnd_cnt = 0;
vector<int> g_rnd_num;

int g_io_cnt = 0;
int g_io_start = 0;
int g_total_io_time = 0;

vector<Event*> g_event_queue;
vector<Process*> g_done_process_queue;

int g_current_time;
bool g_call_scheduler;

Process* g_current_running_process;
Scheduler* g_scheduler;

map<int, int*> g_proc_utilization;

int myrandom(int burst) {
	int random;

	if(g_rnd_cnt == g_total_rnd_num)
		g_rnd_cnt = 0;

	random = 1 + (g_rnd_num[g_rnd_cnt] % burst);
	g_rnd_cnt++;

	return random;
}

Event* get_event(){
	Event *event;
	if( g_event_queue.size() == 0 )
		return NULL;
	event = g_event_queue[0];
	g_event_queue.erase(g_event_queue.begin());
	return event;
}
int get_next_event_time(){
	if( g_event_queue.size() == 0 )
			return -1;
	return g_event_queue[0]->evt_timestamp;
}

void create_event(Process *proc, Event::TRANSITION trans){
	if(!proc->isCompleted){
		int evt_timestamp;
		switch(trans){
		case Event::TRANS_TO_READY:
			if(proc->next_state == Process::CREATED){
				evt_timestamp = proc->arrival_time;
			}
			else if(proc->next_state == Process::BLOCK){
				evt_timestamp = g_current_time + proc->io_burst;
			}
			else if(proc->next_state == Process::RUNNING){
				if(proc->qt_cnt == 0){
					evt_timestamp = g_current_time + proc->cpu_burst;
				}
				else if(proc->qt_cnt > 0){
					if(proc->cpu_burst > g_scheduler->quantum)
						evt_timestamp = g_current_time + g_scheduler->quantum;//check quantum when finishing running
					else
						evt_timestamp = g_current_time + proc->cpu_burst;
				}
			}
			break;
		case Event::TRANS_TO_RUN:
			evt_timestamp = g_current_time;
			if(proc->qt_cnt == 0){
				int cpu_burst = myrandom(proc->cpu_burst_range);
				if(cpu_burst <= g_scheduler->quantum){
					if(cpu_burst > proc->rem_cpu)
						cpu_burst = proc->rem_cpu;
				}
				else{
					if(cpu_burst > proc->rem_cpu)
						cpu_burst = proc->rem_cpu;
					if(cpu_burst > g_scheduler->quantum){
						proc->qt_cnt = cpu_burst /g_scheduler->quantum;
						if( (cpu_burst%(g_scheduler->quantum)) > 0) proc->qt_cnt = proc->qt_cnt + 1;
					}
				}
				proc->cpu_burst = cpu_burst;
			}
			break;
		case Event::TRANS_TO_TERMINATE:
			evt_timestamp = g_current_time + proc->cpu_burst;
			break;
		case Event::TRANS_TO_BLOCK:
			int io_burst = myrandom(proc->io_burst_range);
			evt_timestamp = g_current_time + proc->cpu_burst;
			proc->io_burst = io_burst;
			break;

		}

		Event *newEvt = new Event(proc, trans, evt_timestamp);

		//find the right place
		int i = 0;
		while( (i<g_event_queue.size()) && (newEvt->evt_timestamp >= g_event_queue[i]->evt_timestamp) ){i++;}
		g_event_queue.insert( g_event_queue.begin() + i, newEvt);
	}

}

void prnt_trace(Event *evt){/*
	int timestamp = g_current_time;
	int pid = evt->evt_process->pid;
	int timein_cur_state = evt->evt_process->timein_prev_state;

	cout<<timestamp<<" "<<pid<<" "<<timein_cur_state<<":";

	if(evt->evt_process->isCompleted) cout<<"Done";
	else{
		switch(evt->evt_process->cur_state){
		case Process::CREATED:
			cout<<"CREATED -> ";
			break;
		case Process::READY:
			cout<<"READY -> ";
			break;
		case Process::RUNNING:
			cout<<"RUNNG -> ";
			break;
		case Process::BLOCK:
			cout<<"BLOCK -> ";
			break;
		}
		switch(evt->evt_process->next_state){
		case Process::CREATED:
			cout<<"CREATED";
			break;
		case Process::READY:
			cout<<"READY";
			if(evt->evt_process->cur_state==Process::RUNNING)
				cout<<" cb="<<evt->evt_process->cpu_burst<<" rem="<<evt->evt_process->rem_cpu<<" prio="<<evt->evt_process->dynamic_priority;
			break;
		case Process::RUNNING:
			cout<<"RUNNG cb="<<evt->evt_process->cpu_burst<<" rem="<<evt->evt_process->rem_cpu<<" prio="<<evt->evt_process->dynamic_priority;
			break;
		case Process::BLOCK:
			cout<<"BLOCK ib="<<evt->evt_process->io_burst<<" rem="<<evt->evt_process->rem_cpu;
			break;
		}
	}
	cout<<endl;*/
}
void update_process_remain_cpu_time(Process* proc){
	int* utilization = proc->update_remain_cpu_time(g_scheduler->quantum);
	if(utilization != NULL){
		g_proc_utilization.insert( pair<int, int*>(utilization[0], utilization));
		g_done_process_queue.push_back(proc);
	}
}
void update_process_io_time(Process* proc){
	proc->done_io = proc->done_io + proc->io_burst;
}
void update_process_cw_time(Process* proc){
	proc->cpu_waiting_time = proc->cpu_waiting_time + proc->timein_prev_state;
}

void simulation() {
	Event* evt;
	while( (evt = get_event()) ) {
		g_current_time = evt->evt_timestamp;
		evt->evt_process->timein_prev_state = g_current_time - evt->evt_process->state_timestamp;
		evt->evt_process->state_timestamp = g_current_time;
		switch(evt->transition) {  // which state to transition to?
			case Event::TRANS_TO_READY:
				evt->evt_process->cur_state = evt->evt_process->next_state;
				evt->evt_process->next_state = Process::READY;
				// must come from BLOCKED or from PREEMPTION
				if(evt->evt_process->cur_state == Process::CREATED){
					g_scheduler->add_process(evt->evt_process);//add to run queue
					g_call_scheduler = true; //TODO ??? conditional on whether something is run
					prnt_trace(evt);
				}
				else if(evt->evt_process->cur_state == Process::RUNNING){
					update_process_remain_cpu_time(evt->evt_process);
					if(evt->evt_process->qt_cnt > 0){
						if(evt->evt_process->cpu_burst > g_scheduler->quantum)
							evt->evt_process->cpu_burst = evt->evt_process->cpu_burst - g_scheduler->quantum;
						else
							evt->evt_process->cpu_burst = evt->evt_process->cpu_burst - evt->evt_process->cpu_burst;
						evt->evt_process->qt_cnt--;
						g_scheduler->update_process_dynamic_prio(evt->evt_process);
					}
					prnt_trace(evt);
					g_scheduler->add_process(evt->evt_process);//add to run queue
					g_current_running_process = NULL;
					g_call_scheduler = true; //TODO ??? conditional on whether something is run
				}
				else if(evt->evt_process->cur_state == Process::BLOCK){
					g_io_cnt--;
					if(g_io_cnt == 0)
						g_total_io_time = g_total_io_time + g_current_time - g_io_start;
					update_process_io_time(evt->evt_process);
					evt->evt_process->dynamic_priority = -1;
					g_scheduler->add_process(evt->evt_process);//add to run queue
					g_call_scheduler = true; //TODO ??? conditional on whether something is run
					prnt_trace(evt);
				}
				break;
			case Event::TRANS_TO_RUN:// create event for either preemption or blocking
				evt->evt_process->cur_state = evt->evt_process->next_state;
				evt->evt_process->next_state = Process::RUNNING;
				update_process_cw_time(evt->evt_process);
				if( (evt->evt_process->cpu_burst<=g_scheduler->quantum)
						&& (evt->evt_process->rem_cpu == evt->evt_process->cpu_burst) )
					create_event(evt->evt_process, Event::TRANS_TO_TERMINATE);
				else if(evt->evt_process->qt_cnt > 1)	create_event(evt->evt_process, Event::TRANS_TO_READY);
				else	create_event(evt->evt_process, Event::TRANS_TO_BLOCK);
				prnt_trace(evt);
				break;
			case Event::TRANS_TO_BLOCK://create an event for when process becomes READY again
				evt->evt_process->cur_state = evt->evt_process->next_state;
				evt->evt_process->next_state = Process::BLOCK;
				update_process_remain_cpu_time(evt->evt_process);
				if(evt->evt_process->qt_cnt > 0){
					if(evt->evt_process->cpu_burst > g_scheduler->quantum)
						evt->evt_process->cpu_burst = evt->evt_process->cpu_burst - g_scheduler->quantum;
					else
						evt->evt_process->cpu_burst = evt->evt_process->cpu_burst - evt->evt_process->cpu_burst;
					evt->evt_process->qt_cnt--;
					g_scheduler->update_process_dynamic_prio(evt->evt_process);
				}
				g_call_scheduler = true;
				g_current_running_process = NULL;
				if(g_io_cnt == 0)
					g_io_start = g_current_time;
				if(!evt->evt_process->isCompleted)	g_io_cnt++;
				prnt_trace(evt);
				create_event(evt->evt_process, Event::TRANS_TO_READY);
				break;
			case Event::TRANS_TO_TERMINATE:
				evt->evt_process->cur_state = evt->evt_process->next_state;
				evt->evt_process->next_state = Process::TERMINATED;
				update_process_remain_cpu_time(evt->evt_process);
				g_call_scheduler = true;
				g_current_running_process = NULL;
				prnt_trace(evt);
				break;
//			case Event::TRANS_TO_PREEMPT:
//				// add to runqueue (no event is generated)
//				evt->evt_process->next_state = Process::BLOCK;
//				g_scheduler->add_process(evt->evt_process);//add to run queue
//				g_call_scheduler = true;
//				if(evt->evt_process->cur_state == Process::RUNNING){
//					update_process_remain_cpu_time(evt->evt_process);
//					g_current_running_process = NULL;
//				}
//				break;
		}
		delete evt;//remove current event object from Memory

		if(g_call_scheduler) {
			if (get_next_event_time() == g_current_time) {
				continue;//process next event from Event queue
			}
			g_call_scheduler = false;
			if (g_current_running_process == NULL) {
				g_current_running_process = g_scheduler->get_next_process_to_run();//TODO delete pointer
				if (g_current_running_process == NULL) {
					continue;
				}
				else{
					// create event to make process runnable for same time.
					create_event(g_current_running_process, Event::TRANS_TO_RUN);
				}
			}
		}
	}
}

void printResult(){
	cout<<g_scheduler_info;
	if(g_scheduler_info == "RR" || g_scheduler_info=="PRIO")
		cout<<" "<<g_scheduler->quantum;
	cout<<endl;
	int proc_cnt = g_proc_utilization.size();
	int total_TC = 0;
	int total_IT = 0;
	int total_TT = 0;
	int total_CW = 0;
	for(int i = 0; i<proc_cnt; i++){
		printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n",g_proc_utilization[i][0],g_proc_utilization[i][1],
				g_proc_utilization[i][2],g_proc_utilization[i][3],g_proc_utilization[i][4],g_proc_utilization[i][5],
				g_proc_utilization[i][6],g_proc_utilization[i][7],g_proc_utilization[i][8],g_proc_utilization[i][9]);
		total_TC = total_TC + g_proc_utilization[i][2];
		total_IT = total_IT + g_proc_utilization[i][8];
		total_TT = total_TT + g_proc_utilization[i][7];
		total_CW = total_CW + g_proc_utilization[i][9];
	}
	int total_FT = g_proc_utilization[proc_cnt-1][6];
	for(int i=0; i<proc_cnt; i++){
		if( total_FT < g_proc_utilization[i][6] )
			total_FT = g_proc_utilization[i][6];
	}
	double cpu_utilization = (total_TC*1.0/total_FT)*100;
	double io_utilization = (g_total_io_time*1.0/total_FT)*100;
	double avg_TT = total_TT*1.0/proc_cnt;
	double avg_CW = total_CW*1.0/proc_cnt;
	double throughput = double(proc_cnt)*100/total_FT;
	printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n", total_FT,cpu_utilization,io_utilization,avg_TT,avg_CW,throughput);
}

int main(int argc, char* argv[]) {
	int flag_v = 0, flag_s = 0;
	char *svalue = NULL;
	int c;

	//1. get argument [-v] [-s<schedspec>] inputfile randfile
	while ((c = getopt (argc, argv, "vs:")) != -1)
	switch (c)
	{
	case 'v':
		flag_v = 1;
		break;
	case 's':
		svalue = optarg;
		break;
	case '?':
		if (optopt == 'c')
			fprintf (stderr, "Option -%c requires an argument.\n", optopt);
		else if (isprint (optopt))
			fprintf (stderr, "Unknown option `-%c'.\n", optopt);
		else
			fprintf (stderr,"Unknown option character `\\x%x'.\n",optopt);
		return 1;
		default:
		abort ();
	}

	switch(svalue[0]) {
	case 'F':
		g_scheduler = new FCFS(10000);
		g_scheduler_info = "FCFS";
		break;
	case 'L':
		g_scheduler = new LCFS(10000);
		g_scheduler_info = "LCFS";
		break;
	case 'S':
		g_scheduler = new SJF(10000);
		g_scheduler_info = "SJF";
		break;
	case 'R':{
		string quantum_r = &svalue[1];
		g_scheduler = new RR(atoi(quantum_r.c_str()));
		g_scheduler_info = "RR";
		break;}
	case 'P':{
		string quantum_p = &svalue[1];
		g_scheduler = new PRIO(atoi(quantum_p.c_str()));
		g_scheduler_info = "PRIO";
		break;}
	}

	g_f.open(argv[2]);//"lab2_assign/input2");//f.open(argv[1]);
	if(!g_f.is_open()){
		cout<<"There's no such a file.";
	}
	//1-1. get random file
	ifstream random_f;
	random_f.open(argv[3]);//"lab2_assign/rfile");
	if(!random_f.is_open()){
		cout<<"There's no such a file.";
	}
	random_f>>g_total_rnd_num;
	for(int i=0; i<g_total_rnd_num; i++){
		int rnd_num;
		random_f>>rnd_num;
		g_rnd_num.push_back(rnd_num);
	}

	//2. read input files
	int pid = 0;
	string input;
	int count=0;
	int AT, TC, CB, IO;
	while( !g_f.eof() ){
		g_f>>input;
		switch(count%4){
		case 0:
			AT = atoi(input.c_str());
			break;
		case 1:
			TC = atoi(input.c_str());
			break;
		case 2:
			CB = atoi(input.c_str());
			break;
		case 3:
			IO = atoi(input.c_str());
//			cout<<"AT: "<<AT<<", TC: "<<TC<<", CB: "<<CB<<", IO: "<<IO<<endl;
			//3. create process
			Process* proc = new Process(pid, myrandom(MAX_STATIC_PRIORITY), AT, TC, CB, IO, Process::CREATED);
			//4. create events for new processes
			create_event(proc, Event::TRANS_TO_READY);
			pid++;
			break;
		}
		count++;
	}

	//5. start simulation
	simulation();

	printResult();

	g_f.close();
	random_f.close();
	delete g_scheduler;
	while(g_done_process_queue.size()!=0){
		delete g_done_process_queue[0];
		g_done_process_queue.erase(g_done_process_queue.begin());
	}

	return 0;
}


