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



#include <assert.h>
#include <pthread.h>

#include <gtk/gtkitemfactory.h>

#include <strstream>

// #define DEBUG
#include "debug.h"

#include "MainWindowController.h"
#include "Script.h"

#include "RgbBuffer.h"
#include "TSDrawingArea.h"
#include "showAbout.h"
#include "ProgressDialog.h"
#include "Thread.h"
#include "Requester.h"
#include "GuiThread.h"
#include "Misc.h"
#include "RadioButtonBuilder.h"



static void scriptExecuted (ExecuteScriptStruct *ess)
{
	if (!GuiThread::isGuiThread()) {
		GuiThread::addCommand ((void (*)(void*)) scriptExecuted, ess);
		GuiThread::executeCommands();
		return;
	}
	MainWindowController *mw = (MainWindowController *) ess->userData;
	mw->allowScriptExecution (true);
	if (ess->errorOccured()) {
		mw->show();
		Document *doc = (Document *) ess->userData2;
		mw->showError (doc, ess->getErrorBegin(), ess->getErrorEnd(), ess->getErrorString());
	}
	delete ess;
}

void MainWindowController::toggleDisplayedState()
{
	displayedState = !displayedState;
	if (displayedState) {
		gtk_widget_show_all(window);
	} else {
		gtk_widget_hide(window);
		
	}
}

void MainWindowController::showError (Document *doc, int error_begin, int error_end, const char *errorString)
{
	assert(error_begin >= 0 && error_end >= 0 && error_begin <= error_end);

	editDocument (doc);

	GtkText *text = tw;
	
	int length = gtk_text_get_length ((GtkText *) tw);
	if (error_begin <= length && error_end <= length) {
		gtk_text_set_point(tw, error_begin);
		gtk_editable_set_position (GTK_EDITABLE(text), error_begin);
		gtk_editable_select_region (GTK_EDITABLE(text), error_begin, error_end);
	}
	Misc::alert (errorString);
}

void MainWindowController::allowScriptExecution(bool val)
{
  	gtk_widget_set_sensitive (executeScriptButton, val);
  	gtk_widget_set_sensitive (drawSurfaceButton,   val);
	gtk_widget_set_sensitive (ditherSurfaceButton, val);
	gtk_widget_set_sensitive (drawCurveButton,     val);
	gtk_widget_set_sensitive (ditherCurveButton,   val);
	gtk_widget_set_sensitive (colorSaveButton, val && colorSaveButtonState);
	gtk_widget_set_sensitive (ditheredSaveButton, val && ditheredSaveButtonState);
	gtk_widget_set_sensitive (saveColor_MenuItem, val && colorSaveButtonState);
	gtk_widget_set_sensitive (saveDithered_MenuItem, val && ditheredSaveButtonState);
	gtk_widget_set_sensitive (executeScript_MenuItem, val);
	gtk_widget_set_sensitive (drawSurface_MenuItem, val);
	gtk_widget_set_sensitive (ditherSurface_MenuItem, val);
	gtk_widget_set_sensitive (drawCurve_MenuItem, val);
	gtk_widget_set_sensitive (ditherCurve_MenuItem, val);
}

bool MainWindowController::mayClose()
{
	if (Script::isScriptRunning()) {
		Misc::alert ("A script is being executed. You'll have to stop it first.");
		return false;
	}
	if (actualDocument)
		actualDocument->setContents(tw.getContents ());

	int count = Document::countChangedDocuments();
	if (count == 0)
		return true;

	ostrstream str;
	if (count == 1)
		str << "One script has been changed.";
	else
		str << count << " scripts have been changed.";
	str << "Exiting will destroy all your changes !" << ends;
	int choice = showRequester (str.str(), "Don't exit", "Forget changes and exit");
	// str.freeze(0);

	return choice != 0;
}

gint MainWindowController::handle_delete_event (GtkWidget *widget, GdkEvent *event, gpointer data)
{
	DELETE_EVENT_CHECK_REQUESTER;
	MainWindowController *mwc = (MainWindowController *) data;
	return mwc->mayClose() ? FALSE : TRUE;
}

void MainWindowController::addCommandSeparator()
{
	gtk_box_pack_start(GTK_BOX(buttons), gtk_hseparator_new(), FALSE, FALSE, 5);
}

