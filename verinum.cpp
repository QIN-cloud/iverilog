#include "config.h"

#include  "verinum.h"
#include  <iostream>
#include  <cassert>

verinum::verinum()
: bits_(0), nbits_(0), has_len_(false), has_sign_(false), string_flag_(false)
{
}

verinum::verinum(const V*bits, unsigned nbits, bool has_len)
: has_len_(has_len), has_sign_(false), string_flag_(false)
{
      nbits_ = nbits;
      bits_ = new V [nbits];
      for (unsigned idx = 0 ;  idx < nbits ;  idx += 1) {
	    bits_[idx] = bits[idx];
      }
}

verinum::verinum(const string&str)
: has_len_(true), has_sign_(false), string_flag_(true)
{
      nbits_ = str.length() * 8;
      bits_ = new V [nbits_];

      unsigned idx, cp;
      V*bp = bits_+nbits_;
      for (idx = nbits_, cp = 0 ;  idx > 0 ;  idx -= 8, cp += 1) {
	    char ch = str[cp];
	    *(--bp) = (ch&0x80) ? V1 : V0;
	    *(--bp) = (ch&0x40) ? V1 : V0;
	    *(--bp) = (ch&0x20) ? V1 : V0;
	    *(--bp) = (ch&0x10) ? V1 : V0;
	    *(--bp) = (ch&0x08) ? V1 : V0;
	    *(--bp) = (ch&0x04) ? V1 : V0;
	    *(--bp) = (ch&0x02) ? V1 : V0;
	    *(--bp) = (ch&0x01) ? V1 : V0;
      }
}

verinum::verinum(verinum::V val, unsigned n, bool h)
: has_len_(h), has_sign_(false), string_flag_(false)
{
      nbits_ = n;
      bits_ = new V[nbits_];
      for (unsigned idx = 0 ;  idx < nbits_ ;  idx += 1)
	    bits_[idx] = val;
}

verinum::verinum(unsigned long val, unsigned n)
: has_len_(true), has_sign_(false), string_flag_(false)
{
      nbits_ = n;
      bits_ = new V[nbits_];
      for (unsigned idx = 0 ;  idx < nbits_ ;  idx += 1) {
	    bits_[idx] = (val&1) ? V1 : V0;
	    val >>= 1;
      }
}

verinum::verinum(const verinum&that)
{
      string_flag_ = that.string_flag_;
      nbits_ = that.nbits_;
      bits_ = new V[nbits_];
      has_len_ = that.has_len_;
      has_sign_ = that.has_sign_;
      for (unsigned idx = 0 ;  idx < nbits_ ;  idx += 1)
	    bits_[idx] = that.bits_[idx];
}

verinum::verinum(const verinum&that, unsigned nbits)
{
      string_flag_ = false;
      nbits_ = nbits;
      bits_ = new V[nbits_];
      has_len_ = true;
      has_sign_ = that.has_sign_;

      unsigned copy = nbits;
      if (copy > that.nbits_)
	    copy = that.nbits_;
      for (unsigned idx = 0 ;  idx < copy ;  idx += 1)
	    bits_[idx] = that.bits_[idx];

      if (copy < nbits_) {
	    if (has_sign_) {
		  for (unsigned idx = copy ;  idx < nbits_ ;  idx += 1)
			bits_[idx] = bits_[idx-1];
	    } else {
		  for (unsigned idx = copy ;  idx < nbits_ ;  idx += 1)
			bits_[idx] = verinum::V0;
	    }
      }
}

verinum::verinum(long that)
: has_len_(false), has_sign_(true), string_flag_(false)
{
      long tmp;

      tmp = that/2;
      nbits_ = 1;
      while ((tmp != 0) && (tmp != -1)) {
	    nbits_ += 1;
	    tmp /= 2;
      }

      nbits_ += 1;

      bits_ = new V[nbits_];
      for (unsigned idx = 0 ;  idx < nbits_ ;  idx += 1) {
	    bits_[idx] = (that & 1)? V1 : V0;
	    that /= 2;
      }
}

