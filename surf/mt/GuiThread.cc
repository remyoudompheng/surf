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



#ifndef NO_GUI

#include <unistd.h>
#include <assert.h>
#include <iostream.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "GuiThread.h"
#include "Thread.h"

// #define DEBUG
#include "debug.h"

#define MAXFUNC 1024


static Mutex tryingToWakeupMutex;
static int tryingToWakeup=0; 
static bool initialized=false;

static Command commands[MAXFUNC];
static int commandCount=0;
static int firstCommand=0;
int filedes[2];
static Condition commandQueue;

static void signalCommandExecuted (void *cond)
{
	BEGIN("signalCommandExecuted");
	Condition *c = (Condition *) cond;
	c->lock();
	c->value = 1;
	c->broadcast();
	c->unlock();
}


bool GuiThread::haveGUI()
{
	return initialized;
}

Condition *condi=0;
void GuiThread::dmess()
{
#if 0
	cout << endl;
	commandQueue.lock();
	cout << "GuiThread::dmess->    commandCount:" << commandCount << "  ";
	commandQueue.unlock();
	tryingToWakeupMutex.lock();
	cout << "tryingToWakeup:" << tryingToWakeup << "  ";
	tryingToWakeupMutex.unlock();
	cout << "condi: " << condi << "   ";
#endif
}

void GuiThread::executeCommands()
{
        BEGIN("GuiThread::executeCommands");
	assert(!GuiThread::isGuiThread());
	Condition commandExecuted;
	condi = &commandExecuted;
	commandExecuted.lock();
	addCommand(signalCommandExecuted, &commandExecuted);
	wakeup();
	while (commandExecuted.value == 0) {
// 		cout << "Thread::   Waiting for command execution..." << endl;
		commandExecuted.wait();
	}
// 	cout << "Thread::commands are executed" << endl;
	commandExecuted.unlock();
	condi = 0;
}

int GuiThread::getFileDescriptor()
{
	return filedes[0];
}

void GuiThread::addCommand (void (*comm)(void *), void *arg)
{
	BEGIN("addCommand");
	assert(!GuiThread::isGuiThread());

	commandQueue.lock();
	while (commandCount == MAXFUNC) {
		wakeup();
		commandQueue.wait();
	}
	int ind = (firstCommand+commandCount) % MAXFUNC;
	commands[ind].comm = comm;
	commands[ind].arg = arg;
	commandCount++;
	commandQueue.unlock();
}


void GuiThread::init()
{
	int result = pipe(filedes);
	if (result != 0) {
		cerr << "Could not create pipe: " << strerror(errno) << endl;
		exit(EXIT_FAILURE);
	}
	initialized = true;
}

void GuiThread::deinit()
{
	if (initialized) {
		close(filedes[0]);
		close(filedes[1]);
	}
}

void GuiThread::readAll()
{
	BEGIN("GuiThread::readAll");
	assert(GuiThread::isGuiThread());


	tryingToWakeupMutex.lock();
	TRACE(tryingToWakeup);
	if (tryingToWakeup) {
		char c;
		read(filedes[0], &c, 1);
	}
	tryingToWakeup=0;
	tryingToWakeupMutex.unlock();
	
}

void GuiThread::wakeup()
{
	BEGIN("GuiThread::wakeup");
	assert(!GuiThread::isGuiThread());

	tryingToWakeupMutex.lock();
	if (!tryingToWakeup) {
		write(filedes[1], "a", 1);
		tryingToWakeup=1;
	}
	tryingToWakeupMutex.unlock();
}

bool GuiThread::getNextCommand (Command &comm)
{
	BEGIN("getNextCommand");
	assert(GuiThread::isGuiThread());

	bool result;
	commandQueue.lock();
	if (commandCount == 0) {
		result = false;
	} else {
		result = true;
		comm = commands[firstCommand];
		firstCommand = (firstCommand+1) % MAXFUNC;
		commandCount--;
		if (commandCount == 0)
			commandQueue.broadcast();
	}
	commandQueue.unlock();
	return result;
}

bool GuiThread::isGuiThread ()
{
	return Thread::getThread() == 0;
}
#endif
