#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
#define clrscr() system("cls")
#else
#define clrscr() system("clear")
#endif


struct params {
    int start_time;
    int target_time;
    int end_time;
    int update_display;
};

void display_usage() {
    printf("Usage: wtdisplay [-s <t>] [-t <t>] [-e <t>]\n");
    printf("Options:\n");
    printf(" -h     Print help page\n");
    printf(" -s     Start of work (Default 6:00)\n");
    printf(" -t     Work time target\n");
    printf(" -e     End of work\n");
    printf(" -i     Display update interval (Default: 0)\n");
}

/* Function: graceful_exit
 * -----------------------
 * End the program while either printing an error message into stderr, or
 * display the program's help page.
 *
 * error_message | pointer to a string that contains an error message
 */
void graceful_exit(char *error_message) {
    if (error_message) {
        fprintf(stderr, "%s", error_message);
    } else {
        display_usage();
    }

    exit(EXIT_FAILURE);
}


int netto_work_time(int brutto, char *target_break_info) {
    int seconds = brutto;
    if (brutto > 60 * 6 * 60) {
        seconds -= 30 * 60;
    }

    if (brutto > 60 * 9 * 60) {
        seconds -= 15 * 60;
    }

    if (seconds < 6 * 60 * 60) {
        target_break_info = "; +30min";
    }

    return seconds;
}

void estimated_departure(struct tm start_time, int target_time) {
    struct tm estimated_departure = start_time;

    estimated_departure.tm_sec += target_time + (60 * 30);
    mktime(&estimated_departure);
    printf("Estimated departure: %d:%d\n\n", estimated_departure.tm_hour,
           estimated_departure.tm_min);
}

void print_brutto(int brutto_seconds) {
    printf("Work time (breaks included): %d:%dh\n",
           brutto_seconds / 60 / 60,
           ((brutto_seconds / 60) % 60));
}

void print_netto(int netto_seconds, char *target_diff_f) {
    printf("Work time: %d:%dh (%s)\n", netto_seconds / 60 / 60,
           ((netto_seconds / 60) % 60), target_diff_f);
}

void print_diff(int target_diff, char *target_diff_f, char *target_break) {
    sprintf(target_diff_f, "%s%d:%dh%s", target_diff > 0 ? "+" : "-",
            abs(target_diff / 60 / 60), abs((target_diff / 60) % 60), target_break);
}

void update_display(struct params flags, char *input_time, struct tm start_time, struct tm time_now, int target_time) {
    int netto_seconds, brutto_seconds, target_diff;
    char *target_break = "";
    char target_diff_f[13];
    do {
        clrscr();
        printf("Work started at: %s\n", input_time);
        printf("Target time: 7:48h\n");

        // Calculate actual time at work
        brutto_seconds = (int) difftime(mktime(&time_now), mktime(&start_time));
        netto_seconds = netto_work_time(brutto_seconds, target_break);
        target_diff = netto_seconds - target_time;

        estimated_departure(start_time, target_time);
        print_diff(target_diff, target_diff_f, target_break);
        print_netto(netto_seconds, target_diff_f);
        print_brutto(brutto_seconds);
        sleep(flags.update_display);
    } while (flags.update_display != 0);
}


int main(int argc, char **argv) {
    int option;
    int hh, mm, ss;
    hh = mm = ss = 0;
    char input_time[] = "6:00";

    time_t now;
    time(&now);

    struct tm start_time = *localtime(&now);
    struct tm time_now = *localtime(&now);

    int target_time = ((7 * 60) + 48) * 60;

    struct params flags = {0, 0, 0, 0};

    while ((option = getopt(argc, argv, "s:t:e:i:h")) != -1) {
        switch (option) {
            case 's':
                strcpy(input_time, optarg);
                flags.start_time = 1;
                break;
            case 't':
                flags.target_time = 1;
                break;
            case 'e':
                flags.end_time = 1;
                break;
            case 'i':
                flags.update_display = atoi(optarg);
                break;
            default:
                graceful_exit(NULL);
        }
    }

    if (flags.start_time == 1 && flags.end_time == 1 && flags.target_time == 1) {
        graceful_exit("Cannot specify a target time, when start and end time have been entered!");
    }

    sscanf_s(input_time, "%d:%d:%d", &hh, &mm, &ss);
    start_time.tm_hour = hh;
    start_time.tm_min = mm;
    start_time.tm_sec = ss;

    update_display(flags, input_time, start_time, time_now, target_time);
}