GtkWidget *MainWindowController::addCommandButton (const char *command, const char *tip)
{
	GtkWidget *button = gtk_button_new_with_label (command);
	if (tip)
		mygtk_set_tip(button, tip);
	gtk_box_pack_start (GTK_BOX(buttons), button, FALSE, FALSE, 0);
	
	return button;
}


void MainWindowController::selectedDocument (GtkWidget *widget, gpointer data)
{
	MainWindowController *mwc = (MainWindowController *) data;
	Document *doc = (Document *) gtk_object_get_data (GTK_OBJECT(widget), "document");
	mwc->editDocument(doc);
}


void MainWindowController::addedDocument (Document *doc)
{
	GtkWidget *item = gtk_menu_item_new_with_label (doc->getName());
	gtk_menu_append(GTK_MENU(loadedScripts), item);
	gtk_object_set_data (GTK_OBJECT(item), "document", doc);
	gtk_signal_connect (GTK_OBJECT(item), "activate", GTK_SIGNAL_FUNC(selectedDocument), this);
	gtk_widget_show(item);
}

void MainWindowController::showAbout()
{
	::showAbout();
}

void MainWindowController::quit()
{
	if (mayClose()) {
		gtk_main_quit();
	}
}

void MainWindowController::saveScript()
{
	if (!actualDocument) {
		Misc::alert ("Strange internal error");
	} else {
		actualDocument->setContents(tw.getContents ());
	}

	
	if (!actualDocument->isChanged()) {
		showAlert ("Document not changed. Not saving.");
		return;
	}

	if (actualDocument->needsName()) {
		saveScriptAs();
		return;
	}

	actualDocument->save();
}

void MainWindowController::saveScriptAs()
{
	gtk_window_set_title (GTK_WINDOW(filesel), "Select filename for saving...");
	gtk_widget_show(GTK_WIDGET(filesel));
	gdk_window_raise (GTK_WIDGET(filesel)->window);
	gtk_file_selection_set_filename (GTK_FILE_SELECTION(filesel), "");
	docToSave = actualDocument;
}

void MainWindowController::newDocument()
{
	editDocument(Document::newUnnamed());
}



gint MainWindowController::fileSelectionDelete (GtkWidget *widget, GdkEvent *event, gpointer data)
{
	MainWindowController *mwc = (MainWindowController *) data;
	mwc->fileSelectionCancelled();
	return TRUE;
}



void MainWindowController::internalExecuteScript (const char *before, const char *user, const char *after, bool run_commands)
{
	bool preview [4];
	preview[0] = preview[1] = preview[2] = preview[3] = true;
	
	int i;
	for (i=0; i<3; i++) {
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(previewButtons[i]))) {
			int j;
			for (j=0; j<i+1; j++) {
				preview[j] = false;
			}
			break;
		}
	}


	ostrstream ostr;
	if (before==0) {
		SymbolTable st;
		cw.writeSymbols (st);
		wrw.writeSymbols(st);
		ostr << st << ends;
		before=ostr.str();
	}

 	TSDrawingArea *dis = new TSDrawingArea();
	dis->setColorArea (colorWindow->getSurfDrawingArea());
	dis->setDitherArea (bitmapWindow->getSurfDrawingArea());

//  	bitmapWindow->show();
//  	colorWindow->show();

	ExecuteScriptStruct *ess = new ExecuteScriptStruct();
	ess->setPreview(preview);
	ess->executeBeforeUserScript (before);
	ess->setUserScript(user, run_commands);
	ess->executeAfterUserScript (after);

	ess->setDrawingArea(dis);
	ess->callWhenDone (scriptExecuted);
	ess->userData = this;
	ess->userData2 = actualDocument;

	if (!Script::startScriptExecution (ess, true)) {
		delete ess;
		return;
	}
	allowScriptExecution(false);

	ProgressDialog *pd = new ProgressDialog();
	pd->setThread(ess->getThread());
}

void MainWindowController::executeScript()
{	
	internalExecuteScript (0, tw.getContents(), "", true);
}

void MainWindowController::destroy() 
{
	gtk_main_quit();
}


void MainWindowController::drawSurface() 
{
	internalExecuteScript (0, tw.getContents(), "draw_surface;", false);
}

