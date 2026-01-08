#pragma once

typedef const xcb_query_extension_reply_t* (*PFN_xcb_get_extension_data)(
    xcb_connection_t *c,
    xcb_extension_t  *ext
);
typedef xcb_generic_event_t* (*PFN_xcb_wait_for_special_event)(
    xcb_connection_t      *c,
    xcb_special_event_t   *se
);
typedef int (*PFN_xcb_flush)(xcb_connection_t *c);
typedef void (*PFN_xcb_disconnect)(xcb_connection_t *c);
typedef xcb_generic_error_t* (*PFN_xcb_request_check)(
    xcb_connection_t *c,
    xcb_void_cookie_t cookie
);
typedef void (*PFN_xcb_prefetch_extension_data)(
    xcb_connection_t *c,
    xcb_extension_t  *ext
);
typedef void (*PFN_xcb_unregister_for_special_event)(
    xcb_connection_t    *c,
    xcb_special_event_t *se
);
typedef xcb_get_geometry_cookie_t (*PFN_xcb_get_geometry)(
    xcb_connection_t *c,
    xcb_drawable_t    drawable
);
typedef xcb_get_geometry_reply_t* (*PFN_xcb_get_geometry_reply)(
    xcb_connection_t          *c,
    xcb_get_geometry_cookie_t  cookie,
    xcb_generic_error_t      **e
);
typedef xcb_connection_t* (*PFN_xcb_connect)(
    const char *displayname,
    int        *screenp
);
typedef uint32_t (*PFN_xcb_generate_id)(xcb_connection_t *c);
typedef xcb_void_cookie_t (*PFN_xcb_copy_area_checked)(
    xcb_connection_t *c,
    xcb_drawable_t    src_drawable,
    xcb_drawable_t    dst_drawable,
    xcb_gcontext_t    gc,
    int16_t           src_x,
    int16_t           src_y,
    int16_t           dst_x,
    int16_t           dst_y,
    uint16_t          width,
    uint16_t          height
);
typedef xcb_void_cookie_t (*PFN_xcb_create_gc)(
    xcb_connection_t *c,
    xcb_gcontext_t    cid,
    xcb_drawable_t    drawable,
    uint32_t          value_mask,
    const void       *value_list
);
typedef xcb_void_cookie_t (*PFN_xcb_free_gc)(
    xcb_connection_t *c,
    xcb_gcontext_t    gc
);
typedef xcb_generic_event_t* (*PFN_xcb_poll_for_special_event)(
    xcb_connection_t    *c,
    xcb_special_event_t *se
);
typedef xcb_special_event_t* (*PFN_xcb_register_for_special_xge)(
    xcb_connection_t *c,
    xcb_extension_t  *ext,
    uint32_t          eid,
    uint32_t         *stamp
);