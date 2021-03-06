#include <sylvan_config.h>

#include <assert.h>
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sylvan.h>
#include <sylvan_common.h>
#include <sylvan_mtbdd_int.h>
#include <sylvan_storm_rational_function.h>

#include <storm_function_wrapper.h>

#undef SYLVAN_STORM_RATIONAL_FUNCTION_DEBUG

#ifdef SYLVAN_STORM_RATIONAL_FUNCTION_DEBUG
int depth = 0;

#define LOG_I(funcName) do { for (int i = 0; i < depth; ++i) { printf(" "); } ++depth; printf("Entering function " funcName "\n"); } while (0 != 0);
#define LOG_O(funcName) do { --depth; for (int i = 0; i < depth; ++i) { printf(" "); } printf("Leaving function " funcName "\n"); } while (0 != 0);
#else
#define LOG_I(funcName)
#define LOG_O(funcName)
#endif

/**
 * helper function for hash
 */
#ifndef rotl64
//static inline uint64_t
//rotl64(uint64_t x, int8_t r)
//{
//    return ((x<<r) | (x>>(64-r)));
//}
#endif

static uint64_t
sylvan_storm_rational_function_hash(const uint64_t v, const uint64_t seed)
{
	LOG_I("i-hash")
    /* Hash the storm::RationalFunction in pointer v */
    
	storm_rational_function_ptr x = (storm_rational_function_ptr)v;

	uint64_t hash = storm_rational_function_hash(x, seed);

#ifdef SYLVAN_STORM_RATIONAL_FUNCTION_DEBUG
	printf("Hashing ptr %p with contents ", x);
	print_storm_rational_function(x);
	printf(" with seed %zu, hash = %zu\n", seed, hash);
#endif

	LOG_O("i-hash")
	return hash;
}

static int
sylvan_storm_rational_function_equals(const uint64_t left, const uint64_t right)
{
	LOG_I("i-equals")
    /* This function is called by the unique table when comparing a new
       leaf with an existing leaf */
	storm_rational_function_ptr a = (storm_rational_function_ptr)(size_t)left;
	storm_rational_function_ptr b = (storm_rational_function_ptr)(size_t)right;

    /* Just compare x and y */
	int result = storm_rational_function_equals(a, b);
	
	LOG_O("i-equals")
    return result;
}

static void
sylvan_storm_rational_function_create(uint64_t *val)
{
	LOG_I("i-create")
	
#ifdef SYLVAN_STORM_RATIONAL_FUNCTION_DEBUG
	void* tmp = (void*)*val;
	printf("sylvan_storm_rational_function_create(ptr = %p, value = ", tmp);
	print_storm_rational_function(*((storm_rational_function_ptr*)(size_t)val));
	printf(")\n");
#endif
	
    /* This function is called by the unique table when a leaf does not yet exist.
       We make a copy, which will be stored in the hash table. */
	storm_rational_function_ptr* x = (storm_rational_function_ptr*)(size_t)val;
	storm_rational_function_init(x);
	
#ifdef SYLVAN_STORM_RATIONAL_FUNCTION_DEBUG
	tmp = (void*)*val;
	printf("sylvan_storm_rational_function_create_2(ptr = %p)\n", tmp);
#endif
	
	LOG_O("i-create")
}

static void
sylvan_storm_rational_function_destroy(uint64_t val)
{
	LOG_I("i-destroy")
    /* This function is called by the unique table
       when a leaf is removed during garbage collection. */
	storm_rational_function_ptr x = (storm_rational_function_ptr)(size_t)val;
	storm_rational_function_destroy(x);
	LOG_O("i-destroy")
}

static uint32_t sylvan_storm_rational_function_type;
static uint64_t CACHE_STORM_RATIONAL_FUNCTION_AND_EXISTS;

/**
 * Initialize storm::RationalFunction custom leaves
 */
