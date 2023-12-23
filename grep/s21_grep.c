#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 999999

struct flags {
  int flag_e;
  int flag_i;
  int flag_v;
  int flag_c;
  int flag_l;
  int flag_n;
  int flag_h;
  int flag_s;
  int flag_f;
  int flag_o;
};
struct flags flag = {0};
struct flags *p_flags = &flag;

void pars(int argc, char *argv[], char *pattern);
extern void switchCase(int val, char *pattern);
extern void parser(char *pattern, char *argv[]);
extern void work_file(char *argv[], int var, int *num_files, char *pattern);
extern void patt_f(char *pattern);
extern void count_files(char *argv[], int *num_files, int var);
extern void check_flag(int number_files, char *argv[], char *buffer,
                       int num_str, int numberArg, char *pattern);
extern void flag_n(int num_str, int n);
extern void flags_c(int testFiles, char *nameFile, int num_str);
extern void flag_f(char *fileName, char *pattern);
extern void flag_o(char *buffer, char *pattern);
extern void flag_l(char *nameFile);

int main(int argc, char *argv[]) {
  int returnResult = 0;
  char pattern[SIZE] = {0};
  if ((argc > 2) && (returnResult != 1)) {
    pars(argc, argv, pattern);
  }
  return 0;
}

void pars(int argc, char *argv[], char *pattern) {
  int testArg = 1;
  int i;
  while ((i = getopt_long(argc, argv, "e:ivclnhsf:o?", 0, NULL)) != -1) {
    switchCase(i, pattern);
    if (flag.flag_e && argc == 2) {
      testArg = 0;
      break;
    }
  }
  if ((flag.flag_e || flag.flag_f) && argc < 4) {
    testArg = 0;
  }

  if ((!flag.flag_e && !flag.flag_f) && argc < 3) {
    testArg = 0;
  }

  if (testArg) {
    parser(pattern, argv);
  }
}

void switchCase(int val, char *pattern) {
  switch (val) {
    case 'e':
      p_flags->flag_e = 1;
      patt_f(pattern);
      break;
    case 'i':
      p_flags->flag_i = 1;
      break;
    case 'v':
      p_flags->flag_v = 1;
      break;
    case 'c':
      p_flags->flag_c = 1;
      break;
    case 'l':
      p_flags->flag_l = 1;
      break;
    case 'n':
      p_flags->flag_n = 1;
      break;
    case 'h':
      p_flags->flag_h = 1;
      break;
    case 's':
      p_flags->flag_s = 1;
      break;
    case 'f':
      p_flags->flag_f = 1;
      flag_f(optarg, pattern);
      break;
    case 'o':
      p_flags->flag_o = 1;
      break;
    case '?':
    default:
      perror("n/a");
      exit(1);
  }
}

void parser(char *pattern, char *argv[]) {  //выбор варианта сборки
  int var = 0;
  int num_files = 0;                  //кол-во файлов
  if (!flag.flag_e & !flag.flag_f) {  // если нет шаблона или файла для
                                      // паттерна, то стандарт сборка
    pattern = argv[optind];
    var = 1;
    count_files(argv, &num_files, var);  // посчет кол-ва файлов
  }
  if (!var) {  // в др случае сборка по готов патт
    count_files(argv, &num_files, var);
  }
  work_file(argv, var, &num_files, pattern);
}

void work_file(char *argv[], int var, int *num_files,
               char *pattern) {  //отработка по файлам
  for (int i = 0; i < *num_files; i++) {
    FILE *fp;
    regex_t re;
    int test_l = 0;           //тест флага l
    int num_str_fl_C = 0;     //кол-во совпадений для c
    int num_str = 1;          //кол-во строк
    char buffer[SIZE] = {0};  //для записи строки
    if (flag.flag_i) {
      regcomp(&re, pattern, REG_EXTENDED | REG_ICASE);
    } else {
      regcomp(&re, pattern, REG_EXTENDED);
    }
    if ((fp = fopen(argv[optind + var + i], "r")) != NULL) {
      while (fgets(buffer, SIZE, fp) != NULL) {
        if (regexec(&re, buffer, 0, NULL, 0) == 0 && flag.flag_v == 0) {
          if (flag.flag_c && flag.flag_l) {
            num_str_fl_C = 1;
          } else if (flag.flag_c && !flag.flag_l) {
            num_str_fl_C++;
          }
          if (!flag.flag_l) {
            check_flag(*num_files, argv, buffer, num_str, optind + var + i,
                       pattern);
          } else {
            test_l = 1;
          }
        } else if (regexec(&re, buffer, 0, NULL, 0) != 0 && flag.flag_v) {
          if (flag.flag_c && flag.flag_l) {
            num_str_fl_C = 1;
          } else if (flag.flag_c && !flag.flag_l) {
            num_str_fl_C++;
          }
          if (!flag.flag_l) {
            check_flag(*num_files, argv, buffer, num_str, optind + var + i,
                       pattern);
          } else {
            test_l = 1;
          }
        }
        num_str++;
      }
      if (flag.flag_c) {
        flags_c(*num_files, argv[optind + var + i], num_str_fl_C);
      }
      if (flag.flag_l && test_l) {
        flag_l(argv[optind + var + i]);
      }
      fclose(fp);
    }
    regfree(&re);
  }
}

