#ifndef LIBAPAC_H
#define LIBAPAC_H

/*
    LIBC HEADERS
*/

#include <stdlib.h>
#include <stdint.h>

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

///@note returns the big integer with the greater absolute value among the two
///@note returns 1 if abs(op1) >= abs(op2), else if abs(op1) <= abs(op2) returns 0
uint8_t apz_abs_greater(const apz_t *op1, const apz_t *op2);

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

/**
 * @result result_arr[limb] = abs(op1_arr[limb]) + abs(op2_arr[limb])
 *
 * @note adds seg_count number of limbs of op1_arr to op2_arr and returns a carry if produced (1) otherwise (0)
 * @warning result_arr should not overlap with either of the operand arrays
 */
uint8_t apz_abs_add_x64(u64 *result_arr, const u64 *op1_arr, const u64 *op2_arr, u64 seg_count);

/**
 * @result result = abs(op1_arr) + abs(val)
 *
 * @note assumes result_arr has at least seg_count number of limbs
 * @warning result_arr should not overlap with either of the operand arrays
 */
uint8_t apz_abs_add_ui_x64(u64 *result_arr, const u64 *op1_arr, u64 val, u64 seg_count);

/**
 * @result result_arr[limb] = abs(op1_arr[limb]) - abs(op2_arr[limb])
 *
 * @note subtracts seg_count number of limbs of op1_arr from op2_arr and returns a borrow if produced (1) otherwise (0)
 * @warning result_arr should not overlap with either of the operand arrays
 */
uint8_t apz_abs_sub_x64(u64 *result_arr, const u64 *op1_arr, const u64 *op2_arr, u64 seg_count);

/**
 * @result result = op1 * op2, each operand having seg_count limbs
 *
 * @note caller has to assure result arr has (seg_count * 2) limbs
 * @warning result_arr should not overlap with either of the operand arrays
 */
void apz_mul_basecase_x64(u64 *result_arr, const u64 *arr1, const u64 *arr2, u64 arr1_size, u64 arr2_size);

/**
 * @result result = op1 * op2, each operand having seg_count limbs
 *
 * @note caller has to assure result arr has (seg_count * 2) limbs
 * @warning result_arr should not overlap with either of the operand arrays
 */
void apz_mul_karatsuba_x64(u64 *result_arr, const u64 *op1_arr, const u64 *op2_arr, u64 seg_count);

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
    result->is_negative = APZ_NEG;

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