void
sylvan_storm_rational_function_init()
{
    /* Register custom leaf 3 */
    sylvan_storm_rational_function_type = mtbdd_register_custom_leaf(sylvan_storm_rational_function_hash, sylvan_storm_rational_function_equals, sylvan_storm_rational_function_create, sylvan_storm_rational_function_destroy);
	
	if (SYLVAN_STORM_RATIONAL_FUNCTION_TYPE_ID != sylvan_storm_rational_function_type) {
		printf("ERROR - ERROR - ERROR\nThe Sylvan Type ID is NOT correct.\nIt was assumed to be %u, but it is actually %u!\nYou NEED to fix this by changing the macro \"SYLVAN_STORM_RATIONAL_FUNCTION_TYPE_ID\" and recompiling StoRM!\n\n", SYLVAN_STORM_RATIONAL_FUNCTION_TYPE_ID, sylvan_storm_rational_function_type);
		assert(0);
	}	
	
	CACHE_STORM_RATIONAL_FUNCTION_AND_EXISTS = cache_next_opid();
}

uint32_t sylvan_storm_rational_function_get_type() {
	return sylvan_storm_rational_function_type;
}

/**
 * Create storm::RationalFunction leaf
 */
MTBDD
mtbdd_storm_rational_function(storm_rational_function_ptr val)
{
	LOG_I("i-mtbdd_")
	uint64_t terminalValue = (uint64_t)val;
	
#ifdef SYLVAN_STORM_RATIONAL_FUNCTION_DEBUG
	printf("mtbdd_storm_rational_function(ptr = %p, value = ", val);
	print_storm_rational_function(val);
	printf(")\n");
#endif
	
	MTBDD result = mtbdd_makeleaf(sylvan_storm_rational_function_type, terminalValue);
	
	LOG_O("i-mtbdd_")
	return result;
}

/**
 * Converts a BDD to a MTBDD with storm::RationalFunction leaves
 */
TASK_IMPL_2(MTBDD, mtbdd_op_bool_to_storm_rational_function, MTBDD, a, size_t, v)
{
	LOG_I("task_impl_2 to_srf")
	if (a == mtbdd_false) {
		MTBDD result = mtbdd_storm_rational_function(storm_rational_function_get_zero());
		LOG_O("task_impl_2 to_srf - ZERO")
		return result;
	}
	if (a == mtbdd_true) {
		MTBDD result = mtbdd_storm_rational_function(storm_rational_function_get_one());
		LOG_O("task_impl_2 to_srf - ONE")
		return result;
	}
    
    // Ugly hack to get rid of the error "unused variable v" (because there is no version of uapply without a parameter).
    (void)v;
	
    LOG_O("task_impl_2 to_srf - INVALID")
    return mtbdd_invalid;
}

TASK_IMPL_1(MTBDD, mtbdd_bool_to_storm_rational_function, MTBDD, dd)
{
    return mtbdd_uapply(dd, TASK(mtbdd_op_bool_to_storm_rational_function), 0);
}

/**
 * Operation "plus" for two storm::RationalFunction MTBDDs
 * Interpret partial function as "0"
 */
TASK_IMPL_2(MTBDD, sylvan_storm_rational_function_op_plus, MTBDD*, pa, MTBDD*, pb)
{
	LOG_I("task_impl_2 op_plus")
    MTBDD a = *pa, b = *pb;

    /* Check for partial functions */
    if (a == mtbdd_false) return b;
    if (b == mtbdd_false) return a;

    /* If both leaves, compute plus */
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
		storm_rational_function_ptr ma = (storm_rational_function_ptr)mtbdd_getvalue(a);
		storm_rational_function_ptr mb = (storm_rational_function_ptr)mtbdd_getvalue(b);

		storm_rational_function_ptr mres = storm_rational_function_plus(ma, mb);
        MTBDD res = mtbdd_storm_rational_function(mres);
        
		storm_rational_function_destroy(mres);

        return res;
    }

    /* Commutative, so swap a,b for better cache performance */
    if (a < b) {
        *pa = b;
        *pb = a;
    }

    return mtbdd_invalid;
}

/**
 * Operation "minus" for two storm::RationalFunction MTBDDs
 * Interpret partial function as "0"
 */
