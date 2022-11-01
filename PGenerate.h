#ifndef IVL_PGenerate_H
#define IVL_PGenerate_H
/*
 * Copyright (c) 2006-2020 Stephen Williams (steve@icarus.com)
 *
 *    This source code is free software; you can redistribute it
 *    and/or modify it in source code form under the terms of the GNU
 *    General Public License as published by the Free Software
 *    Foundation; either version 2 of the License, or (at your option)
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

# include  "PNamedItem.h"
# include  "StringHeap.h"
# include  "HName.h"
# include  "PScope.h"
# include  <list>
# include  <map>
# include  <valarray>
# include  "pform_types.h"
# include  "AnalysisControler.h"
# include  "CfgNode.h"
# include  "slice.h"
# include  "PGate.h"
# include  "Statement.h"

class Design;
class NetScope;
class PExpr;
class PFunction;
class PProcess;
class PTask;
class PGate;
class PWire;

/*
 * This represents a generate scheme. The interpretation of the
 * members depends on the scheme_type.
 *
 * GS_LOOP
 *
 * GS_CASE
 *    loop_test is the expression to be compared.
 *    generates contains only GS_CASE_ITEM schemes.
 * GS_CASE_ITEM
 *    The parent points to the GS_CASE that contains this item.
 *    the loop_test is compared with the parent->loop_test expression.
 */
class PGenerate : public PNamedItem, public LexicalScope {

    public:
      explicit PGenerate(LexicalScope*parent, unsigned id_number);
      ~PGenerate();

	// Generate schemes have an ID number, for when the scope is
	// implicit.
      const unsigned id_number;
      perm_string scope_name;

	// This is used during parsing to stack lexical scopes within
	// this generate scheme.
//      LexicalScope*lexical_scope;

      enum scheme_t {GS_NONE, GS_LOOP, GS_CONDIT, GS_ELSE,
		     GS_CASE, GS_CASE_ITEM, GS_NBLOCK};
      scheme_t scheme_type;

	// generate loops have an index variable and three
	// expressions: for (index = <init>; <test>; index=<step>)
	// the index is local if it was declared in the init expression,
	// e.g. for (genvar index = <init>; <test>; index=<step>)
      bool local_index;
      perm_string loop_index;
      PExpr*loop_init;
      PExpr*loop_test;
      PExpr*loop_step;
	// Case items may have multiple guard expression values. It is
	// enough for any on of the guards to match the case statement
	// test value.
      std::valarray<PExpr*> item_test;

	// defparam assignments found in this scope.
      typedef pair<pform_name_t,PExpr*> named_expr_t;
      list<named_expr_t>defparms;

      list<PGate*> gates;
      void add_gate(PGate*);

	// Tasks instantiated within this scheme.
      map<perm_string,PTask*> tasks;
      map<perm_string,PFunction*>funcs;

	// Generate schemes can contain further generate schemes.
      list<PGenerate*> generate_schemes;
//      PGenerate*parent;

      virtual GenerateNode* build_node(PDesign& de);
      void dump_slice(std::ostream&out, unsigned ind, slicer* s) const;
      virtual svector<unsigned>& get_linenos()
      {
            svector<unsigned>* tmp = new svector<unsigned>();
            tmp = new svector<unsigned>(*tmp, get_lineno());
            for (list<PGate*>::const_iterator idx = gates.begin()
             ; idx != gates.end() ; ++ idx ) {
                tmp = new svector<unsigned>(*tmp, (*idx)->get_lineno());
            }
            for (list<PProcess*>::const_iterator idx = behaviors.begin()
             ; idx != behaviors.end() ; ++ idx ) {
                tmp = new svector<unsigned>(*tmp, (*idx)->get_linenos());
            }
            for (list<PCallTask*>::const_iterator idx = elab_tasks.begin()
                   ; idx != elab_tasks.end() ; ++ idx ) {
                tmp = new svector<unsigned>(*tmp, (*idx)->get_lineno());
            }
            for (list<PGenerate*>::const_iterator idx = generate_schemes.begin()
             ; idx != generate_schemes.end() ; ++ idx ) {
                tmp = new svector<unsigned>(*tmp, (*idx)->get_linenos());
            }
            return *tmp;
      }

	// This method is called by the elaboration of a module to
	// generate scopes. the container is the scope that is to
	// contain the generated scope.
      bool generate_scope(Design*des, NetScope*container);

	// Elaborate signals within any of the generated scopes that
	// were made by this generate block within the given container scope.
      bool elaborate_sig(Design*des, NetScope*container) const;
      bool elaborate(Design*des, NetScope*container) const;

      void dump(ostream&out, unsigned indent) const;

      SymbolType symbol_type() const;

    private:
      bool generate_scope_loop_(Design*des, NetScope*container);
      bool generate_scope_condit_(Design*des, NetScope*container, bool else_flag);
      bool generate_scope_case_(Design*des, NetScope*container);
      bool generate_scope_nblock_(Design*des, NetScope*container);

	// Call probe during elaborate_scope to calculate the
	// direct_nested_ flag. It is OK to store the direct_nested_
	// information here because "direct nested" is a property of
	// the lexical generate code.
      void probe_for_direct_nesting_(void);
      bool direct_nested_;

	// Elaborate_scope within a generated scope.
      void elaborate_subscope_(Design*des, NetScope*scope);
      void elaborate_subscope_direct_(Design*des, NetScope*scope);

	// These are the scopes created by generate_scope.
      list<NetScope*>scope_list_;
	// internal function called on each scope generated by this scheme.
      bool elaborate_sig_(Design*des, NetScope*scope) const;
      bool elaborate_sig_direct_(Design*des, NetScope*scope) const;
      bool elaborate_(Design*des, NetScope*scope) const;
      bool elaborate_direct_(Design*des, NetScope*scope) const;

    private: // not implemented
      PGenerate(const PGenerate&);
      PGenerate& operator= (const PGenerate&);
};

extern std::ostream& operator << (std::ostream&, PGenerate::scheme_t);

#endif /* IVL_PGenerate_H */
