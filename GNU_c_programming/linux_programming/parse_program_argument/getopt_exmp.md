```cpp
int main(int argc, char *argv[])
{
    int result;
    extern int opterr, optopt, optind;
    extern char* optarg;
    opterr=0;
    puts("The following parsed outcome to command-line argments by getopt(-a, -b* or -c*):");
```
the third argument `optstring` in `getopt(int argc, char *const argv[], const char *optstring)` will be processed in separated option character by global variable `optopt` `optind` `optarg`.
`getopt()` has three global variables to store characters in optstring temporarily:

`optopt`: When `getopt()` encounters an unknown option character or an option with a missing required argument, it stores that option character in this variable. You can use this for providing your own diagnostic messages.

`optind`: This variable is set by `getopt()` to the index of the next element of the argv array to be processed. Once `getopt()` has found all of the option arguments, you can use this variable to determine where the remaining non-option arguments begin. The initial value of `optind` is `1`.

`optarg`: This variable is set by `getopt()` to point at the value of the option argument, for those options that accept arguments.
```cpp
    while((result=getopt(argc, argv, "ab:c::"))!=-1)
    {/* -a -b*, -c* are option characters*/
        switch(result)
        {
            case '?':
                if(optopt=='b')
                    fprintf(stderr, "Option -%c requires an argument attached. optarg==%s\toptind==%d\t",
                    optopt, optarg, optind);
                else if(isprint(optopt))
                    fprintf(stderr, "Unknown option \'%c\':\toptarg==%s\toptind==%d\t",
                    optopt, optarg, optind);
                else
                    fprintf(stderr, "Unknown option character \'%x\':\toptarg==%s\toptind==%d\t",
                    optopt, optarg, optind);
                break;
            default:
                printf("getopt returns \'%c\':\toptarg==%s\toptind==%d\t",
                result, optarg, optind);
                break;
        }
        printf("argv[%d]==%s\n", optind, argv[optind]);
    }

    puts("Here is parsed argument values:");
    for(result=1; result<argc; result++)
        printf("argv[%d]==%s\n", result, argv[result]);
    printf("optind==%d\n", optind);
    for(result=optind; result<argc; result++)
        printf("No-option argument values that fail to be parsed: argv[%d]==%s\n",
        result, argv[result]);
    return 0;
}
```
Compile the above program into executable file named parse.
Run parse with following command-line argument:
```shell
./parse 92 -a84 -ccc a
```

This means `getopt()` parse three command-line argv `./parse 92 -a84 -ccc a`.
The following parsed outcome to command-line argments by `getopt(-a, -b* or -c*)`:
```shell
getopt returns 'a':     optarg==(null)  optind==2       argv[2]==-a84
Unknown option '8':     optarg==(null)  optind==2       argv[2]==-a84
Unknown option '4':     optarg==(null)  optind==3       argv[3]==-ccc
getopt returns 'c':     optarg==cc      optind==4       argv[4]==a
```

Here is parsed argument values:
```shell
argv[1]==-a84
argv[2]==-ccc
argv[3]==92
argv[4]==a
optind==3
No-option argument values that fail to be parsed: argv[3]==92
No-option argument values that fail to be parsed: argv[4]==a
```

Run parse with following command-line argument:
```shell
./parse -ac b -b
```
This means `getopt()` parse two command-line argv `-ac b -b`.
The following parsed outcome to command-line argments by `getopt(-a, -b* or -c*)`:
```shell
getopt returns 'a':     optarg==(null)  optind==1       argv[1]==-ac
getopt returns 'c':     optarg==(null)  optind==2       argv[2]==b
Option -b requires an argument attached. optarg==(null) optind==4       argv[4]==(null)
Here is parsed argument values:
argv[1]==-ac
argv[2]==-b
argv[3]==b
optind==3
No-option argument values that fail to be parsed: argv[3]==b
```
Because `b` is followed a colon in `"ab:c::"`, so an argument must follow `b` in command-line.