TASK_IMPL_2(MTBDD, sylvan_storm_rational_function_op_minus, MTBDD*, pa, MTBDD*, pb)
{
    LOG_I("task_impl_2 op_minus")
	MTBDD a = *pa, b = *pb;

    /* Check for partial functions */
    if (a == mtbdd_false) return sylvan_storm_rational_function_neg(b);
    if (b == mtbdd_false) return a;

    /* If both leaves, compute plus */
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
		storm_rational_function_ptr ma = (storm_rational_function_ptr)mtbdd_getvalue(a);
		storm_rational_function_ptr mb = (storm_rational_function_ptr)mtbdd_getvalue(b);

		storm_rational_function_ptr mres = storm_rational_function_minus(ma, mb);
		MTBDD res = mtbdd_storm_rational_function(mres);

		storm_rational_function_destroy(mres);

        return res;
    }

    return mtbdd_invalid;
}

/**
 * Operation "times" for two storm::RationalFunction MTBDDs.
 * One of the parameters can be a BDD, then it is interpreted as a filter.
 * For partial functions, domain is intersection
 */
TASK_IMPL_2(MTBDD, sylvan_storm_rational_function_op_times, MTBDD*, pa, MTBDD*, pb)
{
	LOG_I("task_impl_2 op_times")
    MTBDD a = *pa, b = *pb;

    /* Check for partial functions and for Boolean (filter) */
    if (a == mtbdd_false || b == mtbdd_false) return mtbdd_false;

    /* If one of Boolean, interpret as filter */
    if (a == mtbdd_true) return b;
    if (b == mtbdd_true) return a;

    /* Handle multiplication of leaves */
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
		storm_rational_function_ptr ma = (storm_rational_function_ptr)mtbdd_getvalue(a);
		storm_rational_function_ptr mb = (storm_rational_function_ptr)mtbdd_getvalue(b);

		storm_rational_function_ptr mres = storm_rational_function_times(ma, mb);		
		MTBDD res = mtbdd_storm_rational_function(mres);

		storm_rational_function_destroy(mres);

        return res;
    }

    /* Commutative, so make "a" the lowest for better cache performance */
    if (a < b) {
        *pa = b;
        *pb = a;
    }

    return mtbdd_invalid;
}

/**
 * Operation "divide" for two storm::RationalFunction MTBDDs.
 * For partial functions, domain is intersection
 */
TASK_IMPL_2(MTBDD, sylvan_storm_rational_function_op_divide, MTBDD*, pa, MTBDD*, pb)
{
	LOG_I("task_impl_2 op_divide")
    MTBDD a = *pa, b = *pb;

    /* Check for partial functions */
    if (a == mtbdd_false || b == mtbdd_false) return mtbdd_false;

    /* Handle division of leaves */
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
		storm_rational_function_ptr ma = (storm_rational_function_ptr)mtbdd_getvalue(a);
		storm_rational_function_ptr mb = (storm_rational_function_ptr)mtbdd_getvalue(b);

		storm_rational_function_ptr mres = storm_rational_function_divide(ma, mb);
		MTBDD res = mtbdd_storm_rational_function(mres);

		storm_rational_function_destroy(mres);

        return res;
    }

    return mtbdd_invalid;
}

/**
 * The abstraction operators are called in either of two ways:
 * - with k=0, then just calculate "a op b"
 * - with k<>0, then just calculate "a := a op a", k times
 */

TASK_IMPL_3(MTBDD, sylvan_storm_rational_function_abstract_op_plus, MTBDD, a, MTBDD, b, int, k)
{
	LOG_I("task_impl_3 abstract_op_plus")
    if (k==0) {
        return mtbdd_apply(a, b, TASK(sylvan_storm_rational_function_op_plus));
    } else {
        MTBDD res = a;
        for (int i=0; i<k; i++) {
            mtbdd_refs_push(res);
            res = mtbdd_apply(res, res, TASK(sylvan_storm_rational_function_op_plus));
            mtbdd_refs_pop(1);
        }
        return res;
    }
}

TASK_IMPL_3(MTBDD, sylvan_storm_rational_function_abstract_op_times, MTBDD, a, MTBDD, b, int, k)
{
	LOG_I("task_impl_3 abstract_op_times")
    if (k==0) {
        return mtbdd_apply(a, b, TASK(sylvan_storm_rational_function_op_times));
    } else {
        MTBDD res = a;
        for (int i=0; i<k; i++) {
            mtbdd_refs_push(res);
            res = mtbdd_apply(res, res, TASK(sylvan_storm_rational_function_op_times));
            mtbdd_refs_pop(1);
        }
        return res;
    }
}

