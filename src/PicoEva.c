/*-----------------------------------*/
/*             >>>Pico<<<            */
/*            Theo D'Hondt           */
/*   VUB Programming Technology Lab  */
/*             (c) 1997              */
/*-----------------------------------*/
/*            Evaluation             */
/*-----------------------------------*/

#include "Pico.h"
#include "PicoMai.h"
#include "PicoEnv.h"
#include "PicoMem.h"
#include "PicoNat.h"
#include "PicoEva.h"

/* private constants */

#define CAL _eval_CAL_   
#define EXP _eval_EXP_   
#define NAT _eval_NAT_   

/* private prototypes */

static _NIL_TYPE_ APL(_NIL_TYPE_);
static _NIL_TYPE_ ASS(_NIL_TYPE_);
static _NIL_TYPE_ ATA(_NIL_TYPE_);
static _NIL_TYPE_ ATV(_NIL_TYPE_);
static _NIL_TYPE_ BND(_NIL_TYPE_);
static _NIL_TYPE_ CHG(_NIL_TYPE_);
static _NIL_TYPE_ DEF(_NIL_TYPE_);
static _NIL_TYPE_ IDX(_NIL_TYPE_);
static _NIL_TYPE_ INI(_NIL_TYPE_);
static _NIL_TYPE_ NYI(_NIL_TYPE_);
static _NIL_TYPE_ REF(_NIL_TYPE_);
static _NIL_TYPE_ RET(_NIL_TYPE_);
static _NIL_TYPE_ RPL(_NIL_TYPE_);
static _NIL_TYPE_ SET(_NIL_TYPE_);
static _NIL_TYPE_ SLF(_NIL_TYPE_);
static _NIL_TYPE_ SWP(_NIL_TYPE_);
static _NIL_TYPE_ TBL(_NIL_TYPE_);
static _NIL_TYPE_ VAR(_NIL_TYPE_);
static _NIL_TYPE_ FIM(_NIL_TYPE_);
static _NIL_TYPE_ FAB(_NIL_TYPE_);
static _NIL_TYPE_ MIX(_NIL_TYPE_);
static _NIL_TYPE_ SWI(_NIL_TYPE_);

/* private variables */

static const _BYT_TYPE_ TAB_tab[] = { 0, /* VOI */
1, /* NAT */
0, /* FRC */
0, /* TXT */
1, /* TAB */
1, /* FUN */
1, /* VAR */
1, /* APL */
1, /* TBL */
1, /* DEF */
1, /* SET */
1, /* DCT */
1, /* ENV */
1, /* MAT */
0, /* NYI */
0, /* NYI */
0 }; /* NBR */

static const _CNT_TYPE_ CNT_tab[] = { SLF, /* VOI */
SLF, /* NAT */
SLF, /* FRC */
SLF, /* TXT */
SLF, /* TAB */
SLF, /* FUN */
VAR, /* VAR */
APL, /* APL */
TBL, /* TBL */
DEF, /* DEF */
SET, /* SET */
SLF, /* DCT */
SLF, /* ENV */
SLF, /* MAT */
NYI, /* NYI */
NYI, /* NYI */
SLF }; /* NBR */

/* private functions */

/*------------------------------------------------------------------------*/
/*  NYI                                                                   */
/*     expr-stack: [... ... ... ... ... EXP] -> [... ... ... ... ... ...] */
/*     cont-stack: [... ... ... ... ... NYI] -> [... ... ... ... ... ERR] */
/*------------------------------------------------------------------------*/
static _NIL_TYPE_ NYI(_NIL_TYPE_) {
	_error_(_AGR_ERROR_);
}

/*------------------------------------------------------------------------*/
/*  APL                                                                   */
/*     expr-stack: [... ... ... ... ... APL] -> [... ... ... ... FUN ARG] */
/*     cont-stack: [... ... ... ... ... APL] -> [... ... ... ... ... CAL] */
/*                                                                        */
/*     expr-stack: [... ... ... ... ... APL] -> [... ... ... ... FUN ARG] */
/*     cont-stack: [... ... ... ... ... APL] -> [... ... ... ... CAL EXP] */
/*                                                                        */
/*     expr-stack: [... ... ... ... ... APL] -> [... ... ... ... NBR ARG] */
/*     cont-stack: [... ... ... ... ... APL] -> [... ... ... ... ... NAT] */
/*                                                                        */
/*     expr-stack: [... ... ... ... ... APL] -> [... ... ... ... NBR ARG] */
/*     cont-stack: [... ... ... ... ... APL] -> [... ... ... ... NAT EXP] */
/*------------------------------------------------------------------------*/

static _NIL_TYPE_ APL(_NIL_TYPE_) {
	_EXP_TYPE_ apl, arg, dct, fun, nam, nbr;
	_TAG_TYPE_ tag;
	_stk_claim_();
	_stk_peek_EXP_(apl);
	nam = _ag_get_APL_NAM_(apl);
	arg = _ag_get_APL_ARG_(apl);
	_dct_locate_(nam, dct, _DCT_);
	fun = _ag_get_DCT_VAL_(dct);
	tag = _ag_get_TAG_(fun);
	switch (tag) {
	case _FUN_TAG_:
		_stk_poke_EXP_(fun);
		_stk_poke_CNT_(CAL);
		break;
	case _NAT_TAG_:
		nbr = _ag_get_NAT_NBR_(fun);
		_stk_poke_EXP_(nbr);
		_stk_poke_CNT_(NAT);
		break;
	default:
		_error_msg_(_NAF_ERROR_, nam);
	}
	_stk_push_EXP_(arg);
	tag = _ag_get_TAG_(arg);
	if (tag != _TAB_TAG_)
		_stk_push_CNT_(EXP);
}

/*------------------------------------------------------------------------*/
/*  ASS                                                                   */
/*     expr-stack: [... ... ... ... DCT VAL] -> [... ... ... ... ... VAL] */
/*     cont-stack: [... ... ... ... ... ASS] -> [... ... ... ... ... ...] */
/*------------------------------------------------------------------------*/
static _NIL_TYPE_ ASS(_NIL_TYPE_) {
	_EXP_TYPE_ dct, val;
	_stk_pop_EXP_(val);
	_stk_peek_EXP_(dct);
	_ag_set_DCT_VAL_(dct, val);
	_ag_set_DCT_DCT_(dct, _DCT_);
	_DCT_ = dct;
	_stk_poke_EXP_(val);
	_stk_zap_CNT_();
}

