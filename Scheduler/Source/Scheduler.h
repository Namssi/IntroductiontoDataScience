/*
 * Scheduler.h
 *
 *  Created on: Oct 20, 2016
 *      Author: namssi
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <iostream>
#include <vector>
#include "Process.h"

using namespace std;

class Scheduler {
public:
	Scheduler();

	vector< pair<int, Process*> > run_queue;

	int quantum;   // shared by all so the simulation doesn't have to distinguish
	               // set to huge by all but the preemptive schedulers
	public:
	virtual void add_process(Process *p) = 0;
	virtual Process* get_next_process_to_run(void) = 0;
	virtual void update_process_dynamic_prio(Process *p) = 0;

	virtual ~Scheduler();
};

class FCFS : public Scheduler{
public:
	FCFS(int quan);

	void add_process(Process *proc);
	Process* get_next_process_to_run();
	void update_process_dynamic_prio(Process *p);

};

class LCFS : public Scheduler{
public:
	LCFS(int quan);

	void add_process(Process *proc);
	Process* get_next_process_to_run(void);
	void update_process_dynamic_prio(Process *p);
};

class SJF : public Scheduler{
public:
	SJF(int quan);
	void add_process(Process *proc);
	Process* get_next_process_to_run(void);
	void update_process_dynamic_prio(Process *p);
};

class RR : public Scheduler{
public:
	RR(int quan);
	void add_process(Process *proc);
	Process* get_next_process_to_run(void);
	void update_process_dynamic_prio(Process *p);
};


class PRIO : public Scheduler{
#define MAX_DYN_PRIORITY 3
#define ACTIVE 0
#define EXPIRED 1

	vector< Process* > process_queue[2][4];//(2, vector<Process*>(4));//
	int act_idx;
	int exp_idx;

public:
	PRIO(int quan);
	void add_process(Process *p);
	Process* get_next_process_to_run(void);
	void update_process_dynamic_prio(Process *p);
};


#endif /* SCHEDULER_H_ */
