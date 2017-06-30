#include "lcc.h"

static Symbol *symtab = NULL, *cur_func = NULL;
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
    abort();
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

Symbol *make_func_signature(Symbol *signature) {
    Symbol *func_def = make_symbol();
    func_def->attr = function_definition;
    // AMD64 ABI required (rsp + 8) % 16 == 0, and after pushing %rsp (rsp % 16) == 0.
    func_def->offset = func_def->rsp = 0;
    symtab_append(func_def);
    set_cur_func(func_def);
    func_def->basic_type = signature->basic_type;
    func_def->name = signature->name;
    func_def->param = signature->param;
    func_def->code = make_assembly();
    for (int i = 0; i < size(signature->param); i++) {
        Symbol *arg = (Symbol *) at(signature->param, i);
        symtab_append(arg);
        arg->offset = allocate_stack(arg->basic_type);
        assembly_push_back(func_def->code, sprint("\t# passing %s %d byte(s) %d(%%rbp)",
                                                  str(arg->name), actual_size[arg->basic_type], -arg->offset));
        assembly_push_back(func_def->code, sprint("\tmov%c   %%%s, %d(%%rbp)",
                                                  op_suffix[arg->basic_type],
                                                  arguments_reg[i][arg->basic_type],
                                                  -arg->offset));
    }
    return func_def;
}

void free_symbol(Symbol *sym) {
    free(sym);
}

Symbol *make_func_declaration(Symbol *type, Symbol *signature) {
    Symbol *decl = make_symbol();
    decl->attr = function_declaration;
    decl->name = signature->name;
    decl->param = signature->param;
    decl->basic_type = type->basic_type;
    symtab_append(decl);
    free_symbol(type);
    free_symbol(signature);
    return decl;
}

void symtab_append(Symbol *sym) {
    sym->parent = symtab;
    symtab = sym;
}

void make_new_scope() {
    Symbol *scope = make_symbol();
    scope->attr = new_scope;
    symtab_append(scope);
}

void destroy_scope() {
    while (symtab->attr != new_scope) symtab = symtab->parent;
    symtab = symtab->parent;
}

Symbol *make_empty_expression_stat() {
    Symbol *stat = make_symbol();
    stat->attr = statement;
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

int allocate_stack(Data_type type) {
    int size = actual_size[type];
    Symbol *func = get_cur_func();
    for (int i = 0; i < size; i++)
        // rsp only could be increased; stack top is designed to do alloc/free.
        // Otherwise func call alignment cannot be satisfied
        if ((func->offset + i + size) % size == 0) {
            func->offset += i + size;
            while (func->offset > func->rsp) func->rsp += 16;
            return func->offset;
        }
    yyerror("Allocate stack error");
}

Symbol *make_func_definition(Symbol *func_def, Symbol *stat) {
    Assembly *code = make_assembly();
    assembly_push_back(code,
                       sprint("\t.globl %s\n\t.type  %s, @function", str(func_def->name), str(func_def->name)));
    assembly_push_back(code, sprint("%s:", str(func_def->name)));
    assembly_push_back(code, make_string("\tpushq  %rbp"));
    assembly_push_back(code, make_string("\tmovq   %rsp, %rbp"));
    assembly_push_back(code, sprint("\tsubq   %%rsp, %d", func_def->rsp));
    func_def->code = assembly_cat(code, func_def->code);
    func_def->code = assembly_cat(func_def->code, stat->code);
    free_symbol(stat);
    unset_cur_func();
    return func_def;
}

void set_cur_func(Symbol *func) {
    cur_func = func;
}

void unset_cur_func() {
    cur_func = NULL;
}

Symbol *get_cur_func() {
    return cur_func;
}

Symbol *make_declaration(Symbol *type, Symbol *list) {
    Symbol *decl = make_symbol();
    decl->code = make_assembly();
    if (get_cur_func() != NULL) {
        for (int i = 0; i < size(list->init_list); i++) {
            Symbol *local = (Symbol *) at(list->init_list, i);
            local->attr = local_var;
            local->basic_type = type->basic_type;
            local->offset = allocate_stack(local->basic_type);
            symtab_append(local);
            assembly_cat(decl->code, local->code);
            assembly_push_back(decl->code,
                               sprint("\t# allocate %s %d byte(s) %d(%%rbp)",
                                      str(local->name), actual_size[local->basic_type], -local->offset));
        }
        return decl;
    }
}

Symbol *make_init_list() {
    Symbol *sym = make_symbol();
    sym->attr = init_list;
    sym->init_list = make_vector();
    return sym;
}

Symbol *init_list_push_back(Symbol *list, Symbol *init) {
    vec_push_back(list->init_list, init);
    return list;
}

Symbol *block_item_cat(Symbol *list, Symbol *block) {
    list->code = assembly_cat(list->code, block->code);
    return list;
}

