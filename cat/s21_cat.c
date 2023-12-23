#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int print_file(char *name, char *flags);
void flags_parser(char *flags, int argc, char **argv, int *index);
struct s_avi_flags {
  char flag;               // флаг
  char *equivalent_flags;  // простые флаги
};
void append_flags(char *flags, char flag);
void append_flag(char *flags, char flag);
typedef int bool;
int print_file(char *name, char *flags);
void print_symb(int c, int *prev, char *flags, int *index, bool *eline_printed);

int main(int argc, char **argv) {
  char flags[8] = "\0";  // массив флагов
  int index_end_flags = 1;
  flags_parser(flags, argc, argv, &index_end_flags);

  if (index_end_flags == argc - 1) {
    print_file("-", flags);
  }
  for (int i = index_end_flags + 1; i < argc; i++) {
    if (strcmp(argv[i], "--") == 0) continue;
    print_file(argv[i], flags);
  }
  return 0;
}

int print_file(char *name, char *flags) {
  int err_code = 0;
  FILE *f;
  if (strcmp("-", name) == 0)
    f = stdin;  // если имя файла "-" работаем с stdin
  else
    f = fopen(name, "rt");

  if (f != NULL) {
    int index = 0;
    bool eline_printed = 0;
    int c = fgetc(f), prev = '\n';
    while (c != EOF) {
      print_symb(c, &prev, flags, &index, &eline_printed);
      c = fgetc(f);
    }
    if (f != stdin) fclose(f);  // закрываем файл только если он не stdin
  } else {
    err_code = 1;
  }
  return err_code;
}

void flags_parser(char *flags, int argc, char **argv, int *index) {
  for (int i = 1; i < argc; i++) {  //прох по всем арг кр 1
    if (argv[i][0] != '-' ||
        strcmp(argv[i], "--") == 0 ||  //если нач не с -, или яв строкой - или
                                       //-- -> флагов больше нет, сл файлы
        strcmp(argv[i], "-") == 0) {
      break;
    } else {
      *index = i;
      for (size_t j = 1; j < strlen(argv[i]);
           j++) {  //прох по всем арг, кроме 1, т.к. флаги могут быть слитно
        append_flags(flags, argv[i][j]);
      }
    }
  }
}

void append_flags(char *flags, char flag) {  //переч флагов
  // int err_code = 1; //по кд
  struct s_avi_flags avi_flags[8] = {{'b', "b"},  {'E', "E"}, {'e', "Ev"},
                                     {'n', "n"},  {'s', "s"}, {'T', "T"},
                                     {'t', "Tv"}, {'v', "v"}};

  for (int i = 0; i < 8; i++) {
    if (avi_flags[i].flag == flag) {
      for (size_t j = 0; j < strlen(avi_flags[i].equivalent_flags); j++) {
        append_flag(flags, avi_flags[i].equivalent_flags[j]);
      }
      // err_code = 0; //меняем на 0
      break;
    }
  }
}

void append_flag(char *flags, char flag) {  //добавление 1 флага в их массив

  if (strchr(flags, flag) == NULL) {  // если такого флага нет
    char temp[2] = "-";  // создаем временную строку
    temp[0] = flag;      // например ['v', '\0']
    strcat(flags, temp);  // объеденяем её со строкой флагов
  }
}

void print_symb(int c, int *prev, char *flags, int *index,
                bool *eline_printed) {
  if (!(strchr(flags, 's') != NULL && *prev == '\n' && c == '\n' &&
        *eline_printed)) {  // если s и это пустая строка и пустая строка уже
                            // была выведена, пропустим

    if (*prev == '\n' && c == '\n')
      *eline_printed = 1;
    else
      *eline_printed = 0;

    if (((strchr(flags, 'n') != NULL && strchr(flags, 'b') == NULL) ||
         (strchr(flags, 'b') != NULL && c != '\n')) &&
        *prev == '\n') {  // если ( (n без b) или (b и текущий символ не '\n') )
                          // и пред символ '\n'
      *index += 1;
      printf("%6d\t", *index);
    }

    if (strchr(flags, 'E') != NULL && c == '\n') printf("$");
    if (strchr(flags, 'T') != NULL && c == '\t') {
      printf("^");
      c = '\t' + 64;
    }
    if (strchr(flags, 'v') != NULL) {
      if ((c > 127) && (c <= 159)) printf("M-^");
      if (((c < 32) && c != '\n' && c != '\t') || (c == 127)) printf("^");
      if (((c < 32) || (c > 126 && c <= 159)) && c != '\n' && c != '\t') {
        if (c > 126)
          c = c - 128 + 64;
        else
          c += 64;
      }
    }
    fputc(c, stdout);
  }
  *prev = c;
}
