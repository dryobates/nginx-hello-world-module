#include <nginx.h>
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

#include <string.h>

typedef struct {
  ngx_flag_t enabled;
} ngx_http_hello_world_loc_conf_t;

ngx_module_t ngx_http_hello_world_module;

static ngx_int_t
ngx_http_hello_world_handler(ngx_http_request_t *r)
{
  ngx_http_hello_world_loc_conf_t *location_conf = ngx_http_get_module_loc_conf(r, ngx_http_hello_world_module);
  if (!location_conf->enabled || location_conf->enabled == NGX_CONF_UNSET) {
    return NGX_DECLINED;
  }
  r->headers_out.status = NGX_HTTP_OK;
  ngx_http_send_header(r);

  ngx_buf_t    *b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
  if (b == NULL) {
      ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, 
          "Failed to allocate response buffer.");
      return NGX_HTTP_INTERNAL_SERVER_ERROR;
  }
  ngx_str_t result = ngx_string("Hello World");
  b->pos = result.data;
  b->last = result.data + result.len;
  b->memory = 1;
  b->last_buf = 1;
  ngx_chain_t   out;
  out.buf = b;
  out.next = NULL;
  return ngx_http_output_filter(r, &out);
}


static ngx_int_t
ngx_http_hello_world_init(ngx_conf_t *cf)
{
  ngx_http_handler_pt *h;
  ngx_http_core_main_conf_t *cmcf;

  cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

  h = ngx_array_push(&cmcf->phases[NGX_HTTP_CONTENT_PHASE].handlers);
  if (h == NULL) {
    return NGX_ERROR;
  }

  *h = ngx_http_hello_world_handler;

  return NGX_OK;
}

static void*
ngx_http_hello_world_create_loc_conf(ngx_conf_t *cf)
{
  ngx_http_hello_world_loc_conf_t *conf;

  conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_hello_world_loc_conf_t));
  if (conf == NULL) {
    return NULL;
  }

  conf->enabled = NGX_CONF_UNSET;

  return conf;
}

static char*
ngx_http_hello_world_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
  ngx_http_hello_world_loc_conf_t *prev = (ngx_http_hello_world_loc_conf_t*)parent;
  ngx_http_hello_world_loc_conf_t *conf = (ngx_http_hello_world_loc_conf_t*)child;

  ngx_conf_merge_value(conf->enabled, prev->enabled, 0);

  return NGX_CONF_OK;
}


static ngx_command_t ngx_http_hello_world_commands[] = {
  {
    ngx_string("hello_world_enabled"),
    NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
    ngx_conf_set_flag_slot,
    NGX_HTTP_LOC_CONF_OFFSET,
    offsetof(ngx_http_hello_world_loc_conf_t, enabled),
    NULL
  },

  ngx_null_command
};


static ngx_http_module_t ngx_http_hello_world_module_ctx = {
  NULL,                                 /* preconfiguration */
  ngx_http_hello_world_init,                  /* postconfiguration */
  NULL,                                 /* create main configuration */
  NULL,                                 /* init main configuration */
  NULL,                                 /* create server configuration */
  NULL,                                 /* merge server configuration */
  ngx_http_hello_world_create_loc_conf, /* create location configuration */
  ngx_http_hello_world_merge_loc_conf   /* merge location configuration */
};


ngx_module_t ngx_http_hello_world_module = {
  NGX_MODULE_V1,
  &ngx_http_hello_world_module_ctx,/* module context */
  ngx_http_hello_world_commands,   /* module directives */
  NGX_HTTP_MODULE,                 /* module type */
  NULL,                            /* init master */
  NULL,                            /* init module */
  NULL,                            /* init process */
  NULL,                            /* init thread */
  NULL,                            /* exit thread */
  NULL,                            /* exit process */
  NULL,                            /* exit master */
  NGX_MODULE_V1_PADDING
};