verinum::~verinum()
{
      delete[]bits_;
}

verinum& verinum::operator= (const verinum&that)
{
      if (this == &that) return *this;
      delete[]bits_;
      nbits_ = that.nbits_;
      bits_ = new V[that.nbits_];
      for (unsigned idx = 0 ;  idx < nbits_ ;  idx += 1)
	    bits_[idx] = that.bits_[idx];

      has_len_ = that.has_len_;
      has_sign_ = that.has_sign_;
      string_flag_ = that.string_flag_;
      return *this;
}

verinum::V verinum::get(unsigned idx) const
{
      assert(idx < nbits_);
      return bits_[idx];
}

verinum::V verinum::set(unsigned idx, verinum::V val)
{
      assert(idx < nbits_);
      return bits_[idx] = val;
}

unsigned long verinum::as_ulong() const
{
      if (nbits_ == 0)
	    return 0;

      if (!is_defined())
	    return 0;

      unsigned top = nbits_;
      if (top >= (8 * sizeof(unsigned long)))
	  top = 8 * sizeof(unsigned long);

      unsigned long val = 0;
      unsigned long mask = 1;
      for (unsigned idx = 0 ;  idx < top ;  idx += 1, mask <<= 1)
	    if (bits_[idx] == V1)
		  val |= mask;

      return val;
}

/*
 * This function returns the native long integer that represents the
 * value of this object. It accounts for sign extension if the value
 * is signed.
 *
 * If the value is undefined, return 0.
 *
 * This function presumes that the native format is 2s compliment
 * (pretty safe these days) and masks/sets bits accordingly. If the
 * value is too large for the native form, it truncates the high bits.
 */
signed long verinum::as_long() const
{
      if (nbits_ == 0)
	    return 0;

      if (!is_defined())
	    return 0;

      signed long val = 0;

      if (has_sign_ && (bits_[nbits_-1] == V1)) {
	    unsigned top = nbits_;
	    if (top > (8 * sizeof(long) - 1))
		  top = 8 * sizeof(long) - 1;

	    val = -1;
	    signed long mask = ~1L;
	    for (unsigned idx = 0 ;  idx < top ;  idx += 1) {
		  if (bits_[idx] == V0)
			val &= mask;

		  mask = (mask << 1) | 1L;
	    }

      } else {
	    unsigned top = nbits_;
	    if (top > (8 * sizeof(long) - 1))
		  top = 8 * sizeof(long) - 1;

	    signed long mask = 1;
	    for (unsigned idx = 0 ;  idx < top ;  idx += 1, mask <<= 1)
		  if (bits_[idx] == V1)
			val |= mask;
      }

      return val;
}

string verinum::as_string() const
{
      assert( nbits_%8 == 0 );
      if (nbits_ == 0)
	    return "";

      char*tmp = new char[nbits_/8+1];
      char*cp = tmp;
      for (unsigned idx = nbits_ ;  idx > 0 ;  idx -= 8) {
	    V*bp = bits_+idx;
	    *cp = 0;
	    if (*(--bp) == V1) *cp |= 0x80;
	    if (*(--bp) == V1) *cp |= 0x40;
	    if (*(--bp) == V1) *cp |= 0x20;
	    if (*(--bp) == V1) *cp |= 0x10;
	    if (*(--bp) == V1) *cp |= 0x08;
	    if (*(--bp) == V1) *cp |= 0x04;
	    if (*(--bp) == V1) *cp |= 0x02;
	    if (*(--bp) == V1) *cp |= 0x01;
	    if (*cp != 0) {
		  cp += 1;
		  *cp = 0;
	    }
      }

      tmp[nbits_/8] = 0;
      string result = string(tmp);
      delete[]tmp;
      return result;
}

