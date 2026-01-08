#pragma once

#include <xcb/dri3.h>
typedef xcb_dri3_open_cookie_t (*PFN_xcb_dri3_open)(
    xcb_connection_t *c,
    xcb_drawable_t drawable,
    uint32_t provider
);
typedef xcb_dri3_open_reply_t *(*PFN_xcb_dri3_open_reply)(
    xcb_connection_t *c,
    xcb_dri3_open_cookie_t cookie,
    xcb_generic_error_t **e
);
typedef int *(*PFN_xcb_get_reply_fds)(
    xcb_connection_t *c,
    void *reply
);
typedef int* (*PFN_xcb_dri3_open_reply_fds)(
    xcb_connection_t*, 
    xcb_dri3_open_reply_t*
);
typedef xcb_dri3_query_version_cookie_t (*PFN_xcb_dri3_query_version)(
    xcb_connection_t*, 
    uint32_t, 
    uint32_t
);
typedef xcb_dri3_query_version_reply_t* (*PFN_xcb_dri3_query_version_reply)(
    xcb_connection_t*, 
    xcb_dri3_query_version_cookie_t, 
    xcb_generic_error_t**
);
typedef xcb_dri3_buffer_from_pixmap_reply_t* (*PFN_xcb_dri3_buffer_from_pixmap_reply)(
    xcb_connection_t*,
    xcb_dri3_buffer_from_pixmap_cookie_t,
    xcb_generic_error_t** 
);
typedef int* (*PFN_xcb_dri3_buffer_from_pixmap_reply_fds)(
    xcb_connection_t*,
    xcb_dri3_buffer_from_pixmap_reply_t*
);
typedef xcb_dri3_buffer_from_pixmap_cookie_t (*PFN_xcb_dri3_buffer_from_pixmap)(
    xcb_connection_t*,
    xcb_pixmap_t
);
typedef xcb_void_cookie_t (*PFN_xcb_dri3_pixmap_from_buffer_checked)(
    xcb_connection_t*,
    xcb_pixmap_t,
    xcb_drawable_t,
    uint32_t,
    uint16_t,
    uint16_t,
    uint16_t,
    uint8_t,
    uint8_t,
    int32_t
);