/*------------------------------------------------------------------------*/
/*  ATA                                                                   */
/*     expr-stack: [EXP DCT ARG TAB NBR APL] -> [EXP DCT ARG TAB NBR APL] */
/*     cont-stack: [... ... ... ... ... ATA] -> [... ... ... ... ... ATA] */
/*                                                                        */
/*     expr-stack: [EXP DCT ARG TAB NBR APL] -> [... ... ... ... DCT EXP] */
/*     cont-stack: [... ... ... ... ... ATA] -> [... ... ... ... RET EXP] */
/*                                                                        */
/*     expr-stack: [EXP DCT ARG TAB NBR APL] -> [... ... ... ... ... EXP] */
/*     cont-stack: [... ... ... ... ... ATA] -> [... ... ... ... ... EXP] */
/*------------------------------------------------------------------------*/
static _NIL_TYPE_ ATA(_NIL_TYPE_) {
	_EXP_TYPE_ act, apl, arg, dct, exp, fun, nam, nbr, par, tab;
	_CNT_TYPE_ cnt;
	_UNS_TYPE_ ctr, siz;
	_mem_claim_();
	fun = _ag_make_FUN_();
	_stk_pop_EXP_(apl);
	_stk_pop_EXP_(nbr);
	_stk_pop_EXP_(tab);
	_stk_pop_EXP_(arg);
	_stk_peek_EXP_(dct);
	siz = _ag_get_TAB_SIZ_(arg);
	ctr = _ag_get_NBU_(nbr);
	act = _ag_get_TAB_EXP_(arg, ctr);
	nam = _ag_get_APL_NAM_(apl);
	par = _ag_get_APL_ARG_(apl);
	_ag_set_FUN_NAM_(fun, nam);
	_ag_set_FUN_ARG_(fun, par);
	_ag_set_FUN_EXP_(fun, act);
	// _ag_set_FUN_DCT_(fun, dct);
	_ag_set_FUN_DCT_(fun, _DCT_);
	// jdk
	_ag_set_TAB_EXP_(tab, ctr, fun);
	if (ctr < siz) {
		_stk_push_EXP_(arg);
		_stk_push_EXP_(tab);
		nbr = _ag_succ_NBR_(nbr);
		_stk_push_EXP_(nbr);
		_stk_push_EXP_(apl);
	} else {
		_ag_set_DCT_VAL_(dct, tab);
		_stk_zap_EXP_();
		_stk_zap_CNT_();
		_stk_peek_CNT_(cnt);
		if (cnt != RET) {
			_stk_peek_EXP_(exp);
			_stk_poke_EXP_(_DCT_);
			_stk_push_EXP_(exp);
			_stk_push_CNT_(RET);
		}
		_stk_push_CNT_(EXP);
		_DCT_ = dct;
	}
}

/*------------------------------------------------------------------------*/
/*  ATV                                                                   */
/*     expr-stack: [EXP DCT ARG TAB NBR VAL] -> [EXP DCT ARG TAB NBR EXP] */
/*     cont-stack: [... ... ... ... ... ATV] -> [... ... ... ... ATV EXP] */
/*                                                                        */
/*     expr-stack: [EXP DCT ARG TAB NBR VAL] -> [... ... ... ... DCT EXP] */
/*     cont-stack: [... ... ... ... ... ATV] -> [... ... ... ... RET EXP] */
/*                                                                        */
/*     expr-stack: [EXP DCT ARG TAB NBR VAL] -> [... ... ... ... ... EXP] */
/*     cont-stack: [... ... ... ... ... ATV] -> [... ... ... ... ... EXP] */
/*------------------------------------------------------------------------*/
static _NIL_TYPE_ ATV(_NIL_TYPE_) {
	_EXP_TYPE_ act, arg, dct, exp, nbr, tab, val;
	_CNT_TYPE_ cnt;
	_UNS_TYPE_ ctr, siz;
	_stk_claim_();
	_stk_pop_EXP_(val);
	_stk_pop_EXP_(nbr);
	_stk_pop_EXP_(tab);
	_stk_peek_EXP_(arg);
	siz = _ag_get_TAB_SIZ_(arg);
	ctr = _ag_get_NBU_(nbr);
	_ag_set_TAB_EXP_(tab, ctr, val);
	if (ctr < siz) {
		act = _ag_get_TAB_EXP_(arg, ctr+1);
		_stk_push_EXP_(tab);
		nbr = _ag_succ_NBR_(nbr);
		_stk_push_EXP_(nbr);
		_stk_push_EXP_(act);
		_stk_push_CNT_(EXP);
	} else {
		_stk_zap_EXP_();
		_stk_pop_EXP_(dct);
		_ag_set_DCT_VAL_(dct, tab);
		_stk_zap_CNT_();
		_stk_peek_CNT_(cnt);
		if (cnt != RET) {
			_stk_peek_EXP_(exp);
			_stk_poke_EXP_(_DCT_);
			_stk_push_EXP_(exp);
			_stk_push_CNT_(RET);
		}
		_stk_push_CNT_(EXP);
		_DCT_ = dct;
	}
}

/*------------------------------------------------------------------------*/
/*  BND                                                                   */
/*     expr-stack: [EXP PAR ARG NBR DCT VAL] -> [... ... ... ... DCT EXP] */
/*     cont-stack: [... ... ... ... ... BND] -> [... ... ... ... RET EXP] */
/*                                                                        */
/*     expr-stack: [EXP PAR ARG NBR DCT VAL] -> [... ... ... ... ... EXP] */
/*     cont-stack: [... ... ... ... ... BND] -> [... ... ... ... ... EXP] */
/*                                                                        */
/*     expr-stack: [EXP PAR ARG NBR DCT VAL] -> [EXP PAR ARG NBR DCT EXP] */
/*     cont-stack: [... ... ... ... ... BND] -> [... ... ... ... BND EXP] */
/*                                                                        */
/*     expr-stack: [EXP PAR ARG NBR DCT VAL] -> [EXP PAR ARG NBR DCT FUN] */
/*     cont-stack: [... ... ... ... ... BND] -> [... ... ... ... ... BND] */
/*------------------------------------------------------------------------*/
static _NIL_TYPE_ BND(_NIL_TYPE_) {
	_EXP_TYPE_ act, arg, dct, exp, fun, frm, nam, nbr, par, val, xdc;
	_CNT_TYPE_ cnt;
	_TAG_TYPE_ tag;
	_UNS_TYPE_ ctr, siz;
	_stk_claim_();
	_mem_claim_();
	_stk_pop_EXP_(val);
	_stk_pop_EXP_(dct);
	_ag_set_DCT_VAL_(dct, val);
	_stk_pop_EXP_(nbr);
	_stk_pop_EXP_(arg);
	siz = _ag_get_TAB_SIZ_(arg);
	ctr = _ag_get_NBU_(nbr);
	if (ctr == siz) {
		_stk_zap_EXP_();
		_stk_zap_CNT_();
		_stk_peek_CNT_(cnt);
		if (cnt != RET) {
			_stk_peek_EXP_(exp);
			_stk_poke_EXP_(_DCT_);
			_stk_push_EXP_(exp);
			_stk_push_CNT_(RET);
		}
		_stk_push_CNT_(EXP);
		_DCT_ = dct;
	} else {
		_stk_peek_EXP_(par);
		frm = _ag_get_TAB_EXP_(par, ++ctr);
		act = _ag_get_TAB_EXP_(arg, ctr);
		tag = _ag_get_TAG_(frm);
		_stk_push_EXP_(arg);
		nbr = _ag_succ_NBR_(nbr);
		_stk_push_EXP_(nbr);
		xdc = _ag_make_DCT_();
		_stk_push_EXP_(xdc);
		_ag_set_DCT_DCT_(xdc, dct);
		switch (tag) {
		case _VAR_TAG_:
			nam = _ag_get_VAR_NAM_(frm);
			_ag_set_DCT_NAM_(xdc, nam);
			_stk_push_EXP_(act);
			_stk_push_CNT_(EXP);
			break;
		case _APL_TAG_:
			fun = _ag_make_FUN_();
			nam = _ag_get_APL_NAM_(frm);
			arg = _ag_get_APL_ARG_(frm);
			_ag_set_DCT_NAM_(xdc, nam);
			_ag_set_FUN_NAM_(fun, nam);
			_ag_set_FUN_ARG_(fun, arg);
			_ag_set_FUN_EXP_(fun, act);
			_ag_set_FUN_DCT_(fun, _DCT_);
			_stk_push_EXP_(fun);
			break;
		default:
			_error_(_IPM_ERROR_);
		}
	}
}

