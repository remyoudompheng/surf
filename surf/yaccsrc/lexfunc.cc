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

#include <mymemory.h>
#include <monomarith.h>
#include <polyarith.h>
#include <polylexyacc.h>
#include <polyyacc.h>

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef YYTEXT_POINTER
extern char* yytext;
#else
extern char yytext[];
#endif

namespace {
symtab* s_first = 0;
symtab* s_last  = 0;

int main_formula_data_index	= 0;
int main_formula_data_length	= 0;
char* main_formula_data_ptr	= 0;
}


// Global error data:

char yyerrorstring[1024];
long error_begin_char;
long char_number; // current position in input_string
long line_number; // current lineno in input_string

long goto_label;
long goto_line;
int goto_flag;

// set yacc/lex input to buffer:

void set_the_yyinput(char* buffer, long int ch_no, long int ln_no)
{
	main_formula_data_ptr = buffer;
	main_formula_data_length = strlen(buffer);
	main_formula_data_index = 0;

	char_number = ch_no;
	line_number = ln_no;
}

extern "C" int the_yyinput(char* buffer, int max_size)
{
	// input from the string: main_formula_data_ptr

	int d = main_formula_data_length - main_formula_data_index;
	int n = (max_size > d ? d : max_size);

	if(n > 0) {
                memcpy(buffer, main_formula_data_ptr, n);
                main_formula_data_ptr += n;
                main_formula_data_index += n;
	}

	return  n;
}


// Add name to symbol table at begin:

symtab* symtab_add_name(const char* name)
{
	symtab* s = new symtab();

	s->name    = strdup(name);
	s->type    = SYM_UNSPEC;
	s->surface = false;
	s->constant= false;
	s->ptr     = 0;
	s->label   = char_number;
	s->lineno  = line_number;

	s->prev = 0; // at the beginning
	s->next = s_first;

	if(s_first != 0) {
		s_first->prev = s;
	}
	if(s_last == 0) {
		s_last = s;
	}
	s_first = s;
	
	return  s;
}


// Add name to symbol table at begin which is a surface variable or command:

symtab* symtab_add_surface_name(const char* name, symtyp type,
				int constant, void *ptr)
{
	symtab* s = symtab_add_name(name);

	s->type = type;
	s->surface = true;
	s->constant = constant;
	s->ptr = ptr;

	return  s;
}


// Find name of symbol and return symtab pointer:

symtab* symtab_find_name(const char* name)
{
	symtab* s = s_first;

	while(s != 0 && strcmp(name, s->name) != 0) {
		s = s->next;
	}

	return  s;
}

// Delete symbol in symtab by symtab:

void symtab_delete_element(symtab* s)
{
	if(s != 0) {
		if(s->prev == 0 && s->next == 0) {
			s_first = s_last = 0;
		} else if(s->prev == 0) {
			s_first = s->next;
			s->next->prev = 0;
		} else if(s->next == 0) {
			s_last = s->prev;
			s->prev->next = 0;
		} else {
			s->prev->next = s->next;
			s->next->prev = s->prev;
		}

		symtab_clean(s);
		delete s;
	}
}

// Delete symbol in symtab by name:

void symtab_delete_name(const char* name)
{
	symtab* s = symtab_find_name(name);
	symtab_delete_element(s);
}

// Delete user defined symbols in symtab:

void symtab_delete_user_names()
{
	symtab* s = s_first;
	symtab* s_next = 0;

	while(s != 0) {
		if(!s->surface) {
			s_next = s->next;
			symtab_delete_element(s);
			s = s_next;
		} else {
			s = s->next;
		}
	}
}

// Delete system symbols in symtab:

void symtab_delete_surface_names()
{
	symtab* s = s_first;
	symtab* s_next = 0;

	while(s != 0) {
		if(s->surface) {
			s_next = s->next;
			symtab_delete_element(s);
			s = s_next;
		} else {
			s = s->next;
		}
	}
}

// Delete all symbols in symtab:

