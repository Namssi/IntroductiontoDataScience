/*
 * Event.cpp
 *
 *  Created on: Oct 21, 2016
 *      Author: namssi
 */

#include "Event.h"

//Event::Event() {
//	// TODO Auto-generated constructor stub
//}

Event::Event(Process* proc, TRANSITION trans, int time){
	evt_process = proc;
	transition = trans;
	evt_timestamp = time;
}

Event::~Event() {
	// TODO Auto-generated destructor stub
}

