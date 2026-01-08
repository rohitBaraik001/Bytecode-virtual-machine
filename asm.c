#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    const char *mnemonic;
    int opcode;
    int has_operand;
} Instr;

static Instr table[] = {
    {"PUSH",  1,   1}, {"POP",   2,   0}, {"DUP",  3,   0},
    {"ADD",  16,  0}, {"SUB",  17,  0}, {"MUL",  18,  0},
    {"DIV",  19,  0}, {"CMP",  20,  0},
    {"JMP",  32,  1}, {"JZ",   33,  1}, {"JNZ",  34,  1},
    {"STORE",48,  1}, {"LOAD", 49,  1},
    {"CALL", 64,  1}, {"RET",  65,  0},
    {"HALT", 255, 0},
    {NULL,    0,   0}
};

#define MAX_LABELS 512
#define MAX_NAME   64

typedef struct {
    char name[MAX_NAME];
    int addr; /* index in int bytecode array */
} Label;

static Label labels[MAX_LABELS];
static int label_count = 0;

static void die(const char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

static void strip_comment(char *line) {
    for (int i = 0; line[i]; i++) {
        if (line[i] == ';' || line[i] == '#') {
            line[i] = '\0';
            return;
        }
    }
}

static char *ltrim(char *s) {
    while (*s && isspace((unsigned char)*s)) s++;
    return s;
}

static void rtrim_inplace(char *s) {
    int n = (int)strlen(s);
    while (n > 0 && isspace((unsigned char)s[n - 1])) {
        s[n - 1] = '\0';
        n--;
    }
}

static int is_blank(const char *s) {
    for (; *s; s++) {
        if (!isspace((unsigned char)*s)) return 0;
    }
    return 1;
}

static int is_label_token(const char *tok) {
    size_t n = strlen(tok);
    return (n >= 2 && tok[n - 1] == ':');
}

static void label_name_from_token(const char *tok, char out[MAX_NAME]) {
    size_t n = strlen(tok);
    if (n - 1 >= MAX_NAME) die("Label name too long");
    memcpy(out, tok, n - 1);
    out[n - 1] = '\0';
}

static int find_instr(const char *mnemonic, Instr *out) {
    for (int i = 0; table[i].mnemonic; i++) {
        if (strcmp(mnemonic, table[i].mnemonic) == 0) {
            if (out) *out = table[i];
            return 1;
        }
    }
    return 0;
}

static int find_label_addr(const char *name) {
    for (int i = 0; i < label_count; i++) {
        if (strcmp(labels[i].name, name) == 0) {
            return labels[i].addr;
        }
    }
    return -1;
}

static void add_label(const char *name, int addr) {
    if (find_label_addr(name) != -1) {
        fprintf(stderr, "Duplicate label: %s\n", name);
        exit(1);
    }
    if (label_count >= MAX_LABELS) die("Too many labels");

    strncpy(labels[label_count].name, name, MAX_NAME - 1);
    labels[label_count].name[MAX_NAME - 1] = '\0';
    labels[label_count].addr = addr;
    label_count++;
}

static int next_token(char **p, char tok[128]) {
    char *s = *p;
    s = ltrim(s);
    if (*s == '\0') return 0;

    int k = 0;
    while (*s && !isspace((unsigned char)*s)) {
        if (k < 127) tok[k++] = *s;
        s++;
    }
    tok[k] = '\0';
    *p = s;
    return 1;
}

static int parse_int_strict(const char *s, int *out) {
    char *end = NULL;
    long v = strtol(s, &end, 10);
    if (s[0] == '\0') return 0;
    if (end == NULL || *end != '\0') return 0;
    *out = (int)v;
    return 1;
}

static void pass1_collect_labels(FILE *fp) {
    char line[512];
    int out_index = 0; 

    while (fgets(line, sizeof(line), fp)) {
        strip_comment(line);
        rtrim_inplace(line);
        char *p = ltrim(line);
        if (*p == '\0' || is_blank(p)) continue;

        
        char tok[128];
        while (next_token(&p, tok)) {
            if (is_label_token(tok)) {
                char lname[MAX_NAME];
                label_name_from_token(tok, lname);
                add_label(lname, out_index);
                continue;
            }
            Instr ins;
            if (!find_instr(tok, &ins)) {
                fprintf(stderr, "Unknown instruction in pass1: %s\n", tok);
                exit(1);
            }

            out_index += 1; 
            if (ins.has_operand) {
                char op[128];
                if (!next_token(&p, op)) {
                    fprintf(stderr, "Missing operand for %s\n", ins.mnemonic);
                    exit(1);
                }
                out_index += 1;
            }
            break;
        }
    }
}

static void pass2_emit(FILE *fp, FILE *out) {
    char line[512];

    while (fgets(line, sizeof(line), fp)) {
        strip_comment(line);
        rtrim_inplace(line);
        char *p = ltrim(line);
        if (*p == '\0' || is_blank(p)) continue;

        char tok[128];
        while (next_token(&p, tok)) {
            if (is_label_token(tok)) {
                continue;
            }
            Instr ins;
            if (!find_instr(tok, &ins)) {
                fprintf(stderr, "Unknown instruction: %s\n", tok);
                exit(1);
            }
            fprintf(out, "%d ", ins.opcode);
            if (ins.has_operand) {
                char op[128];
                if (!next_token(&p, op)) {
                    fprintf(stderr, "Missing operand for %s\n", ins.mnemonic);
                    exit(1);
                }

                int val;
                if (parse_int_strict(op, &val)) {
                    fprintf(out, "%d ", val);
                } else {
                    int addr = find_label_addr(op);
                    if (addr == -1) {
                        fprintf(stderr, "Undefined label: %s\n", op);
                        exit(1);
                    }
                    fprintf(out, "%d ", addr);
                }
            }

            break;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s input.asm output.bc\n", argv[0]);
        return 1;
    }

    FILE *in1 = fopen(argv[1], "r");
    if (!in1) {
        perror("failed to open input.asm");
        return 1;
    }
    label_count = 0;
    pass1_collect_labels(in1);
    fclose(in1);

    FILE *in2 = fopen(argv[1], "r");
    FILE *out = fopen(argv[2], "w");
    if (!in2 || !out) {
        perror("file error");
        return 1;
    }

    pass2_emit(in2, out);

    fclose(in2);
    fclose(out);
    return 0;
}