/*------------------------------------------------------------------------*/
/*  CHG                                                                   */
/*     expr-stack: [... ... ... ... DCT VAL] -> [... ... ... ... ... VAL] */
/*     cont-stack: [... ... ... ... ... CHG] -> [... ... ... ... ... ...] */
/*------------------------------------------------------------------------*/
static _NIL_TYPE_ CHG(_NIL_TYPE_) {
	_EXP_TYPE_ dct, val;
	_stk_pop_EXP_(val);
	_stk_peek_EXP_(dct);
	_ag_set_DCT_VAL_(dct, val);
	_stk_poke_EXP_(val);
	_stk_zap_CNT_();
}

/*------------------------------------------------------------------------*/
/*  DEF                                                                   */
/*     expr-stack: [... ... ... ... ... DEF] -> [... ... ... ... DCT EXP] */
/*     cont-stack: [... ... ... ... ... DEF] -> [... ... ... ... ASS EXP] */
/*                                                                        */
/*     expr-stack: [... ... ... ... ... DEF] -> [... ... ... ... ... FUN] */
/*     cont-stack: [... ... ... ... ... DEF] -> [... ... ... ... ... ...] */
/*                                                                        */
/*     expr-stack: [... ... ... ... ... DEF] -> [... ... ... DCT EXP IDX] */
/*     cont-stack: [... ... ... ... ... DEF] -> [... ... ... ... IDX EXP] */
/*------------------------------------------------------------------------*/
static _NIL_TYPE_ DEF(_NIL_TYPE_) {
	_EXP_TYPE_ arg, dct, def, exp, fun, idx, inv, nam;
	_TAG_TYPE_ tag;
	_stk_claim_();
	_mem_claim_();
	dct = _ag_make_DCT_();
	_stk_peek_EXP_(def);
	inv = _ag_get_DEF_INV_(def);
	exp = _ag_get_DEF_EXP_(def);
	tag = _ag_get_TAG_(inv);
	switch (tag) {
	case _VAR_TAG_:
		nam = _ag_get_VAR_NAM_(inv);
		_ag_set_DCT_NAM_(dct, nam);
		_stk_poke_EXP_(dct);
		_stk_push_EXP_(exp);
		_stk_poke_CNT_(ASS);
		_stk_push_CNT_(EXP);
		break;
	case _APL_TAG_:
		fun = _ag_make_FUN_();
		nam = _ag_get_APL_NAM_(inv);
		arg = _ag_get_APL_ARG_(inv);
		_ag_set_DCT_NAM_(dct, nam);
		_ag_set_DCT_VAL_(dct, fun);
		_ag_set_DCT_DCT_(dct, _DCT_);
		_DCT_ = dct;
		_ag_set_FUN_NAM_(fun, nam);
		_ag_set_FUN_ARG_(fun, arg);
		_ag_set_FUN_EXP_(fun, exp);
		_ag_set_FUN_DCT_(fun, dct);
		_stk_poke_EXP_(fun);
		_stk_zap_CNT_();
		break;
	case _TBL_TAG_:
		nam = _ag_get_TBL_NAM_(inv);
		idx = _ag_get_TBL_IDX_(inv);
		_ag_set_DCT_NAM_(dct, nam);
		_stk_poke_EXP_(dct);
		_stk_push_EXP_(exp);
		_stk_push_EXP_(idx);
		//pushed de index expressie om daarna geevalueerd te worden
		_stk_poke_CNT_(IDX);
		_stk_push_CNT_(EXP);
		break;
	default:
		_error_(_AGR_ERROR_);
	}
}

