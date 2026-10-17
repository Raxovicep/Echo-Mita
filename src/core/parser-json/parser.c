#include "parser.h"
#include <json-c/json.h>
#include <string.h>

char *parse_json(const char *text) {
  json_object *parsed_json = json_tokener_parse(text);
  if (!parsed_json)
    return NULL;

  json_object *text_obj = {0};

  if (json_object_object_get_ex(parsed_json, "text", &text_obj)) {
    char *result = strdup(json_object_get_string(text_obj));
    json_object_put(parsed_json);
    return result;
  }
  json_object_put(parsed_json);
  return NULL;
}
