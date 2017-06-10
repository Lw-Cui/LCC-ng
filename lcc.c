#include "lcc.h"

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
    return list;
}

Symbol *make_func_declarator(Symbol *name, Symbol *param_list) {
    Symbol *list = make_symbol();
    list->attr = func_declarator;
    list->name = name->name;
    list->param = param_list->param;
    return list;
}

Symbol *make_func_signature(Symbol *type, Symbol *declarator) {
    Symbol *signature = make_symbol();
    signature->attr = func_signature;
    signature->basic_type = type->basic_type;
    signature->param = declarator->param;
    signature->name = declarator->name;
    return signature;
}

Symbol *make_func_definition(Symbol *signature, Symbol *stat) {
    Symbol *func_def = make_symbol();
    func_def->attr = function_definition;
    func_def->basic_type = signature->basic_type;
    func_def->name = signature->name;
    func_def->param = signature->param;
    return func_def;
}
