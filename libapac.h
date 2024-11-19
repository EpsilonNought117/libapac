#ifndef LIBAPAC_H
#define LIBAPAC_H

/*
    LIBC HEADERS
*/

#ifndef REPORT_ERR
#include <stdio.h>
#define REPORT_ERR(stream, fmt, ...) fprintf(stream, fmt, ##__VA_ARGS__)

/*
    can get rid of this dependency by redifining REPORT_ERR(stream, fmt, ...)
*/

#endif

#ifndef ASSERT
#include <assert.h>
#define ASSERT(x) assert(x)

/*
    can get rid of this dependency if needed by redefining ASSERT(x)
*/

#endif

#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>

/*
    AMD64 AND x64 INTRINSICS HEADERS
*/

#include <intrin.h>

/*
    ERROR HANDLING
*/

typedef enum LIBAPAC_ERRORS
{
    LIBAPAC_OKAY,
    LIBAPAC_OOM,
    LIBAPAC_MATH_ERR,

} libapac_err;

/*
    LIBAPAC DYNAMIC MEMORY ALLOCATION FUNCTION
*/

void *(*malloc_ptr)(size_t init_size);
void *(*realloc_ptr)(void *old_buffer, size_t new_size);
void (*free_ptr)(void *memory);

/// @note for setting memory allocation methods
void set_memory_func_ptrs(void *(*func_ptr1)(size_t init_size), void *(*func_ptr2)(void *old_buffer, size_t new_size), void (*func_ptr3)(void *memory));

/*
    ARBITRARY PRECISION INTEGER DEFINITION
*/

typedef uint64_t u64;

#define APZ_ZPOS 0
#define APZ_NEG 1

#define POW_10_TO_19 1000 * 1000 * 1000 * 1000 * 1000 * 1000 * 10 // 10^19

typedef struct apz_t
{
    /**
     *   stores the arbitrary precision integer in an array of 64 bit unsigned integers
     */
    u64 *num_array;

    /**
     *   seg_alloc -> number of segments allocated currently for the arbitrary precision integer
     */
    u64 seg_alloc;

    /**
     *   seg_in_use -> number of segments in use out of the total allocated
     */
    u64 seg_in_use;

    u64 is_negative; // denotes whether the number is zero/positive or negative

} apz_t;

/**
 *  APZ INIT FUNCTIONS
 */

/// @brief initialize a big integer with positive 64 bit value in Least significant limb
/// @note  the value result->is_negative is set to APZ_ZPOS
libapac_err apz_init_pos64(apz_t *result, size_t init_size_limbs, uint64_t value);

/// @brief initialize a big integer with negative 64 bit value in Least significant limb
/// @note  the value result->is_negative is set to APZ_NEG
libapac_err apz_init_neg64(apz_t *result, size_t init_size_limbs, uint64_t value);

libapac_err apz_grow(apz_t *result, size_t new_size_limbs);

libapac_err apz_shrink_fit(apz_t *result); // shrink the Big Integer so allocated limbs are equal to limbs in use

libapac_err apz_free(apz_t *result); // makes result = NULL

libapac_err apz_reset(apz_t *result);

/**
 * APZ ASSIGMENT FUNCTIONS
 */

libapac_err apz_copy(apz_t *result, apz_t *op1);

libapac_err apz_set_string_hex(apz_t *result, uint8_t *string, uint64_t string_len);

libapac_err apz_set_string_dec(apz_t *result, uint8_t *string, uint64_t string_len);

/**
 *  APZ OPERATION LIMIT FUNCTIONS
 */

u64 inline apz_limit_add(apz_t *op1, apz_t *op2);

u64 inline apz_limit_mul(apz_t *op1, apz_t *op2);

u64 inline apz_limit_sqr(apz_t *op1);

u64 inline apz_limit_exp(apz_t *op1, apz_t *op2);

u64 inline apz_limit_mod_exp(apz_t *op1, apz_t *op2);

/**
 *  APZ HIGH LEVEL BASIC ARITHMETIC FUNCTIONS
 */

int8_t apz_abs_greater(const apz_t *op1, const apz_t *op2);

/// @note result = op1 + op2
/// @note result sign set according to values of op1 and op2
libapac_err apz_hl_add(apz_t *result, const apz_t *op1, const apz_t *op2);

