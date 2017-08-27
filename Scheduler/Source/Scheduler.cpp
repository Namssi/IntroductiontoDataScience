/*
 * Scheduler.cpp
 *
 *  Created on: Oct 20, 2016
 *      Author: namssi
 */

#include "Scheduler.h"

Scheduler::Scheduler() {
	// TODO Auto-generated constructor stub

}

Scheduler::~Scheduler() {
	// TODO Auto-generated destructor stub
}


FCFS::FCFS(int quan){
	quantum = quan;
}
void FCFS::add_process(Process *proc){
	run_queue.push_back( pair<int, Process*> (proc->pid, proc) );
}
Process* FCFS::get_next_process_to_run(){
	Process* proc;

	if( run_queue.size() == 0 )
		return NULL;
	proc = run_queue[0].second;
	run_queue.erase(run_queue.begin());

	return proc;
}
void FCFS::update_process_dynamic_prio(Process *p){	}





LCFS::LCFS(int quan){
	quantum = quan;
}
void LCFS::add_process(Process *proc){
	run_queue.push_back( pair<int, Process*> (proc->pid, proc) );
}
Process* LCFS::get_next_process_to_run(void){
	Process* proc;

	if( run_queue.size() == 0 )
		return NULL;
	proc = run_queue[run_queue.size()-1].second;
	run_queue.erase(run_queue.end());

	return proc;
}
void LCFS::update_process_dynamic_prio(Process *p){	}





SJF::SJF(int quan){
	quantum = quan;
}
void SJF::add_process(Process *proc){
	run_queue.push_back( pair<int, Process*> (proc->pid, proc) );
}
Process* SJF::get_next_process_to_run(void){
	Process* proc;

	if( run_queue.size() == 0 )
		return NULL;
	int min = run_queue[0].second->rem_cpu;
	int min_idx = 0;
	for(int i = 0; i<run_queue.size(); i++){
		if(min > run_queue[i].second->rem_cpu){
			min = run_queue[i].second->rem_cpu;
			min_idx = i;
		}
	}
	proc = run_queue[min_idx].second;
	run_queue.erase(run_queue.begin()+min_idx);

	return proc;
}
void SJF::update_process_dynamic_prio(Process *p){	}





RR::RR(int quan){
	quantum = quan;
}
void RR::add_process(Process *proc){
	run_queue.push_back( pair<int, Process*> (proc->pid, proc) );
}
Process* RR::get_next_process_to_run(void){
	Process* proc;

	if( run_queue.size() == 0 )
		return NULL;
	proc = run_queue[0].second;
	run_queue.erase(run_queue.begin());

	return proc;
}
void RR::update_process_dynamic_prio(Process *p){	}




PRIO::PRIO(int quan){
	quantum = quan;
	act_idx = 0;
	exp_idx = 1;
}
void PRIO::add_process(Process *p){
	if(p->dynamic_priority < 0){
		p->dynamic_priority = p->static_priority-1;
		if(p->cur_state == Process::BLOCK) process_queue[act_idx][p->dynamic_priority].push_back( p);
		else process_queue[exp_idx][p->dynamic_priority].push_back( p);

//		for(int i=0; i<=MAX_DYN_PRIORITY ;i++){
//			for(int j=0; j<process_queue[exp_idx][i].size(); j++ ){
//				cout<<"EXP PRIO "<<i<<" => pid: "<<process_queue[exp_idx][i][j]->pid
//						<<", dyn_prio: "<<process_queue[exp_idx][i][j]->dynamic_priority<<endl;
//			}
//		}
	}
	else{
		process_queue[act_idx][p->dynamic_priority].push_back( p);

//		for(int i=0; i<=MAX_DYN_PRIORITY ;i++){
//			for(int j=0; j<process_queue[act_idx][i].size(); j++ ){
//				cout<<"ACTIVE PRIO "<<i<<" => pid: "<<process_queue[act_idx][i][j]->pid
//						<<", dyn_prio: "<<process_queue[act_idx][i][j]->dynamic_priority<<endl;
//			}
//		}
	}
}
Process* PRIO::get_next_process_to_run(void){
	Process* process = NULL;
	bool changed = false;
	for(int i=MAX_DYN_PRIORITY; i>=0; i--){
		if(process_queue[act_idx][i].size() > 0){
			process = process_queue[act_idx][i][0];
			process_queue[act_idx][i].erase(process_queue[act_idx][i].begin());
			break;
		}
		if( (!changed) && (i==0) ){//empty
			if(act_idx == 0){
				act_idx = 1;
				exp_idx = 0;
				i=MAX_DYN_PRIORITY+1;
				changed = true;
				continue;
			}
			else{
				act_idx = 0;
				exp_idx = 1;
				i=MAX_DYN_PRIORITY+1;
				changed = true;
				continue;
			}
		}
	}
	return process;
}
void PRIO::update_process_dynamic_prio(Process *p){
	p->dynamic_priority--;
//	if(p->dynamic_priority < 0){
//		p->dynamic_priority = p->static_priority-1;
//		process_queue[exp_idx][p->dynamic_priority].push_back( p);
//		for(int i=0; i<=MAX_DYN_PRIORITY ;i++){
//			for(int j=0; j<process_queue[exp_idx][i].size(); j++ ){
//				cout<<"EXP PRIO "<<i<<" => pid: "<<process_queue[exp_idx][i][j]->pid
//						<<", dyn_prio: "<<process_queue[exp_idx][i][j]->dynamic_priority<<endl;
//			}
//		}
//	}
//	else{
//		process_queue[act_idx][p->dynamic_priority].push_back( p);//add at lower prio q
		//process_queue[act_idx][i].erase(process_queue[act_idx][i].begin());
//	}
}
