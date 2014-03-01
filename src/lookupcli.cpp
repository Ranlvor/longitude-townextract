// getopt is used to check for the --color-flag
#include <getopt.h>

#include "Lookup.h"
#include "stringToNumber.h"
#include "Output.h"

int main(int argc, char *argv[]) {
    static struct option long_options[] = {
        {"color",                no_argument, 0, 'c'},
        {0,0,0,0}
    };

    while (1) {
        int c = getopt_long(argc, argv, "c", long_options, 0);

        if (c == -1)
            break;

        switch (c) {
            case 'c':
                Output::usecolor = true;
                break;
            default:
                exit(1);
        }
    }

    // check for proper command line args
    if(optind != argc-2)
        Output::err("usage: %s [--color] latitude longitude", argv[0]);

    const char* latitude = argv[optind];
    const char* longitude = argv[optind+1];

    Output::debug("Looking up %s %s", latitude, longitude);

    Lookup l;
    std::string result = l.lookup(StringToNumber<double>(latitude), StringToNumber<double>(longitude));
    Output::info("Match: %s", result.c_str());
    fprintf(stdout, "%s\n", result.c_str());
}