/// @note result = op1 + value
/// @note result sign set according to value and sign of op1
libapac_err apz_hl_add_ui(apz_t *result, const apz_t *op1, uint64_t value); // result = op1 + value

/// @note result = op1 - op2
/// @note result sign set according to values of op1 and op2
libapac_err apz_hl_sub(apz_t *result, const apz_t *op1, const apz_t *op2); // result = op1 - op2

/// @note result = op1 - value
/// @note result sign set according to values of op1 and value
libapac_err apz_hl_sub_pos64(apz_t *result, const apz_t *op1, uint64_t value); // result = op1 - value

/// @note result = value - op1
/// @note result sign set according to values of op1 and value
libapac_err apz_hl_pos64_sub(apz_t *result, const uint64_t value, const apz_t *op1); // result = value - op1

libapac_err apz_hl_mul(apz_t *result, const apz_t *op1, const apz_t *op2); // result = op1 * op2

libapac_err apz_hl_mul_pos64(apz_t *result, const apz_t *op1, const uint64_t value); // result = op1 * value

libapac_err apz_hl_mul_neg64(apz_t *result, const apz_t *op1, const uint64_t value); // result = op1 * value

/**
 *   APZ LOW LEVEL FUNCTIONS SPECIFIC TO x64 ARCH
 */

uint8_t apz_abs_add_x64(u64 *result_arr, const u64 *op1, const u64 *op2, u64 seg_count);

uint8_t apz_abs_add_ui_x64(u64 *result_arr, const u64 *op1_arr, u64 val, u64 seg_count);

uint8_t apz_abs_sub_x64(u64 *result, const u64 *op1, const u64 *op2, u64 op1_size, u64 op2_size);

void apz_mul_ui_x64(u64 *result, const u64 *op1, u64 op1_size, u64 val);

void apz_mul_basecase_x64(u64 *result_arr, const u64 *arr1, const u64 *arr2, u64 arr1_size, u64 arr2_size);

int8_t apz_abs_cmp(const u64 *op1, const u64 *op2, u64 size1, u64 size2);

void apz_mul_x64(u64 *result_arr, const u64 *op1, const u64 *op2, u64 size1, u64 size2);

void apz_base_compliment(u64 *op1, u64 size);

#define KARATSUBA_THRESHOLD 1 // number of limbs/segments after which karatsuba dominates basecase mul in speed

void apz_mul_karatsuba_x64(u64 *result, const u64 *op1, const u64 *op2, u64 size, u64 *workspace);
#endif

#ifdef LIBAPAC_IMPLEMENTATION

/*
    LIBAPAC MEMORY ALLOCATION METHOD IMPLEMENTATION
*/

void set_memory_func_ptrs(
    void *(*func_ptr1)(size_t init_size),
    void *(*func_ptr2)(void *old_buffer, size_t new_size),
    void (*func_ptr3)(void *memory))
{
    if (!func_ptr1 && !func_ptr2 && !func_ptr3)
    {
        malloc_ptr = &malloc;
        realloc_ptr = &realloc;
        free_ptr = &free;
        return;
    }
    else if (func_ptr1 && func_ptr2 && func_ptr3)
    {
        malloc_ptr = func_ptr1;
        realloc_ptr = func_ptr2;
        free_ptr = func_ptr3;
        return;
    }
    else
    {
        ASSERT("Function parameters not correct in 'set_memory_func_ptrs'!");
    }
}

/*
    APZ INIT FUNCTION DEFINITIONS
*/

libapac_err apz_init_pos64(apz_t *result, size_t init_size, uint64_t init_value)
{
    ASSERT(result && init_size && ((init_size >> 3) < ULLONG_MAX));

    u64 alloc_size = (u64)init_size << 3;

    result->num_array = (u64 *)malloc_ptr(alloc_size);

    if (!result->num_array)
    {
        REPORT_ERR(stderr, "Memory allocation failed in 'apz_init_pos64'!");
        return LIBAPAC_OOM;
    }

    result->seg_alloc = init_size;
    memset(result->num_array, 0, result->seg_alloc * sizeof(u64));

    result->num_array[0] = init_value;
    result->seg_in_use = 1 && init_value;
    result->is_negative = APZ_ZPOS;

    return LIBAPAC_OKAY;
}