bool verinum::is_before(const verinum&that) const
{
      if (that.nbits_ > nbits_) return true;
      if (that.nbits_ < nbits_) return false;

      for (unsigned idx = nbits_  ;  idx > 0 ;  idx -= 1) {
	    if (bits_[idx-1] < that.bits_[idx-1]) return true;
	    if (bits_[idx-1] > that.bits_[idx-1]) return false;
      }
      return false;
}

bool verinum::is_defined() const
{
      for (unsigned idx = 0 ;  idx < nbits_ ;  idx += 1) {
	    if (bits_[idx] == Vx) return false;
	    if (bits_[idx] == Vz) return false;
      }
      return true;
}

bool verinum::is_zero() const
{
      for (unsigned idx = 0 ;  idx < nbits_ ;  idx += 1)
	    if (bits_[idx] != V0) return false;

      return true;
}

/*
 * This function returns a version of the verinum that has only as
 * many bits as are needed to accurately represent the value. It takes
 * into account the signedness of the value.
 *
 * If the input value has a definite length, then that value is just
 * returned as is.
 */
verinum trim_vnum(const verinum&that)
{
      unsigned tlen;

      if (that.has_len())
	    return that;

      if (that.len() < 2)
	    return that;

      if (that.has_sign()) {
	    unsigned top = that.len()-1;
	    verinum::V sign = that.get(top);

	    while ((top > 0) && (that.get(top) == sign))
		  top -= 1;

	      /* top points to the first digit that is not the
		 sign. Set the length to include this and one proper
		 sign bit. */

	    if (that.get(top) != sign)
		  top += 1;

	    tlen = top+1;

      } else {

	      /* If the result is unsigned and has an indefinite
		 length, then trim off leading zeros. */
	    tlen = 1;
	    for (unsigned idx = tlen ;  idx < that.len() ;  idx += 1)
		  if (that.get(idx) != verinum::V0)
			tlen = idx + 1;

      }

      verinum tmp (verinum::V0, tlen, false);
      tmp.has_sign(that.has_sign());
      for (unsigned idx = 0 ;  idx < tmp.len() ;  idx += 1)
	    tmp.set(idx, that.get(idx));

      return tmp;
}

ostream& operator<< (ostream&o, verinum::V v)
{
      switch (v) {
	  case verinum::V0:
	    o << "0";
	    break;
	  case verinum::V1:
	    o << "1";
	    break;
	  case verinum::Vx:
	    o << "x";
	    break;
	  case verinum::Vz:
	    o << "z";
	    break;
      }
      return o;
}

/*
 * This operator is used by various dumpers to write the verilog
 * number in a Verilog format.
 */
ostream& operator<< (ostream&o, const verinum&v)
{
      if (v.is_string()) {
	    o << "\"" << v.as_string() << "\"";
	    return o;
      }

	/* If the verinum number has a fixed length, dump all the bits
	   literally. This is how we express the fixed length in the
	   output. */
      if (v.has_len()) {
	    o << v.len();
		if (v.has_sign())
		  ;//o << "'sb";
	    else
		  o << "'b";

	    if (v.len() == 0) {
		  o << "0";
		  return o;
	    }

	    for (unsigned idx = v.len() ;  idx > 0 ;  idx -= 1)
		  o << v[idx-1];

	    return o;
      }

	/* If the number is fully defined (no x or z) then print it
	   out as a decimal number. */
      if (v.is_defined()) {
	    if (v.has_sign())
		  o << /*"'sd" <<*/ v.as_long();
	    else
		  o << /*"'d" <<*/ v.as_ulong();
	    return o;
      }

	/* Oh, well. Print the minimum to get the value properly
	   displayed. */
      if (v.has_sign())
	    ;//o << "'sb";
      else
	    o << "'b";
	
      if (v.len() == 0) {
	    o << "0";
	    return o;
      }

      verinum::V trim_left = v.get(v.len()-1);
      unsigned idx;

      for (idx = v.len()-1;  idx > 0;  idx -= 1)
	    if (trim_left != v.get(idx-1))
		  break;

      o << trim_left;

      while (idx > 0) {
	    o << v.get(idx-1);
	    idx -= 1;
      }

      return o;
}