/*------------------------------------------------------------------------*/
/*  IDX                                  1                        2   3   */ //exp is de exp waarmee de tabel wordt opgevuld
/*     expr-stack: [... ... ... DCT EXP TAB] -> [... ... ... DCT EXP EXP] */
/*     cont-stack: [... ... ... ... ... IDX] -> [... ... ... ... FAB EXP] */
/*                                       1                                */
/*     expr-stack: [... ... ... DCT EXP TAB] -> [DCT EXP TAB SIZ *1* EXP] */ //siz is the dim size size
/*     cont-stack: [... ... ... ... ... IDX] -> [... ... ... INI FIM EXP] */
/*                                       1                                */
/*     expr-stack: [... ... ... DCT EXP TAB] -> [... ... ... ... ... *E*] */ //empty
/*     cont-stack: [... ... ... ... ... IDX] -> [... ... ... ... ... ...] */
/*  1 Tab with the dimensions			                                  */
/*  2 expression to fill up the table	                                  */
/*  3 index expression					                                  */
/*------------------------------------------------------------------------*/
static _NIL_TYPE_ IDX(_NIL_TYPE_) {
	_EXP_TYPE_ dct, exp, dim, tab;
	_TAG_TYPE_ tag;
	_UNS_TYPE_ siz;
	_stk_claim_();
	_stk_pop_EXP_(dim);
	tag = _ag_get_TAG_(dim);
	if (tag == _TAB_TAG_) {
		siz = _ag_get_TAB_SIZ_(dim);
		if (siz == 1) { //oude tabellen
			dim = _ag_get_TAB_EXP_(dim, 1);
			_stk_push_EXP_(dim);
			_stk_poke_CNT_(FAB);
			_stk_push_CNT_(EXP);
		} else if (siz > 0) {
			_EXP_TYPE_ first_dim, mat_size;
			_UNS_TYPE_ mat_size_int;
			mat_size_int = 1; //neutral element for the multiplication
			mat_size = _ag_make_NBU_(mat_size_int);
			first_dim = _ag_get_TAB_EXP_(dim, 1);
			_stk_push_EXP_(dim);
			_stk_push_EXP_(mat_size);
			_stk_push_EXP_(_ONE_);
			_stk_push_EXP_(first_dim);
			_stk_poke_CNT_(INI);
			_stk_push_CNT_(FIM);
			_stk_push_CNT_(EXP);
		} else {
			_error_(_SIZ_ERROR_);
		}
	} else {
		_error_(_SIZ_ERROR_);
	}
}
/*------------------------------------------------------------------------*/
/*  FIM                                                                   */
/*     expr-stack: [... ... EXP TAB SIZ IDX VAL] -> [... MAT EXP *1* EXP] */
/*     cont-stack: [... ... ... ... ... INI FIM] -> [... ... ... INI EXP] */
/*                      1                                                 */
/*     expr-stack: [DCT EXP TAB SIZ IDX VAL] -> [DCT EXP TAB SIZ IDX EXP] */ //recursie stap
/*     cont-stack: [... ... ... ... INI FIM] -> [... ... ... INI FIM EXP] */
/*  1 expression that fills up the matrice                                */
/*------------------------------------------------------------------------*/
static _NIL_TYPE_ FIM(_NIL_TYPE_) {
	_EXP_TYPE_ tab, siz, idx, val, exp;
	_UNS_TYPE_ size_int, index_int, val_int, tab_siz_int;
	_stk_claim_();
	_stk_pop_EXP_(val);
	_stk_pop_EXP_(idx);
	_stk_pop_EXP_(siz);
	_stk_peek_EXP_(tab);
	index_int = _ag_get_NBU_(idx);
	size_int = _ag_get_NBU_(siz);
	val_int = _ag_get_NBU_(val);
	size_int *= val_int;
	tab_siz_int = _ag_get_TAB_SIZ_(tab);
	_ag_set_TAB_EXP_(tab, index_int, val);
	//put simple exp back in the tab so we can transfer them later in the dimesions
	if (index_int < tab_siz_int) {
		idx = _ag_succ_NBR_(idx);
		index_int = index_int + 1;
		exp = _ag_get_TAB_EXP_(tab, index_int);
		siz = _ag_make_NBU_(size_int);
		_stk_push_EXP_(siz);
		_stk_push_EXP_(idx);
		_stk_push_EXP_(exp);
		_stk_push_CNT_(EXP);
	} else {
		_EXP_TYPE_ mat;
		_stk_pop_EXP_(tab);
		_stk_pop_EXP_(exp);
		size_int += _DIM_SIZE_SIZE_;
		size_int += tab_siz_int;
		_mem_claim_SIZ_(size_int);
		mat = _ag_make_MAT(size_int);
		_TAG_TYPE_ tag;
		tag = _ag_get_TAG_(mat);
		_ag_set_MAT_dim_siz_(mat, _ag_make_NBU_(tab_siz_int));
		_UNS_TYPE_ i;
		for (i = 1; i <= tab_siz_int; i++) {
			_ag_set_MAT_dim_(mat, _ag_get_TAB_EXP_(tab, i), i);
		}
		_stk_push_EXP_(mat);
		_stk_push_EXP_(exp);
		_stk_push_EXP_(_ONE_);
		_stk_push_EXP_(exp);
		_stk_poke_CNT_(EXP);
	}

}

/*------------------------------------------------------------------------*/
/*  FAB                                                                   */
/*     expr-stack: [... ... ... DCT EXP IDX] -> [... DCT TAB EXP *1* EXP] */
/*     cont-stack: [... ... ... ... ... FAB] -> [... ... ... ... INI EXP] */
/*                                                                        */
/*     expr-stack: [... ... ... DCT EXP IDX] -> [... ... ... ... ... *E*] */
/*     cont-stack: [... ... ... ... ... FAB] -> [... ... ... ... ... ...] */
/*                                                                        */
/*------------------------------------------------------------------------*/

static _NIL_TYPE_ FAB(_NIL_TYPE_) {
	_EXP_TYPE_ tab, exp, idx, dct;
	_UNS_TYPE_ siz;
	_TAG_TYPE_ tag;
	_stk_claim_();
	_stk_pop_EXP_(idx);
	_stk_peek_EXP_(exp);
	tag = _ag_get_TAG_(idx);
	if (tag == _NBR_TAG_) {
		siz = _ag_get_NBU_(idx);
		if (siz > 0) {
			_mem_claim_SIZ_(siz);
			tab = _ag_make_TAB_(siz);
			_stk_poke_EXP_(tab);
			_stk_push_EXP_(exp);
			_stk_push_EXP_(_ONE_);
			_stk_push_EXP_(exp);
			_stk_poke_CNT_(INI);
			_stk_push_CNT_(EXP);
		} else if (siz == 0) {
			_stk_zap_EXP_();
			_stk_peek_EXP_(dct);
			_ag_set_DCT_VAL_(dct, _EMPTY_);
			_ag_set_DCT_DCT_(dct, _DCT_);
			_DCT_ = dct;
			_stk_poke_EXP_(_EMPTY_);
			_stk_zap_CNT_();
		} else {
			_error_(_IIX_ERROR_);
		}
	} else {
		_error_(_IIX_ERROR_);
	}
}