libapac_err apz_init_neg64(apz_t *result, size_t init_size, uint64_t init_value)
{
    ASSERT(result && init_size && ((init_size >> 3) < ULLONG_MAX));

    u64 alloc_size = (u64)init_size << 3;

    result->num_array = (u64 *)malloc_ptr(alloc_size);

    if (!result->num_array)
    {
        REPORT_ERR(stderr, "Memory allocation failed in 'apz_init_neg64'!");
        return LIBAPAC_OOM;
    }

    result->seg_alloc = init_size;
    memset(result->num_array, 0, result->seg_alloc * sizeof(u64));

    result->num_array[0] = init_value;
    result->seg_in_use = 1 && init_value;
    result->is_negative = APZ_NEG && init_value;

    return LIBAPAC_OKAY;
}

libapac_err apz_grow(apz_t *result, size_t new_size)
{
    ASSERT(result && (new_size > result->seg_alloc) && ((new_size >> 3) < ULLONG_MAX));

    u64 temp_size = (result->seg_alloc * 3) / 2 + 8;

    if (temp_size > new_size)
    {
        new_size = temp_size;
    }

    u64 *temp_mem = (u64 *)realloc_ptr(result->num_array, new_size * sizeof(u64));

    if (!temp_mem)
    {
        REPORT_ERR(stderr, "Memory allocation failed in 'apz_grow'!");
        return LIBAPAC_OOM;
    }

    u64 *extended_addr = temp_mem + result->seg_alloc * sizeof(u64);

    memset(extended_addr, 0, new_size - result->seg_alloc);

    result->num_array = temp_mem;
    result->seg_alloc = new_size;
    temp_mem = NULL;

    return LIBAPAC_OKAY;
}

libapac_err apz_shrink_fit(apz_t *result)
{
    ASSERT(result && result->num_array);

    u64 *temp_mem = (u64 *)realloc_ptr(result->num_array, sizeof(u64) * result->seg_in_use);

    if (!temp_mem)
    {
        REPORT_ERR(stderr, "Memory allocation failed in 'apz_shrink_fit'!");
        return LIBAPAC_OOM;
    }

    result->num_array = temp_mem;
    result->seg_alloc = result->seg_in_use;
    temp_mem = NULL;

    return LIBAPAC_OKAY;
}

libapac_err apz_free(apz_t *result)
{
    ASSERT(result && result->num_array);

    free_ptr(result->num_array);

    result->num_array = NULL;

    free_ptr(result);

    return LIBAPAC_OKAY;
}

libapac_err apz_reset(apz_t *result)
{
    ASSERT(result && result->num_array);

    memset(result->num_array, 0, result->seg_alloc * sizeof(u64));

    result->seg_in_use = 0;
    result->is_negative = APZ_ZPOS;

    return LIBAPAC_OKAY;
}

/*
    APZ HIGH LEVEL BASIC ARITHMETIC FUNCTION DEFINITIONS (PLATFORM INDEPENDENT)
*/

int8_t apz_abs_greater(const apz_t *op1, const apz_t *op2)
{
    ASSERT(op1 && op2);
    ASSERT(op1->num_array && op2->num_array);

    if (op1->seg_in_use > op2->seg_in_use)
    {
        return 1;
    }
    else if (op2->seg_in_use > op1->seg_in_use)
    {
        return -1;
    }
    else
    {
        u64 counter;

        for (counter = op1->seg_in_use - 1; counter >= 1; counter--)
        {
            if (op1->num_array[counter] != op2->num_array[counter])
            {
                goto is_greater_hl;
            }
        }

        goto is_greater_hl;

    is_greater_hl:
        return (op1->num_array[counter] > op2->num_array[counter] ? 1 : (op1->num_array[counter] < op2->num_array[counter] ? -1 : 0));
    }
}