/**
 * Operation "neg" for one storm::RationalFunction MTBDD
 */
TASK_IMPL_2(MTBDD, sylvan_storm_rational_function_op_neg, MTBDD, dd, size_t, p)
{
	LOG_I("task_impl_2 op_neg")
    /* Handle partial functions */
    if (dd == mtbdd_false) return mtbdd_false;

    /* Compute result for leaf */
    if (mtbdd_isleaf(dd)) {
		storm_rational_function_ptr mdd = (storm_rational_function_ptr)mtbdd_getvalue(dd);

		storm_rational_function_ptr mres = storm_rational_function_negate(mdd);
		MTBDD res = mtbdd_storm_rational_function(mres);

		storm_rational_function_destroy(mres);

        return res;
    }

    return mtbdd_invalid;
    (void)p;
}

/**
 * Operation "replace leaves" for one storm::RationalFunction MTBDD
 */
TASK_IMPL_2(MTBDD, sylvan_storm_rational_function_op_replace_leaves, MTBDD, dd, size_t, context)
{
	LOG_I("task_impl_2 op_replace")
    /* Handle partial functions */
    if (dd == mtbdd_false) return mtbdd_false;

    /* Compute result for leaf */
    if (mtbdd_isleaf(dd)) {
		if (mtbdd_gettype(dd) != sylvan_storm_rational_function_type) {
			assert(0);
		}
		
		storm_rational_function_ptr mdd = (storm_rational_function_ptr)mtbdd_getvalue(dd);
		return storm_rational_function_leaf_parameter_replacement(dd, mdd, (void*)context);
    }

    return mtbdd_invalid;
}
/**
 * Operation to double for one storm::RationalFunction MTBDD
 */
TASK_IMPL_2(MTBDD, sylvan_storm_rational_function_op_to_double, MTBDD, dd, size_t, p)
{
	LOG_I("task_impl_2 op_toDouble")
    /* Handle partial functions */
    if (dd == mtbdd_false) return mtbdd_false;

    /* Compute result for leaf */
    if (mtbdd_isleaf(dd)) {
		if (mtbdd_gettype(dd) != sylvan_storm_rational_function_type) {
			printf("Can not convert to double, this has type %u!\n", mtbdd_gettype(dd));
			assert(0);
		}

		storm_rational_function_ptr mdd = (storm_rational_function_ptr)mtbdd_getvalue(dd);
		MTBDD result = mtbdd_double(storm_rational_function_get_constant(mdd));
		return result;
    }

    return mtbdd_invalid;
	(void)p;
}

/**
 * Multiply <a> and <b>, and abstract variables <vars> using summation.
 * This is similar to the "and_exists" operation in BDDs.
 */
