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



#include <iostream.h>
#include <pthread.h>

#include "Thread.h"

#define DEBUG
#include "debug.h"

static bool is_multithreaded=false;
static pthread_key_t threadPointer;

struct MyArgument {
	void * (*startFunc) (void *);
	void *arg;
	Thread *thread;
};

void * Thread::myStartFunc (void *data)
{
	MyArgument *arg = (MyArgument *) data;
	pthread_setspecific (threadPointer, arg->thread);
	arg->thread->mutex.lock();
	arg->thread->mutex.unlock();
	void *result = arg->startFunc(arg->arg);


	arg->thread->mutex.lock();
	arg->thread->is_ready = true;
	arg->thread->mutex.unlock();
	delete arg;

	return result;
}

bool Thread::isReady ()
{
	return is_ready;
}

void Thread::stop()
{
	should_stop=true;
}

int Thread::start (void * (*startFunc) (void *), void *argument)
{
	if (!is_multithreaded) {
		pthread_key_create (&threadPointer, 0);
		pthread_setspecific (threadPointer,0);
		is_multithreaded=true;
	} 

	mutex.lock();
	bool can_start = is_ready;
	if (is_ready) {
		is_ready=false;
		should_stop=false;
	}
	mutex.unlock();
	
	if (!can_start)
		return false;

	pthread_t pt;
	MyArgument *arg = new MyArgument();
	arg->arg = argument;
	arg->startFunc = startFunc;
	arg->thread = this;
	return pthread_create (&pt, 0, myStartFunc, arg);
}

Thread::Thread()
{
	done   = -1.0;
	doing  = 0;
	is_ready = true;
}

Thread * Thread::getThread()
{
	return is_multithreaded ? (Thread *)pthread_getspecific(threadPointer) : 0;
}

void Thread::setDoing (const char *str, double val)
{
	if (!is_multithreaded) {
		cout << str << endl;
		return;
	}

	Thread *thread = getThread();
	if (!thread)
		return;
	thread->mutex.lock();
	thread->doing = str;
	thread->done = val;
	thread->mutex.unlock();
}

void Thread::setDone (double val)
{
	Thread *thread = getThread();
	if (!thread)
		return;
	thread->mutex.lock();
	thread->done = val;
	thread->mutex.unlock();
}

void Thread::getProgress(const char *&_doing, double &_done)
{
	mutex.lock();
	_doing = doing;
	_done = done;
	mutex.unlock();
}