libapac_err apz_hl_add(apz_t *result, const apz_t *op1, const apz_t *op2)
{
    ASSERT(result && op1 && op2);
    ASSERT(result->num_array && op1->num_array && op2->num_array);

    const apz_t *max_elem, *min_elem;
    uint8_t carry = 0;
    uint8_t op_to_do = 0; // abs addition by default
    libapac_err ret_val = LIBAPAC_OKAY;

    int8_t greater = apz_abs_greater(op1, op2);

    if (greater == -1)
    {
        max_elem = op2;
        min_elem = op1;
    }
    else
    {
        max_elem = op1;
        min_elem = op2;
    }

    if (max_elem->is_negative != min_elem->is_negative)
    {
        op_to_do = 1; // to do abs subtraction
    }

    if (!op_to_do && result->seg_alloc < (max_elem->seg_in_use + 1)) // if absolute operation is addition
    {
        ret_val = apz_grow(result, max_elem->seg_in_use + 1);
    }
    else if (op_to_do && result->seg_alloc < max_elem->seg_in_use) // if absolute operation is subtraction
    {
        ret_val = apz_grow(result, max_elem->seg_in_use);
    }

    if (ret_val == LIBAPAC_OOM) // check if out of memory error has happened and handle
    {
        REPORT_ERR(stderr, "Out of memory in apz_hl_add!");
        return ret_val;
    }

    apz_reset(result);

    if (!op_to_do)
    {
    }
    else
    {
        // carry variable functions as borrow

        // carry (AKA borrow) is never 1 after this loop (just how subtraction works)
    }

    if (carry)
    {
        result->num_array[max_elem->seg_in_use] += carry;
    }

    result->is_negative = max_elem->is_negative;
    result->seg_in_use = max_elem->seg_in_use + (carry && 1);

    return ret_val;
}

libapac_err apz_hl_add_ui(apz_t *result, const apz_t *op1, uint64_t value)
{
    ASSERT(result && op1 && value);
    ASSERT(result->num_array && op1->num_array);

    libapac_err ret_val = LIBAPAC_OKAY;

    if (op1->is_negative == APZ_NEG && result->seg_alloc < op1->seg_in_use)
    {
        ret_val = apz_grow(result, op1->seg_in_use);
    }
    else if (op1->is_negative == APZ_ZPOS && result->seg_alloc < op1->seg_in_use + 1)
    {
        ret_val = apz_grow(result, op1->seg_in_use + 1);
    }

    if (ret_val == LIBAPAC_OOM)
    {
        REPORT_ERR(stderr, "Out of memory in apz_hl_add_ui!");
        return ret_val;
    }

    uint8_t carry_or_borrow = 0;
    apz_reset(result);

    if (op1->is_negative == APZ_NEG)
    {
        if (op1->seg_in_use == 1 && op1->num_array[0] < value)
        {
            result->num_array[0] = value - op1->num_array[0];
            result->is_negative = APZ_ZPOS;
            result->seg_in_use = 1;
            return ret_val;
        }
        else
        {
            carry_or_borrow = _subborrow_u64(carry_or_borrow, op1->num_array[0], value, &result->num_array[0]);
            carry_or_borrow = _subborrow_u64(carry_or_borrow, op1->num_array[1], 0, &result->num_array[1]);
        }

        result->is_negative = APZ_NEG;
        result->seg_in_use = op1->seg_in_use;
        return ret_val;
    }
    else
    {
        carry_or_borrow = _addcarry_u64(carry_or_borrow, op1->num_array[0], value, &result->num_array[0]);

        for (uint64_t counter = 1; counter < op1->seg_in_use; counter++)
        {
            carry_or_borrow = _addcarry_u64(carry_or_borrow, op1->num_array[counter], 0, &result->num_array[counter]);
        }
    }

    if (carry_or_borrow) // indicates carry here
    {
        result->num_array[op1->seg_in_use] = carry_or_borrow;
    }

    result->is_negative = APZ_ZPOS;
    result->seg_in_use = op1->seg_in_use + (carry_or_borrow && 1);
    return ret_val;
}

libapac_err apz_hl_sub(apz_t *result, const apz_t *op1, const apz_t *op2)
{
    ASSERT(result && op1 && op2);
    ASSERT(result->num_array && op1->num_array && op2->num_array);

    const apz_t *max_elem, *min_elem;
    uint8_t borrow = 0;   // might function as carry depending on scenario
    uint8_t op_to_do = 1; // abs subtraction by default
    libapac_err ret_val = LIBAPAC_OKAY;

    int8_t greater = apz_abs_greater(op1, op2);

    if (greater == -1)
    {
        max_elem = op2;
        min_elem = op1;
    }
    else
    {
        max_elem = op1;
        min_elem = op2;
    }

    if (max_elem->is_negative != min_elem->is_negative)
    {
        op_to_do = 0; // to do abs addition
    }

    if (!op_to_do && result->seg_alloc < (max_elem->seg_in_use + 1)) // if operation is addition
    {
        ret_val = apz_grow(result, max_elem->seg_in_use + 1);
    }
    else if (op_to_do && result->seg_alloc < max_elem->seg_in_use) // if operation is subtraction
    {
        ret_val = apz_grow(result, max_elem->seg_in_use);
    }

    if (ret_val == LIBAPAC_OOM) // check if out of memory error has happened and handle
    {
        return ret_val;
    }

    apz_reset(result);

    if (op_to_do) // do absolute subtraction
    {
    }
    else // do absolute addition
    {
        // borrow variable functions as carry
    }

    if (borrow)
    {
        result->num_array[max_elem->seg_in_use] += borrow;
    }

    result->seg_in_use = max_elem->seg_in_use + (borrow && 1);
    result->is_negative = max_elem->is_negative && borrow;
    return ret_val;
}