verinum::V operator == (const verinum&left, const verinum&right)
{
      if (left.len() != right.len())
	    return verinum::V0;

      for (unsigned idx = 0 ;  idx < left.len() ;  idx += 1)
	    if (left[idx] != right[idx])
		  return verinum::V0;

      return verinum::V1;
}

verinum::V operator <= (const verinum&left, const verinum&right)
{
      unsigned idx;
      for (idx = left.len() ; idx > right.len() ;  idx -= 1) {
	    if (left[idx-1] != verinum::V0) return verinum::V0;
      }

      for (idx = right.len() ; idx > left.len() ;  idx -= 1) {
	    if (right[idx-1] != verinum::V0) return verinum::V1;
      }

      while (idx > 0) {
	    if (left[idx-1] == verinum::Vx) return verinum::Vx;
	    if (left[idx-1] == verinum::Vz) return verinum::Vx;
	    if (right[idx-1] == verinum::Vx) return verinum::Vx;
	    if (right[idx-1] == verinum::Vz) return verinum::Vx;
	    if (left[idx-1] > right[idx-1]) return verinum::V0;
	    idx -= 1;
      }

      return verinum::V1;
}

verinum::V operator < (const verinum&left, const verinum&right)
{
      unsigned idx;
      for (idx = left.len() ; idx > right.len() ;  idx -= 1) {
	    if (left[idx-1] != verinum::V0) return verinum::V0;
      }

      for (idx = right.len() ; idx > left.len() ;  idx -= 1) {
	    if (right[idx-1] != verinum::V0) return verinum::V1;
      }

      while (idx > 0) {
	    if (left[idx-1] == verinum::Vx) return verinum::Vx;
	    if (left[idx-1] == verinum::Vz) return verinum::Vx;
	    if (right[idx-1] == verinum::Vx) return verinum::Vx;
	    if (right[idx-1] == verinum::Vz) return verinum::Vx;
	    if (left[idx-1] > right[idx-1]) return verinum::V0;
	    idx -= 1;
      }

      return verinum::V0;
}

static verinum::V add_with_carry(verinum::V l, verinum::V r, verinum::V&c)
{
      unsigned sum = 0;
      switch (c) {
	  case verinum::Vx:
	  case verinum::Vz:
	    c = verinum::Vx;
	    return verinum::Vx;
	  case verinum::V0:
	    break;
	  case verinum::V1:
	    sum += 1;
      }

      switch (l) {
	  case verinum::Vx:
	  case verinum::Vz:
	    c = verinum::Vx;
	    return verinum::Vx;
	  case verinum::V0:
	    break;
	  case verinum::V1:
	    sum += 1;
	    break;
      }

      switch (r) {
	  case verinum::Vx:
	  case verinum::Vz:
	    c = verinum::Vx;
	    return verinum::Vx;
	  case verinum::V0:
	    break;
	  case verinum::V1:
	    sum += 1;
	    break;
      }

      if (sum & 2)
	    c = verinum::V1;
      else
	    c = verinum::V0;
      if (sum & 1)
	    return verinum::V1;
      else
	    return verinum::V0;
}

verinum v_not(const verinum&left)
{
      verinum val = left;
      for (unsigned idx = 0 ;  idx < val.len() ;  idx += 1)
	    switch (val[idx]) {
		case verinum::V0:
		  val.set(idx, verinum::V1);
		  break;
		case verinum::V1:
		  val.set(idx, verinum::V0);
		  break;
		default:
		  val.set(idx, verinum::Vx);
		  break;
	    }

      return val;
}

/*
 * Addition works a bit at a time, from the least significant up to
 * the most significant. The result is signed only if both of the
 * operands are signed. The result is also expanded as needed to
 * prevent overflow. It is up to the caller to shrink the result back
 * down if that is the desire.
 */
