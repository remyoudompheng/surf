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
 * Johannes Gutenberg-Universitaet Mainz, Fachbereich 17 Mathematik,
 * hereby disclaims all copyright interest in the program `surf'
 * (which makes passes at compilers) written by Stephan Endrass,
 * Hans Huelf, Ruediger Oertel, Kai Schneider, Ralf Schmitt and
 * Johannes Beigel
 *
 */

#include <assert.h>
#include <iostream.h>
#include <stdio.h>
#include <string.h>

#include "Document.h"
#include "Script.h"

#include "mystrdup.h"
#include "Misc.h"

static GList *listeners = 0;
static GList *documents = 0;


int Document::unnamedCount=0;

void Document::freeAllDocuments()
{
	while (documents) {
		Document *doc = (Document *) documents->data;
		delete doc;
	}
}

void DocumentListener::invokeMethodOnAllListeners (void (DocumentListener::*method)(Document *), Document *doc)
{
	GList *l = listeners;
	while (l) {
		DocumentListener *dl = (DocumentListener *) l->data;
		(dl->*method)(doc);
		l = l->next;
	}
}


DocumentListener::~DocumentListener()
{
	listeners = g_list_remove(listeners, this);
}

DocumentListener::DocumentListener()
{
	listeners = g_list_prepend (listeners, this);
}



void DocumentListener::addedDocument (Document *doc)
{
}

void DocumentListener::removedDocument (Document *doc)
{
}
void DocumentListener::changedName(Document *doc)
{
}


bool Document::needsName() const
{
	return name==0;
}

int Document::countChangedDocuments()
{
	int count=0;
	GList *l = documents;
	while (l) {
		Document *doc = (Document *) l->data;
		if (doc->isChanged())
			count++;
		l = l->next;
	}
	return count;
}

Document::Document()
{
	documents = g_list_prepend (documents,this);
	text = 0;
	name = 0;
	changed = false;
	unnamedNumber=0;
}

Document::~Document()
{
	documents = g_list_remove (documents, this);
	delete text;
	delete name;
}

const char *Document::getName () const
{
	if (name)
		return name;
	
	static char str[32];
	sprintf (str, "*unnamed %d*", unnamedNumber);
	return str;
}

void Document::setName (const char *newName)
{
	if (name && strcmp(name,newName)==0)
		return;
	delete name;
	name = mystrdup(newName);
	DocumentListener::invokeMethodOnAllListeners (&DocumentListener::changedName, this);
}

void Document::setContents (const char *contents)
{
	if (strcmp(contents,text)==0) {
		return;
	}
	
	delete [] text;
	text = new char [strlen(contents)+1];
	strcpy (text, contents);
	changed = true;
}

Document *Document::loadDocument (const char *name)
{
	assert(name);
	char *contents = Script::readFile (name);
	
	if (!contents)
		return 0;
	
	Document *doc = new Document ();
	doc->text = contents;
	doc->name = new char[strlen(name)+1];
	strcpy (doc->name, name);
	DocumentListener::invokeMethodOnAllListeners (&DocumentListener::addedDocument, doc);
	return doc;
}


Document *Document::newUnnamed ()
{
	Document *doc = new Document();
	doc->unnamedNumber = ++unnamedCount;
	doc->text = new char[1];
	doc->text[0]=0;
	DocumentListener::invokeMethodOnAllListeners (&DocumentListener::addedDocument, doc);
	return doc;
}

void Document::save()
{
	assert(!needsName());
	saveAs (getName());
}

void Document::saveAs(const char *filename) 
{
	FILE *f = fopen (filename, "w");
	if (f == 0) {
		Misc::alert ("Could not open file for writing.");
		return;
	}
	int length = strlen(text);
	if (fprintf (f, "%s", text) != length) {
		Misc::alert ("Writing failed.");
	}
	fclose(f);
	changed = false;
}
