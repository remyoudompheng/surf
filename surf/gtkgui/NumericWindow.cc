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
#include <stdio.h>
#include <stdlib.h>

#include "NumericWindow.h"

#include "TableBuilder.h"
#include "RadioButtonBuilder.h"
#include "OptionMenuBuilder.h"

#define USE_OPTION_MENU

NumericWindow::NumericWindow()
{

#ifdef USE_OPTION_MENU
	OptionMenuBuilder omb;
	omb.addOption("bezier all roots algorithm",	"bezier_all_roots");
	omb.addOption("D-chain and bisection",		"d_chain_bisection");
	omb.addOption("D-chain and regula falsi",	"d_chain_regula_falsi");
	omb.addOption("D-chain and pegasus",		"d_chain_pegasus");
	omb.addOption("D-chain and anderson-bjoerck",	"d_chain_anderson_bjoerck");
	omb.addOption("D-chain and newton",		"d_chain_newton");
	GtkWidget *om = omb.getOptionMenu();
	controlWidget (om, "root_finder");
#else	
	RadioButtonBuilder rb;
	rb.addButton("bezier all roots algorithm",	"bezier_all_roots");
	rb.addButton("D-chain and bisection",		"d_chain_bisection");
	rb.addButton("D-chain and regula falsi",	"d_chain_regula_falsi");
	rb.addButton("D-chain and pegasus",		"d_chain_pegasus");
	rb.addButton("D-chain and anderson-bjoerck",	"d_chain_anderson_bjoerck");
	rb.addButton("D-chain and newton",		"d_chain_newton");
	controlWidget (rb.getWidget(), "root_finder");
#endif
	
	maxIterationsSpinButton = mygtk_new_spin(0, 10, 50000, 1, 10, 0.5, 0);
	controlWidget (maxIterationsSpinButton, "iterations");
		
	epsilonEntry = gtk_entry_new ();
	

	TableBuilder tb;
	tb.setSize(3);

#ifdef USE_OPTION_MENU
	tb.newLine ("rootfinder:", om);
#else
	tb.newLine ("rootfinder:", rb.getContainer(), 1/6.0);
#endif

	tb.newLine ("max. iterations:",  maxIterationsSpinButton);
	tb.newLine ("epsilon:", epsilonEntry);
	table = tb.getTable();
}

void NumericWindow::writeSymbols (SymbolTable &st)
{
	double epsilon = atof (gtk_entry_get_text (GTK_ENTRY(epsilonEntry)));
	st.insertDouble ("epsilon", epsilon);
}

void NumericWindow::readSymbols (SymbolTable &st)
{
	const char *epsilon = st.lookup ("epsilon");
	gtk_entry_set_text (GTK_ENTRY(epsilonEntry), epsilon);
}