void MainWindowController::ditherSurface()
{
	internalExecuteScript (0, tw.getContents(), "dither_surface;", false);	
}

void MainWindowController::drawCurve()
{
	internalExecuteScript (0, tw.getContents(), "clear_screen; draw_curve;", false);	
}

void MainWindowController::ditherCurve()
{
	internalExecuteScript (0, tw.getContents(), "dither_curve;", false);	
}


void MainWindowController::navigate()
{
	navwin.show ();
}

void MainWindowController::loadScript()
{
	docToSave = 0;
	gtk_window_set_title (GTK_WINDOW(filesel), "Select file to load...");
	gtk_widget_show(GTK_WIDGET(filesel));
	gdk_window_raise (GTK_WIDGET(filesel)->window);
}

void MainWindowController::editDocument (Document *doc)
{
	assert(doc);
	if (doc == actualDocument)
		return;
	
	if (actualDocument)
		actualDocument->setContents(tw.getContents ());

	// FIXME
	
	actualDocument = doc;
	tw.setContents(doc->getContents());
	tw.setName (doc->getName());
	
}

void MainWindowController::fileSelected()
{
	gtk_widget_hide(GTK_WIDGET(filesel));
	const char *name = gtk_file_selection_get_filename (filesel);

	if (docToSave) {
		if (actualDocument)
			actualDocument->setContents(tw.getContents ());

		docToSave->saveAs (name);
		docToSave->setName (name);
		if (docToSave == actualDocument)
			tw.setName (actualDocument->getName());
		docToSave = 0;

	} else {
		Document *doc = Document::loadDocument (name);
		if (doc)
			editDocument(doc);
	}

}


void MainWindowController::fileSelectionCancelled()
{
	docToSave = 0;
	gtk_widget_hide(GTK_WIDGET(filesel));
}


void MainWindowController::saveDitheredImage()
{
	saveImageDialog.show(ImageFormats::dithered);
}

void MainWindowController::saveColorImage()
{
	saveImageDialog.show(ImageFormats::color);
}


// struct _GtkItemFactoryEntry
// {
//   gchar *path;
//   gchar *accelerator;

//   GtkItemFactoryCallback callback;
//   guint			 callback_action;

//   /* possible values:
//    * NULL		-> "<Item>"
//    * ""			-> "<Item>"
//    * "<Title>"		-> create a title item
//    * "<Item>"		-> create a simple item
//    * "<CheckItem>"	-> create a check item
//    * "<ToggleItem>"	-> create a toggle item
//    * "<RadioItem>"	-> create a radio item
//    * <path>		-> path of a radio item to link against
//    * "<Separator>"	-> create a separator
//    * "<Branch>"		-> create an item to hold sub items
//    * "<LastBranch>"	-> create a right justified item to hold sub items
//    */
//   gchar		 *item_type;
// };

MainWindowController::~MainWindowController()
{
	delete colorWindow;
	delete bitmapWindow;
}

