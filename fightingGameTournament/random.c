#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_GAMES 64
#define LINE_BUFFER_SIZE 256

//Portable RNG

#define PMrand_a 16807
#define PMrand_m 2147483647
#define PMrand_q (PMrand_m / PMrand_a)
#define PMrand_r (PMrand_m % PMrand_a)

static long int seed = 1;

double PMrand() {
	long int hi = seed / PMrand_q;
	long int lo = seed % PMrand_q;
	long int test = PMrand_a * lo - PMrand_r * hi;
	if (test > 0)
		seed = test;
	else
		seed = test + PMrand_m;
	return (double)seed / PMrand_m;
}

//Portable getline

size_t getline(char **lineptr, size_t *n, FILE *stream) {
    char *bufptr = NULL;
    char *p = bufptr;
    size_t size;
    int c;

    if (lineptr == NULL) {
        return -1;
    }
    if (stream == NULL) {
        return -1;
    }
    if (n == NULL) {
        return -1;
    }
    bufptr = *lineptr;
    size = *n;

    c = fgetc(stream);
    if (c == EOF) {
        return -1;
    }
    if (bufptr == NULL) {
        bufptr = malloc(128);
        if (bufptr == NULL) {
            return -1;
        }
        size = 128;
    }
    p = bufptr;
    while(c != EOF) {
        if ((p - bufptr) > (ssize_t)(size - 1)) {
            size = size + 128;
            bufptr = realloc(bufptr, size);
            if (bufptr == NULL) {
                return -1;
            }
        }
        *p++ = c;
        if (c == '\n') {
            break;
        }
        c = fgetc(stream);
    }

    *p++ = '\0';
    *lineptr = bufptr;
    *n = size;

    return p - bufptr - 1;
}

char default_file [] = "input.txt";
char usage [] = " [input file]\n";
char startup_message [] = "Starting things up\n";
char main_loop [] = 
"\nFIGHTING TOURNAMENT RANDOMIZER\nSelect an option:\nList    - List all games and their current chances\nRandom  - Select the next game\nForce # - Force the next game to be game #\nExit    - The tournament is over\n\n";

typedef struct {
	char   *name;
	double  chance;
} game;

game gl[MAX_GAMES];
int games = 0;

char *parse_cmd (int argc, char **argv) {
	printf("%s", startup_message);
	char *file = NULL;
	if (argc == 1) {
		file = default_file;
	} else if (argc == 2) {
		file = argv[1];
	} else {
		printf("usage:\n %s%s", argv[0], usage);
		exit(0);
	}
	return file;
}

void force (int n) {
	printf("THE NEXT GAME IS\n%s\n", gl[n].name);	
	double add = (gl[n].chance - ((double) 1)/(games*4)) / (games-1);
	for (int i=0; i<games; i++) {
		if (i == n)
			continue;
		gl[i].chance += add;
	}
	gl[n].chance = ((double) 1)/(games*4);
}

void run () {
	double r = PMrand();
	for (int i=0; i<games; i++) {
		if (gl[i].chance > r) {
			force(i);
			return;
		}
		else
			r -= gl[i].chance;
	}
	run();
}

void list () {
	for (int i=0; i<games; i++) {
		printf("%i   %f%%   %s\n", i+1, gl[i].chance * 100, gl[i].name);
	}
}

int main (int argc, char **argv) {
	char *file = parse_cmd(argc, argv);

	printf("Using input file: %s\n",file);

	FILE *in = fopen(file, "r");
	if (in == NULL) {
		perror("Failed to open file");
		return 1;
	}

	char line[LINE_BUFFER_SIZE];

	while (fgets(line, LINE_BUFFER_SIZE, in)) {
		if (games == MAX_GAMES) {
			printf("Too many games: max games %i\n", MAX_GAMES);
			return 1;
		}
		gl[games].name = malloc(strlen(line)+1);
		if (line[strlen(line)-1] == '\n')
			line[strlen(line)-1] = '\0';
		strcpy(gl[games].name,line);
		games++;
	}

	for (int i=0; i<games; i++) {
		gl[i].chance = ((double) 1)/games;
	}

	char *l = NULL;
	size_t n = 0;

	seed = time(NULL);

	while (1) {
		printf ("%s", main_loop);

		size_t res = getline(&l, &n, stdin);

		printf("\n");

		if (res == 0) {
			printf("\n%s THAT AIN'T AN OPTION BRUH\n", l);
			continue;
		}

		for(int i = 0; l[i]; i++) {
			l[i] = tolower(l[i]);
			if (l[i] == '\n')
				l[i] = '\0';
		}

		if (!strcmp(l, "list") || !strcmp(l, "l")) {
			list();
			continue;
		}

		if (!strcmp(l, "random") || !strcmp(l, "r")) {
			run();
			continue;
		}

		if (!strncmp(l, "force ", 6)) {
			char c = '\0';
			char *d = &c;
			int i = (int) strtol(l+6, &d, 10);
			if (*d != '\0' || i < 1 || i > games) {
				printf("Invalid number\n");
				continue;
			}
			force(i-1);
			continue;
		}

		if (!strncmp(l, "f ", 2)) {
			char c = '\0';
			char *d = &c;
			int i = (int) strtol(l+2, &d, 10);
			if (*d != '\0' || i < 1 || i > games) {
				printf("Invalid number\n");
				continue;
			}
			force(i-1);
			continue;
		}

		if (!strcmp(l, "exit")) {
			break;
		}

		printf("\n%s THAT AIN'T AN OPTION BRUH\n", l);
	}

	return 0;
}