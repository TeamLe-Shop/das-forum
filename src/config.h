#ifndef CONFIG_H
#define CONFIG_H

#include <jansson.h>

struct Config {
    json_t* root;
    FILE* file;
};

typedef struct Config Config;

/* Creates a new configuration structure from a file.
 *
 * JSON parsing errors are written to stderr.
 */
Config* Config_New(char* filename);

void Config_Destroy(Config* cfg);

/* Returns a string configuration value. */
const char* Config_GetString(Config* cfg, char* key, const char* default_value);

/* Returns an integer configuration value. */
long long Config_GetInteger(Config* cfg, char* key, long long default_value);


#endif /* CONFIG_H */
