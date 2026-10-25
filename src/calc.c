// Alisultan Dadashov 251ADB251
// https://github.com/AlisultanDadashov/calc-lab-home
// Compile with: gcc -O2 -Wall -Wextra -std=c17 -o calc calc.c -lm
// Resources used: <Ai used to correct some mistakes, and to improve the wording.> <
// also Ai used to fint the reasons of unknown errors.
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#define STUDENT_NAME    "alisultan"
#define STUDENT_SURNAME "dadashov"
#define STUDENT_ID      "251ADB251"



//each value has is own type, so when performing operations, it checks both operands' types and performs the operation accordingly. If both operands.
// if at least one of them is VAL_FLOAT, the calculution can be done using double,
// or it uses lonmg long to keep it as a integer
// this way calculaions stay accurate, for example 5/2 = 2, instead of 2.5
typedef enum {
    VAL_INT,
    VAL_FLOAT
} ValueType;

typedef struct {
    ValueType type;
    union {
        long long int_val;
        double float_val;
    };
} Value;

static Value make_int(long long x)  { Value v; v.type = VAL_INT;   v.int_val = x;   return v; }
static Value make_float(double x)   { Value v; v.type = VAL_FLOAT; v.float_val = x; return v; }

static double as_double(Value v) {
    return v.type == VAL_FLOAT ? v.float_val : (double)v.int_val;
}

static int is_zero(Value v) {
    return v.type == VAL_INT ? v.int_val == 0 : v.float_val == 0.0;
}

static long long int_pow(long long base, long long exp) {
    long long result = 1;
    while (exp > 0) {
        if (exp & 1) result *= base;
        exp >>= 1;
        if (exp) base *= base;
    }
    return result;
}



typedef struct {
    const char *buf;
    size_t len;
    size_t idx;
    int error_occurred;
    size_t error_pos;
} Parser;

static void set_error(Parser *parser, size_t pos) {
    if (!parser->error_occurred) {
        parser->error_occurred = 1;
        parser->error_pos = pos;
    }
}

static void skip_whitespace(Parser *parser) {
    while (parser->idx < parser->len &&
           (parser->buf[parser->idx] == ' ' || parser->buf[parser->idx] == '\t')) {
        parser->idx++;
    }
}

static char peek(Parser *p) {
    skip_whitespace(p);
    return p->idx < p->len ? p->buf[p->idx] : '\n';
}
// expr calls term, term calls powerm, power calls primary. so so tigher operators mean
// deeper.so expr/term loop (left-associative); power calls itself for the exponent
static Value parse_expr(Parser *parser);
static Value parse_term(Parser *parser);
static Value parse_power(Parser *parser);
static Value parse_primary(Parser *parser);

static Value parse_primary(Parser *p) {
    char c = peek(p);

    if (c == '(') {
        p->idx++;
        Value v = parse_expr(p);
        if (p->error_occurred) return v;
        if (peek(p) != ')') {
            // ')' reported at the end of the line where it should have been 
            set_error(p, p->idx + 1);
            return v;
        }
        p->idx++;
        return v;
    }

    if (isdigit((unsigned char)c)) {
        size_t start = p->idx;
        while (p->idx < p->len && isdigit((unsigned char)p->buf[p->idx])) p->idx++;
 // '.' needs a digit after it; otherwise it stays unread and is reported at its own
 // also i took help from ai at this part cause i couldnt find what causes the error
        if (p->idx + 1 < p->len && p->buf[p->idx] == '.' &&
            isdigit((unsigned char)p->buf[p->idx + 1])) {
            p->idx++;
            while (p->idx < p->len && isdigit((unsigned char)p->buf[p->idx])) p->idx++;

            size_t n = p->idx - start;
            char *tmp = (char *)malloc(n + 1);
            if (!tmp) { set_error(p, start + 1); return make_int(0); }
            memcpy(tmp, p->buf + start, n);
            tmp[n] = '\0';
            double d = strtod(tmp, NULL);
            free(tmp);
            return make_float(d);
        }

        long long v = 0;
        for (size_t i = start; i < p->idx; i++) v = v * 10 + (p->buf[i] - '0');
        return make_int(v);
    }

    set_error(p, p->idx + 1);
    return make_int(0);
}

