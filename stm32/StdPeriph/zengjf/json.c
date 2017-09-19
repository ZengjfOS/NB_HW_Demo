#include "zengjf.h"

//jansson Test
void jansson_pack_test(void)
{
        
    json_t *root;
    char *out;
    json_error_t error;
    
    printf("\r\n\r\n---------Jansson Test Begin---------\r\n");

    /* Build an empty JSON object */
    root = json_pack("{s:i}", "zengjf", 5);

    out = json_dumps(root, JSON_ENCODE_ANY);
    printf("encode json data: \r\n\t%s\r\n", out);
    
    free(root);
    free(out);
    
    root = json_loads("{\"zengjf\": \"5\", \"zengsf\": 6}", 0, &error);    
    if(!root)
    {
        printf("error: on line %d: %s\r\n", error.line, error.text);
    }

    out = json_dumps(root, JSON_ENCODE_ANY);
    printf("decode json data: \r\n\t%s\r\n", out);
    printf("show root[\"zengjf\"] value:%s\r\n", json_string_value(json_object_get(root, "zengjf")));
    printf("show root[\"zengsf\"] value:%d\r\n", (int)json_integer_value(json_object_get(root, "zengsf")));
    
    free(root);
    free(out);
    
    printf("---------Jansson Test Over---------\r\n");
}