void symtab_delete_total()
{
	symtab* s = s_first;
		
	while(s != 0) {
		symtab* s_next = s->next;
		symtab_delete_element(s);
		s = s_next;
	}
}

// Lookup a name in the symbol table.
// Add name if not already there

symtab* symtab_lookup_name(const char* name)
{
	symtab* s = symtab_find_name(name);
	if(s == 0) {
		s = symtab_add_name(name);
	}
	return s;
}

// Free memory associated with a symtab:

void symtab_clean(symtab* s)
{
	switch(s->type) {
        case SYM_STRING:
		if(s->str != 0 && !s->surface) {
			delete_char(s->str);
			s->str = 0;
		}
		break;

        case SYM_POLYXYZ:
		if(s->p.n > 0 && !s->surface) {
			delete_coeff_polyxyz(&s->p);
		}
		break;

        case SYM_MONOM:
        case SYM_DOUBLE:
        case SYM_INTEGER:
        case SYM_COMMAND:
        case SYM_LABEL:
        case SYM_UNSPEC:
		break;
	}
}


// Default error routine:

void yyerror(const char* s)
{
	strncpy(yyerrorstring, s, sizeof(yyerrorstring));
	error_begin_char = char_number - strlen(yytext);
}


// scan labels before interpreting:

enum PreParserState {
	STATE_START = 0,
	STATE_STRING,
	STATE_COMMENTA,
	STATE_COMMENTB,
	STATE_LABEL,
	STATE_ACCEPT
};

const int MAXNAMELENGTH = 1024;

void scan_labels(char* buffer)
{
	int l = 0; // line number
	int length = strlen(buffer);
	PreParserState state = STATE_START;
	int name_length = 0;
	char name[MAXNAMELENGTH + 1];
	symtab* s;

	for(int c = 0; c < length; c++) {
		if(buffer[c] == '\n') {
			l++;
		}
		
		switch(state) {
		case STATE_START:
			
			if(buffer[c]=='\"') {
				state = STATE_STRING;
			} else if(isalpha(buffer[c]) || buffer[c] == '_') {
				state = STATE_LABEL;
			} else if(buffer[c] == '/') {
				state = STATE_COMMENTA;
			}
			break;

		case STATE_STRING:

			if(buffer[c] == '\"') {
				state = STATE_START;
			}
			break;
			
		case STATE_COMMENTA:

			if(buffer[c] == '/') {
				state = STATE_COMMENTB;
			} else if(buffer[c] == '\"') {
				state = STATE_STRING;
			} else if(isalpha(buffer[c]) || buffer[c] == '_') {
				state = STATE_LABEL;
			} else {
				state = STATE_START;
			}
			break;
			
		case STATE_COMMENTB:
			
			if(buffer[c] == '\n') {
				state = STATE_START;
			}
			break;

		case STATE_LABEL:

			if(buffer[c] == '\"') {
				state = STATE_STRING;
				name_length = 0;
			} else if(buffer[c]==':') {
				state = STATE_ACCEPT;
			} else if(isalnum(buffer[c]) || buffer[c] == '_') {
				state = STATE_LABEL;
				name_length++;
			} else if(buffer[c] == '/') {
				state = STATE_COMMENTA;
			} else {
				state = STATE_START;
				name_length = 0;
			}

			break;

		case STATE_ACCEPT:
			
			if(buffer[c] == '\"') {
				state = STATE_STRING;
			} else if(isalpha(buffer[c]) || buffer[c] == '_') {
				state = STATE_LABEL;
			} else if( buffer[c]=='/' ) {
				state = STATE_COMMENTA;
			} else {
				state = STATE_START;
			}

			strncpy(name, &(buffer[c - name_length - 2]), name_length + 1);
			name[name_length + 1] = '\0';

			s = symtab_add_name(name);

			s->type = SYM_LABEL;
			s->label = c + 1;
			s->lineno = l;

			name_length = 0;
			
			break;

		default:
			break;
		}
	}
}

