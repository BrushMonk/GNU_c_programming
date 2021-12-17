#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <getopt.h>
int main(int argc, char *argv[])
{
    int result;
    extern int opterr, optopt, optind;
    extern char* optarg;
    opterr = 0;
    puts("The following parsed outcome to command-line argments by getopt(-a, -b* or -c*):");
    while((result = getopt(argc, argv, "ab:c::")) != -1)
    {/* -a -b*, -c* are option characters*/
        switch(result)
        {
            case '?':
                if (optopt == 'b')
                    fprintf(stderr, "Option -%c requires an argument attached. optarg == %s\toptind == %d\t",
                    optopt, optarg, optind);
                else if (isprint(optopt))
                    fprintf(stderr, "Unknown option \'%c\':\toptarg == %s\toptind == %d\t",
                    optopt, optarg, optind);
                else
                    fprintf(stderr, "Unknown option character \'%x\':\toptarg == %s\toptind == %d\t",
                    optopt, optarg, optind);
                break;
            default:
                printf("getopt returns \'%c\':\toptarg == %s\toptind == %d\t",
                result, optarg, optind);
                break;
        }
        printf("argv[%d] == %s\n", optind, argv[optind]);
    }

    puts("Here is parsed argument values:");
    for (result = 1; result < argc; result++)
        printf("argv[%d] == %s\n", result, argv[result]);
    printf("optind == %d\n", optind);
    for (result = optind; result < argc; result++)
        printf("No-option argument values that fail to be parsed: argv[%d] == %s\n",
        result, argv[result]);
    return 0;
}