static Value parse_power(Parser *p) {
    Value left = parse_primary(p);
    if (p->error_occurred) return left;

    skip_whitespace(p);
    if (p->idx + 1 < p->len && p->buf[p->idx] == '*' && p->buf[p->idx + 1] == '*') {
        size_t op_pos = p->idx + 1;
        p->idx += 2;
        skip_whitespace(p);
        size_t right_pos = p->idx + 1;
        Value right = parse_power(p);
        if (p->error_occurred) return left;

        if (left.type == VAL_INT && right.type == VAL_INT) {
            if (right.int_val < 0) {
                set_error(p, right_pos);
                return left;
            }
            return make_int(int_pow(left.int_val, right.int_val));
        }
        double r = pow(as_double(left), as_double(right));
        if (!isfinite(r)) {
            set_error(p, op_pos);
            return left;
        }
        return make_float(r);
    }
    return left;
}

static Value parse_term(Parser *p) {
    Value left = parse_power(p);
    if (p->error_occurred) return left;

    while (1) {
        char op = peek(p);
        if (op != '*' && op != '/') break;
        size_t op_pos = p->idx + 1;
        p->idx++;
        skip_whitespace(p);
        size_t right_pos = p->idx + 1;
        Value right = parse_power(p);
        if (p->error_occurred) return left;

        if (op == '/' && is_zero(right)) {
            set_error(p, right_pos);
            return left;
        }
        if (left.type == VAL_INT && right.type == VAL_INT) {
            left = make_int(op == '*' ? left.int_val * right.int_val
                                      : left.int_val / right.int_val);
        } else {
            double l = as_double(left), r = as_double(right);
            double res = (op == '*') ? l * r : l / r;
            if (!isfinite(res)) { set_error(p, op_pos); return left; }
            left = make_float(res);
        }
    }
    return left;
}

static Value parse_expr(Parser *p) {
    Value left = parse_term(p);
    if (p->error_occurred) return left;

    while (1) {
        char op = peek(p);
        if (op != '+' && op != '-') break;
        size_t op_pos = p->idx + 1;
        p->idx++;
        Value right = parse_term(p);
        if (p->error_occurred) return left;

        if (left.type == VAL_INT && right.type == VAL_INT) {
            left = make_int(op == '+' ? left.int_val + right.int_val
                                      : left.int_val - right.int_val);
        } else {
            double l = as_double(left), r = as_double(right);
            double res = (op == '+') ? l + r : l - r;
            if (!isfinite(res)) { set_error(p, op_pos); return left; }
            left = make_float(res);
        }
    }
    return left;
}

typedef struct { char *data; size_t len, cap; } Out;

//the reults are stored temporarly because if an error occur, the program only prints
// ERROR:<pos>. the buffer grows by doubling its size when needed. making it faster to add new results every time.

static int out_append(Out *o, const char *s, size_t n) {
    if (o->len + n > o->cap) {
        size_t nc = o->cap ? o->cap : 4096;
        while (nc < o->len + n) nc *= 2;
        char *nd = (char *)realloc(o->data, nc);
        if (!nd) return 0;
        o->data = nd;
        o->cap = nc;
    }
    memcpy(o->data + o->len, s, n);
    o->len += n;
    return 1;
}

static int evaluate_file(const char *in_path, const char *out_path) {
    FILE *fin = fopen(in_path, "rb");
    if (!fin) { fprintf(stderr, "Error: cannot open input file %s\n", in_path); return 0; }

    if (fseek(fin, 0, SEEK_END) != 0) { fclose(fin); return 0; }
    long size = ftell(fin);
    if (size < 0 || fseek(fin, 0, SEEK_SET) != 0) { fclose(fin); return 0; }

    char *buf = (char *)malloc((size_t)size + 1);
    if (!buf) { fclose(fin); return 0; }
    size_t n = fread(buf, 1, (size_t)size, fin);
    fclose(fin);

    Parser p = {buf, n, 0, 0, 0};
    Out out = {NULL, 0, 0};
    int ok = 1;

    size_t pos = 0;
    while (pos < n) {
        p.idx = pos;
        char c = peek(&p);
        if (c != '\n' && c != '#') {
            Value v = parse_expr(&p);
            if (!p.error_occurred && peek(&p) != '\n')
                set_error(&p, p.idx + 1);
            if (p.error_occurred) break;
            char tmp[64];
            int k = (v.type == VAL_INT) ? snprintf(tmp, sizeof tmp, "%lld\n", v.int_val)
                                        : snprintf(tmp, sizeof tmp, "%.15g\n", v.float_val);
            if (!out_append(&out, tmp, (size_t)k)) { ok = 0; break; }
        }
        while (p.idx < n && buf[p.idx] != '\n') p.idx++;
        pos = p.idx + 1;
    }

    if (ok) {
        FILE *fout = fopen(out_path, "w");
        if (!fout) {
            fprintf(stderr, "Error: cannot write output file %s\n", out_path);
            ok = 0;
        } else {
            if (p.error_occurred) fprintf(fout, "ERROR:%zu\n", p.error_pos);
            else if (out.len > 0) fwrite(out.data, 1, out.len, fout);
            if (fclose(fout) != 0) ok = 0;
        }
    } else {
        fprintf(stderr, "Error: out of memory while evaluating %s\n", in_path);
    }
    free(out.data);
    free(buf);
    return ok;
}

