#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

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


int main(int argc, char **argv) {
    int option;
    char *input_time, *parsed_time;

    time_t now;
    time(&now);

    struct tm start_time = *localtime(&now);
    struct tm time_now = *localtime(&now);
    int netto_seconds, brutto_seconds, target_diff;
    int target_time = ((7 * 60) + 48) * 60;
    char *target_break = "";

    char target_diff_f[13];

    while ((option = getopt(argc, argv, "s:")) != -1) {
        switch (option) {
            case 's':
                input_time = optarg;
                printf("Work started at: %s\n", input_time);
                printf("Target time: 7:48h\n");
                break;
        }
    }

    parsed_time = strtok(input_time, ":");
    int i = 0;
    while (parsed_time != NULL) {
        switch (i) {
            case 0:
                start_time.tm_hour = atoi(parsed_time);
                break;

            case 1:
                start_time.tm_min = atoi(parsed_time);
                break;
        }
        i++;
        parsed_time = strtok(NULL, ":");
    }

    // Calculate actual time at work
    brutto_seconds = (int) difftime(mktime(&time_now), mktime(&start_time));
    netto_seconds = netto_work_time(brutto_seconds, target_break);
    target_diff = netto_seconds - target_time;

    estimated_departure(start_time, target_time);
    print_diff(target_diff, target_diff_f, target_break);
    print_netto(netto_seconds, target_diff_f);
    print_brutto(brutto_seconds);
}