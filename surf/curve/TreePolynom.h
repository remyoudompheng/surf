/*
 *   surf - visualizing algebraic curves and algebraic surfaces
 *   Copyright (C) 1996-1997 Friedrich-Alexander-Universitaet
 *                           Erlangen-Nuernberg
 *                 1997-2001 Johannes Gutenberg-Universitaet Mainz
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



#ifndef TREEPOLYNOM_H
#define TREEPOLYNOM_H

#include <debug.h>
#include <RBTree.h>
#include <RefCounter.h>

#include<iostream>

template<class Monom> 
class TreePolynomNode : public RBNode
{
public:
	Monom monom;
	
	static void free (RBNode *node) {delete ((TreePolynomNode*)node);};
	static void copy (RBNode *dst, const RBNode *node) 
		{
			((TreePolynomNode*)dst)->monom=((TreePolynomNode*)node)->monom;
		};
	static RBNode *newNode () {return new TreePolynomNode();};
};


template<class Monom> 
class TreePolynom : public RefCounter
{

public:
	TreePolynom() {root=NIL;};
	~TreePolynom(){freeNodes ((TreePolynomNode<Monom> *)root);};
	
	void addMonom(const Monom &mon);
	void subMonom(const Monom &mon);
	void mulMonom(const Monom &mon);

	void addPoly(const TreePolynom &tp);
	void subPoly(const TreePolynom &tp);
	
	const Monom &lmonom() const;
	TreePolynom *clone() const;

	int degreeInVariable (int var) 
		{
			RBNode *it;
			int degree=-1;
			iteratorInit(it, root);
			while (iteratorIsValid(it)) {
				TreePolynomNode<Monom> *node = (TreePolynomNode<Monom>*) it;
				int tmp;
				if ((tmp = node->monom.getExponent(var)) > degree)
					degree=tmp;
				iteratorNext(it);
			}
			return degree;
		}
	void swapVariables(int var1, int var2) 
		{
			RBNode *it;
			iteratorInit(it, root);
			while(iteratorIsValid(it)) {
				TreePolynomNode<Monom> *node = (TreePolynomNode<Monom>*) it;
				swap(node->monom.getExponent(var1), node->monom.getExponent(var2));
				iteratorNext(it);
			}
		};

	int _isNull() const {return root == NIL;};

	void print (class std::ostream &os) const;

	static TreePolynom* multiply (TreePolynom *tp1, TreePolynom *tp2);


	void withMonomsPerform ( void (*action) (Monom *, void *), void *data)
		{
			RBNode *it;
			iteratorInit(it, root);
			while(iteratorIsValid(it)) {
				TreePolynomNode<Monom> *node = (TreePolynomNode<Monom>*) it;
				action(&node->monom, data);
				iteratorNext(it);
			}

		};

	void withAllMonomsPerform ( void (*action) (Monom *) )
		{
			RBNode *it;
			iteratorInit(it, root);
			while(iteratorIsValid(it)) {
				TreePolynomNode<Monom> *node = (TreePolynomNode<Monom>*) it;
				action(&node->monom);
				iteratorNext(it);
			}
		};
protected:
	RBNode *root;


	void freeNodes(TreePolynomNode<Monom> *node)
		{
			if (node == NIL)
				return;
			freeNodes ((TreePolynomNode<Monom> *)node->left);
			freeNodes ((TreePolynomNode<Monom> *)node->right);
			delete node;
		}
};

template<class Monom> 
void TreePolynom<Monom>::addMonom (const Monom &mon)
{
	int cmp=0; // FIXME...just to suppress compiler warnings
	TreePolynomNode<Monom> *current=(TreePolynomNode<Monom> *) root;
	TreePolynomNode<Monom> *parent=0;

	while (current != NIL) {
		cmp = mon.lexcmp(current->monom);
		if (cmp == 0) {
			current->monom += mon;
			if (current->monom.isNull()) {
				deleteNode(current, root, TreePolynomNode<Monom>::copy, TreePolynomNode<Monom>::free); 
			}
			return;
		}
		
		parent=current;
		current = (TreePolynomNode<Monom>*)(cmp < 0 ? current->left : current->right);
	}
	
	TreePolynomNode<Monom> *x = new TreePolynomNode<Monom>();
	x->monom = mon;
	if (parent) {
		// cmp might not be used uninitialized, because if parent != 0 we made a
		// call to cmp = lexorder(mon, current->monom) (see above)
		if (cmp<0)
			parent->left = x;
		else
			parent->right = x;
	} else {
		root = x;
	}
	x->parent = parent;

// 	x->left = NIL;
// 	x->right = NIL;
// 	x->color = RED;
	
	insertFixup(x, root);	
}


template<class Monom> 
void TreePolynom<Monom>::subMonom (const Monom &mon)
{
	int cmp=0; // FIXME ...no compiler warnings
	TreePolynomNode<Monom> *current=(TreePolynomNode<Monom> *) root;
	TreePolynomNode<Monom> *parent=0;

	while (current != NIL) {
		cmp = mon.lexcmp(current->monom);
		if (cmp == 0) {
			current->monom -= mon;
			if (current->monom.isNull()) {
				deleteNode(current, root, TreePolynomNode<Monom>::copy, TreePolynomNode<Monom>::free); 
			}
			return;
		}
		
		parent=current;
		current = (TreePolynomNode<Monom>*)(cmp < 0 ? current->left : current->right);
	}
	
	TreePolynomNode<Monom> *x = new TreePolynomNode<Monom>();
	x->monom = mon;
	x->monom.negate();
	if (parent) {
		// cmp might not be used uninitialized, because if parent != 0 we made a
		// call to cmp = lexorder(mon, current->monom) (see above)
		if (cmp<0)
			parent->left = x;
		else
			parent->right = x;
	} else {
		root = x;
	}
	x->parent = parent;

// 	x->left = NIL;
// 	x->right = NIL;
// 	x->color = RED;
	
	insertFixup(x, root);	
}

template<class Monom> 
void TreePolynom<Monom>::mulMonom(const Monom &mon)
{
	RBNode *it;
	iteratorInit (it, root);
	while (iteratorIsValid(it)) {
		TreePolynomNode<Monom> *node = (TreePolynomNode<Monom> *)it;
		node->monom *= mon;
		iteratorNext(it);
	}
}

template<class Monom> 
void TreePolynom<Monom>::addPoly (const TreePolynom &tp)
{
	RBNode *it;
	iteratorInit (it, tp.root);
	while (iteratorIsValid(it)) {
		addMonom( ((TreePolynomNode<Monom>*)it)->monom);
		iteratorNext(it);
	}
}

template<class Monom> 
void TreePolynom<Monom>::subPoly (const TreePolynom &tp)
{
	RBNode *it;
	iteratorInit (it, tp.root);
	while (iteratorIsValid(it)) {
		subMonom( ((TreePolynomNode<Monom>*)it)->monom);
		iteratorNext(it);
	}
}



template<class Monom> 
const Monom & TreePolynom<Monom>::lmonom() const
{
	RBNode *it;
	iteratorInit(it, root);
	assert(iteratorIsValid(it));
	
	return ((TreePolynomNode<Monom> *) it)->monom;
}

template<class Monom>
TreePolynom<Monom> * TreePolynom<Monom>::clone() const
{
	TreePolynom *tp = new TreePolynom();
	tp->root = cloneTree(root, 0, TreePolynomNode<Monom>::newNode, TreePolynomNode<Monom>::copy);
	return tp;
}


template<class Monom>
void TreePolynom<Monom>::print (ostream &os) const
{
	RBNode *it;
	iteratorInit (it, root);
	if (!iteratorIsValid(it)) {
		os << "0";
		return;
	}
	while (iteratorIsValid(it)) {
		TreePolynomNode<Monom> *node = (TreePolynomNode<Monom> *)it;
		os << node->monom;
		iteratorNext(it);
		if (iteratorIsValid(it))
			os << "+";
	}
}

template<class Monom> 
ostream & operator << (ostream &os, const TreePolynom<Monom> &tp)
{
	tp.print(os);
	return os;
}

template<class Monom> 
TreePolynom<Monom> * TreePolynom<Monom>::multiply (TreePolynom<Monom> *tp1, TreePolynom<Monom> *tp2)
{
	assert(tp1);
	assert(tp2);


	TreePolynom<Monom> *result = new TreePolynom<Monom>();
	
	RBNode *it1, *it2;
	
	iteratorInit(it1, tp1->root);
	while (iteratorIsValid(it1)) {
		iteratorInit(it2, tp2->root);
		Monom &it1mon = ((TreePolynomNode<Monom> *)it1)->monom;
		while (iteratorIsValid(it2)) {
			Monom m (it1mon);
			m *= ((TreePolynomNode<Monom>*)it2)->monom;
			result->addMonom(m);
			iteratorNext(it2);
		}
		iteratorNext(it1);
	}
	return result;
}

template<class Monom> 
TreePolynom<Monom> * multiply (TreePolynom<Monom> *tp1, TreePolynom<Monom> *tp2)
{
	return TreePolynom<Monom>::multiply (tp1, tp2);
}


template<class Monom> 
class TreePoly
{
public:
	TreePoly(TreePolynom<Monom> *p) : poly(p) {};
	TreePoly() : poly(new TreePolynom<Monom>()) {};
	TreePoly(const TreePoly &tp) : poly(tp.poly) {retain(poly);};
	~TreePoly() {release(poly);};
	
	TreePoly & operator = (const TreePoly &tp) {assert(&tp!=this);release(poly); poly=tp.poly; retain(poly); return *this;};

	TreePoly clone() const {return TreePoly(poly->clone());};

	TreePoly operator += (const TreePoly p) {poly->addPoly(*p.poly); return *this;};
	TreePoly operator -= (const TreePoly p) {poly->subPoly(*p.poly); return *this;};
	TreePoly operator + (const TreePoly &p) const 
		{
			
			TreePolynom<Monom> *result = poly->clone();
			result->addPoly(*p.poly);

			return result;
		};

	TreePoly operator - (const TreePoly &p) const 
		{
			TreePolynom<Monom> *result = poly->clone();
			result->subPoly(*p.poly);
			return result;
		};

	TreePoly operator * (const TreePoly &p) const 
		{
			TreePolynom<Monom> *retval = multiply(poly, p.poly);
			return retval;
		}

	void addMonom(const Monom &mon) {poly->addMonom(mon);};
	int degreeInVariable(int var) {return poly->degreeInVariable(var);};
	void swapVariables(int var1, int var2) {poly->swapVariables(var1, var2);};
	const Monom &lmonom () const {return poly->lmonom();};
	
	

	// friend class ostream &operator<<(ostream &os, const TreePoly<Monom> &tp);
	int _isNull () const {return poly->_isNull();};

 	void withAllMonomsPerform ( void (*action) (Monom *) ) 
 		{poly->withAllMonomsPerform(action);};

	void withMonomsPerform ( void (*action) (Monom *, void *), void *data ) 
		{poly->withMonomsPerform(action,data);};

	void print (class std::ostream &os) const { os << *poly;}

protected:
	TreePolynom<Monom> *poly;
};


template<class Monom> 
int isNull (const TreePoly<Monom> &tp)
{
	return tp._isNull();
}

template<class Monom> 
void setNull (TreePoly<Monom> &tp)
{
	// todo
}

template<class Monom> 
ostream &operator<<(ostream &os, const TreePoly<Monom> &tp)
{
	tp.print(os);
	return os;
	//return os << *tp.poly;
}


template<class Monom> 
TreePoly<Monom> divide (TreePoly<Monom> f, TreePoly<Monom> g)
{
	TreePoly<Monom> result;
	while (!isNull(f)) {
		Monom m = f.lmonom();
		m /= g.lmonom();
		result.addMonom(m);
		
		TreePoly<Monom> tmp;
		tmp.addMonom(m);
		f = f-g*tmp;
	}
	return result;
}
#endif
