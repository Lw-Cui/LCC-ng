#ifndef LCC_NG_LCC_INTERNAL_H
#define LCC_NG_LCC_INTERNAL_H

#include "lcc.h"

#define MALLOC(type) ((type*) calloc(1, sizeof(type)))

typedef enum {
    type_specifier,             // int
    identifier,                 // a
    parameter_declaration,      // int a
    parameter_list,             // int a, int b
    func_declarator,            // foo(int a, int b)
    function_defination,        // int foo(int a, int b) { return a; }
    function_declaration,       // int foo(int a, int b)

    new_scope,                  // { ... }
    statement,                  // ...

    local_var,                  // int a = 6, b = 5
    temporary,                  // temporary variable
    init_declarator_list,       // a = 6, b = 5

    expression,                 // a + b
} Attribute;

typedef struct {
    Attribute attr;
} Entity;

typedef struct {
    Entity *base;
    String *name;
} Identifier;

typedef enum {
    BYTE,
    WORD,
    LONG_WORD,
    QUAD_WORD,
} Size_type;

typedef struct {
    Entity *base;
    Size_type size_type;
} Data_type;

static Entity *new_entity(Attribute attr);

static Entity *new_identifier(char *);

static Entity *new_data_type(Size_type type);

#endif //LCC_NG_LCC_INTERNAL_H