uint8_t apz_abs_greater(const apz_t *op1, const apz_t *op2)
{
    ASSERT(op1 && op2);
    ASSERT(op1->num_array && op2->num_array);

    if (op1->seg_in_use > op2->seg_in_use)
    {
        return 1;
    }
    else if (op2->seg_in_use > op1->seg_in_use)
    {
        return 0;
    }
    else
    {
        uint64_t counter = op1->seg_in_use - 1;

        while (counter != 0)
        {
            if (op1->num_array[counter] > op2->num_array[counter])
            {
                return 1;
            }
            else if (op1->num_array[counter] < op2->num_array[counter])
            {
                return 0;
            }
            else
            {
                counter--;
            }
        }

        return (op1->num_array[counter] >= op2->num_array[counter] ? 1 : 0);
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

    if (apz_abs_greater(op1, op2))
    {
        max_elem = op1;
        min_elem = op2;
    }
    else
    {
        max_elem = op2;
        min_elem = op1;
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

    uint64_t counter = min_elem->seg_in_use;
    apz_reset(result);

    if (!op_to_do)
    {
        carry = apz_abs_add_x64(result->num_array, max_elem->num_array, min_elem->num_array, min_elem->seg_in_use);

        if (max_elem->seg_in_use > min_elem->seg_in_use)
        {
            while (counter < max_elem->seg_in_use)
            {
                carry = _addcarry_u64(carry, max_elem->num_array[counter], 0, &result->num_array[counter]);
                counter++;
            }
        }
    }
    else
    {
        // carry variable functions as borrow

        carry = apz_abs_sub_x64(result->num_array, max_elem->num_array, min_elem->num_array, min_elem->seg_in_use);

        if (max_elem->seg_in_use > min_elem->seg_in_use)
        {
            while (counter < max_elem->seg_in_use)
            {
                carry = _subborrow_u64(carry, max_elem->num_array[counter], 0, &result->num_array[counter]);
                counter++;
            }
        }

        // carry (AKA borrow) is never 1 after this loop (just how subtraction works)
    }

    if (carry)
    {
        result->num_array[counter] += carry;
    }

    result->is_negative = max_elem->is_negative;
    result->seg_in_use = max_elem->seg_in_use + (carry & 1);

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
    result->seg_in_use = op1->seg_in_use + (carry_or_borrow & 1);
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

    if (apz_abs_greater(op1, op2))
    {
        max_elem = op1;
        min_elem = op2;
    }
    else
    {
        max_elem = op2;
        min_elem = op1;
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

    uint64_t counter = min_elem->seg_in_use;
    apz_reset(result);

    if (op_to_do) // do absolute subtraction
    {
        borrow = apz_abs_sub_x64(result->num_array, max_elem->num_array, min_elem->num_array, min_elem->seg_in_use);

        if (max_elem->seg_in_use > min_elem->seg_in_use)
        {
            while (counter < max_elem->seg_in_use)
            {
                borrow = _subborrow_u64(borrow, max_elem->num_array[counter], 0, &result->num_array[counter]);
                counter++;
            }
        }
    }
    else // do absolute addition
    {
        // borrow variable functions as carry

        borrow = apz_abs_add_x64(result->num_array, max_elem->num_array, min_elem->num_array, min_elem->seg_in_use);

        if (max_elem->seg_in_use > min_elem->seg_in_use)
        {
            while (counter < max_elem->seg_in_use)
            {
                borrow = _addcarry_u64(borrow, max_elem->num_array[counter], 0, &result->num_array[counter]);
                counter++;
            }
        }
    }

    if (borrow)
    {
        result->num_array[counter] += borrow;
    }

    result->seg_in_use = max_elem->seg_in_use;
    result->is_negative = max_elem->is_negative;
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

uint8_t apz_abs_add_x64(u64 *result_arr, const u64 *op1_arr, const u64 *op2_arr, u64 seg_count)
{
    // call this function if the operands are same size in segments

    uint64_t counter = 0; // counter to keep track of number of segments processed
    uint8_t carry = 0;    // carry variable for carry propagation

    while (counter < seg_count)
    {
        carry = _addcarry_u64(carry, op1_arr[counter], op2_arr[counter], &result_arr[counter]);
        counter++;
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

uint8_t apz_abs_sub_x64(u64 *result_arr, const u64 *op1_arr, const u64 *op2_arr, u64 seg_count)
{
    // call this function if the operands are same size in segments

    uint64_t counter = 0; // counter to keep track of number of segments processed
    uint8_t borrow = 0;   // borrow variable for borrow propagation

    while (counter < seg_count)
    {
        borrow = _subborrow_u64(borrow, op1_arr[counter], op2_arr[counter], &result_arr[counter]);
        counter++;
    }

    return borrow;
}

uint8_t _apz_abs_sub_x64(u64* result, u64* a, u64* b, u64 a_size, u64 b_size)
{
    u64 min_size = a_size < b_size ? a_size : b_size;
    uint8_t borrow = 0;   

    u64 i = 0;
    for(; i < min_size; i++)
        borrow = _subborrow_u64(borrow, a[i], b[i], &result[i]);

    for(; i < a_size; i++)
        borrow = _subborrow_u64(borrow, a[i], 0, &result[i]);

    for(; i < b_size; i++)
        borrow = _subborrow_u64(borrow, 0, b[i], &result[i]);
    
    return borrow;
}   

void apz_mul_basecase_x64(u64 *result_arr, const u64 *arr1, const u64 *arr2, u64 arr1_size, u64 arr2_size)
{
    for (u64 i = 0; i < arr1_size; i++)
    {
        u64 low64 = 0, high64 = 0, temp_reg = 0;
        uint8_t aux_carry = 0;

        for (u64 j = 0; j < arr2_size; j++)
        {
            temp_reg = high64;
            low64 = _umul128(arr1[i], arr2[j], &high64);
            aux_carry = _addcarry_u64(aux_carry, temp_reg + low64, result_arr[i + j], &result_arr[i + j]);
        }

        result_arr[i + arr2_size] += aux_carry + high64;
    }
}

#endif