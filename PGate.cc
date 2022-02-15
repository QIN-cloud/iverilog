/*
<<<<<<< Updated upstream
 * Copyright (c) 1999-2021 Stephen Williams (steve@icarus.com)
=======
 * Copyright (c) 1999-2019 Stephen Williams (steve@icarus.com)
>>>>>>> Stashed changes
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

# include "config.h"

# include  "PGate.h"
# include  "PExpr.h"
# include  "verinum.h"
# include  <cassert>

<<<<<<< Updated upstream
using namespace std;

=======
>>>>>>> Stashed changes
void PGate::set_pins_(list<PExpr*>*pins)
{
      assert(pins);
      assert(pins->size() == pins_.size());

      for (size_t idx = 0 ; idx < pins_.size() ; idx += 1) {
	    pins_[idx] = pins->front();
	    pins->pop_front();
      }

      assert(pins->empty());
      delete pins;
}

PGate::PGate(perm_string name, list<PExpr*>*pins, const list<PExpr*>*del)
<<<<<<< Updated upstream
: name_(name), pins_(pins? pins->size() : 0), ranges_(0)
=======
: name_(name), pins_(pins? pins->size() : 0)
>>>>>>> Stashed changes
{
      if (pins) set_pins_(pins);
      if (del) delay_.set_delays(del);
      str0_ = IVL_DR_STRONG;
      str1_ = IVL_DR_STRONG;
}

PGate::PGate(perm_string name, list<PExpr*>*pins, PExpr*del)
<<<<<<< Updated upstream
: name_(name), pins_(pins? pins->size() : 0), ranges_(0)
=======
: name_(name), pins_(pins? pins->size() : 0)
>>>>>>> Stashed changes
{
      if (pins) set_pins_(pins);
      if (del) delay_.set_delay(del);
      str0_ = IVL_DR_STRONG;
      str1_ = IVL_DR_STRONG;
}

PGate::PGate(perm_string name, list<PExpr*>*pins)
<<<<<<< Updated upstream
: name_(name), pins_(pins? pins->size() : 0), ranges_(0)
=======
: name_(name), pins_(pins? pins->size() : 0)
>>>>>>> Stashed changes
{
      if (pins) set_pins_(pins);
      str0_ = IVL_DR_STRONG;
      str1_ = IVL_DR_STRONG;
}

PGate::~PGate()
{
}

<<<<<<< Updated upstream
void PGate::set_ranges(list<pform_range_t>*ranges)
{
      assert(ranges_ == 0);
      ranges_ = ranges;
}

=======
>>>>>>> Stashed changes
ivl_drive_t PGate::strength0() const
{
      return str0_;
}

void PGate::strength0(ivl_drive_t s)
{
      str0_ = s;
}

ivl_drive_t PGate::strength1() const
{
      return str1_;
}

void PGate::strength1(ivl_drive_t s)
{
      str1_ = s;
}

void PGate::elaborate_scope(Design*, NetScope*) const
{
}

/*
 * This method is used during elaboration to calculate the
 * rise/fall/decay times for the gate. These values were set in pform
 * by the constructor, so here I evaluate the expression in the given
 * design context and save the calculated delays into the output
 * parameters. This method understands how to handle the different
 * numbers of expressions.
 */

void PGate::eval_delays(Design*des, NetScope*scope,
			NetExpr*&rise_expr,
			NetExpr*&fall_expr,
			NetExpr*&decay_expr,
			bool as_net_flag) const
{
      delay_.eval_delays(des, scope,
			 rise_expr, fall_expr, decay_expr,
			 as_net_flag);
}

unsigned PGate::delay_count() const
{
      return delay_.delay_count();
}

PNamedItem::SymbolType PGate::symbol_type() const
{
      return INSTANCE;
}

PGAssign::PGAssign(list<PExpr*>*pins)
: PGate(perm_string(), pins)
{
      assert(pin_count() == 2);
}

PGAssign::PGAssign(list<PExpr*>*pins, list<PExpr*>*dels)
: PGate(perm_string(), pins, dels)
{
      assert(pin_count() == 2);
}

PGAssign::~PGAssign()
{
}

PGBuiltin::PGBuiltin(Type t, perm_string name,
		     list<PExpr*>*pins,
		     list<PExpr*>*del)
<<<<<<< Updated upstream
: PGate(name, pins, del), type_(t)
=======
: PGate(name, pins, del), type_(t), msb_(0), lsb_(0)
>>>>>>> Stashed changes
{
}

