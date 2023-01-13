#include "stdafx.h"

#define BUFFER_SIZE 32768
#define JSON_TOKENS 256

// Tokenize the JSON string
jsmntok_t * json_tokenize(char *js) {
	jsmn_parser parser;
	jsmn_init(&parser);

	unsigned int n = JSON_TOKENS;
	jsmntok_t * tokens = (jsmntok_t *)malloc(sizeof(jsmntok_t) * n);

	int ret = jsmn_parse(&parser, js, strlen(js), tokens, n);

	while (ret == JSMN_ERROR_NOMEM)
	{
		n = n * 2 + 1;
		tokens = (jsmntok_t *)realloc(tokens, sizeof(jsmntok_t) * n);

		ret = jsmn_parse(&parser, js, strlen(js), tokens, n);
	}

	if (ret == JSMN_ERROR_INVAL) {
		XWF_OutputMessage(L"Error parsing JSON response: Invalid JSON string.", 0x0);
		return NULL;
	}
	if (ret == JSMN_ERROR_PART) {
		XWF_OutputMessage(L"Error parsing JSON response: Truncated JSON string.", 0x0);
		return NULL;
	}

	return tokens;
}

// Retrieve a string value from the JSON body
char * json_token_tostr(char *js, jsmntok_t *t) {
	js[t->end] = '\0';
	return js + t->start;
}

// Check to see if a JSON token matches a given string
BOOL json_token_streq(char *js, jsmntok_t *t, char *s) {
	return (strncmp(js + t->start, s, t->end - t->start) == 0
		&& strlen(s) == (size_t)(t->end - t->start));
}

// Parse the JSON Response
BOOL ParseJSONResponse(char * szJSON, std::map<char *, char *> *parsedResponse, char * KEYS[], size_t numKeys) {
	// Enum to keep track of the state of the parser
	typedef enum { START, KEY, VALUE, SKIP, STOP } parse_state;

	// tokenize the JSON string
	jsmntok_t * tokens = json_tokenize(szJSON);

	if (tokens == NULL) {
		return FALSE;
	}

	// Initial parser state
	parse_state state = START;

	// Placeholder for the number of tokens
	size_t object_tokens = 0;

	// Return value
	BOOL retVal = false;

	// Skip offset - if we are in SKIP mode, make sure that we
	// keep skipping until we reach this offset
	int SkipOffset = 0;

	// The index of the KEYS array where a value was matched
	int key_index = -1;
	
	// i is the number of key/value pairs; j is the number of individual items
	for (size_t i = 0, j = 1; j > 0; i++, j--) {
		// Get the token
		jsmntok_t * t = &tokens[i];

		// Sanity check
		if (t->start == -1 || t->end == -1) {
			XWF_OutputMessage(L"Error parsing JSON: uninitialized token.", 0x0);
			state = STOP;
		}

		// Check to see if we're done skipping; if not, don't count it against our increment counter
		if (state == KEY && t->start < SkipOffset) {
			j++;
			continue;
		}
		else {
			SkipOffset = 0;
		}

		// Recurse if needed
		if (t->type == JSMN_OBJECT) {
			if (state != START) {
				bool parseResult = ParseJSONResponse(&szJSON[t->start], parsedResponse, KEYS, numKeys);
				if (parseResult) {
					retVal = true;
				}
			}
			else {
				// Initialize the number of expected properties
				j = (t->size * 2);
			}
		}

		switch (state) {
		case START:
			{
				if (t->type != JSMN_OBJECT) {
					XWF_OutputMessage(L"Error: the root element is not a JSMN_OBJECT.", 0x0);
					state = STOP;
					break;
				}

				// Change the state to key
				state = KEY;

				// Get the number of tokens
				object_tokens = t->size;

				// Sanity checks
				if (object_tokens == 0) {
					XWF_OutputMessage(L"No tokens received in JSON response.", 0x0);
					state = STOP;
					break;
				}
			}
			break;
		case KEY:
			{
				object_tokens--;
			
				// Sanity check
				if (t->type != JSMN_STRING && t->type != JSMN_PRIMITIVE) {
					XWF_OutputMessage(L"Error: Invalid JSON received from server (Error code 0x1).", 0x0);
					state = STOP;
					break;
				}

				state = SKIP;

				for (int i = 0; i < numKeys; i++) {
					if (json_token_streq(szJSON, t, KEYS[i])) {
						key_index = i;
						state = VALUE;
						break;
					}
				}
			}
			break;
		case SKIP:
			{
			SkipOffset = t->end;

			state = KEY;
				
			if (object_tokens == 0) {
				state = STOP;
				break;
			}
			}
			break;
		case VALUE:
			{
				// Sanity check
				if (key_index == -1) {
					XWF_OutputMessage(L"Error: Invalid JSON received from server (Error code 0x2).", 0x0);
					state = STOP;	// value without a key
					break;
				}

				// If we're here we've found at least one JSON token / key that we're interested, so we have something to highlight
				retVal = true;

				// Append the key and value to a string that will be returned
				parsedResponse->insert(std::make_pair(KEYS[key_index], json_token_tostr(szJSON, t)));

				// Set the state back to KEY so we can get the next KEY
				state = KEY;

				// Set key_index back to -1 since we've already ingested this key
				key_index = -1;

				// If we are out of tokens the stop
				if (object_tokens == 0)
					state = STOP;
			}
			break;
		case STOP:
			{
				j = 0;
			}
		}

	}

	free(tokens);
	return retVal;
}