MainWindowController::MainWindowController() 
	: colorWindow (new ColorWindow (this, true)), bitmapWindow(new ColorWindow(this, false)),
	  navwin(this),
	  actualDocument(0), docToSave(0),
	  colorSaveButtonState(false), ditheredSaveButtonState(false)
{
	static GtkItemFactoryEntry entries[] = {
		{(gchar*)"/_File", 0,                              0, 0, (gchar *)"<Branch>"},
		{(gchar*)"/File/tearoff", 0,                       0, 0, (gchar*)"<Tearoff>"},
		{(gchar*)"/File/_New", (gchar*)"<control>N",       MENUCALL(newDocument), 0,0},
		{(gchar*)"/File/_Open...", (gchar*)"<control>O",   MENUCALL(loadScript), 0, 0},
		{(gchar*)"/File/Loaded Scripts", 0,                0, 0, (gchar *)"<Branch>"},
		{(gchar*)"/File/Loaded Scripts/tearoff", 0,        0, 0, (gchar *)"<Tearoff>"},
		{(gchar*)"/File/Separator1", 0,                    0, 0, (gchar *)"<Separator>"},
		{(gchar*)"/File/_Save", (gchar*)"<control>S",      MENUCALL(saveScript), 0, 0},
		{(gchar*)"/File/Save _As...", (gchar*)"<control>A", MENUCALL(saveScriptAs), 0, 0},
		{(gchar*)"/File/Separator2", 0,                    0, 0, (gchar *)"<Separator>"},
		{(gchar*)"/File/_Quit", (gchar*)"<control>Q",      MENUCALL(quit), 0, 0},
		{(gchar*)"/_Command", 0,                           0, 0, (gchar*)"<Branch>"},
		{(gchar*)"/Command/tearoff", 0,                    0, 0, (gchar*)"<Tearoff>"},
		{(gchar*)"/Command/_Execute Script", (gchar*)"<control>E", MENUCALL(executeScript), 0, 0},
		{(gchar*)"/Command/Separator1", 0,                 0, 0, (gchar*)"<Separator>"},
		{(gchar*)"/Command/Draw Surface", 0,              MENUCALL(drawSurface), 0, 0},
		{(gchar*)"/Command/Dither Surface", 0,            MENUCALL(ditherSurface), 0, 0},
		{(gchar*)"/Command/Separator2", 0,                 0, 0, (gchar*)"<Separator>"},
		{(gchar*)"/Command/Draw Curve", 0,                MENUCALL(drawCurve), 0, 0},
		{(gchar*)"/Command/Dither Curve", 0,              MENUCALL(ditherCurve), 0, 0},
		{(gchar*)"/Command/Separator3", 0,                 0, 0, (gchar*)"<Separator>"},
		{(gchar*)"/Command/Configuration...", 0,           MENUCALL(configuration), 0, 0},
		{(gchar*)"/_Image", 0,                             0, 0, (gchar *)"<Branch>"},
		{(gchar*)"/Image/taroff", 0,                       0, 0, (gchar *)"<Tearoff>"},
		{(gchar*)"/Image/New Color Window", 0,             MENUCALL(newColorWindow), 0, 0},
		{(gchar*)"/Image/Save Color Image...", 0,          MENUCALL(saveColorImage), 0, 0},
		{(gchar*)"/Image/Separator1", 0,                   0, 0, (gchar*)"<Separator>"},
		{(gchar*)"/Image/New Dither Window", 0,            MENUCALL(newDitherWindow), 0, 0},
		{(gchar*)"/Image/Save Dithered Image...", 0,       MENUCALL(saveDitheredImage), 0, 0},
		{(gchar*)"/_Help", 0,                              0, 0, (gchar*)"<LastBranch>"},
		{(gchar*)"/Help/About...", 0,                      MENUCALL(showAbout), 0, 0},
	};
	
	GtkAccelGroup *accel_group;
	accel_group = gtk_accel_group_new ();
	
	GtkItemFactory *fac = gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>",  accel_group);
	gtk_item_factory_create_items(fac, sizeof(entries)/sizeof(entries[0]), entries, this);
	

	GtkWidget *wid = gtk_item_factory_get_widget(fac,"<main>");

	loadedScripts = gtk_item_factory_get_widget(fac,"/File/Loaded Scripts");
	assert(loadedScripts);
	saveColor_MenuItem = gtk_item_factory_get_widget(fac, "/Image/Save Color Image...");
	assert(saveColor_MenuItem);
	gtk_widget_set_sensitive(saveColor_MenuItem, false);
	saveDithered_MenuItem = gtk_item_factory_get_widget(fac, "/Image/Save Dithered Image...");
	assert(saveDithered_MenuItem);
	gtk_widget_set_sensitive(saveDithered_MenuItem, false);
	executeScript_MenuItem = gtk_item_factory_get_widget(fac, "/Command/Execute Script");
	assert(executeScript_MenuItem);
	drawSurface_MenuItem = gtk_item_factory_get_widget(fac, "/Command/Draw Surface");
	assert(drawSurface_MenuItem);
	ditherSurface_MenuItem = gtk_item_factory_get_widget(fac, "/Command/Dither Surface");
	assert(ditherSurface_MenuItem);
	drawCurve_MenuItem = gtk_item_factory_get_widget(fac, "/Command/Draw Curve");
	assert(drawCurve_MenuItem);
	ditherCurve_MenuItem = gtk_item_factory_get_widget(fac, "/Command/Dither Curve");
	assert(ditherCurve_MenuItem);

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);	
	gtk_signal_connect (GTK_OBJECT(window), "delete_event", GTK_SIGNAL_FUNC(handle_delete_event), this);

	gtk_accel_group_attach(accel_group, GTK_OBJECT(window));
	filesel = GTK_FILE_SELECTION(gtk_file_selection_new(""));
	VOIDCONNECT(filesel->ok_button, "clicked", fileSelected);
	VOIDCONNECT(filesel->cancel_button, "clicked", fileSelectionCancelled);
	gtk_signal_connect (GTK_OBJECT(filesel), "delete_event", GTK_SIGNAL_FUNC(fileSelectionDelete), this);


	buttons = gtk_vbox_new (FALSE, 5);
	gtk_container_border_width(GTK_CONTAINER(buttons), 5);


	GtkWidget *tmpbutton;

	tmpbutton = addCommandButton ("configuration...",
				      "Opens the configuration window, where you can adjust a lot of parameters.");
	VOIDCONNECT(tmpbutton, "clicked", configuration);
	

	
	addCommandSeparator();

	tmpbutton = addCommandButton ("execute script",
				      "Parses script and runs all commands.");
	VOIDCONNECT(tmpbutton, "clicked", executeScript);
	executeScriptButton = tmpbutton;
	
	tmpbutton = addCommandButton ("draw surface", 
				      "Parses script without running any commands,then calls draw_surface. You must set the surface polynom.");
	VOIDCONNECT(tmpbutton, "clicked", drawSurface);
	drawSurfaceButton = tmpbutton;

	tmpbutton = addCommandButton ("dither surface", 
				      "Parses script without running any commands,then calls dither_surface. You have to draw a surface first.");
	VOIDCONNECT (tmpbutton, "clicked", ditherSurface);
	ditherSurfaceButton = tmpbutton;

	tmpbutton = addCommandButton ("draw curve",
				      "Parses script without running any commands,then calls draw_curve. You must set the curve polynom.");
	VOIDCONNECT (tmpbutton, "clicked", drawCurve);
	drawCurveButton = tmpbutton;

	tmpbutton = addCommandButton ("dither curve",
				      "Parses script without running any commands, then calls dither_curve.");
	VOIDCONNECT (tmpbutton, "clicked", ditherCurve);
	ditherCurveButton = tmpbutton;

	addCommandSeparator();

	tmpbutton = addCommandButton("navigate",
				     "adjust rotation, scaling and position");
	VOIDCONNECT(tmpbutton, "clicked", navigate);

	addCommandSeparator();

	tmpbutton = addCommandButton ("save color image",
				      "saves the image in the current color window");
  	VOIDCONNECT(tmpbutton, "clicked", saveColorImage);
	gtk_widget_set_sensitive(tmpbutton, false);
	colorSaveButton = tmpbutton;

	tmpbutton = addCommandButton ("save dithered image",
				      "saves the image in the current dither window");
  	VOIDCONNECT(tmpbutton, "clicked", saveDitheredImage);
	gtk_widget_set_sensitive(tmpbutton, false);
	ditheredSaveButton = tmpbutton;

	addCommandSeparator();

	tmpbutton = addCommandButton ("new color window",
				      "Opens a new color window. The new window will be used the next time you draw something.");
	VOIDCONNECT(tmpbutton, "clicked", newColorWindow);

	tmpbutton = addCommandButton ("new dither window",
				      "Opens a new dither window. The new window will be used the next time you dither something.");
	VOIDCONNECT(tmpbutton, "clicked", newDitherWindow);

	addCommandSeparator();

	tmpbutton = addCommandButton ("load script...",
				      "Open a file selection dialog and load a script. This will not close your current script.");
	VOIDCONNECT(tmpbutton, "clicked", loadScript);

	tmpbutton = addCommandButton ("save script",
				      "Saves the current script. To save under a different name, choose the \"Script/Save as\" menu item.");
	VOIDCONNECT(tmpbutton, "clicked", saveScript);



	frame = gtk_frame_new ("Command");

	gtk_container_add (GTK_CONTAINER(frame), buttons);

	mainBox = gtk_hbox_new (FALSE,5);
	gtk_box_pack_start (GTK_BOX(mainBox), tw.getContainer(), TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX(mainBox), frame, FALSE, FALSE, 0);

	

	gtk_window_set_title (GTK_WINDOW(window), PACKAGE " " VERSION);
	VOIDCONNECT(window, "destroy", destroy);


	

	
	GtkWidget *preview = gtk_hbox_new (TRUE, 5);