PGBuiltin::PGBuiltin(Type t, perm_string name,
		     list<PExpr*>*pins,
		     PExpr*del)
<<<<<<< Updated upstream
: PGate(name, pins, del), type_(t)
=======
: PGate(name, pins, del), type_(t), msb_(0), lsb_(0)
>>>>>>> Stashed changes
{
}


PGBuiltin::~PGBuiltin()
{
}

<<<<<<< Updated upstream
=======
void PGBuiltin::set_range(PExpr*msb, PExpr*lsb)
{
      assert(msb_ == 0);
      assert(lsb_ == 0);

      msb_ = msb;
      lsb_ = lsb;
}

>>>>>>> Stashed changes
const char* PGBuiltin::gate_name() const
{
      switch(type_) {
	case AND:
	    return "AND";
	    break;
	case NAND:
	    return "NAND";
	    break;

	case OR:
	    return "OR";
	    break;
	case NOR:
	    return "NOR";
	    break;

	case XOR:
	    return "XOR";
	    break;
	case XNOR:
	    return "XNOR";
	    break;

	case BUF:
	    return "BUF";
	    break;
	case NOT:
	    return "NOT";
	    break;

	case BUFIF0:
	    return "BUFIF0";
	    break;
	case NOTIF0:
	    return "NOTIF0";
	    break;

	case BUFIF1:
	    return "BUFIF1";
	    break;
	case NOTIF1:
	    return "NOTIF1";
	    break;

	case NMOS:
	    return "NMOS";
	    break;
	case RNMOS:
	    return "RNMOS";
	    break;

	case PMOS:
	    return "PMOS";
	    break;
	case RPMOS:
	    return "RPMOS";
	    break;

	case TRAN:
	    return "TRAN";
	    break;
	case RTRAN:
	    return "RTRAN";
	    break;

	case TRANIF0:
	    return "TRANIF0";
	    break;
	case RTRANIF0:
	    return "RTRANIF0";
	    break;

	case TRANIF1:
	    return "TRANIF1";
	    break;
	case RTRANIF1:
	    return "RTRANIF1";
	    break;

	case CMOS:
	    return "CMOS";
	    break;
	case RCMOS:
	    return "RCMOS";
	    break;

	case PULLUP:
	    return "PULLUP";
	    break;
	case PULLDOWN:
	    return "PULLDOWN";
	    break;
      }

      return "<unknown>";
}

PGModule::PGModule(perm_string type, perm_string name, list<PExpr*>*pins)
: PGate(name, pins), bound_type_(0), type_(type), overrides_(0), pins_(0),
<<<<<<< Updated upstream
  npins_(0), parms_(0), nparms_(0)
=======
  npins_(0), parms_(0), nparms_(0), msb_(0), lsb_(0)
>>>>>>> Stashed changes
{
}

PGModule::PGModule(perm_string type, perm_string name,
		   named<PExpr*>*pins, unsigned npins)
: PGate(name, 0), bound_type_(0), type_(type), overrides_(0), pins_(pins),
<<<<<<< Updated upstream
  npins_(npins), parms_(0), nparms_(0)
=======
  npins_(npins), parms_(0), nparms_(0), msb_(0), lsb_(0)
>>>>>>> Stashed changes
{
}

PGModule::PGModule(Module*type, perm_string name)
: PGate(name, 0), bound_type_(type), overrides_(0), pins_(0),
<<<<<<< Updated upstream
  npins_(0), parms_(0), nparms_(0)
=======
  npins_(0), parms_(0), nparms_(0), msb_(0), lsb_(0)
>>>>>>> Stashed changes
{
}

PGModule::~PGModule()
{
}

void PGModule::set_parameters(list<PExpr*>*o)
{
      assert(overrides_ == 0);
      overrides_ = o;
}

void PGModule::set_parameters(named<PExpr*>*pa, unsigned npa)
{
      assert(parms_ == 0);
      assert(overrides_ == 0);
      parms_ = pa;
      nparms_ = npa;
}

<<<<<<< Updated upstream
=======
void PGModule::set_range(PExpr*msb, PExpr*lsb)
{
      assert(msb_ == 0);
      assert(lsb_ == 0);

      msb_ = msb;
      lsb_ = lsb;
}

>>>>>>> Stashed changes
perm_string PGModule::get_type() const
{
      return type_;
}