/*------------------------------------------------------------------------*/
/*  INI (initializes a certain array)                                     */
/*     expr-stack: [... DCT TAB EXP NBR VAL] -> [... DCT TAB EXP NBR EXP] */ //recursie stap
/*     cont-stack: [... ... ... ... ... INI] -> [... ... ... ... INI EXP] */
/*                                                                        */
/*     expr-stack: [... DCT MAT EXP NBR VAL] -> [... DCT MAT EXP NBR EXP] */
/*     cont-stack: [... ... ... ... ... INI] -> [... ... ... ... INI EXP] */
/*                                                                        */
/*     expr-stack: [... DCT TAB EXP NBR VAL] -> [... ... ... ... ... TAB] */
/*     cont-stack: [... ... ... ... ... INI] -> [... ... ... ... ... ...] */
/*                                                                        */
/*     expr-stack: [... DCT MAT EXP NBR VAL] -> [... ... ... ... ... MAT] */
/*     cont-stack: [... ... ... ... ... INI] -> [... ... ... ... ... ...] */
/*------------------------------------------------------------------------*/
static _NIL_TYPE_ INI(_NIL_TYPE_) {
	_EXP_TYPE_ dct, exp, nbr, tab, val;
	_UNS_TYPE_ ctr, siz;
	_TAG_TYPE_ tag;
	_stk_claim_();
	_stk_pop_EXP_(val);
	_stk_pop_EXP_(nbr);
	_stk_pop_EXP_(exp);
	_stk_peek_EXP_(tab);
	tag = _ag_get_TAG_(tab);
	ctr = _ag_get_NBU_(nbr); //hoever we in het opvullen zijn
	if (tag == _TAB_TAG_) {
		siz = _ag_get_TAB_SIZ_(tab);
		_ag_set_TAB_EXP_(tab, ctr, val);
	} else if (tag == _MAT_TAG_) {
		_EXP_TYPE_ dim_size;
		dim_size = _ag_get_DIM_SIZE_(tab);
		siz = _ag_get_MAT_TOT_SIZE(tab);
		siz = siz - _ag_get_NBU_(dim_size);
		siz = siz - _DIM_SIZE_SIZE_;
		_ag_set_MAT_val_(tab, _ag_get_NBU_(dim_size), val, ctr);
	} else {
		_error_(_TAG_ERROR_);
	}
	if (ctr < siz) {
		nbr = _ag_succ_NBR_(nbr);
		_stk_push_EXP_(exp);
		_stk_push_EXP_(nbr);
		_stk_push_EXP_(exp);
		_stk_push_CNT_(EXP);
	} else {
		_stk_zap_EXP_();
		_stk_peek_EXP_(dct);
		_ag_set_DCT_VAL_(dct, tab);
		_ag_set_DCT_DCT_(dct, _DCT_);
		_DCT_ = dct;
		_stk_poke_EXP_(tab);
		_stk_zap_CNT_();
	}
}

/*------------------------------------------------------------------------*/
/*  REF                                                                   */
/*     expr-stack: [... ... ... ... TAB NBR] -> [... ... ... ... ... VAL] */
/*     cont-stack: [... ... ... ... ... REF] -> [... ... ... ... ... ...] */
/*------------------------------------------------------------------------*/
static _NIL_TYPE_ REF(_NIL_TYPE_) {
	_EXP_TYPE_ exp, nbr, tab;
	_UNS_TYPE_ ctr, siz;
	_TAG_TYPE_ tag;
	_stk_pop_EXP_(nbr);
	_stk_peek_EXP_(tab);
	tag = _ag_get_TAG_(tab);
	if (TAB_tab[tag]) {
		siz = _ag_get_TAB_SIZ_(tab);
		tag = _ag_get_TAG_(nbr);
		if (tag == _NBR_TAG_) {
			ctr = _ag_get_NBU_(nbr);
			if ((ctr > 0) && (ctr <= siz)) {
				exp = _ag_get_TAB_EXP_(tab, ctr);
				_TAG_TYPE_ tag =  _ag_get_TAG_(exp);
				_stk_poke_EXP_(exp);
				_stk_zap_CNT_();
			} else
				_error_(_RNG_ERROR_);
		} else
			_error_(_IIX_ERROR_);
	} else {
		_error_(_NAT_ERROR_);
	}
}

/*------------------------------------------------------------------------*/
/*  RET                                                                   */
/*     expr-stack: [... ... ... ... DCT VAL] -> [... ... ... ... ... VAL] */
/*     cont-stack: [... ... ... ... ... RET] -> [... ... ... ... ... ...] */
/*------------------------------------------------------------------------*/
static _NIL_TYPE_ RET(_NIL_TYPE_) {
	_EXP_TYPE_ val;
	_stk_pop_EXP_(val);
	_stk_peek_EXP_(_DCT_);
	_stk_poke_EXP_(val);
	_stk_zap_CNT_();
	_ESCAPE_;
}

/*------------------------------------------------------------------------*/
/*  RPL (executes a set to a tab or mat)						          */
/*     expr-stack: [... ... ... TAB VAL NBR] -> [... ... ... ... ... TAB] */
/*     cont-stack: [... ... ... ... ... RPL] -> [... ... ... ... ... ...] */
/*																		  */
/*     expr-stack: [... ... ... MAT VAL NBR] -> [... ... ... ... ... MAT] */
/*     cont-stack: [... ... ... ... ... RPL] -> [... ... ... ... ... ...] */
/*------------------------------------------------------------------------*/
static _NIL_TYPE_ RPL(_NIL_TYPE_) {
	_EXP_TYPE_ nbr, tab, val;
	_UNS_TYPE_ ctr, siz;
	_TAG_TYPE_ tag;
	_stk_pop_EXP_(nbr);
	_stk_pop_EXP_(val);
	_stk_peek_EXP_(tab);
	tag = _ag_get_TAG_(tab);
	if (TAB_tab[tag]) {
		siz = _ag_get_TAB_SIZ_(tab);
		tag = _ag_get_TAG_(nbr);
		if (tag == _NBR_TAG_) {
			ctr = _ag_get_NBU_(nbr);
			if ((ctr > 0) && (ctr <= siz)) {
				_ag_set_TAB_EXP_(tab, ctr, val);
				_stk_zap_CNT_();
			} else
				_error_(_RNG_ERROR_);
		} else
			_error_(_IIX_ERROR_);
	} else
		_error_(_NAT_ERROR_);
}