/*
    APZ OPERATION LIMIT FUNCTION DEFINITIONS
*/

u64 inline apz_limit_add(apz_t *op1, apz_t *op2)
{
    return (op1->seg_in_use > op2->seg_in_use ? op1->seg_in_use + 1 : op2->seg_in_use + 1);
}

u64 inline apz_limit_mul(apz_t *op1, apz_t *op2)
{
    return (op1->seg_in_use + op2->seg_in_use);
}

u64 inline apz_limit_sqr(apz_t *op1)
{
    return (2 * op1->seg_in_use);
}

u64 inline apz_limit_exp(apz_t *op1, apz_t *op2)
{
    return (op1->seg_in_use * op2->seg_in_use);
}

/*
    APZ LOW LEVEL FUNCTIONS (x64 ARCH) IMPLEMENTATION
*/

uint8_t apz_abs_add_x64(u64 *result, const u64 *op1, const u64 *op2, u64 seg_count)
{
    uint8_t carry = 0;

    for (u64 i = 0; i < seg_count; i++)
    {
        carry = _addcarry_u64(carry, op1[i], op2[i], &result[i]);
    }

    return carry;
}

uint8_t apz_abs_add_ui_x64(u64 *result_arr, const u64 *op1_arr, u64 val, u64 seg_count)
{
    uint64_t counter = 0;
    uint8_t carry = 0;

    carry = _addcarry_u64(carry, op1_arr[counter], val, &result_arr[counter]);
    counter++;

    while (counter < seg_count)
    {
        carry = _addcarry_u64(carry, op1_arr[counter], 0, &result_arr[counter]);
        counter++;
    }

    return carry;
}

uint8_t apz_abs_sub_x64(u64 *result, const u64 *op1, const u64 *op2, u64 op1_size, u64 op2_size)
{
    uint8_t borrow = 0;
    ASSERT(op1_size >= op2_size);
    for (u64 i = 0; i < op2_size; i++)
        borrow = _subborrow_u64(borrow, op1[i], op2[i], &result[i]);

    for (u64 i = op2_size; i < op1_size; i++)
        borrow = _subborrow_u64(borrow, op1[i], 0, &result[i]);

    ASSERT(borrow == 0);
}

void apz_mul_ui_x64(u64 *result, const u64 *op1, u64 op1_size, u64 val)
{
    u64 low64 = 0, high64 = 0, temp_reg = 0;
    uint8_t aux1 = 0, aux2 = 0;

    for (u64 j = 0; j < op1_size; j++)
    {
        temp_reg = high64 + aux2;
        low64 = _umul128(op1[j], val, &high64);

        aux1 = _addcarry_u64(0, temp_reg, low64, &temp_reg);
        aux1 = _addcarry_u64(aux1, high64, 0, &high64);
        aux2 = _addcarry_u64(0, temp_reg, result[j], &result[j]);
    }

    result[op1_size] += aux2 + high64;

    return;
}

void apz_mul_basecase_x64(u64 *result_arr, const u64 *arr1, const u64 *arr2, u64 arr1_size, u64 arr2_size)
{
    for (u64 i = 0; i < arr1_size; i++)
    {
        u64 low64 = 0, high64 = 0, temp_reg = 0;
        uint8_t aux1 = 0, aux2 = 0;

        for (u64 j = 0; j < arr2_size; j++)
        {
            temp_reg = high64 + aux2;
            low64 = _umul128(arr1[i], arr2[j], &high64);

            aux1 = _addcarry_u64(0, temp_reg, low64, &temp_reg);
            aux1 = _addcarry_u64(aux1, high64, 0, &high64);
            aux2 = _addcarry_u64(0, temp_reg, result_arr[i + j], &result_arr[i + j]);
        }

        result_arr[i + arr2_size] += aux2 + high64;
    }

    return;
}