//  	RadioButtonBuilder rbb (false);
//  	rbb.setBox (preview);
//  	rbb.addButton ("1x1",0);
//  	rbb.addButton ("3x3",0);
//  	rbb.addButton ("9x9",0);
//  	rbb.addButton ("27x27",0);



	previewButtons[0] = gtk_toggle_button_new_with_label ("3x3");
	previewButtons[1] = gtk_toggle_button_new_with_label ("9x9");
	previewButtons[2] = gtk_toggle_button_new_with_label ("27x27");

	int i;
	for (i=0; i<3; i++) {
		gtk_box_pack_start (GTK_BOX(preview), previewButtons[i], FALSE, TRUE, 0);
		gtk_signal_connect (GTK_OBJECT(previewButtons[i]), "toggled", (GtkSignalFunc) & previewToggled, this);
	}


	GtkWidget *width = mygtk_new_spin (200, 16, 6000, 1, 5, 0.2, 0);
	GtkWidget *height = mygtk_new_spin (200, 16, 6000, 1, 5, 0.2, 0);

	wrw.addWidget (width, "width");
	wrw.addWidget (height, "height");

	GtkWidget *sizeContainer = gtk_hbox_new (FALSE, 10);
// 	gtk_container_border_width(GTK_CONTAINER(sizeContainer), 5);

	gtk_box_pack_start (GTK_BOX(sizeContainer), gtk_label_new("width:") , FALSE, FALSE ,0);
	gtk_box_pack_start (GTK_BOX(sizeContainer), width, FALSE, FALSE ,0);
	gtk_box_pack_start (GTK_BOX(sizeContainer), gtk_label_new("height:") , FALSE, FALSE ,0);
	gtk_box_pack_start (GTK_BOX(sizeContainer), height, FALSE, FALSE ,0);


	GtkWidget *previewContainer = gtk_hbox_new (FALSE, 10);
	gtk_box_pack_start (GTK_BOX(previewContainer), gtk_label_new ("Preview:"), FALSE, FALSE ,0);
	gtk_box_pack_start (GTK_BOX(previewContainer), preview, FALSE, FALSE ,0);
	gtk_container_border_width(GTK_CONTAINER(previewContainer), 5);
	mygtk_set_tip (previewContainer, "toller tip");

	GtkWidget *dummy = gtk_vbox_new(FALSE,5);
	gtk_box_pack_start(GTK_BOX(dummy), wid, FALSE, FALSE,0);
	gtk_box_pack_start(GTK_BOX(dummy), mainBox, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(dummy), previewContainer, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(previewContainer), sizeContainer, FALSE, FALSE, 0);


	


	gtk_container_border_width(GTK_CONTAINER(mainBox), 5);

	gtk_container_add (GTK_CONTAINER(window), dummy);