/*------------------------------------------------------------------------*/
/*  SET                                                                   */
/*     expr-stack: [... ... ... ... ... SET] -> [... ... ... ... DCT EXP] */
/*     cont-stack: [... ... ... ... ... SET] -> [... ... ... ... CHG EXP] */
/*                                                                        */
/*     expr-stack: [... ... ... ... ... SET] -> [... ... ... ... ... FUN] */
/*     cont-stack: [... ... ... ... ... SET] -> [... ... ... ... ... ...] */
/*                                                            1       2   */
/*     expr-stack: [... ... ... ... ... SET] -> [... ... ... TAB NBR EXP] */
/*     cont-stack: [... ... ... ... ... SET] -> [... ... ... RPL SWP EXP] */
/*													  2			     	  */
/*     expr-stack: [... ... ... ... ... SET] -> [... EXP MAT *1* *1* EXP] */
/*     cont-stack: [... ... ... ... ... SET] -> [... ... RPL SWI MIX EXP] */
/*																     	  */
/*																		  */
/*  1 Could also be a matrice because TAB and MAT both use TBL            */
/*  2 Is the exp to set on the new position                               */
/*  3 Dimension tab where we need to execute the assignment               */
/*------------------------------------------------------------------------*/
static _NIL_TYPE_ SET(_NIL_TYPE_) {
	_EXP_TYPE_ arg, dct, exp, fun, idx, inv, nam, set, tab;
	_TAG_TYPE_ tag;
	_stk_claim_();
	_mem_claim_();
	_stk_peek_EXP_(set);
	inv = _ag_get_SET_INV_(set);
	exp = _ag_get_SET_EXP_(set);
	tag = _ag_get_TAG_(inv);
	switch (tag) {
	case _VAR_TAG_:
		nam = _ag_get_VAR_NAM_(inv);
		_dct_locate_(nam, dct, _DCT_);
		_stk_poke_EXP_(dct);
		_stk_push_EXP_(exp);
		_stk_poke_CNT_(CHG);
		_stk_push_CNT_(EXP);
		break;
	case _APL_TAG_:
		fun = _ag_make_FUN_();
		inv = _ag_get_SET_INV_(set);
		nam = _ag_get_APL_NAM_(inv);
		arg = _ag_get_APL_ARG_(inv);
		_dct_locate_(nam, dct, _DCT_);
		_ag_set_DCT_VAL_(dct, fun);
		_ag_set_FUN_NAM_(fun, nam);
		_ag_set_FUN_ARG_(fun, arg);
		_ag_set_FUN_EXP_(fun, exp);
		_ag_set_FUN_DCT_(fun, _DCT_);
		_stk_poke_EXP_(fun);
		_stk_zap_CNT_();
		break;
	case _TBL_TAG_:
		nam = _ag_get_TBL_NAM_(inv);
		idx = _ag_get_TBL_IDX_(inv);
		_dct_locate_(nam, dct, _DCT_);
		tab = _ag_get_DCT_VAL_(dct); //could be a matrice too
		tag = _ag_get_TAG_(tab);
		if (tag == _MAT_TAG_) {
			_stk_poke_EXP_(exp);
			_stk_push_EXP_(tab);
			_stk_push_EXP_(idx);
			_stk_push_EXP_(_ZERO_);
			_stk_push_EXP_(_ONE_);
			_stk_push_EXP_(_ag_get_TAB_EXP_(idx, 1));
			_stk_poke_CNT_(RPL);
			_stk_push_CNT_(SWP);
			_stk_push_CNT_(SWI);
			_stk_push_CNT_(MIX);
			_stk_push_CNT_(EXP);
		} else if (tag == _TAB_TAG_) {
			_stk_poke_EXP_(tab);
			if (_ag_get_TAB_SIZ_(idx) != 1) {
				_error_(_RNG_ERROR_);
				return;
			} else {
				idx = _ag_get_TAB_EXP_(idx, 1);
				_stk_push_EXP_(idx);
				_stk_push_EXP_(exp);
				_stk_poke_CNT_(RPL);
				_stk_push_CNT_(SWP);
				// swaps the value and the index, also pushes exp to evaluate the index
				_stk_push_CNT_(EXP);
			}
		} else {
			_error_(_TAG_ERROR_);
		}
		break;
	default:
		_error_(_AGR_ERROR_);
	}
}

/*------------------------------------------------------------------------*/
/*  MIX (calculates the index in a matrice)						          */
/*     expr-stack: [... MAT TAB SIZ CTR VAL] -> [... MAT TAB SIZ CTR EXP] */
/*     cont-stack: [... ... ... ... ... MIX] -> [... ... ... ... MIX EXP] */
/*																	  1	  */
/*     expr-stack: [... MAT TAB SIZ CTR VAL] -> [... ... ... ... MAT IDX] */
/*     cont-stack: [... ... ... ... ... MIX] -> [... ... ... ... ... ...] */
/*																		  */
/*------------------------------------------------------------------------*/
static _NIL_TYPE_ MIX(_NIL_TYPE_) {
	_EXP_TYPE_ mat, siz, ctr, val, dim, dim_check, dim_siz, tab;
	_UNS_TYPE_ dim_siz_int, ctr_int, siz_int, cur_idx, dim_int;
	_stk_claim_();
	_stk_pop_EXP_(val);
	_stk_pop_EXP_(ctr);
	_stk_pop_EXP_(siz);
	_stk_pop_EXP_(tab);
	_stk_peek_EXP_(mat);
	siz_int = _ag_get_NBU_(siz);
	dim_siz = _ag_get_DIM_SIZE_(mat);
	dim_siz_int = _ag_get_NBU_(dim_siz);
	ctr_int = _ag_get_NBU_(ctr);
	dim = _ag_get_MAT_DIM_(mat, dim_siz_int - (ctr_int - 1));
	dim_check = _ag_get_MAT_DIM_(mat, ctr_int);
	if (dim_siz_int > ctr_int) {
		cur_idx = _ag_get_NBU_(val);
		dim_int = _ag_get_NBU_(dim);
		if ((cur_idx > 0) && (cur_idx <= _ag_get_NBU_(dim_check))) {
			siz_int += ((cur_idx - 1) * _ag_get_NBU_(dim));
			siz = _ag_make_NBU_(siz_int);
			ctr = _ag_succ_NBR_(ctr);
			val = _ag_get_TAB_EXP_(tab, _ag_get_NBU_(ctr));
			_stk_push_EXP_(tab);
			_stk_push_EXP_(siz);
			_stk_push_EXP_(ctr);
			_stk_push_EXP_(val);
			_stk_push_CNT_(EXP);
		} else {
			_error_(_IIX_ERROR_);
		}
	} else {
		_UNS_TYPE_ cur_idx;
		cur_idx = _ag_get_NBU_(val);
		if ((cur_idx > 0) && (cur_idx <= _ag_get_NBU_(dim_check))) {
			siz_int += cur_idx;
			siz_int += dim_siz_int;
			siz_int += _DIM_SIZE_SIZE_;
			siz = _ag_make_NBU_(siz_int);
			_stk_poke_EXP_(mat);
			_stk_push_EXP_(siz);
			_stk_zap_CNT_();
		} else {
			_UNS_TYPE_ tst = _ag_get_NBU_(dim);
			_error_(_IIX_ERROR_);
		}
	}
}

/*------------------------------------------------------------------------*/
/*  SWI                                                                   */
/*     expr-stack: [... ... ... EXP MAT IDX] -> [... ... ... MAT IDX EXP] */
/*     cont-stack: [... ... ... ... ... SWI] -> [... ... ... ... ... EXP] */
/*                                                                        */
/*------------------------------------------------------------------------*/
static _NIL_TYPE_ SWI(_NIL_TYPE_) {
	_EXP_TYPE_ idx, mat, exp;

	_stk_pop_EXP_(idx);
	_stk_pop_EXP_(mat);
	_stk_pop_EXP_(exp);
	_stk_push_EXP_(mat);
	_stk_push_EXP_(idx);
	_stk_push_EXP_(exp);
	_stk_poke_CNT_(EXP);
}

