/*
 * Process.h
 *
 *  Created on: Oct 20, 2016
 *      Author: namssi
 */

#ifndef PROCESS_H_
#define PROCESS_H_

using namespace std;

class Process {
public:
	enum STATE{
		CREATED,
		READY,
		RUNNING,
		BLOCK,
		TERMINATED
	};

	int utilization[10];

	int pid;
	int static_priority;
	int dynamic_priority;
	int qt_cnt;

	int arrival_time;
	int total_cpu_time;
	int cpu_burst_range;
	int io_burst_range;

	int cpu_burst;
	int io_burst;

	int rem_cpu;
	int done_io;

	STATE cur_state;
	STATE next_state;

	int state_timestamp;//start time of cur state
	int timein_prev_state;

	int finishing_time;
	int turnaround_time;
	int io_time;
	int cpu_waiting_time;

	bool isCompleted;

//	Process();
	Process(int pid, int pri, int a_time, int t_time, int c_burst, int i_burst, STATE c_state);
	int* update_remain_cpu_time(int quan);
	void generate_utilization();
	virtual ~Process();

};

#endif /* PROCESS_H_ */