static void get_basename_no_ext(const char *path, char *dest, size_t max_len) {
    size_t end = strlen(path);
    while (end > 1 && path[end - 1] == '/') end--;
    size_t start = end;
    while (start > 0 && path[start - 1] != '/') start--;
    size_t len = end - start;
    if (len >= max_len) len = max_len - 1;
    memcpy(dest, path + start, len);
    dest[len] = '\0';
    if (len > 4 && strcmp(dest + len - 4, ".txt") == 0) dest[len - 4] = '\0';
}

static int ensure_dir(const char *path) {
    struct stat st;
    if (stat(path, &st) == 0) return S_ISDIR(st.st_mode);
    return mkdir(path, 0777) == 0;
}

int main(int argc, char *argv[]) {
    const char *input_arg = NULL, *out_arg = NULL, *dir_arg = NULL;

    for (int i = 1; i < argc; i++) {
        const char *a = argv[i];
        if (strcmp(a, "-d") == 0 || strcmp(a, "--dir") == 0) {
            if (i + 1 >= argc) { fprintf(stderr, "Error: %s needs a value\n", a); return 1; }
            dir_arg = argv[++i];
        } else if (strcmp(a, "-o") == 0 || strcmp(a, "--output-dir") == 0) {
            if (i + 1 >= argc) { fprintf(stderr, "Error: %s needs a value\n", a); return 1; }
            out_arg = argv[++i];
        } else if (a[0] == '-') {
            fprintf(stderr, "Error: unknown option %s\n", a);
            return 1;
        } else {
            input_arg = a;
        }
    }
    if (!dir_arg && !input_arg) {
        fprintf(stderr, "Usage: calc [-d DIR | --dir DIR] [-o OUTDIR | --output-dir OUTDIR] [input.txt]\n");
        return 1;
    }

    struct stat st;
    const char *src = dir_arg ? dir_arg : input_arg;
    if (stat(src, &st) != 0) {
        fprintf(stderr, "Error: cannot find %s\n", src);
        return 1;
    }

    char base[256], outdir[1024];
    get_basename_no_ext(src, base, sizeof base);
    if (out_arg) snprintf(outdir, sizeof outdir, "%s", out_arg);
    else snprintf(outdir, sizeof outdir, "%s_%s_%s", base, STUDENT_NAME, STUDENT_ID);

    int status = 0;

    if (dir_arg) {
        DIR *dir = opendir(dir_arg);
        if (!dir) { fprintf(stderr, "Error: cannot open directory %s\n", dir_arg); return 1; }
        if (!ensure_dir(outdir)) {
            fprintf(stderr, "Error: cannot create output folder %s\n", outdir);
            closedir(dir);
            return 1;
        }
        struct dirent *e;
        while ((e = readdir(dir)) != NULL) {
            size_t len = strlen(e->d_name);
            if (len <= 4 || strcmp(e->d_name + len - 4, ".txt") != 0) continue;

            char in_path[1024];
            snprintf(in_path, sizeof in_path, "%s/%s", dir_arg, e->d_name);
            struct stat fst;
            if (stat(in_path, &fst) != 0 || !S_ISREG(fst.st_mode)) continue;

            char fbase[256], out_path[2048];
            get_basename_no_ext(e->d_name, fbase, sizeof fbase);
            snprintf(out_path, sizeof out_path, "%s/%s_%s_%s_%s.txt",
                     outdir, fbase, STUDENT_NAME, STUDENT_SURNAME, STUDENT_ID);
            if (!evaluate_file(in_path, out_path)) status = 1;
        }
        closedir(dir);
    } else {
        if (!ensure_dir(outdir)) {
            fprintf(stderr, "Error: cannot create output folder %s\n", outdir);
            return 1;
        }
        char out_path[2048];
        snprintf(out_path, sizeof out_path, "%s/%s_%s_%s_%s.txt",
                 outdir, base, STUDENT_NAME, STUDENT_SURNAME, STUDENT_ID);
        if (!evaluate_file(input_arg, out_path)) status = 1;
    }
    return status;
}