/**************** main() ****************/
int main(const int argc, char* argv[]) {

    char* map = NULL;
    char* seed = NULL;

}

/* ******************* parseArgs ******************* */
/* Handle the messy business of parsing command-line arguments.
 * We return 'void' because we only return when successful; otherwise,
 * we print error or usage message to stderr and exit non-zero.
 * 
 * Adapted from parseArgs.c on CS50 Github
 */
static void parseArgs(const int argc, char* argv[],
                      char** map, char** seed) {

    // Requires 3 arguments
    if (argc == 2){

    }if (argc != 3) {
        fprintf(stderr, "USAGE: server map.txt [seed]\n");
        exit(1);
    }

    // Save pointers to the arguments
    *map = argv[1];
    *seed = argv[2];

    // pageDirectory must be crawler-generated
    if (!pagedir_validateCrawler(*pageDirectory)) {
        fprintf(stderr, "ERROR: pageDirectory is not crawler-generated.\n");
        exit(2);
    }

    // indexFilename must be readable
    FILE* indexFile;
    if ((indexFile = fopen(*indexFilename, "r")) == NULL) {
        fprintf(stderr, "ERROR: The file at indexFilename is not readable.\n");
        exit(3);
    }
    fclose(indexFile); 


}