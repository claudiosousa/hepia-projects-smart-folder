/** Parses a string expression and return a formal representation.
    @file

    Completely unit tested
*/

#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>

/** Type of expression token

    Used to flag bitwise the `parser_crit_t` enum values,
    and later to obtain the category of a given `parser_crit_t` enum value

    @see parser_crit_t
 */
typedef enum {
    OPERATOR = 1 << 7,
    PARENTHESIS = 1 << 6,
    CRITERIA = 1 << 5,
} parser_crit_type_t;

/** List of possible expression tokens

    The enum values encode information:
     - the 3 most significant bits hold the category (operator, criteria or parenthesis)
     - the 4 least significant bits hold the token order
         + must start at 0 and be consecutive
     - operators are listed by priority ASC
         + the encoded priority information is used in the parsing algorithm

    @see parser_crit_type_t
    @see parser_t
 */
typedef enum {
    OR = OPERATOR,  // operators below, order by priority DESC
    AND,
    NOT,
    NAME = CRITERIA | 3,  // below are criteria, reset 4 lsb to the correct order value
    USER,
    GROUP,
    PERM,
    SIZE,
    ATIME,
    MTIME,
    CTIME,
    LPARENTHESIS = PARENTHESIS,  // parenthesis below
    RPARENTHESIS
} parser_crit_t;

/** The comparator used to evaluate criteria values.

    Calculated for criteria that accepts the +- in the values.
      - Example: `-SIZE +20k` or `-NAME -.txt`

    @see parser_t
*/
typedef enum { MAX, MIN, EXACT } parser_comp_t;

/** Formal expression representation

    Contains the formal representation of the parsed expression in a ordered chained list.
    Each instance contains the information about a parsed expression token
    and the pointer to the `next` parsed expression token.

    The order of the tokens in the chained list is not the same as the in the original string representation.
    Is has been reordered to encoded the priority of the operators and parenthesis in a infix notation
 */
typedef struct parser_t {
    /** The criteria for the token.
        - e.g. the `SIZE` in the exp `-SIZE +20`

        @see parser_crit_t
     */
    parser_crit_t crit;
    /** The value of the criteria, the type depend on the type of 'crit'.
        - e.g. the `20` in the exp `-SIZE +20`

        @see parser_crit_t
    */
    void *value;
    /** The comparison to be used when evaluating the expression validity.
        - e.g. the `+` in the exp `-SIZE +20`

        @see parser_comp_t
    */
    parser_comp_t comp;

    /** The next token in the expression*/
    struct parser_t *next;
} parser_t;

/** Parses the string expression, converting it to a `parser_t` representation.

    This is done in multiple steps:
      -# tokens syntax is verified
      -# criteria values units and comparison characters are verified
      -# a object representation is created for each token
      -# tokens are reordered to an infix notation encoding the operators priority and parenthesis

    The algorithm used in step *4* is based on the [Shunting yard algorithm](https://en.wikipedia.org/wiki/Shunting-yard_algorithm) (by Edsger Dijkstra).
    The original algorithm is intended to parse mathematical expressions and was adapt to parse our boolean expressions.
    @see parser_t

    @param expression The list of tokens forming the expression (space splitted)
    @param size The number of elements in `expression`
    @returns A `parser_t` representation of the expression, or *NULL* is the expression is invalid
*/
parser_t *parser_parse(char *expression[], size_t size);

/** Frees the memory allocated for a `parser_t` instance.
    @see parser_t
    @param expression The `parser_t instance to free
*/
void parser_free(parser_t *expression);

#endif