verinum operator + (const verinum&left, const verinum&right)
{
      unsigned min = left.len();
      if (right.len() < min) min = right.len();

      unsigned max = left.len();
      if (right.len() > max) max = right.len();

      bool signed_flag = left.has_sign() && right.has_sign();
      verinum::V*val_bits = new verinum::V[max+1];

      verinum::V carry = verinum::V0;
      for (unsigned idx = 0 ;  idx < min ;  idx += 1)
	    val_bits[idx] = add_with_carry(left[idx], right[idx], carry);

      verinum::V rpad = signed_flag? right[right.len()-1] : verinum::V0;
      verinum::V lpad = signed_flag? left[left.len()-1] : verinum::V0;

      if (left.len() > right.len()) {

	    for (unsigned idx = min ;  idx < left.len() ;  idx += 1)
		  val_bits[idx] = add_with_carry(left[idx], rpad, carry);

      } else {

	    for (unsigned idx = min ;  idx < right.len() ;  idx += 1)
		  val_bits[idx] = add_with_carry(lpad, right[idx], carry);
      }

      if (signed_flag) {
	    val_bits[max] = add_with_carry(lpad, rpad, carry);
	    if (val_bits[max] != val_bits[max-1])
		  max += 1;
      }

      verinum val (val_bits, max, false);
      val.has_sign(signed_flag);

      delete[]val_bits;

      return val;
}

verinum operator - (const verinum&left, const verinum&r)
{
      verinum right;
      unsigned min = left.len();
      if (r.len() < min) {
	    right = verinum(verinum::V0, min);
	    for (unsigned idx = 0 ;  idx < r.len() ;  idx += 1)
		  right.set(idx, r[idx]);

      } else {
	    right = r;
      }

      right = v_not(right);

      unsigned max = left.len();
      if (right.len() > max) max = right.len();

      verinum val (verinum::V0, max);

      verinum::V carry = verinum::V1;
      for (unsigned idx = 0 ;  idx < min ;  idx += 1)
	    val.set(idx, add_with_carry(left[idx], right[idx], carry));

      assert(left.len() <= right.len());
      for (idx = min ;  idx < max ;  idx += 1)
	    val.set(idx, add_with_carry(verinum::V0, right[idx], carry));

      val.has_sign(left.has_sign() && r.has_sign());
      return val;
}

/*
 * This multiplies two verinum numbers together into a verinum
 * result. The resulting number is as large as the sum of the sizes of
 * the operand.
 *
 * The algorithm used is successive shift and add operations,
 * implemented as the nested loops.
 *
 * If either value is not completely defined, then the result is not
 * defined either.
 */
verinum operator * (const verinum&left, const verinum&right)
{
      const bool has_len_flag = left.has_len() && right.has_len();

	/* If either operand is not fully defined, then the entire
	   result is undefined. Create a result that is the right size
	   and is filled with 'bx bits. */
      if (! (left.is_defined() && right.is_defined())) {
	    verinum result (verinum::Vx, left.len()+right.len(), has_len_flag);
	    result.has_sign(left.has_sign() || right.has_sign());
	    return result;
      }

      verinum result(verinum::V0, left.len() + right.len(), has_len_flag);
      result.has_sign(left.has_sign() || right.has_sign());

      for (unsigned rdx = 0 ;  rdx < right.len() ;  rdx += 1) {

	    if (right.get(rdx) == verinum::V0)
		  continue;

	    verinum::V carry = verinum::V0;
	    for (unsigned ldx = 0 ;  ldx < left.len() ;  ldx += 1) {
		  result.set(ldx+rdx, add_with_carry(left[ldx],
						     result[rdx+ldx],
						     carry));
	    }
      }

      return trim_vnum(result);
}

/*
 * This operator divides the left number by the right number. If
 * either value is signed, the result is signed. If both values have a
 * defined length, then the result has a defined length.
 */