/*------------------------------------------------------------------------*/
/*  SLF                                                                   */
/*     expr-stack: [... ... ... ... ... VOI] -> [... ... ... ... ... VOI] */
/*     cont-stack: [... ... ... ... ... SLF] -> [... ... ... ... ... ...] */
/*                                                                        */
/*     expr-stack: [... ... ... ... ... NAT] -> [... ... ... ... ... NAT] */
/*     cont-stack: [... ... ... ... ... SLF] -> [... ... ... ... ... ...] */
/*                                                                        */
/*     expr-stack: [... ... ... ... ... FRC] -> [... ... ... ... ... FRC] */
/*     cont-stack: [... ... ... ... ... SLF] -> [... ... ... ... ... ...] */
/*                                                                        */
/*     expr-stack: [... ... ... ... ... TXT] -> [... ... ... ... ... TXT] */
/*     cont-stack: [... ... ... ... ... SLF] -> [... ... ... ... ... ...] */
/*                                                                        */
/*     expr-stack: [... ... ... ... ... TAB] -> [... ... ... ... ... TAB] */
/*     cont-stack: [... ... ... ... ... SLF] -> [... ... ... ... ... ...] */
/*                                                                        */
/*     expr-stack: [... ... ... ... ... FUN] -> [... ... ... ... ... FUN] */
/*     cont-stack: [... ... ... ... ... SLF] -> [... ... ... ... ... ...] */
/*                                                                        */
/*     expr-stack: [... ... ... ... ... DCT] -> [... ... ... ... ... DCT] */
/*     cont-stack: [... ... ... ... ... SLF] -> [... ... ... ... ... ...] */
/*                                                                        */
/*     expr-stack: [... ... ... ... ... ENV] -> [... ... ... ... ... ENV] */
/*     cont-stack: [... ... ... ... ... SLF] -> [... ... ... ... ... ...] */
/*                                                                        */
/*     expr-stack: [... ... ... ... ... NBR] -> [... ... ... ... ... NBR] */
/*     cont-stack: [... ... ... ... ... SLF] -> [... ... ... ... ... ...] */
/*                                                                        */
/*------------------------------------------------------------------------*/
static _NIL_TYPE_ SLF(_NIL_TYPE_) {
	_stk_zap_CNT_();
}

/*------------------------------------------------------------------------*/
/*  SWP                                                                   */
/*     expr-stack: [... ... ... TAB EXP VAL] -> [... ... ... TAB VAL EXP] */
/*     cont-stack: [... ... ... ... ... SWP] -> [... ... ... ... ... EXP] */
/*------------------------------------------------------------------------*/
static _NIL_TYPE_ SWP(_NIL_TYPE_) {
	_EXP_TYPE_ exp, val;
	_stk_pop_EXP_(val);
	_stk_peek_EXP_(exp);
	_stk_poke_EXP_(val);
	_stk_push_EXP_(exp);
	_stk_poke_CNT_(EXP);
}

/*------------------------------------------------------------------------*/
/*  TBL                                                                   */
/*     expr-stack: [... ... ... ... ... TBL] -> [... ... ... ... TAB EXP] */
/*     cont-stack: [... ... ... ... ... TBL] -> [... ... ... ... REF EXP] */
/*------------------------------------------------------------------------*/
static _NIL_TYPE_ TBL(_NIL_TYPE_) {
	_EXP_TYPE_ dct, idx, nam, tab, tbl;
	_TAG_TYPE_ tag;
	_stk_claim_();
	_stk_peek_EXP_(tbl);
	nam = _ag_get_TBL_NAM_(tbl);
	idx = _ag_get_TBL_IDX_(tbl);
	_dct_locate_(nam, dct, _DCT_);
	tab = _ag_get_DCT_VAL_(dct);
	tag = _ag_get_TAG_(tab);
	if (tag == _MAT_TAG_
			&& (_ag_get_NBU_(_ag_get_DIM_SIZE_(tab)) == _ag_get_TAB_SIZ_(idx))) {
		_stk_poke_EXP_(tab);
		_stk_push_EXP_(idx);
		_stk_push_EXP_(_ZERO_);
		_stk_push_EXP_(_ONE_);
		_stk_push_EXP_(_ag_get_TAB_EXP_(idx, 1));
		_stk_poke_CNT_(REF);
		_stk_push_CNT_(MIX);
		_stk_push_CNT_(EXP);
	} else if (tag == _TAB_TAG_ && (_ag_get_TAB_SIZ_(idx) == 1)) {
		idx = _ag_get_TAB_EXP_(idx, 1);
		_stk_poke_EXP_(tab);
		_stk_push_EXP_(idx);
		_stk_poke_CNT_(REF);
		_stk_push_CNT_(EXP);
	} else {
		_error_(_IIX_ERROR_);
	}
}

/*------------------------------------------------------------------------*/
/*  VAR                                                                   */
/*     expr-stack: [... ... ... ... ... VAR] -> [... ... ... ... ... VAL] */
/*     cont-stack: [... ... ... ... ... VAR] -> [... ... ... ... ... ...] */
/*------------------------------------------------------------------------*/
static _NIL_TYPE_ VAR(_NIL_TYPE_) {
	_EXP_TYPE_ dct, nam, val, var;
	_stk_peek_EXP_(var);
	nam = _ag_get_VAR_NAM_(var);
	_dct_locate_(nam, dct, _DCT_);
	val = _ag_get_DCT_VAL_(dct);
	_stk_poke_EXP_(val);
	_stk_zap_CNT_();
}

/* public functions */

