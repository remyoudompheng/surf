/*
 *   surf - visualizing algebraic curves and algebraic surfaces
 *   Copyright (C) 1996-1997 Friedrich-Alexander-Universitaet
 *                           Erlangen-Nuernberg
 *                 1997-2000 Johannes Gutenberg-Universitaet Mainz
 *   Authors: Stephan Endrass, Hans Huelf, Ruediger Oertel,
 *            Kai Schneider, Ralf Schmitt, Johannes Beigel
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */



#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <pthread.h>



class Mutex
{
public:
	Mutex()			{pthread_mutex_init(&mutex,0);	};
	~Mutex()		{pthread_mutex_destroy(&mutex); };

	bool tryLock()		{return pthread_mutex_trylock(&mutex)==0;}
	void lock ()		{pthread_mutex_lock(&mutex);	};
	void unlock()		{pthread_mutex_unlock(&mutex);	};
	
protected:
	pthread_mutex_t mutex;
	friend class Condition;
};

class Condition : public Mutex
{
public:
	Condition ()		{value=0;pthread_cond_init(&condition, 0);	};
	~Condition()		{pthread_cond_destroy(&condition);};

	void broadcast()	{pthread_cond_broadcast(&condition);	};
	void signal()		{pthread_cond_signal(&condition);	};
	void wait ()            {pthread_cond_wait(&condition, &mutex);	};
	int value;

protected:
	pthread_cond_t condition;
};

class Thread
{
private:
	Thread (const Thread &);
	void operator=(const Thread &);

public:
	Thread();
	int start (void * (*startFunc) (void *), void *argument);
	void stop();
	static bool shouldStop() {Thread *t=getThread(); return t ? t->should_stop : false;};
	
	static void setDoing (const char *str, double val=-1.0);
	static void setDone (double done);
	static Thread * getThread();

	void getProgress (const char * &doing, double &done);
	bool isReady();

protected:
	Mutex mutex;
	const char *doing;
	double done;
	bool is_ready;
	bool should_stop;

protected:
	static void * myStartFunc (void *data);

};



#endif
