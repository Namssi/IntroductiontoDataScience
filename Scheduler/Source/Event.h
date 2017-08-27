/*
 * Event.h
 *
 *  Created on: Oct 21, 2016
 *      Author: namssi
 */

#ifndef EVENT_H_
#define EVENT_H_

#include "Process.h"

class Event {
public:
	enum TRANSITION{
		TRANS_TO_READY,
		TRANS_TO_RUN,
		TRANS_TO_BLOCK,
		TRANS_TO_TERMINATE,
	};

	Process* evt_process;
	TRANSITION transition;
	int evt_timestamp;

//	Event();
	Event(Process* proc, TRANSITION trans, int time);
	virtual ~Event();
};

#endif /* EVENT_H_ */
