#pragma once

// Get a list of tokens from the JSON
jsmntok_t * json_tokenize(char *js);

// Retrieve a string value from the JSON body
char * json_token_tostr(char *js, jsmntok_t *t);

// Check to see if a JSON token matches a given string
BOOL json_token_streq(char *js, jsmntok_t *t, char *s);

// Parse the JSON response
BOOL ParseJSONResponse(char * szJSON, std::map<char *, char *> *parsedResponse, char * KEYS[], size_t numKeys);