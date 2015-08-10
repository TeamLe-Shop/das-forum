#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include "util.h"

Config* Config_New(char* filename)
{
    bool failed = false;
    Config* config = malloc(sizeof(Config));

    config->file = fopen(filename, "r+");
    if (config->file == NULL) {
        error("Failed to open configuration file '%s'", 0, filename);
        failed = true;
    } else {
        char data[2048];
        size_t end = fread(data, 1, 2047, config->file);
        data[end] = 0;
        json_error_t json_error;
        config->root = json_loads(data, 0, &json_error);

        if (config->root == NULL) {
            error("JSON Error in config file on line %d: %s\n", 0,
                  json_error.line, json_error.text);
            failed = true;
        }
    }

    if (failed) {
        fclose(config->file);
        json_decref(config->root);
        free(config);
        return NULL;
    }

    return config;
}

void Config_Destroy(Config* cfg)
{
    fclose(cfg->file);
    json_decref(cfg->root);
    free(cfg);
}

const char* Config_GetString(Config* cfg, char* key, const char* default_value)
{
    const char* ret = json_string_value(json_object_get(cfg->root, key));
    if (ret) {
        return ret;
    } else {
        return default_value;
    }
}

long long Config_GetInteger(Config* cfg, char* key, long long default_value)
{
    long long ret = json_integer_value(json_object_get(cfg->root, key));
    if (ret) {
        return ret;
    } else {
        return default_value;
    }
}
