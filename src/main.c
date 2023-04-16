#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "input.h"

int main(int argc, char** argv) {
    const char* usage = "USAGE: ez_overlay [OPTIONS]\n"
                        "  OPTIONS\n"
                        "    --help   Display this message\n"
                        "    -mid     The mouse device id\n"
                        "    -kid     The keyboard device id\n";

    if (argc > 5 || strcmp(argv[1], "--help") == 0) {
        printf("%s\n", usage);
        return EXIT_SUCCESS;
    }

    int mid = -1, kid = -1;
    for (int i = 0; i < argc; i++) {
        const char* arg = argv[i];
        if (strcmp(arg, "-mid") == 0) {
            if (i + 1 >= argc) {
                printf("Expected an argument after '-mid'\n");
                printf("%s\n", usage);
                return EXIT_SUCCESS;
            }
            mid = (int) strtol(argv[i + 1], NULL, 10);
            i += 1;
        } else if (strcmp(arg, "-kid") == 0) {
            if (i + 1 >= argc) {
                printf("Expected an argument after '-kid'\n");
                printf("%s\n", usage);
                return EXIT_SUCCESS;
            }
            kid = (int) strtol(argv[i + 1], NULL, 10);
            i += 1;
        }
    }


    init(mid, kid);
    return 0;
}