//   	bitmapWindow->show();
//   	colorWindow->show();


	editDocument(Document::newUnnamed());

	cw.readSymbols (Script::getDefaultValues());

	navwin.readSymbols(Script::getDefaultValues());
	
	displayedState = false;

//	if (showme)
//		gtk_widget_show_all(window);
}

void MainWindowController::show()
{
	gtk_widget_show_all(window);
	displayedState = true;
}

void MainWindowController::previewToggled(GtkWidget *widget, gpointer data)
{
	if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
		return;

	MainWindowController *mwc = (MainWindowController *) data;
	int i;
	for (i=0; i<3; i++) {
		if (widget != mwc->previewButtons[i]) {
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(mwc->previewButtons[i]), FALSE);
		}
	}
	
}

void MainWindowController::enableSaveButton(SaveButtonType which)
{
	switch (which) {
	case Color:
		colorSaveButtonState = true;
		break;
	case Dithered:
		ditheredSaveButtonState = true;
		break;
	}
}

void MainWindowController::drawSurfaceWithParams()
{
	std::ostrstream ostr;
	SymbolTable st;
	navwin.writeSymbols(st);
	ostr << st << "draw_surface;" << std::ends;
	internalExecuteScript(0, tw.getContents(), ostr.str(), false);
}

void MainWindowController::evaluateScript()
{
	internalExecuteScript(0, tw.getContents(), "", false);
}
