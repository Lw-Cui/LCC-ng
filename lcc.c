#include "lcc.h"

static Symbol *symtab = NULL;

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
    push_back(list->param, decl);
    free_symbol(decl);
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
    func_def->code = stat->code;
    free_symbol(signature);
    free_symbol(stat);
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