verinum operator / (const verinum&left, const verinum&right)
{
      const bool has_len_flag = left.has_len() && right.has_len();

      unsigned use_len = left.len();

	/* If either operand is not fully defined, then the entire
	   result is undefined. Create a result that is the right size
	   and is filled with 'bx bits. */
      if (! (left.is_defined() && right.is_defined())) {
	    verinum result (verinum::Vx, use_len, has_len_flag);
	    result.has_sign(left.has_sign() || right.has_sign());
	    return result;
      }

	/* If the right expression is a zero value, then the result is
	   filled with 'bx bits. */
      if (right.as_ulong() == 0) {
	    verinum result (verinum::Vx, use_len, has_len_flag);
	    result.has_sign(left.has_sign() || right.has_sign());
	    return result;
      }

      verinum result(verinum::Vz, use_len, has_len_flag);
      result.has_sign(left.has_sign() || right.has_sign());

	/* do the operation differently, depending on whether the
	   result is signed or not. */
      if (result.has_sign()) {

	      /* XXXX FIXME XXXX Use native unsigned division to do
		 the work. This does not work if the result is too
		 large for the native integer. */
	    assert(use_len <= 8*sizeof(long));
	    long l = left.as_long();
	    long r = right.as_long();
	    long v = l / r;
	    for (unsigned idx = 0 ;  idx < use_len ;  idx += 1) {
		  result.set(idx,  (v & 1)? verinum::V1 : verinum::V0);
		  v >>= 1;
	    }

      } else {

	      /* XXXX FIXME XXXX Use native unsigned division to do
		 the work. This does not work if the result is too
		 large for the native integer. */
	    assert(use_len <= 8*sizeof(unsigned long));
	    unsigned long l = left.as_ulong();
	    unsigned long r = right.as_ulong();
	    unsigned long v = l / r;
	    for (unsigned idx = 0 ;  idx < use_len ;  idx += 1) {
		  result.set(idx,  (v & 1)? verinum::V1 : verinum::V0);
		  v >>= 1;
	    }
      }

      return trim_vnum(result);
}

verinum operator % (const verinum&left, const verinum&right)
{
      const bool has_len_flag = left.has_len() && right.has_len();

      unsigned use_len = left.len();

	/* If either operand is not fully defined, then the entire
	   result is undefined. Create a result that is the right size
	   and is filled with 'bx bits. */
      if (! (left.is_defined() && right.is_defined())) {
	    verinum result (verinum::Vx, use_len, has_len_flag);
	    result.has_sign(left.has_sign() || right.has_sign());
	    return result;
      }

	/* If the right expression is a zero value, then the result is
	   filled with 'bx bits. */
      if (right.as_ulong() == 0) {
	    verinum result (verinum::Vx, use_len, has_len_flag);
	    result.has_sign(left.has_sign() || right.has_sign());
	    return result;
      }

      verinum result(verinum::Vz, use_len, has_len_flag);
      result.has_sign(left.has_sign() || right.has_sign());

      if (result.has_sign()) {

	      /* XXXX FIXME XXXX Use native unsigned division to do
		 the work. This does not work if the result is too
		 large for the native integer. */
	    assert(use_len <= 8*sizeof(long));
	    long l = left.as_long();
	    long r = right.as_long();
	    long v = l % r;
	    for (unsigned idx = 0 ;  idx < use_len ;  idx += 1) {
		  result.set(idx,  (v & 1)? verinum::V1 : verinum::V0);
		  v >>= 1;
	    }

      } else {

	      /* XXXX FIXME XXXX Use native unsigned division to do
		 the work. This does not work if the result is too
		 large for the native integer. */
	    assert(use_len <= 8*sizeof(unsigned long));
	    unsigned long l = left.as_ulong();
	    unsigned long r = right.as_ulong();
	    unsigned long v = l % r;
	    for (unsigned idx = 0 ;  idx < use_len ;  idx += 1) {
		  result.set(idx,  (v & 1)? verinum::V1 : verinum::V0);
		  v >>= 1;
	    }
      }

      return result;
}

