#include "print.h"
#include "list.h"
#include "bool.h"
#include "env.h"
#include "scm.h"
#include "pcc32.h"

enum {
    DISPLAY = 0,
    WRITE = 1
};

static scm_object* write_prim(int, scm_object *[]);
static scm_object* display_prim(int, scm_object *[]);
static scm_object* newline_prim(int, scm_object *[]);

static void write(scm_object *port, scm_object *, int);
static void write_list(scm_object *port, scm_object *, int);

void scm_init_print(scm_env *env)
{
    scm_add_prim(env, "write", write_prim, 1, 2);
    scm_add_prim(env, "display", display_prim, 1, 2);
}

void scm_write(scm_object *port, scm_object *obj)
{
    write(port, obj, WRITE);
}

void scm_display(scm_object *port, scm_object *obj)
{
    write(port, obj, DISPLAY);
}

static scm_object* write_prim(int argc, scm_object *argv[])
{
    write(scm_stdout_port, argv[0], WRITE);
    return scm_void;
}

static scm_object* display_prim(int argc, scm_object *argv[])
{
    write(scm_stdout_port, argv[0], DISPLAY);
    return scm_void;
}

static void write(scm_object *port, scm_object *obj, int notdisplay)
{
    FILE* f = ((scm_output_port *)port)->f;// TODO:

    int oc = getTextColor();
    setTextColor(notdisplay ? LIGHT_BLUE : LIGHT_MAGENTA);

    switch (SCM_TYPE(obj)) {
        case scm_true_type:
            fprintf(f, "#t");
            break;
        case scm_false_type:
            fprintf(f, "#f");
            break;
        case scm_integer_type:
            fprintf(f, "%ld", SCM_INT_VAL(obj));
            break;
        case scm_float_type:
            fprintf(f, "%lf", SCM_FLOAT_VAL(obj));
            break;
        case scm_char_type:
            if (notdisplay) {
                switch (SCM_CHAR_VAL(obj)) {
                    case '\n':
                        fprintf(f, "#\\newline");
                        break;
                    case ' ':
                        fprintf(f, "#\\space");
                        break;
                    default:
                        fprintf(f, "#\\%c", SCM_CHAR_VAL(obj));
                }
            } else {
                fprintf(f, "%c", SCM_CHAR_VAL(obj));
            }
            break;
        case scm_string_type:
            if (notdisplay) {
                fprintf(f, "\"");
                char *str = SCM_CHAR_STR_VAL(obj);
                char *sc;
                char *ps;
                for (sc = str; *sc; sc++) {
                    ps = NULL;
                    switch (*sc) {
                        case '\a': ps = "\\a"; break;
                        case '\b': ps = "\\b"; break;
                        case '\f': ps = "\\f"; break;
                        case '\n': ps = "\\n"; break;
                        case '\r': ps = "\\r"; break;
                        case '\t': ps = "\\t"; break;
                        case '\v': ps = "\\v"; break;
                    }
                    if (ps == NULL)
                        fprintf(f, "%c", *sc);
                    else
                        fprintf(f, "%s", ps);
                }
                fprintf(f, "\"");
            }
            else
                fprintf(f, "%s", SCM_CHAR_STR_VAL(obj));
            break;
        case scm_symbol_type:
            fprintf(f, "%s", SCM_SYMBOL_STR_VAL(obj));
            break;
        case scm_pair_type:
            write_list(port, obj, notdisplay);
            break;
        case scm_null_type:
            fprintf(f, "()");
            break;
        case scm_primitive_type:
            fprintf(f, "#<procedure:%s>", ((scm_primitive_proc *)obj)->name);
            break;
        case scm_compound_type:
            fprintf(f, "#<procedure:%s>", ((scm_compound_proc *)obj)->name);
            break;
        case scm_namespace_type:
            fprintf(f, "#<namespace>");
            break;
        case scm_void_type:
            break;
        default: ;
    }

    setTextColor(oc);
}

static void write_list(scm_object *port, scm_object *list, int notdisplay)
{
    FILE* f = ((scm_output_port *)port)->f;// TODO:

    fprintf(f, "(");
    while (!SCM_NULLP(list)) {
        if (SCM_PAIRP(list)) {
            write(port, SCM_CAR(list), notdisplay);
            if (!SCM_NULLP(SCM_CDR(list))) {
                fprintf(f, " ");
                list = SCM_CDR(list);
            } else {
                list = scm_null;
            }
        } else {
            fprintf(f, ". ");
            write(port, list, notdisplay);
            list = scm_null;
        }
    }
    fprintf(f, ")");
}