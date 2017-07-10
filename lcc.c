#include "lcc.h"

static Symbol *symtab = NULL, *cur_func = NULL;
static Expr_stack stack = {0};
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
    sym->attr = type_specifier;
    sym->basic_type = type;
    return sym;
}

Symbol *make_parameter_declaration(Symbol *type, Symbol *name) {
    Symbol *sym = make_symbol();
    sym->attr = parameter_declaration;
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

Symbol *make_fun_def_step1(Symbol *signature) {
    Symbol *func_def = make_symbol();
    func_def->attr = function_defination;
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
        allocate_stack(arg);
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

void allocate_stack(Symbol *s) {
    int size = actual_size[s->basic_type];
    Symbol *func = get_cur_func();
    s->upper_bound = func->offset;
    for (int i = 0; i < size; i++)
        // rsp only could be increased; stack top is designed to do alloc/free.
        // Otherwise func call alignment cannot be satisfied
        if ((func->offset + i + size) % size == 0) {
            func->offset += i + size;
            while (func->offset > func->rsp) func->rsp += 16;
            s->offset = func->offset;
            return;
        }
}

Symbol *make_func_def_step2(Symbol *func_def, Symbol *stat) {
    Assembly *code = make_assembly();
    assembly_push_back(code,
                       sprint("\t.globl %s\n\t.type  %s, @function", str(func_def->name), str(func_def->name)));
    assembly_push_back(code, sprint("%s:", str(func_def->name)));
    assembly_push_back(code, make_string("\tpushq  %rbp"));
    assembly_push_back(code, make_string("\tmovq   %rsp, %rbp"));
    assembly_push_back(code, sprint("\tsubq   $%d, %%rsp", func_def->rsp));
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
            allocate_stack(local);
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
    sym->attr = init_declarator_list;
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

void expr_stack_init() {
    stack.content = make_vector();
    stack.eax = Unused;
}

void expr_stack_push(Symbol *expr, struct Symbol *s) {
    if (stack.content == NULL)
        expr_stack_init();
    Symbol *new_top = make_symbol();
    new_top->attr = temporary;
    new_top->basic_type = s->basic_type;
    new_top->name = s->name;
    assembly_push_back(expr->code, sprint("\t# push %s", str(s->name)));
    if (stack.eax == Used) {
        Symbol *cur_top = (Symbol *) back(stack.content);
        allocate_stack(cur_top);
        assembly_push_back(expr->code, sprint("\tmov%c   %%%s, %d(%%rbp)",
                                              op_suffix[cur_top->basic_type],
                                              regular_reg[0][cur_top->basic_type],
                                              -cur_top->offset
        ));
    }
    if (s->attr == local_var)
        assembly_push_back(expr->code, sprint("\tmov%c   %d(%%rbp), %%%s",
                                              op_suffix[s->basic_type],
                                              -s->offset,
                                              regular_reg[0][s->basic_type]
        ));
    stack.eax = Used;
    new_top->offset = new_top->upper_bound = 0;
    vec_push_back(stack.content, new_top);
}

// expr stores assembly language generated
void expr_stack_pop(Symbol *assembly, int reg_idx) {
    if (stack.content == NULL || expr_stack_size() == 0)
        yyerror("There is no element in stack");
    Symbol *top = expr_stack_top();
    vec_pop_back(stack.content);
    assembly_push_back(assembly->code, sprint("\t# pop %s", str(top->name)));
    if (stack.eax == Used) {
        stack.eax = Unused;
    } else {
        assembly_push_back(assembly->code, sprint("\tmov%c   %d(%%rbp), %%%s",
                                                  op_suffix[top->basic_type],
                                                  -top->offset,
                                                  regular_reg[reg_idx][top->basic_type]));
        cur_func->offset = top->upper_bound;
    }
}

Symbol *find_symbol(Symbol *name_sym) {
    Symbol *s = symtab;
    while (s != NULL && !equal_string(s->name, name_sym->name)) s = s->parent;
    if (s == NULL) yyerror("Symbol named %s cannot be found", str(name_sym->name));
    return s;
}

Symbol *make_expression() {
    Symbol *expr = make_symbol();
    expr->attr = expression;
    expr->code = make_assembly();
    return expr;
}

Symbol *make_expression_with_assembly(Symbol *p1, Symbol *p2) {
    Symbol *expr = make_expression();
    if (p1) assembly_cat(expr->code, p1->code);
    if (p2) assembly_cat(expr->code, p2->code);
    if (p1 && p1->attr == expression) free_symbol(p1);
    if (p2 && p2->attr == expression) free_symbol(p2);
    return expr;
}

void additive_op(Symbol *expr, char *op_prefix) {
    Symbol *op1 = expr_stack_top();
    expr_stack_pop(expr, 0);
    Symbol *op2 = expr_stack_top();
    expr_stack_pop(expr, 1);
    assembly_push_back(expr->code, sprint("\t# %s %s %s", str(op2->name), op_prefix, str(op1->name)));
    Data_type max_type = max(op1->basic_type, op2->basic_type);
    signal_extend(expr->code, 0, op1->basic_type, max_type);
    signal_extend(expr->code, 1, op2->basic_type, max_type);
    assembly_push_back(expr->code, sprint("\t%s%c   %%%s, %%%s",
                                          op_prefix,
                                          op_suffix[max_type],
                                          regular_reg[1][max_type],
                                          regular_reg[0][max_type]
    ));
    Symbol *res = make_symbol();
    res->attr = temporary;
    res->basic_type = max_type;
    res->name = sprint("(%s %s %s)", str(op2->name), op_prefix, str(op1->name));
    expr_stack_push(expr, res);
}

void extend(Assembly *code, char *(*reg)[4], int idx, Data_type original, Data_type new) {
    if (original >= new) return;
    assembly_push_back(code, sprint("\tmovs%c%c %%%s, %%%s",
                                    op_suffix[original],
                                    op_suffix[new],
                                    reg[idx][original],
                                    reg[idx][new]
    ));
}

void signal_extend(Assembly *code, int idx, Data_type original, Data_type new) {
    extend(code, regular_reg, idx, original, new);
}

Symbol *expr_stack_top() {
    return (Symbol *) back(stack.content);
}

Symbol *make_op_expression(Symbol *p1, enum Op_type op, Symbol *p2) {
    Symbol *sym = make_expression_with_assembly(p1, p2);
    switch (op) {
        case ADD:
            additive_op(sym, "add");
            break;
        case SUB:
            additive_op(sym, "sub");
            break;
        case IMUL:
            multiplicative_op(sym, "imul");
            break;
        case ASSIGN:
            assign_op(sym, p1);
            break;
        default:
            info("unsupported op");
            break;
    }
    return sym;
}

void assign_op(Symbol *expr, Symbol *target) {
    /* result must at eax */
    Symbol *res = expr_stack_top();
    assembly_push_back(expr->code, sprint("\t# assign %s to %s", str(res->name), str(target->name)));
    signal_extend(expr->code, 0, res->basic_type, max(res->basic_type, target->basic_type));
    assembly_push_back(expr->code, sprint("\tmov%c   %%%s, %d(%%rbp)",
                                          op_suffix[target->basic_type],
                                          regular_reg[0][target->basic_type],
                                          -target->offset
    ));
}

void expr_stack_clear() {
    stack.eax = Unused;
    vec_pop_back(stack.content);    // pop eax content
    while (expr_stack_size()) {
        cur_func->offset = expr_stack_top()->upper_bound;
        vec_pop_back(stack.content);
    }
}

int expr_stack_size() {
    return size(stack.content);
}

Symbol *end_statement(Symbol *assembly) {
    expr_stack_clear();
    assembly_push_back(assembly->code, make_string("\t# ------ EOF ------"));
    return assembly;
}

void multiplicative_op(Symbol *expr, char *op_prefix) {
    Symbol *op1 = expr_stack_top();
    expr_stack_pop(expr, 0);
    Symbol *op2 = expr_stack_top();
    expr_stack_pop(expr, 1);
    assembly_push_back(expr->code, sprint("\t# %s %s %s", str(op2->name), op_prefix, str(op1->name)));
    Data_type max_type = max(op1->basic_type, op2->basic_type);
    signal_extend(expr->code, 0, op1->basic_type, max_type);
    signal_extend(expr->code, 1, op2->basic_type, max_type);
    assembly_push_back(expr->code, sprint("\t%s%c  %%%s",
                                          op_prefix,
                                          op_suffix[max_type],
                                          regular_reg[1][max_type]
    ));
    Symbol *res = make_symbol();
    res->attr = temporary;
    res->basic_type = max_type;
    res->name = sprint("(%s %s %s)", str(op2->name), op_prefix, str(op1->name));
    expr_stack_push(expr, res);
}