verinum::V operator | (verinum::V l, verinum::V r)
{
      if (l == verinum::V1)
	    return verinum::V1;
      if (r == verinum::V1)
	    return verinum::V1;
      if (l != verinum::V0)
	    return verinum::Vx;
      if (r != verinum::V0)
	    return verinum::Vx;
      return verinum::V0;
}

verinum::V operator & (verinum::V l, verinum::V r)
{
      if (l == verinum::V0)
	    return verinum::V0;
      if (r == verinum::V0)
	    return verinum::V0;
      if (l != verinum::V1)
	    return verinum::Vx;
      if (r != verinum::V1)
	    return verinum::Vx;
      return verinum::V1;
}

verinum::V operator ^ (verinum::V l, verinum::V r)
{
	if (l == verinum::V0)
	    return r;
      if (r == verinum::V0)
	    return l;
      if ((l == verinum::V1) && (r == verinum::V1))
	    return verinum::V0;

      return verinum::Vx;
}

verinum::V bit_not(verinum::V l)
{
	if(l == verinum::Vx)
		return verinum::Vx;
	if(l == verinum::Vz)
		return verinum::Vz;
	if(l == verinum::V0)
		return verinum::V1;
	if(l == verinum::V1)
		return verinum::V0;
}

verinum::V bit_nor(verinum::V l, verinum::V r)
{
	return bit_not(l | r);
}

verinum::V bit_nand(verinum::V l, verinum::V r)
{
	return bit_not(l & r);
}

verinum::V bit_xnor(verinum::V l, verinum::V r)
{
	return bit_not(l ^ r);
}

verinum operator - (const verinum&left)
{
	verinum zero (verinum::V0, left.len()+1, left.has_len());
	verinum val = zero - left;
	val.has_sign(true);
	return val;
}

verinum bitwise_and(const verinum&left, const verinum&right)
{
	unsigned idx;
	unsigned len = left.len() >= right.len() ? left.len() : right.len();
	verinum val(verinum::Vx,len);

	for (idx = left.len() ; idx > right.len() ;  idx -= 1) {
		val.set(idx - 1, left[idx-1] & verinum::V0);
	}
	
	for (idx = right.len() ; idx > left.len() ;  idx -= 1) {
		val.set(idx - 1, right[idx-1] & verinum::V0);
	}
	
	while (idx > 0) {
		val.set(idx - 1, left[idx-1] & right[idx-1]);
		idx -= 1;
	}
	
	return val;
}

verinum bitwise_or(const verinum&left, const verinum&right)
{
	unsigned idx;
	unsigned len = left.len() >= right.len() ? left.len() : right.len();
	verinum val(verinum::Vx,len);

	for (idx = left.len() ; idx > right.len() ;  idx -= 1) {
		val.set(idx - 1, left[idx-1] | verinum::V0);
	}
	
	for (idx = right.len() ; idx > left.len() ;  idx -= 1) {
		val.set(idx - 1, right[idx-1] | verinum::V0);
	}
	
	while (idx > 0) {
		val.set(idx - 1, left[idx-1] | right[idx-1]);
		idx -= 1;
	}
	
	return val;
}

verinum bitwise_xor(const verinum&left, const verinum&right)
{
	unsigned idx;
	unsigned len = left.len() >= right.len() ? left.len() : right.len();
	verinum val(verinum::Vx,len);

	for (idx = left.len() ; idx > right.len() ;  idx -= 1) {
		val.set(idx - 1, left[idx-1] ^ verinum::V0);
	}
	
	for (idx = right.len() ; idx > left.len() ;  idx -= 1) {
		val.set(idx - 1, right[idx-1] ^ verinum::V0);
	}
	
	while (idx > 0) {
		val.set(idx - 1, left[idx-1] ^ right[idx-1]);
		idx -= 1;
	}
	
	return val;
}

