#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
    void **body;
    int len;
    int capacity;
} Vector;

int size(Vector *vec);

void *back(Vector *vec);

void vec_pop_back(Vector *vec);

void vec_push_back(Vector *vec, void *ptr);

Vector *make_vector();

void **get_array(Vector *vec);

char *c_str(Vector *vec);

void *at(Vector *vec, int i);

void clear(Vector *vec);

void free_vec(Vector *vec);

typedef struct String {
    Vector *impl;
} String;

int len(String *);

char *str(String *);

String *make_string(char *);

String *sprint(char *fmt, ...);

String *string_merge(String *s1, String *s2);

String *string_cat(String *s1, String *s2);

String *char_append(String *s1, char s2);

char string_pos(String *, int pos);

int equal_string(String *s1, String *s2);

typedef struct List_node {
    struct List_node *prev, *next;
    void *body;
} List_node;

List_node *make_list_node(List_node *prev, void *body, List_node *next);

void list_append(List_node *p1_beg, List_node *p1_end, List_node *p2_beg, List_node *p2_end);

#endif
