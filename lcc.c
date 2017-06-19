#include "lcc.h"

static Symbol *symtab = NULL;
FILE *output = NULL;

Symbol *make_symbol() {
    Symbol *ptr = (Symbol *) malloc(sizeof(Symbol));
    memset(ptr, 0, sizeof(Symbol));
    return ptr;
}

Symbol *make_identifier(char *str) {
    Symbol *sym = make_symbol();
    sym->attr = identifier;
    sym->name = make_string(str);
    return sym;
}

Symbol *make_data_type(Data_type type) {
    Symbol *sym = make_symbol();
    sym->attr = data_type;
    sym->basic_type = type;
    return sym;
}

Symbol *make_parameter_declaration(Symbol *type, Symbol *name) {
    Symbol *sym = make_symbol();
    sym->attr = parameter;
    sym->name = name->name;
    sym->basic_type = type->basic_type;
    free_symbol(type);
    free_symbol(name);
    return sym;
}

void info(const char *fmt, ...) {
    fflush(stdout);
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "INFO: ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
}

void yyerror(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    info(fmt, ap);
    va_end(ap);
    exit(0);
}

Symbol *make_parameter_list(Symbol *decl) {
    Symbol *list = make_symbol();
    list->attr = parameter_list;
    list->param = make_vector();
    parameter_list_push_back(list, decl);
    return list;
}

Symbol *make_func_declarator(Symbol *name, Symbol *param_list) {
    Symbol *list = make_symbol();
    list->attr = func_declarator;
    list->name = name->name;
    list->param = param_list->param;
    free_symbol(name);
    free_symbol(param_list);
    return list;
}

Symbol *make_func_definition(Symbol *signature, Symbol *stat) {
    Symbol *func_def = make_symbol();
    func_def->attr = function_definition;
    func_def->parent = symtab;
    func_def->basic_type = signature->basic_type;
    func_def->name = signature->name;
    func_def->param = signature->param;

    Assembly *code = make_assembly();
    assembly_push_back(code, sprint("\t.globl %s\n\t.type  %s, @function", str(func_def->name), str(func_def->name)));
    assembly_push_back(code, sprint("%s:", str(func_def->name)));
    assembly_push_back(code, make_string("\tpushq  %rbp"));
    assembly_push_back(code, make_string("\tmovq   %rsp, %rbp"));
    func_def->code = assembly_cat(code, stat->code);
    free_symbol(signature);
    free_symbol(stat);
    /*
    for (int i = 0; i < size(func_def->param); i++) {
        String *tmp = ((Symbol *)at(func_def->param, i))->name;
        info("%s", str(tmp));
    }
    */
    //TODO: set formal parameter
    return func_def;
}

void free_symbol(Symbol *sym) {
    free(sym);
}

Symbol *make_declaration(Symbol *type, Symbol *declarator) {
    switch (declarator->attr) {
        case func_signature:
            return make_func_declaration(type, declarator);
        default:
            info("not support yet");
            return NULL;
    }
}

Symbol *make_func_declaration(Symbol *type, Symbol *signature) {
    Symbol *decl = make_symbol();
    decl->attr = function_declaration;
    decl->name = signature->name;
    decl->param = signature->param;
    decl->basic_type = type->basic_type;
    decl->parent = symtab;
    free_symbol(type);
    free_symbol(signature);
    return decl;
}

void make_new_scope() {
    Symbol *scope = make_symbol();
    scope->attr = new_scope;
    scope->parent = symtab;
    symtab = scope;
}

void destroy_scope() {
    while (symtab->attr != new_scope) symtab = symtab->parent;
    symtab = symtab->parent;
}

Symbol *make_empty_expression_stat() {
    Symbol *stat = make_symbol();
    stat->attr = statment;
    stat->code = make_assembly();
    return stat;
}

Assembly *make_assembly() {
    Assembly *ptr = malloc(sizeof(Assembly));
    ptr->beg = make_list_node(NULL, NULL, NULL);
    ptr->end = make_list_node(ptr->beg, NULL, NULL);
    return ptr;
}

Assembly *assembly_cat(Assembly *c1, Assembly *c2) {
    if (c2 != NULL) list_append(c1->beg, c1->end, c2->beg, c2->end);
    return c1;
}

void assembly_push_back(Assembly *code, String *piece) {
    make_list_node(code->end->prev, piece, code->end);
}

void assembly_to_file(Symbol *sym) {
    for (List_node *p = sym->code->beg->next; p != sym->code->end; p = p->next)
        fprintf(output, "%s\n", str(p->body));
    free_symbol(sym);
}

Symbol *parameter_list_push_back(Symbol *list, Symbol *decl) {
    vec_push_back(list->param, decl);
    return list;
}