int8_t apz_abs_cmp(const u64 *op1, const u64 *op2, u64 size1, u64 size2)
{
    // returns the operator that is greater in absolute value
    // assumes no top limbs are 0's

    if (size1 > size2)
    {
        return 1;
    }
    else if (size2 > size1)
    {
        return -1;
    }
    else
    {
        u64 counter;

        for (counter = size1 - 1; counter >= 1; counter--)
        {
            if (op1[counter] != op2[counter])
            {
                goto is_greater;
            }
        }

        goto is_greater;

    is_greater:
        return (op1[counter] > op2[counter] ? 1 : (op1[counter] < op2[counter] ? -1 : 0));
    }
}

u64 apz_trim(const u64 *op_arr, u64 size)
{
    u64 new_size = size;

    while (new_size > 0 && op_arr[new_size - 1] == 0)
    {
        new_size--;
    }

    return new_size;
}

void apz_base_compliment(u64 *op1, u64 size)
{
    for (u64 i = size; i-- > 0;)
        op1[i] = ~op1[i];

    apz_abs_add_ui_x64(op1, op1, 1, size);
}

void apz_mul_karatsuba_x64(u64 *result, const u64 *op1, const u64 *op2, u64 size, u64 *workspace)
{
    static uint8_t depth = 0, alloc_ws = 0;

    if (!depth && !workspace)
    {
        workspace = malloc_ptr(sizeof(u64) * 8 * ((size + 1) / 2));
        memset(workspace, 0, sizeof(u64) * 8 * ((size + 1) / 2));
        alloc_ws = 1;
    }

    if (size <= KARATSUBA_THRESHOLD)
    {
        apz_mul_basecase_x64(result, op1, op2, size, size);
        return;
    }

    depth++;

    u64 high = size >> 1;
    u64 low = (size + 1) >> 1;

    apz_mul_karatsuba_x64(result, op1, op2, low, workspace);
    apz_mul_karatsuba_x64(result + low * 2, op1 + low, op2 + low, high, workspace);

    u64 trim_a0 = apz_trim(op1, low);
    u64 trim_a1 = apz_trim(op1 + low, high);

    u64 trim_b0 = apz_trim(op1, low);
    u64 trim_b1 = apz_trim(op1 + low, high);

    int8_t cmp1 = apz_abs_cmp(op1, op1 + low, trim_a0, trim_a1);
    int8_t cmp2 = apz_abs_cmp(op2, op2 + low, trim_b0, trim_b1);

    // accumulate the results of |a0 - a1| and |b0 - b1| in workspace and workspace + low

    if (cmp1 >= 0)
        apz_abs_sub_x64(workspace, op1, op1 + low, low);
    else
        apz_abs_sub_x64(workspace, op1 + low, op1, low);

    if (cmp2 >= 0)
        apz_abs_sub_x64(workspace + low, op2, op2 + low, low);
    else
        apz_abs_sub_x64(workspace + low, op2 + low, op2, low);

    apz_mul_karatsuba_x64(workspace + 2 * low, workspace, workspace + low, low, workspace + 4 * low);

    if ((cmp1 >= 0 && cmp2 >= 0) || (cmp1 == -1 && cmp2 == -1))
    {
        apz_abs_sub_x64(workspace + low * 2, workspace + low * 2, result, low * 2);
        apz_abs_sub_x64(workspace + low * 2, workspace + low * 2, result + low * 2, high * 2);
        apz_base_compliment(workspace + low * 2, low * 2);
    }
    else
    {
        apz_abs_add_x64(workspace + low * 2, workspace + low * 2, result, low * 2);
        apz_abs_add_x64(workspace + low * 2, workspace + low * 2, result + low * 2, high * 2);
    }

    uint8_t c = apz_abs_add_x64(result + low, result + low, workspace + low * 2, low * 2);
    apz_abs_add_ui_x64(result + low * 3, result + low * 3, c, (2 * size - 3 * low));
    depth--;

    if (!depth && alloc_ws)
    {
        free_ptr(workspace);
        alloc_ws = 0;
    }

    return;
}

#endif