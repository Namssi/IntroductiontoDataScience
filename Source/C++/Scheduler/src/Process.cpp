/*
 * Process.cpp
 *
 *  Created on: Oct 20, 2016
 *      Author: namssi
 */

#include <iostream>
#include "Process.h"

using namespace std;

//Process::Process(){
//}

Process::Process(int p_num, int pri, int a_time, int t_time, int c_burst, int i_burst, STATE c_state) {
	// TODO Auto-generated constructor stub
	pid = p_num;
	static_priority = pri;
	dynamic_priority = static_priority -1;
	qt_cnt = 0;

	arrival_time = a_time;
	total_cpu_time = t_time;
	cpu_burst_range = c_burst;
	io_burst_range = i_burst;
	cur_state = c_state;
	state_timestamp = a_time;
	isCompleted = false;

	cur_state = CREATED;
	next_state = CREATED;
	cpu_burst = 0;
	io_burst = 0;
	rem_cpu = total_cpu_time;
	done_io = 0;
	timein_prev_state = 0;
	finishing_time = 0;
	turnaround_time = 0;
	io_time = 0;
	cpu_waiting_time = 0;

}

int* Process::update_remain_cpu_time(int quan){
	if(cpu_burst > quan)
		rem_cpu = rem_cpu - quan;
	else
		rem_cpu = rem_cpu - cpu_burst;
	if(rem_cpu == 0){
		isCompleted = true;
		generate_utilization();
		return utilization;
	}
	return NULL;
}

void Process::generate_utilization(){
	utilization[0] = pid;
	utilization[1] = arrival_time;
	utilization[2] = total_cpu_time;
	utilization[3] = cpu_burst_range;
	utilization[4] = io_burst_range;
	utilization[5] = static_priority;
	utilization[6] = arrival_time + total_cpu_time + done_io + cpu_waiting_time;//finishing time
	utilization[7] = total_cpu_time + done_io + cpu_waiting_time;//turnaround time
	utilization[8] = done_io;//io time
	utilization[9] = cpu_waiting_time;//cpu waiting time
}

Process::~Process() {
	// TODO Auto-generated destructor stub
}

