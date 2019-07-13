#include "ngx_config.h"
#include "ngx_core.h"
#include "ngx_http.h"
/*
typedef struct {
    ngx_int_t   (*preconfiguration)(ngx_conf_t *cf);
    ngx_int_t   (*postconfiguration)(ngx_conf_t *cf);

    void       *(*create_main_conf)(ngx_conf_t *cf);
    char       *(*init_main_conf)(ngx_conf_t *cf, void *conf);

    void       *(*create_srv_conf)(ngx_conf_t *cf);
    char       *(*merge_srv_conf)(ngx_conf_t *cf, void *prev, void *conf);

    void       *(*create_loc_conf)(ngx_conf_t *cf);
    char       *(*merge_loc_conf)(ngx_conf_t *cf, void *prev, void *conf);
} ngx_http_module_t;

struct ngx_command_s {
    ngx_str_t             name;
    ngx_uint_t            type;
    char               *(*set)(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
    ngx_uint_t            conf;
    ngx_uint_t            offset;
    void                 *post;
};
*/

static char * ngx_http_mytest_cmd_set_mytest(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

ngx_command_t ngx_http_mytest_command[] = {
    {
        ngx_string("mytest"),
        NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF\
        |NGX_HTTP_LOC_CONF|NGX_HTTP_LMT_CONF|NGX_CONF_NOARGS,
        ngx_http_mytest_cmd_set_mytest,
        NGX_HTTP_LOC_CONF_OFFSET,
        0,
        NULL
    },
    ngx_null_command
};

ngx_http_module_t ngx_http_mytest_module_ctx = {
    NULL, /* preconfiguration */
    NULL, /* postconfiguration */
    NULL, /* create_main_conf */
    NULL, /* init_main_conf */
    NULL, /* create_srv_conf */
    NULL, /* merge_srv_conf */
    NULL, /* create_loc_conf */
    NULL, /* merge_loc_conf */
};

ngx_module_t ngx_http_mytest_module = {
    NGX_MODULE_V1,
    &ngx_http_mytest_module_ctx, /* module ctx */
    ngx_http_mytest_command, /* module command */
    NGX_HTTP_MODULE, /* module type */
    NULL, /* init_master */
    NULL, /* init_module */
    NULL, /* init_process */
    NULL, /* init_thread */
    NULL, /* exit_thread */
    NULL, /* exit_process */
    NULL, /* exit_master */
    NGX_MODULE_V1_PADDING
};

static ngx_int_t ngx_http_mytest_handler(ngx_http_request_t* r)
{
    ngx_int_t rc;
    ngx_buf_t* buf = NULL;
    ngx_chain_t chain;

    if(!(r->method & (NGX_HTTP_GET | NGX_HTTP_HEAD))) {
        return NGX_HTTP_NOT_ALLOWED;
    }

    rc = ngx_http_discard_request_body(r);
    if(rc != NGX_OK) {
        return rc;
    }

    r->headers_out.status = NGX_HTTP_OK;

    ngx_str_t ctype = ngx_string("text/plain");
    r->headers_out.content_type = ctype;
    
    ngx_str_t resp = ngx_string("hello world!");
    r->headers_out.content_length_n = resp.len;

    rc = ngx_http_send_header(r);
    if(rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
        return rc;
    }

    buf = ngx_create_temp_buf(r->pool, resp.len);
    ngx_memcpy(buf->pos, resp.data, resp.len);
    buf->last = buf->pos + resp.len;
    buf->last_buf = 1;

    chain.buf = buf;
    chain.next = NULL;

    return ngx_http_output_filter(r, &chain);
}

static char * ngx_http_mytest_cmd_set_mytest(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t* clcf = NULL;

    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    
    clcf->handler = ngx_http_mytest_handler;

    return NGX_CONF_OK;
}