void patt_f(char *pattern) {
  if (pattern[0] == 0) {
    strcat(pattern, optarg);
  } else {
    strcat(pattern, "|");
    strcat(pattern, optarg);
  }
}

void count_files(char *argv[], int *num_files, int var) {  //счетчик файлов
  if (var) {
    for (int i = optind + 1; argv[i] != NULL; i++) {
      if (argv[i][0] != '-' ||
          (argv[i - 1][0] != '-' && argv[i - 1][1] != 'f')) {
        *num_files = *num_files + 1;
      }
    }
  } else if (!var) {
    for (int i = optind; argv[i] != NULL; i++) {
      if (argv[i][0] != '-' ||
          (argv[i - 1][0] != '-' && argv[i - 1][1] != 'f')) {
        *num_files = *num_files + 1;
      }
    }
  }
}

void check_flag(int number_files, char *argv[], char *buffer, int num_str,
                int numberArg, char *pattern) {
  if (flag.flag_c == 0) {  // проверка на флаг С
    if (number_files > 1 &&
        flag.flag_h ==
            0) {  // если больше одного файла, то имя файла выводится в stdout
      printf("%s:", argv[numberArg]);
    }
    flag_n(num_str, flag.flag_n);  //если флаг n, то выводится номер строки

    if (flag.flag_o && flag.flag_v == 0) {  // проверка на флаг О
      flag_o(buffer, pattern);
    } else {
      fputs(buffer, stdout);  // если успешно, ты выводим строку
    }
    if (buffer[strlen(buffer)] == '\0' &&
        buffer[strlen(buffer) - 1] !=
            '\n') {  // если последняя строка, то делаем перенос
      printf("\n");
    }
  }
}

void flag_n(int num_str, int n) {  // работа флага n
  if (n) {
    printf("%d:", num_str);
  }
}

void flags_c(int testFiles, char *nameFile, int num_str) {  //работа флага c
  if (testFiles > 1 && flag.flag_h == 0) {
    printf("%s:%d\n", nameFile, num_str);
  } else {
    printf("%d\n", num_str);
  }
}

void flag_f(char *fileName, char *pattern) {  //работа флага f
  FILE *fp;
  char buffer[SIZE];
  if ((fp = fopen(fileName, "r")) == NULL) {
    if (flag.flag_s == 0) {
      perror("n/a");
    }
  }
  while (fgets(buffer, SIZE, fp) != NULL) {
    if (buffer[strlen(buffer)] == '\0' && buffer[strlen(buffer) - 1] == '\n') {
      buffer[strlen(buffer) - 1] = '\0';
    }
    if (pattern[0] == 0) {
      strcat(pattern, buffer);
    } else {
      strcat(pattern, "|");
      strcat(pattern, buffer);
    }
  }
  if (flag.flag_e) {
    pattern[strlen(pattern)] = '\0';
  } else if (!flag.flag_e) {
    pattern[strlen(pattern) - 1] = '\0';
  }
  fclose(fp);
}

void flag_o(char *buffer, char *pattern) {  //работа флага o
  regex_t re;
  regmatch_t pmatch[4024];
  int status = 1;
  char *s = buffer;

  if (flag.flag_i) {
    if ((status = regcomp(&re, pattern, REG_EXTENDED | REG_ICASE)) != 0) {
      printf("n/a %d", status);
      regfree(&re);
    }
  } else if (!flag.flag_i) {
    if ((status = regcomp(&re, pattern, REG_EXTENDED)) != 0) {
      printf("n/a %d", status);
      regfree(&re);
    }
  }

  if (status == 0 && flag.flag_v == 0) {
    for (int i = 0; buffer[i] != '\0'; i++) {
      if (regexec(&re, s, 1, pmatch, 0) != flag.flag_v) {
        break;
      }
      printf("%.*s\n", (int)(pmatch[0].rm_eo - pmatch[0].rm_so),
             s + pmatch[0].rm_so);
      s += pmatch[0].rm_eo;
    }
  }
  regfree(&re);
}

void flag_l(char *nameFile) {  //работа флага l
  printf("%s\n", nameFile);
}