verinum bitwise_xnor(const verinum&left, const verinum&right)
{
	unsigned idx;
	unsigned len = left.len() >= right.len() ? left.len() : right.len();
	verinum val(verinum::Vx,len);

	for (idx = left.len() ; idx > right.len() ;  idx -= 1) {
		val.set(idx - 1, bit_xnor(left[idx-1],verinum::V0));
	}
	
	for (idx = right.len() ; idx > left.len() ;  idx -= 1) {
		val.set(idx - 1, bit_xnor(right[idx-1],verinum::V0));
	}
	
	while (idx > 0) {
		val.set(idx - 1, bit_xnor(left[idx-1],right[idx-1]));
		idx -= 1;
	}
	
	return val;
}

verinum::V unary_and(const verinum&l)
{
	verinum::V val = l[0];
	for(unsigned idx = 1; idx < l.len(); ++idx)
		val = val & l[idx];
	return val;
}

verinum::V unary_nand(const verinum&l)
{
	verinum::V val = l[0];
	for(unsigned idx = 1; idx < l.len(); ++idx)
		val = bit_nand(val, l[idx]);
	return val;
}

verinum::V unary_or(const verinum&l)
{
	verinum::V val = l[0];
	for(unsigned idx = 1; idx < l.len(); ++idx)
		val = val | l[idx];
	return val;
}

verinum::V unary_nor(const verinum&l)
{
	verinum::V val = l[0];
	for(unsigned idx = 1; idx < l.len(); ++idx)
		val = bit_nor(val, l[idx]);
	return val;
}

verinum::V unary_xor(const verinum&l)
{
	verinum::V val = l[0];
	for(unsigned idx = 1; idx < l.len(); ++idx)
		val = val ^ l[idx];
	return val;
}

verinum::V unary_xnor(const verinum&l)
{
	verinum::V val = l[0];
	for(unsigned idx = 1; idx < l.len(); ++idx)
		val = bit_xnor(val, l[idx]);
	return val;
}

verinum::V land(const verinum&left, const verinum&right)//&&
{
	verinum::V l = unary_or(left);
	verinum::V r = unary_or(right);

	verinum::V val = l & r;
	
	return val;
}

verinum::V lor(const verinum&left, const verinum&right)//||
{
	verinum::V l = unary_or(left);
	verinum::V r = unary_or(right);

	verinum::V val = l | r;

	return val;
}

verinum::V lnot(const verinum&left)
{
	verinum::V l = unary_or(left);
	verinum::V val = bit_not(l);
	return val;
}

verinum::V ceq(const verinum&left, const verinum&right)//===
{
	if (left.len() != right.len())
	    return verinum::V0;

      for (unsigned idx = 0 ;  idx < left.len() ;  idx += 1)
	  {
	    if ((left[idx] < 2) && (right[idx] < 2) && (left[idx] != right[idx]))
		  return verinum::V0;
	  }
	  return verinum::V1;
}

verinum::V cneq(const verinum&left, const verinum&right)//!==
{
	verinum::V result = ceq(left, right);
	return bit_not(result);
}

verinum lshift(const verinum& l, const verinum& r)
{
	assert(r.is_defined());
	unsigned long rv = r.as_ulong();
	verinum res(verinum::V0, l.len());
	if (rv < res.len()) {
		unsigned cnt = res.len() - rv;
		for (unsigned idx = 0 ;  idx < cnt ;  idx += 1)
			res.set(idx+rv, l.get(idx));
	}

	return res;
}

verinum rshift(const verinum& l, const verinum& r)
{
	assert(r.is_defined());
	unsigned long rv = r.as_ulong();
	verinum res(verinum::V0, l.len());
	if (rv < res.len()) {
		unsigned cnt = res.len() - rv;
		for (unsigned idx = 0 ;  idx < cnt ;  idx += 1)
			res.set(idx, l.get(idx+rv));
	}

	return res;
}