TASK_IMPL_3(MTBDD, sylvan_storm_rational_function_and_exists, MTBDD, a, MTBDD, b, MTBDD, v)
{
    /* Check terminal cases */

    /* If v == true, then <vars> is an empty set */
    if (v == mtbdd_true) return mtbdd_apply(a, b, TASK(sylvan_storm_rational_function_op_times));

    /* Try the times operator on a and b */
    MTBDD result = CALL(sylvan_storm_rational_function_op_times, &a, &b);
    if (result != mtbdd_invalid) {
        /* Times operator successful, store reference (for garbage collection) */
        mtbdd_refs_push(result);
        /* ... and perform abstraction */
        result = mtbdd_abstract(result, v, TASK(sylvan_storm_rational_function_abstract_op_plus));
        mtbdd_refs_pop(1);
        /* Note that the operation cache is used in mtbdd_abstract */
        return result;
    }

    /* Maybe perform garbage collection */
    sylvan_gc_test();

    /* Check cache. Note that we do this now, since the times operator might swap a and b (commutative) */
    if (cache_get3(CACHE_STORM_RATIONAL_FUNCTION_AND_EXISTS, a, b, v, &result)) return result;

    /* Now, v is not a constant, and either a or b is not a constant */

    /* Get top variable */
    int la = mtbdd_isleaf(a);
    int lb = mtbdd_isleaf(b);
    mtbddnode_t na = la ? 0 : MTBDD_GETNODE(a);
    mtbddnode_t nb = lb ? 0 : MTBDD_GETNODE(b);
    uint32_t va = la ? 0xffffffff : mtbddnode_getvariable(na);
    uint32_t vb = lb ? 0xffffffff : mtbddnode_getvariable(nb);
    uint32_t var = va < vb ? va : vb;

    mtbddnode_t nv = MTBDD_GETNODE(v);
    uint32_t vv = mtbddnode_getvariable(nv);

    if (vv < var) {
        /* Recursive, then abstract result */
        result = CALL(sylvan_storm_rational_function_and_exists, a, b, node_gethigh(v, nv));
        mtbdd_refs_push(result);
        result = mtbdd_apply(result, result, TASK(sylvan_storm_rational_function_op_plus));
        mtbdd_refs_pop(1);
    } else {
        /* Get cofactors */
        MTBDD alow, ahigh, blow, bhigh;
        alow  = (!la && va == var) ? node_getlow(a, na)  : a;
        ahigh = (!la && va == var) ? node_gethigh(a, na) : a;
        blow  = (!lb && vb == var) ? node_getlow(b, nb)  : b;
        bhigh = (!lb && vb == var) ? node_gethigh(b, nb) : b;

        if (vv == var) {
            /* Recursive, then abstract result */
            mtbdd_refs_spawn(SPAWN(sylvan_storm_rational_function_and_exists, ahigh, bhigh, node_gethigh(v, nv)));
            MTBDD low = mtbdd_refs_push(CALL(sylvan_storm_rational_function_and_exists, alow, blow, node_gethigh(v, nv)));
            MTBDD high = mtbdd_refs_push(mtbdd_refs_sync(SYNC(sylvan_storm_rational_function_and_exists)));
            result = CALL(mtbdd_apply, low, high, TASK(sylvan_storm_rational_function_op_plus));
            mtbdd_refs_pop(2);
        } else /* vv > v */ {
            /* Recursive, then create node */
            mtbdd_refs_spawn(SPAWN(sylvan_storm_rational_function_and_exists, ahigh, bhigh, v));
            MTBDD low = mtbdd_refs_push(CALL(sylvan_storm_rational_function_and_exists, alow, blow, v));
            MTBDD high = mtbdd_refs_sync(SYNC(sylvan_storm_rational_function_and_exists));
            mtbdd_refs_pop(1);
            result = mtbdd_makenode(var, low, high);
        }
    }

    /* Store in cache */
    cache_put3(CACHE_STORM_RATIONAL_FUNCTION_AND_EXISTS, a, b, v, result);
    return result;
}

/**
 * Apply a unary operation <op> to <dd>.
 */
TASK_IMPL_3(MTBDD, mtbdd_uapply_nocache, MTBDD, dd, mtbdd_uapply_op, op, size_t, param)
{
    /* Maybe perform garbage collection */
    sylvan_gc_test();

    /* Check cache */
    MTBDD result;
    //if (cache_get3(CACHE_MTBDD_UAPPLY, dd, (size_t)op, param, &result)) return result;

    /* Check terminal case */
    result = WRAP(op, dd, param);
    if (result != mtbdd_invalid) {
        /* Store in cache */
        //cache_put3(CACHE_MTBDD_UAPPLY, dd, (size_t)op, param, result);
        return result;
    }

    /* Get cofactors */
    mtbddnode_t ndd = MTBDD_GETNODE(dd);
    MTBDD ddlow = node_getlow(dd, ndd);
    MTBDD ddhigh = node_gethigh(dd, ndd);

    /* Recursive */
    mtbdd_refs_spawn(SPAWN(mtbdd_uapply_nocache, ddhigh, op, param));
    MTBDD low = mtbdd_refs_push(CALL(mtbdd_uapply_nocache, ddlow, op, param));
    MTBDD high = mtbdd_refs_sync(SYNC(mtbdd_uapply_nocache));
    mtbdd_refs_pop(1);
    result = mtbdd_makenode(mtbddnode_getvariable(ndd), low, high);

    /* Store in cache */
    //cache_put3(CACHE_MTBDD_UAPPLY, dd, (size_t)op, param, result);
    return result;
}