/*------------------------------------------------------------------------*/
/*  CAL                                                                   */
/*     expr-stack: [... ... ... ... FUN ARG] -> [... ... ... ... DCT EXP] */
/*     cont-stack: [... ... ... ... ... CAL] -> [... ... ... ... RET EXP] */
/*                                                                        */
/*     expr-stack: [... ... ... ... FUN ARG] -> [... ... ... ... ... EXP] */
/*     cont-stack: [... ... ... ... ... CAL] -> [... ... ... ... ... EXP] */
/*                                                                        */
/*     expr-stack: [... ... ... ... FUN ARG] -> [EXP DCT ARG TAB *1* EXP] */
/*     cont-stack: [... ... ... ... ... CAL] -> [... ... ... ... ATV EXP] */
/*                                                                        */
/*     expr-stack: [... ... ... ... FUN ARG] -> [EXP DCT ARG TAB *1* APL] */
/*     cont-stack: [... ... ... ... ... CAL] -> [... ... ... ... ... ATA] */
/*                                                                        */
/*     expr-stack: [... ... ... ... FUN ARG] -> [EXP PAR ARG *1* DCT EXP] */
/*     cont-stack: [... ... ... ... ... CAL] -> [... ... ... ... BND EXP] */
/*                                                                        */
/*     expr-stack: [... ... ... ... FUN ARG] -> [EXP PAR ARG *1* DCT FUN] */
/*     cont-stack: [... ... ... ... ... CAL] -> [... ... ... ... ... BND] */
/*------------------------------------------------------------------------*/
_NIL_TYPE_ _eval_CAL_(_NIL_TYPE_) {
	_EXP_TYPE_ act, arg, dct, exp, frm, fun, nam, par, tab, xdc, xfu;
	_CNT_TYPE_ cnt;
	_TAG_TYPE_ tag, xtg;
	_UNS_TYPE_ siz, xsz;
	_stk_claim_();
	_stk_peek_EXP_(arg);
	tag = _ag_get_TAG_(arg);
	if (tag != _TAB_TAG_) {
		_stk_zap_EXP_();
		_stk_peek_EXP_(fun);
		_error_msg_(_NAT_ERROR_, _ag_get_FUN_NAM_(fun));
	}
	siz = _ag_get_TAB_SIZ_(arg);
	_mem_claim_SIZ_(siz);
	_stk_pop_EXP_(arg);
	if (siz == 0) {
		_stk_peek_EXP_(fun);
		par = _ag_get_FUN_ARG_(fun);
		tag = _ag_get_TAG_(par);
		switch (tag) {
		case _VAR_TAG_:
		case _APL_TAG_:
			dct = _ag_make_DCT_();
			par = _ag_get_FUN_ARG_(fun);
			nam = _ag_get_VAR_NAM_(par);
			_ag_set_DCT_NAM_(dct, nam);
			_ag_set_DCT_VAL_(dct, _EMPTY_);
			xdc = _ag_get_FUN_DCT_(fun);
			_ag_set_DCT_DCT_(dct, xdc);
			break;
		case _TAB_TAG_:
			xsz = _ag_get_TAB_SIZ_(par);
			if (xsz == 0)
				dct = _ag_get_FUN_DCT_(fun);
			else
				_error_msg_(_NMA_ERROR_, _ag_get_FUN_NAM_(fun));
			break;
		default:
			_error_msg_(_IPM_ERROR_, _ag_get_FUN_NAM_(fun));
		}
		exp = _ag_get_FUN_EXP_(fun);
		_stk_zap_CNT_();
		_stk_peek_CNT_(cnt);
		if (cnt != RET) {
			_stk_poke_EXP_(_DCT_);
			_stk_push_EXP_(exp);
			_stk_push_CNT_(RET);
		} else
			_stk_poke_EXP_(exp);
		_stk_push_CNT_(EXP);
		_DCT_ = dct;
	} else {
		dct = _ag_make_DCT_();
		_stk_peek_EXP_(fun);
		par = _ag_get_FUN_ARG_(fun);
		exp = _ag_get_FUN_EXP_(fun);
		xdc = _ag_get_FUN_DCT_(fun);
		tag = _ag_get_TAG_(par);
		switch (tag) {
		case _VAR_TAG_:
			nam = _ag_get_VAR_NAM_(par);
			_ag_set_DCT_NAM_(dct, nam);
			_ag_set_DCT_DCT_(dct, xdc);
			_stk_poke_EXP_(exp);
			_stk_push_EXP_(dct);
			_stk_push_EXP_(arg);
			tab = _ag_make_TAB_(siz);
			act = _ag_get_TAB_EXP_(arg, 1);
			_stk_push_EXP_(tab);
			_stk_push_EXP_(_ONE_);
			_stk_push_EXP_(act);
			_stk_poke_CNT_(ATV);
			_stk_push_CNT_(EXP);
			break;
		case _APL_TAG_:
			nam = _ag_get_APL_NAM_(par);
			_ag_set_DCT_NAM_(dct, nam);
			_ag_set_DCT_DCT_(dct, xdc);
			_stk_poke_EXP_(exp);
			_stk_push_EXP_(dct);
			_stk_push_EXP_(arg);
			tab = _ag_make_TAB_(siz);
			_stk_push_EXP_(tab);
			_stk_push_EXP_(_ONE_);
			_stk_push_EXP_(par);
			_stk_poke_CNT_(ATA);
			break;
		case _TAB_TAG_:
			xsz = _ag_get_TAB_SIZ_(par);
			if (siz != xsz)
				_error_msg_(_NMA_ERROR_, _ag_get_FUN_NAM_(fun));
			frm = _ag_get_TAB_EXP_(par, 1);
			xtg = _ag_get_TAG_(frm);
			switch (xtg) {
			case _VAR_TAG_:
				act = _ag_get_TAB_EXP_(arg, 1);
				_stk_poke_EXP_(exp);
				_stk_push_EXP_(par);
				_stk_push_EXP_(arg);
				_stk_push_EXP_(_ONE_);
				nam = _ag_get_VAR_NAM_(frm);
				_ag_set_DCT_NAM_(dct, nam);
				_ag_set_DCT_DCT_(dct, xdc);
				_stk_push_EXP_(dct);
				_stk_push_EXP_(act);
				_stk_poke_CNT_(BND);
				_stk_push_CNT_(EXP);
				break;
			case _APL_TAG_:
				xfu = _ag_make_FUN_();
				par = _ag_get_FUN_ARG_(fun);
				exp = _ag_get_FUN_EXP_(fun);
				xdc = _ag_get_FUN_DCT_(fun);
				act = _ag_get_TAB_EXP_(arg, 1);
				frm = _ag_get_TAB_EXP_(par, 1);
				_stk_poke_EXP_(exp);
				_stk_push_EXP_(par);
				_stk_push_EXP_(arg);
				_stk_push_EXP_(_ONE_);
				nam = _ag_get_APL_NAM_(frm);
				arg = _ag_get_APL_ARG_(frm);
				_ag_set_DCT_NAM_(dct, nam);
				_ag_set_DCT_DCT_(dct, xdc);
				_stk_push_EXP_(dct);
				_ag_set_FUN_NAM_(xfu, nam);
				_ag_set_FUN_ARG_(xfu, arg);
				_ag_set_FUN_EXP_(xfu, act);
				_ag_set_FUN_DCT_(xfu, _DCT_);
				_stk_push_EXP_(xfu);
				_stk_poke_CNT_(BND);
				break;
			default:
				_error_msg_(_IPM_ERROR_, _ag_get_FUN_NAM_(fun));
			}
			break;
		default:
			_error_msg_(_IPM_ERROR_, _ag_get_FUN_NAM_(fun));
		}
	}
}

/*------------------------------------------------------------------------*/
/*  EXP                                                                   */
/*     expr-stack: [... ... ... ... ... EXP] -> [... ... ... ... ... EXP] */
/*     cont-stack: [... ... ... ... ... EXP] -> [... ... ... ... ... ***] */
/*------------------------------------------------------------------------*/
_NIL_TYPE_ _eval_EXP_(_NIL_TYPE_) {
	_EXP_TYPE_ exp;
	_stk_peek_EXP_(exp);
	_CNT_TYPE_ ct = CNT_tab[_ag_get_TAG_(exp)];
	_stk_poke_CNT_(ct);
}
