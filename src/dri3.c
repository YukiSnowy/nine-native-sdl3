/*
 * Copyright © 2014 Axel Davy
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */ 

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>
#include <X11/Xlib-xcb.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include <d3d9.h>

#include "dri3.h"


#ifdef _DEBUG
#define TRACE(...)  fprintf(stderr, __VA_ARGS__)
#define ERR(...)    fprintf(stderr, __VA_ARGS__)
#else
#define TRACE(...)
#define ERR(...)    fprintf(stderr, __VA_ARGS__)
#endif


// --------------------------------- dlls/winex11.drv/dri3.c --------------------------------------------

#include <dlfcn.h>

#include "pfn_xcb.h"
PFN_xcb_get_extension_data xcb_get_extension_data_dyn;
PFN_xcb_wait_for_special_event xcb_wait_for_special_event_dyn;
PFN_xcb_flush xcb_flush_dyn;
PFN_xcb_connect xcb_connect_dyn;
PFN_xcb_disconnect xcb_disconnect_dyn;
PFN_xcb_request_check xcb_request_check_dyn;
PFN_xcb_prefetch_extension_data xcb_prefetch_extension_data_dyn;
PFN_xcb_unregister_for_special_event xcb_unregister_for_special_event_dyn;
PFN_xcb_get_geometry xcb_get_geometry_dyn;
PFN_xcb_get_geometry_reply xcb_get_geometry_reply_dyn;
PFN_xcb_generate_id xcb_generate_id_dyn;
PFN_xcb_copy_area_checked xcb_copy_area_checked_dyn;
PFN_xcb_create_gc xcb_create_gc_dyn;
PFN_xcb_free_gc xcb_free_gc_dyn;
PFN_xcb_poll_for_special_event xcb_poll_for_special_event_dyn;
PFN_xcb_register_for_special_xge xcb_register_for_special_xge_dyn;

typedef xcb_connection_t *(*PFN_XGetXCBConnection)(Display *);
PFN_XGetXCBConnection XGetXCBConnection_dyn;

typedef Display* (*PFN_XOpenDisplay)(const char*);
PFN_XOpenDisplay XOpenDisplay_dyn;
typedef int (*PFN_XFreePixmap)(Display*, Pixmap);
PFN_XFreePixmap XFreePixmap_dyn;

typedef xcb_void_cookie_t (*PFN_xcb_present_notify_msc)(
    xcb_connection_t *c,
    xcb_window_t      window,
    uint32_t          serial,
    uint64_t          target_msc,
    uint64_t          divisor,
    uint64_t          remainder
);
typedef xcb_void_cookie_t (*PFN_xcb_present_select_input_checked)(
    xcb_connection_t *c,
    xcb_present_event_t eid,
    xcb_window_t      window,
    uint32_t          event_mask
);
typedef xcb_void_cookie_t (*PFN_xcb_present_pixmap_checked)(
    xcb_connection_t *c,
    xcb_window_t      window,
    xcb_pixmap_t      pixmap,
    uint32_t          serial,
    xcb_xfixes_region_t valid,
    xcb_xfixes_region_t update,
    int16_t           x_off,
    int16_t           y_off,
    xcb_randr_crtc_t  target_crtc,
    xcb_sync_fence_t  wait_fence,
    xcb_sync_fence_t  idle_fence,
    uint32_t          options,
    uint64_t          target_msc,
    uint64_t          divisor,
    uint64_t          remainder,
    uint32_t          notifies_len,
    const xcb_present_notify_t *notifies
);
typedef xcb_void_cookie_t (*PFN_xcb_present_pixmap)(
    xcb_connection_t *c,
    xcb_window_t      window,
    xcb_pixmap_t      pixmap,
    uint32_t          serial,
    xcb_xfixes_region_t valid,
    xcb_xfixes_region_t update,
    int16_t           x_off,
    int16_t           y_off,
    xcb_randr_crtc_t  target_crtc,
    xcb_sync_fence_t  wait_fence,
    xcb_sync_fence_t  idle_fence,
    uint32_t          options,
    uint64_t          target_msc,
    uint64_t          divisor,
    uint64_t          remainder,
    uint32_t          notifies_len,
    const xcb_present_notify_t *notifies
);
typedef xcb_present_query_version_cookie_t (*PFN_xcb_present_query_version)(
    xcb_connection_t *c,
    uint32_t          major_version,
    uint32_t          minor_version
);
typedef xcb_present_query_version_reply_t* (*PFN_xcb_present_query_version_reply)(
    xcb_connection_t                  *c,
    xcb_present_query_version_cookie_t  cookie,
    xcb_generic_error_t              **e
);
PFN_xcb_present_notify_msc xcb_present_notify_msc_dyn;
PFN_xcb_present_select_input_checked xcb_present_select_input_checked_dyn;
PFN_xcb_present_pixmap_checked xcb_present_pixmap_checked_dyn;
PFN_xcb_present_pixmap xcb_present_pixmap_dyn;
xcb_extension_t *p_present_id;
PFN_xcb_present_query_version xcb_present_query_version_dyn;
PFN_xcb_present_query_version_reply xcb_present_query_version_reply_dyn;

typedef xcb_xfixes_query_version_cookie_t (*pfn_xcb_xfixes_query_version_unchecked)(
    xcb_connection_t *c,
    uint32_t          major_version,
    uint32_t          minor_version
);
typedef xcb_xfixes_query_version_reply_t* (*pfn_xcb_xfixes_query_version_reply)(
    xcb_connection_t                 *c,
    xcb_xfixes_query_version_cookie_t cookie,
    xcb_generic_error_t             **e
);
typedef void (*pfn_xcb_xfixes_create_region)(
    xcb_connection_t     *c,
    xcb_xfixes_region_t   region,
    uint32_t              num_rects,
    const xcb_rectangle_t *rects
);
typedef void (*pfn_xcb_xfixes_destroy_region)(
    xcb_connection_t    *c,
    xcb_xfixes_region_t  region
);
pfn_xcb_xfixes_query_version_unchecked xcb_xfixes_query_version_unchecked_dyn;
pfn_xcb_xfixes_query_version_reply xcb_xfixes_query_version_reply_dyn;
pfn_xcb_xfixes_create_region xcb_xfixes_create_region_dyn;
pfn_xcb_xfixes_destroy_region xcb_xfixes_destroy_region_dyn;

#include "pfn_dri3.h"
PFN_xcb_dri3_open xcb_dri3_open_dyn;
PFN_xcb_dri3_open_reply xcb_dri3_open_reply_dyn;
PFN_xcb_dri3_open_reply_fds xcb_dri3_open_reply_fds_dyn;
PFN_xcb_get_reply_fds xcb_get_reply_fds_dyn;
PFN_xcb_dri3_query_version xcb_dri3_query_version_dyn;
PFN_xcb_dri3_query_version_reply xcb_dri3_query_version_reply_dyn;
PFN_xcb_dri3_pixmap_from_buffer_checked xcb_dri3_pixmap_from_buffer_checked_dyn;
PFN_xcb_dri3_buffer_from_pixmap xcb_dri3_buffer_from_pixmap_dyn;
PFN_xcb_dri3_buffer_from_pixmap_reply xcb_dri3_buffer_from_pixmap_reply_dyn;
PFN_xcb_dri3_buffer_from_pixmap_reply_fds xcb_dri3_buffer_from_pixmap_reply_fds_dyn;
xcb_extension_t *p_dri3_id;

void init_dri3()
{
    void* x11_handle = dlopen("libX11.so.6", RTLD_LAZY);
    XOpenDisplay_dyn = 
        (PFN_XOpenDisplay)dlsym(x11_handle, "XOpenDisplay");
    XFreePixmap_dyn = 
        (PFN_XFreePixmap)dlsym(x11_handle, "XFreePixmap");

    void* xcb_handle = dlopen("libxcb.so.1", RTLD_LAZY);
    xcb_get_extension_data_dyn = 
        (PFN_xcb_get_extension_data)dlsym(xcb_handle, "xcb_get_extension_data");
    xcb_wait_for_special_event_dyn = 
        (PFN_xcb_wait_for_special_event)dlsym(xcb_handle, "xcb_wait_for_special_event");
    xcb_flush_dyn = 
        (PFN_xcb_flush)dlsym(xcb_handle, "xcb_flush");
    xcb_connect_dyn = 
        (PFN_xcb_connect)dlsym(xcb_handle, "xcb_connect");
    xcb_disconnect_dyn = 
        (PFN_xcb_disconnect)dlsym(xcb_handle, "xcb_disconnect");
    xcb_request_check_dyn = 
        (PFN_xcb_request_check)dlsym(xcb_handle, "xcb_request_check");
    xcb_prefetch_extension_data_dyn = 
        (PFN_xcb_prefetch_extension_data)dlsym(xcb_handle, "xcb_prefetch_extension_data");
    xcb_unregister_for_special_event_dyn = 
        (PFN_xcb_unregister_for_special_event)dlsym(xcb_handle, "xcb_unregister_for_special_event");
    xcb_get_geometry_dyn = 
        (PFN_xcb_get_geometry)dlsym(xcb_handle, "xcb_get_geometry");
    xcb_get_geometry_reply_dyn = 
        (PFN_xcb_get_geometry_reply)dlsym(xcb_handle, "xcb_get_geometry_reply");
    xcb_generate_id_dyn = 
        (PFN_xcb_generate_id)dlsym(xcb_handle, "xcb_generate_id");
    xcb_copy_area_checked_dyn = 
        (PFN_xcb_copy_area_checked)dlsym(xcb_handle, "xcb_copy_area_checked");
    xcb_create_gc_dyn = 
        (PFN_xcb_create_gc)dlsym(xcb_handle, "xcb_create_gc");
    xcb_free_gc_dyn = 
        (PFN_xcb_free_gc)dlsym(xcb_handle, "xcb_free_gc");
    xcb_poll_for_special_event_dyn = 
        (PFN_xcb_poll_for_special_event)dlsym(xcb_handle, "xcb_poll_for_special_event");
    xcb_register_for_special_xge_dyn = 
        (PFN_xcb_register_for_special_xge)dlsym(xcb_handle, "xcb_register_for_special_xge");

    void* present_handle = dlopen("libxcb-present.so.0", RTLD_LAZY);
    xcb_present_notify_msc_dyn = 
        (PFN_xcb_present_notify_msc)dlsym(present_handle, "xcb_present_notify_msc");
    xcb_present_select_input_checked_dyn = 
        (PFN_xcb_present_select_input_checked)dlsym(present_handle, "xcb_present_select_input_checked");
    xcb_present_pixmap_checked_dyn = 
        (PFN_xcb_present_pixmap_checked)dlsym(present_handle, "xcb_present_pixmap_checked");
    xcb_present_pixmap_dyn = 
        (PFN_xcb_present_pixmap)dlsym(present_handle, "xcb_present_pixmap");
        
        p_present_id =
        (xcb_extension_t *)dlsym(present_handle, "xcb_present_id");

    xcb_present_query_version_dyn = 
        (PFN_xcb_present_query_version)dlsym(present_handle, "xcb_present_query_version");
    xcb_present_query_version_reply_dyn = 
        (PFN_xcb_present_query_version_reply)dlsym(present_handle, "xcb_present_query_version_reply");

    void* xfixes_handle = dlopen("libxcb-xfixes.so.0", RTLD_LAZY);
    xcb_xfixes_query_version_unchecked_dyn = (pfn_xcb_xfixes_query_version_unchecked)dlsym(xfixes_handle, "xcb_xfixes_query_version_unchecked");
    xcb_xfixes_query_version_reply_dyn     = (pfn_xcb_xfixes_query_version_reply)dlsym(xfixes_handle, "xcb_xfixes_query_version_reply");
    xcb_xfixes_create_region_dyn           = (pfn_xcb_xfixes_create_region)dlsym(xfixes_handle, "xcb_xfixes_create_region");
    xcb_xfixes_destroy_region_dyn          = (pfn_xcb_xfixes_destroy_region)dlsym(xfixes_handle, "xcb_xfixes_destroy_region");

    void *X11_xcb_lib = dlopen("libX11-xcb.so.1", RTLD_LAZY | RTLD_LOCAL);
    XGetXCBConnection_dyn = 
        (PFN_XGetXCBConnection)dlsym(X11_xcb_lib, "XGetXCBConnection");

    void *dri3 = dlopen("libxcb-dri3.so.0", RTLD_LAZY | RTLD_LOCAL);
    xcb_dri3_open_dyn =
        (PFN_xcb_dri3_open)dlsym(dri3, "xcb_dri3_open");
    xcb_dri3_open_reply_dyn =
        (PFN_xcb_dri3_open_reply)dlsym(dri3, "xcb_dri3_open_reply");
    xcb_dri3_open_reply_fds_dyn =
        (PFN_xcb_dri3_open_reply_fds)dlsym(dri3, "xcb_dri3_open_reply_fds");
    xcb_get_reply_fds_dyn =
        (PFN_xcb_get_reply_fds)dlsym(dri3, "xcb_get_reply_fds");
    xcb_dri3_query_version_dyn =
        (PFN_xcb_dri3_query_version)dlsym(dri3, "xcb_dri3_query_version");
    xcb_dri3_query_version_reply_dyn =
        (PFN_xcb_dri3_query_version_reply)dlsym(dri3, "xcb_dri3_query_version_reply");
    xcb_dri3_pixmap_from_buffer_checked_dyn =
        (PFN_xcb_dri3_pixmap_from_buffer_checked)dlsym(dri3, "xcb_dri3_pixmap_from_buffer_checked");
    xcb_dri3_buffer_from_pixmap_dyn =
        (PFN_xcb_dri3_buffer_from_pixmap)dlsym(dri3, "xcb_dri3_buffer_from_pixmap");
    xcb_dri3_buffer_from_pixmap_reply_dyn =
        (PFN_xcb_dri3_buffer_from_pixmap_reply)dlsym(dri3, "xcb_dri3_buffer_from_pixmap_reply");
    xcb_dri3_buffer_from_pixmap_reply_fds_dyn =
        (PFN_xcb_dri3_buffer_from_pixmap_reply_fds)dlsym(dri3, "xcb_dri3_buffer_from_pixmap_reply_fds");

    p_dri3_id =
        (xcb_extension_t *)dlsym(dri3, "xcb_dri3_id");
}

BOOL
DRI3CheckExtension(Display *dpy, int major, int minor)
{
    //xcb_connection_t *xcb_connection = XGetXCBConnection(dpy);
    xcb_connection_t *xcb_connection = XGetXCBConnection_dyn(dpy);

    xcb_dri3_query_version_cookie_t dri3_cookie;
    xcb_dri3_query_version_reply_t *dri3_reply;
    xcb_generic_error_t *error;
    const xcb_query_extension_reply_t *extension;
    int fd;

    //xcb_prefetch_extension_data(xcb_connection, &xcb_dri3_id);
    xcb_prefetch_extension_data_dyn(xcb_connection, p_dri3_id);

    //extension = xcb_get_extension_data(xcb_connection, &xcb_dri3_id);
    extension = xcb_get_extension_data_dyn(xcb_connection, p_dri3_id);
    if (!(extension && extension->present)) {
        ERR("DRI3 extension is not present\n");
        return FALSE;
    }

    //dri3_cookie = xcb_dri3_query_version(xcb_connection, major, minor);
    dri3_cookie = xcb_dri3_query_version_dyn(xcb_connection, major, minor);

    //dri3_reply = xcb_dri3_query_version_reply(xcb_connection, dri3_cookie, &error);
    dri3_reply = xcb_dri3_query_version_reply_dyn(xcb_connection, dri3_cookie, &error);
    if (!dri3_reply) {
        free(error);
        ERR("Issue getting requested version of DRI3: %d,%d\n", major, minor);
        return FALSE;
    }

    if (!DRI3Open(dpy, DefaultScreen(dpy), &fd)) {
        ERR("DRI3 advertised, but not working\n");
        return FALSE;
    }
    close(fd);

    TRACE("DRI3 version %d,%d found. %d %d requested\n", major, minor, (int)dri3_reply->major_version, (int)dri3_reply->minor_version);
    free(dri3_reply);

    return TRUE;
}

BOOL
PRESENTCheckExtension(Display *dpy, int major, int minor)
{
    //xcb_connection_t *xcb_connection = XGetXCBConnection(dpy);
    xcb_connection_t *xcb_connection = XGetXCBConnection_dyn(dpy);

    xcb_present_query_version_cookie_t present_cookie;
    xcb_present_query_version_reply_t *present_reply;
    xcb_generic_error_t *error;
    const xcb_query_extension_reply_t *extension;

    //xcb_prefetch_extension_data(xcb_connection, &xcb_present_id);
    xcb_prefetch_extension_data_dyn(xcb_connection, p_present_id);

    //extension = xcb_get_extension_data(xcb_connection, &xcb_present_id);
    extension = xcb_get_extension_data_dyn(xcb_connection, p_present_id);
    if (!(extension && extension->present)) {
        ERR("PRESENT extension is not present\n");
        return FALSE;
    }

    //present_cookie = xcb_present_query_version(xcb_connection, major, minor);
    present_cookie = xcb_present_query_version_dyn(xcb_connection, major, minor);

    //present_reply = xcb_present_query_version_reply(xcb_connection, present_cookie, &error);
    present_reply = xcb_present_query_version_reply_dyn(xcb_connection, present_cookie, &error);
    if (!present_reply) {
        free(error);
        ERR("Issue getting requested version of PRESENT: %d,%d\n", major, minor);
        return FALSE;
    }

    TRACE("PRESENT version %d,%d found. %d %d requested\n", major, minor, (int)present_reply->major_version, (int)present_reply->minor_version);
    free(present_reply);

    return TRUE;
}

BOOL
DRI3Open(Display *dpy, int screen, int *device_fd)
{
    xcb_dri3_open_cookie_t cookie;
    xcb_dri3_open_reply_t *reply;
    //xcb_connection_t *xcb_connection = XGetXCBConnection(dpy);
    xcb_connection_t *xcb_connection = XGetXCBConnection_dyn(dpy);
    int fd;
    Window root = RootWindow(dpy, screen);

    //cookie = xcb_dri3_open(xcb_connection, root, 0);
    cookie = xcb_dri3_open_dyn(xcb_connection, root, 0);

    //reply = xcb_dri3_open_reply(xcb_connection, cookie, NULL);
    reply = xcb_dri3_open_reply_dyn(xcb_connection, cookie, NULL);
    if (!reply)
        return FALSE;

    if (reply->nfd != 1) {
        free(reply);
        return FALSE;
    }

    //fd = xcb_dri3_open_reply_fds(xcb_connection, reply)[0];
    fd = xcb_dri3_open_reply_fds_dyn(xcb_connection, reply)[0];
    fcntl(fd, F_SETFD, FD_CLOEXEC);

    *device_fd = fd;

    return TRUE;
}

BOOL
DRI3PixmapFromDmaBuf(Display *dpy, int screen, int fd, int width, int height, int stride, int depth, int bpp, Pixmap *pixmap)
{
    //xcb_connection_t *xcb_connection = XGetXCBConnection(dpy);
    xcb_connection_t *xcb_connection = XGetXCBConnection_dyn(dpy);
    Window root = RootWindow(dpy, screen);
    xcb_void_cookie_t cookie;
    xcb_generic_error_t *error;

    //cookie = xcb_dri3_pixmap_from_buffer_checked(xcb_connection,
    cookie = xcb_dri3_pixmap_from_buffer_checked_dyn(xcb_connection,
                                                //(*pixmap = xcb_generate_id(xcb_connection)),
                                                (*pixmap = xcb_generate_id_dyn(xcb_connection)),
                                                root,
                                                0,
                                                width, height, stride,
                                                depth, bpp, fd);
    //error = xcb_request_check(xcb_connection, cookie); /* performs a flush */
    error = xcb_request_check_dyn(xcb_connection, cookie); /* performs a flush */
    if (error) {
        ERR("Error using DRI3 to convert a DmaBufFd to pixmap\n");
        return FALSE;
    }
    return TRUE;
}

BOOL
DRI3DmaBufFromPixmap(Display *dpy, Pixmap pixmap, int *fd, int *width, int *height, int *stride, int *depth, int *bpp)
{
    //xcb_connection_t *xcb_connection = XGetXCBConnection(dpy);
    xcb_connection_t *xcb_connection = XGetXCBConnection_dyn(dpy);
    xcb_dri3_buffer_from_pixmap_cookie_t bp_cookie;
    xcb_dri3_buffer_from_pixmap_reply_t  *bp_reply;

    //bp_cookie = xcb_dri3_buffer_from_pixmap(xcb_connection, pixmap);
    bp_cookie = xcb_dri3_buffer_from_pixmap_dyn(xcb_connection, pixmap);
    //bp_reply = xcb_dri3_buffer_from_pixmap_reply(xcb_connection, bp_cookie, NULL);
    bp_reply = xcb_dri3_buffer_from_pixmap_reply_dyn(xcb_connection, bp_cookie, NULL);
    if (!bp_reply)
        return FALSE;
    //*fd = xcb_dri3_buffer_from_pixmap_reply_fds(xcb_connection, bp_reply)[0];
    *fd = xcb_dri3_buffer_from_pixmap_reply_fds_dyn(xcb_connection, bp_reply)[0];
    *width = bp_reply->width;
    *height = bp_reply->height;
    *stride = bp_reply->stride;
    *depth = bp_reply->depth;
    *bpp = bp_reply->depth;
    return TRUE;
}

struct PRESENTPriv {
    xcb_connection_t *xcb_connection;
    xcb_connection_t *xcb_connection_bis; /* to avoid libxcb thread bugs, use a different connection to present pixmaps */
    XID window;
    uint64_t last_msc;
    uint64_t last_target;
    uint32_t last_serial_given;
    xcb_special_event_t *special_event;
    PRESENTPixmapPriv *first_present_priv;
    int pixmap_present_pending;
    BOOL notify_with_serial_pending;
    pthread_mutex_t mutex_present; /* protect readind/writing present_priv things */
    pthread_mutex_t mutex_xcb_wait;
    BOOL xcb_wait;
};

struct PRESENTPixmapPriv {
    PRESENTpriv *present_priv;
    Pixmap pixmap;
    BOOL released;
    unsigned int width;
    unsigned int height;
    unsigned int depth;
    BOOL present_complete_pending;
    uint32_t serial;
#if D3DADAPTER9_WITHDRI2
    struct {
        BOOL is_dri2;
        struct DRI2priv *dri2_priv;
        GLuint fbo_read;
        GLuint fbo_write;
        GLuint texture_read;
        GLuint texture_write;
    } dri2_info;
#endif
    BOOL last_present_was_flip;
    PRESENTPixmapPriv *next;
};

static PRESENTPixmapPriv *PRESENTFindPixmapPriv(PRESENTpriv *present_priv, uint32_t serial)
{
    PRESENTPixmapPriv *current = present_priv->first_present_priv;

    while (current) {
        if (current->serial == serial)
            return current;
        current = current->next;
    }
    return NULL;
}

static void PRESENThandle_events(PRESENTpriv *present_priv, xcb_present_generic_event_t *ge)
{
    PRESENTPixmapPriv *present_pixmap_priv = NULL;

    switch (ge->evtype) {
        case XCB_PRESENT_COMPLETE_NOTIFY: {
            xcb_present_complete_notify_event_t *ce = (xcb_present_complete_notify_event_t *) ge;
            if (ce->kind == XCB_PRESENT_COMPLETE_KIND_NOTIFY_MSC) {
                if (ce->serial)
                    present_priv->notify_with_serial_pending = FALSE;
                free(ce);
                return;
            }
            present_pixmap_priv = PRESENTFindPixmapPriv(present_priv, ce->serial);
            if (!present_pixmap_priv || ce->kind != XCB_PRESENT_COMPLETE_KIND_PIXMAP) {
                ERR("FATAL ERROR: PRESENT handling failed\n");
                free(ce);
                return;
            }
            present_pixmap_priv->present_complete_pending = FALSE;
            switch (ce->mode) {
                case XCB_PRESENT_COMPLETE_MODE_FLIP:
                    present_pixmap_priv->last_present_was_flip = TRUE;
                    break;
                case XCB_PRESENT_COMPLETE_MODE_COPY:
                    present_pixmap_priv->last_present_was_flip = FALSE;
                    break;
            }
            present_priv->pixmap_present_pending--;
            present_priv->last_msc = ce->msc;
            break;
        }
        case XCB_PRESENT_EVENT_IDLE_NOTIFY: {
            xcb_present_idle_notify_event_t *ie = (xcb_present_idle_notify_event_t *) ge;
            present_pixmap_priv = PRESENTFindPixmapPriv(present_priv, ie->serial);
            if (!present_pixmap_priv || present_pixmap_priv->pixmap != ie->pixmap) {
                ERR("FATAL ERROR: PRESENT handling failed\n");
                free(ie);
                return;
            }
            present_pixmap_priv->released = TRUE;
            break;
        }
    }
    free(ge);
}

static void PRESENTflush_events(PRESENTpriv *present_priv, BOOL assert_no_other_thread_waiting)
{
    xcb_generic_event_t *ev;

    if ((present_priv->xcb_wait && !assert_no_other_thread_waiting) || /* don't steal events to someone waiting */
        !present_priv->special_event)
        return;

    //while ((ev = xcb_poll_for_special_event(present_priv->xcb_connection, present_priv->special_event)) != NULL) {
    while ((ev = xcb_poll_for_special_event_dyn(present_priv->xcb_connection, present_priv->special_event)) != NULL) {
        PRESENThandle_events(present_priv, (xcb_present_generic_event_t *) ev);
    }
}

static BOOL PRESENTwait_events(PRESENTpriv *present_priv, BOOL allow_other_threads)
{
    xcb_generic_event_t *ev;

    if (allow_other_threads) {
        present_priv->xcb_wait = TRUE;
        pthread_mutex_lock(&present_priv->mutex_xcb_wait);
        pthread_mutex_unlock(&present_priv->mutex_present);
    }
    //ev = xcb_wait_for_special_event(present_priv->xcb_connection, present_priv->special_event);
    ev = xcb_wait_for_special_event_dyn(present_priv->xcb_connection, present_priv->special_event);
    if (allow_other_threads) {
        pthread_mutex_unlock(&present_priv->mutex_xcb_wait);
        pthread_mutex_lock(&present_priv->mutex_present);
        present_priv->xcb_wait = FALSE;
    }
    if (!ev) {
        ERR("FATAL error: xcb had an error\n");
        return FALSE;
    }

    PRESENThandle_events(present_priv, (xcb_present_generic_event_t *) ev);
    return TRUE;
}

static struct xcb_connection_t *
create_xcb_connection(Display *dpy)
{
    int screen_num = DefaultScreen(dpy);
    xcb_connection_t *ret;
    xcb_xfixes_query_version_cookie_t cookie;
    xcb_xfixes_query_version_reply_t *rep;

    //ret = xcb_connect(DisplayString(dpy), &screen_num);
    ret = xcb_connect_dyn(DisplayString(dpy), &screen_num);
    cookie = xcb_xfixes_query_version_unchecked_dyn(ret, XCB_XFIXES_MAJOR_VERSION, XCB_XFIXES_MINOR_VERSION);
    rep = xcb_xfixes_query_version_reply_dyn(ret, cookie, NULL);
    if (rep)
        free(rep);
    return ret;
}

BOOL
PRESENTInit(Display *dpy, PRESENTpriv **present_priv)
{
    *present_priv = (PRESENTpriv *) calloc(1, sizeof(PRESENTpriv));
    if (!*present_priv) {
        return FALSE;
    }
    (*present_priv)->xcb_connection = create_xcb_connection(dpy);
    (*present_priv)->xcb_connection_bis = create_xcb_connection(dpy);
    pthread_mutex_init(&(*present_priv)->mutex_present, NULL);
    pthread_mutex_init(&(*present_priv)->mutex_xcb_wait, NULL);
    return TRUE;
}

static void PRESENTForceReleases(PRESENTpriv *present_priv)
{
    PRESENTPixmapPriv *current = NULL;

    if (!present_priv->window)
        return;

    /* There should be no other thread listening for events here.
     * This can happen when hDestWindowOverride changes without reset.
     * This case should never happen, but can happen in theory.*/
    if (present_priv->xcb_wait) {
        //xcb_present_notify_msc(present_priv->xcb_connection, present_priv->window, 0, 0, 0, 0);
        xcb_present_notify_msc_dyn(present_priv->xcb_connection, present_priv->window, 0, 0, 0, 0);
        //xcb_flush(present_priv->xcb_connection);
        xcb_flush_dyn(present_priv->xcb_connection);
        pthread_mutex_lock(&present_priv->mutex_xcb_wait);
        pthread_mutex_unlock(&present_priv->mutex_xcb_wait);
        /* the problem here is that we don't have access to the event the other thread got.
         * It is either presented event, idle event or notify event.
         */
        while (present_priv->pixmap_present_pending >= 2)
            PRESENTwait_events(present_priv, FALSE);
        PRESENTflush_events(present_priv, TRUE);
        /* Remaining events to come can be a pair of present/idle,
         * or an idle, or nothing. To be sure we are after all pixmaps
         * have been presented, add an event to the queue that can only
         * be after the present event, then if we receive an event more,
         * we are sure all pixmaps were presented */
        present_priv->notify_with_serial_pending = TRUE;
        //xcb_present_notify_msc(present_priv->xcb_connection, present_priv->window, 1, present_priv->last_target + 5, 0, 0);
        xcb_present_notify_msc_dyn(present_priv->xcb_connection, present_priv->window, 1, present_priv->last_target + 5, 0, 0);
        //xcb_flush(present_priv->xcb_connection);
        xcb_flush_dyn(present_priv->xcb_connection);
        while (present_priv->notify_with_serial_pending)
            PRESENTwait_events(present_priv, FALSE);
        /* Now we are sure we are not expecting any new event */
    } else {
        while (present_priv->pixmap_present_pending) /* wait all sent pixmaps are presented */
            PRESENTwait_events(present_priv, FALSE);
        PRESENTflush_events(present_priv, TRUE); /* may be remaining idle event */
        /* Since idle events are send with the complete events when it is not flips,
         * we are not expecting any new event here */
    }

    current = present_priv->first_present_priv;
    while (current) {
        if (!current->released) {
            if (!current->last_present_was_flip && !present_priv->xcb_wait) {
                ERR("ERROR: a pixmap seems not released by PRESENT for no reason. Code bug.\n");
            } else {
                /* Present the same pixmap with a non-valid part to force the copy mode and the releases */
                xcb_xfixes_region_t valid, update;
                xcb_rectangle_t rect_update;
                rect_update.x = 0;
                rect_update.y = 0;
                rect_update.width = 8;
                rect_update.height = 1;
                //valid = xcb_generate_id(present_priv->xcb_connection);
                valid = xcb_generate_id_dyn(present_priv->xcb_connection);
                //update = xcb_generate_id(present_priv->xcb_connection);
                update = xcb_generate_id_dyn(present_priv->xcb_connection);
                //xcb_xfixes_create_region(present_priv->xcb_connection, valid, 1, &rect_update);
                xcb_xfixes_create_region_dyn(present_priv->xcb_connection, valid, 1, &rect_update);
                //xcb_xfixes_create_region(present_priv->xcb_connection, update, 1, &rect_update);
                xcb_xfixes_create_region_dyn(present_priv->xcb_connection, update, 1, &rect_update);
                /* here we know the pixmap has been presented. Thus if it is on screen,
                 * the following request can only make it released by the server if it is not */
                //xcb_present_pixmap(present_priv->xcb_connection, present_priv->window,
                xcb_present_pixmap_dyn(present_priv->xcb_connection, present_priv->window,
                                   current->pixmap, 0, valid, update, 0, 0, None, None,
                                   None, XCB_PRESENT_OPTION_COPY | XCB_PRESENT_OPTION_ASYNC, 0, 0, 0, 0, NULL);
                //xcb_flush(present_priv->xcb_connection);
                xcb_flush_dyn(present_priv->xcb_connection);
                PRESENTwait_events(present_priv, FALSE); /* by assumption this can only be idle event */
                PRESENTflush_events(present_priv, TRUE); /* Shoudln't be needed */
            }
        }
        current = current->next;
    }
    /* Now all pixmaps are released (possibility if xcb_wait is true that one is not aware yet),
     * and we don't expect any new Present event to come from Xserver */
}

static void PRESENTFreeXcbQueue(PRESENTpriv *present_priv)
{
    if (present_priv->window) {
        //xcb_unregister_for_special_event(present_priv->xcb_connection, present_priv->special_event);
        xcb_unregister_for_special_event_dyn(present_priv->xcb_connection, present_priv->special_event);
        present_priv->last_msc = 0;
        present_priv->last_target = 0;
        present_priv->special_event = NULL;
    }
}

static BOOL PRESENTPrivChangeWindow(PRESENTpriv *present_priv, XID window)
{
    xcb_void_cookie_t cookie;
    xcb_generic_error_t *error;
    xcb_present_event_t eid;

    PRESENTForceReleases(present_priv);
    PRESENTFreeXcbQueue(present_priv);
    present_priv->window = window;

    if (window) {
        //cookie = xcb_present_select_input_checked(present_priv->xcb_connection,
        cookie = xcb_present_select_input_checked_dyn(present_priv->xcb_connection,
                                                  //(eid = xcb_generate_id(present_priv->xcb_connection)),
                                                  (eid = xcb_generate_id_dyn(present_priv->xcb_connection)),
                                                  window,
                                                  XCB_PRESENT_EVENT_MASK_COMPLETE_NOTIFY|
                                                  XCB_PRESENT_EVENT_MASK_IDLE_NOTIFY);
        //present_priv->special_event = xcb_register_for_special_xge(present_priv->xcb_connection,
        present_priv->special_event = xcb_register_for_special_xge_dyn(present_priv->xcb_connection,
                                                                   p_present_id,
                                                                   eid, NULL);
        //error = xcb_request_check(present_priv->xcb_connection, cookie); /* performs a flush */
        error = xcb_request_check_dyn(present_priv->xcb_connection, cookie); /* performs a flush */
        if (error || !present_priv->special_event) {
            ERR("FAILED to use the X PRESENT extension. Was the destination a window ?\n");
            if (present_priv->special_event)
                //xcb_unregister_for_special_event(present_priv->xcb_connection, present_priv->special_event);
                xcb_unregister_for_special_event_dyn(present_priv->xcb_connection, present_priv->special_event);
            present_priv->special_event = NULL;
            present_priv->window = 0;
        }
    }
    return (present_priv->window != 0);
}

/* Destroy the content, except the link and the struct mem */
static void
PRESENTDestroyPixmapContent(Display *dpy, PRESENTPixmapPriv *present_pixmap)
{
    //XFreePixmap(dpy, present_pixmap->pixmap);
    XFreePixmap_dyn(dpy, present_pixmap->pixmap);
}

void
PRESENTDestroy(Display *dpy, PRESENTpriv *present_priv)
{
    PRESENTPixmapPriv *current = NULL;

    pthread_mutex_lock(&present_priv->mutex_present);

    PRESENTForceReleases(present_priv);

    current = present_priv->first_present_priv;
    while (current) {
        PRESENTPixmapPriv *next = current->next;
        PRESENTDestroyPixmapContent(dpy, current);
        free(current);
        current = next;
    }

    PRESENTFreeXcbQueue(present_priv);

    //xcb_disconnect(present_priv->xcb_connection);
    xcb_disconnect_dyn(present_priv->xcb_connection);
    //xcb_disconnect(present_priv->xcb_connection_bis);
    xcb_disconnect_dyn(present_priv->xcb_connection_bis);
    pthread_mutex_unlock(&present_priv->mutex_present);
    pthread_mutex_destroy(&present_priv->mutex_present);
    pthread_mutex_destroy(&present_priv->mutex_xcb_wait);

    free(present_priv);
}

BOOL
PRESENTPixmapInit(PRESENTpriv *present_priv, Pixmap pixmap, PRESENTPixmapPriv **present_pixmap_priv)
{
    xcb_get_geometry_cookie_t cookie;
    xcb_get_geometry_reply_t *reply;

    //cookie = xcb_get_geometry(present_priv->xcb_connection, pixmap);
    cookie = xcb_get_geometry_dyn(present_priv->xcb_connection, pixmap);
    //reply = xcb_get_geometry_reply(present_priv->xcb_connection, cookie, NULL);
    reply = xcb_get_geometry_reply_dyn(present_priv->xcb_connection, cookie, NULL);

    if (!reply)
        return FALSE;

    *present_pixmap_priv = (PRESENTPixmapPriv *) calloc(1, sizeof(PRESENTPixmapPriv));
    if (!*present_pixmap_priv) {
        free(reply);
        return FALSE;
    }
    pthread_mutex_lock(&present_priv->mutex_present);

    (*present_pixmap_priv)->released = TRUE;
    (*present_pixmap_priv)->pixmap = pixmap;
    (*present_pixmap_priv)->present_priv = present_priv;
    (*present_pixmap_priv)->next = present_priv->first_present_priv;
    (*present_pixmap_priv)->width = reply->width;
    (*present_pixmap_priv)->height = reply->height;
    (*present_pixmap_priv)->depth = reply->depth;

    free(reply);

    present_priv->last_serial_given++;
    (*present_pixmap_priv)->serial = present_priv->last_serial_given;
    present_priv->first_present_priv = *present_pixmap_priv;

    pthread_mutex_unlock(&present_priv->mutex_present);
    return TRUE;
}


BOOL
PRESENTTryFreePixmap(Display *dpy, PRESENTPixmapPriv *present_pixmap_priv)
{
    PRESENTpriv *present_priv = present_pixmap_priv->present_priv;
    PRESENTPixmapPriv *current;

    pthread_mutex_lock(&present_priv->mutex_present);

    if (!present_pixmap_priv->released || present_pixmap_priv->present_complete_pending) {
        pthread_mutex_unlock(&present_priv->mutex_present);
        return FALSE;
    }

    if (present_priv->first_present_priv == present_pixmap_priv) {
        present_priv->first_present_priv = present_pixmap_priv->next;
        goto free_priv;
    }

    current = present_priv->first_present_priv;
    while (current->next != present_pixmap_priv)
        current = current->next;
    current->next = present_pixmap_priv->next;
free_priv:
    PRESENTDestroyPixmapContent(dpy, present_pixmap_priv);
    free(present_pixmap_priv);
    pthread_mutex_unlock(&present_priv->mutex_present);
    return TRUE;
}

BOOL
PRESENTHelperCopyFront(Display *dpy, PRESENTPixmapPriv *present_pixmap_priv)
{
    PRESENTpriv *present_priv = present_pixmap_priv->present_priv;
    xcb_void_cookie_t cookie;
    xcb_generic_error_t *error;

    uint32_t v = 0;
    xcb_gcontext_t gc;

    pthread_mutex_lock(&present_priv->mutex_present);

    if (!present_priv->window) {
        pthread_mutex_unlock(&present_priv->mutex_present);
        return FALSE;
    }

    //xcb_create_gc(present_priv->xcb_connection,
    xcb_create_gc_dyn(present_priv->xcb_connection,
                  //(gc = xcb_generate_id(present_priv->xcb_connection)),
                  (gc = xcb_generate_id_dyn(present_priv->xcb_connection)),
                  present_priv->window,
                  XCB_GC_GRAPHICS_EXPOSURES,
                  &v);
    //cookie = xcb_copy_area_checked(present_priv->xcb_connection,
    cookie = xcb_copy_area_checked_dyn(present_priv->xcb_connection,
                                   present_priv->window,
                                   present_pixmap_priv->pixmap,
                                   gc,
                                   0, 0, 0, 0,
                                   present_pixmap_priv->width,
                                   present_pixmap_priv->height);
    //error = xcb_request_check(present_priv->xcb_connection, cookie);
    error = xcb_request_check_dyn(present_priv->xcb_connection, cookie);
    //xcb_free_gc(present_priv->xcb_connection, gc);
    xcb_free_gc_dyn(present_priv->xcb_connection, gc);
    pthread_mutex_unlock(&present_priv->mutex_present);
    return (error != NULL);
}

BOOL
PRESENTPixmap(Display *dpy, XID window,
              PRESENTPixmapPriv *present_pixmap_priv, D3DPRESENT_PARAMETERS *pPresentationParameters,
              const RECT *pSourceRect, const RECT *pDestRect, const RGNDATA *pDirtyRegion)
{
    PRESENTpriv *present_priv = present_pixmap_priv->present_priv;

    xcb_void_cookie_t cookie;
    xcb_generic_error_t *error;
    int64_t target_msc, presentationInterval;
    xcb_xfixes_region_t valid, update;
    int16_t x_off, y_off;
    uint32_t options = XCB_PRESENT_OPTION_NONE;

    pthread_mutex_lock(&present_priv->mutex_present);

    if (window != present_priv->window)
        PRESENTPrivChangeWindow(present_priv, window);

    if (!window) {
        ERR("ERROR: Try to Present a pixmap on a NULL window\n");
        pthread_mutex_unlock(&present_priv->mutex_present);
        return FALSE;
    }

    PRESENTflush_events(present_priv, FALSE);
    if (!present_pixmap_priv->released || present_pixmap_priv->present_complete_pending) {
        ERR("FATAL ERROR: Trying to Present a pixmap not released\n");
        pthread_mutex_unlock(&present_priv->mutex_present);
        return FALSE;
    }

    target_msc = present_priv->last_msc;
    switch(pPresentationParameters->PresentationInterval) {
        case D3DPRESENT_INTERVAL_DEFAULT:
        case D3DPRESENT_INTERVAL_ONE:
            presentationInterval = 1;
            break;
        case D3DPRESENT_INTERVAL_TWO:
            presentationInterval = 2;
            break;
        case D3DPRESENT_INTERVAL_THREE:
            presentationInterval = 3;
            break;
        case D3DPRESENT_INTERVAL_FOUR:
            presentationInterval = 4;
            break;
        case D3DPRESENT_INTERVAL_IMMEDIATE:
        default:
            presentationInterval = 0;
            options |= XCB_PRESENT_OPTION_ASYNC;
            break;
    }
    target_msc += presentationInterval * (present_priv->pixmap_present_pending + 1);

    /* Note: PRESENT defines some way to do partial copy:
     * presentproto:
     * 'x-off' and 'y-off' define the location in the window where
     *  the 0,0 location of the pixmap will be presented. valid-area
     *  and update-area are relative to the pixmap.
     */
    if (!pSourceRect && !pDestRect && !pDirtyRegion) {
        valid = 0;
        update = 0;
        x_off = 0;
        y_off = 0;
    } else {
        xcb_rectangle_t rect_update;
        xcb_rectangle_t *rect_updates;
        int i;

        rect_update.x = 0;
        rect_update.y = 0;
        rect_update.width = present_pixmap_priv->width;
        rect_update.height = present_pixmap_priv->height;
        x_off = 0;
        y_off = 0;
        if (pSourceRect) {
            x_off = -pSourceRect->left;
            y_off = -pSourceRect->top;
            rect_update.x = pSourceRect->left;
            rect_update.y = pSourceRect->top;
            rect_update.width = pSourceRect->right - pSourceRect->left;
            rect_update.height = pSourceRect->bottom - pSourceRect->top;
        }
        if (pDestRect) {
            x_off += pDestRect->left;
            y_off += pDestRect->top;
            rect_update.width = pDestRect->right - pDestRect->left;
            rect_update.height = pDestRect->bottom - pDestRect->top;
            /* Note: the size of pDestRect and pSourceRect are supposed to be the same size
             * because the driver would have done things to assure that. */
        }
        //valid = xcb_generate_id(present_priv->xcb_connection_bis);
        valid = xcb_generate_id_dyn(present_priv->xcb_connection_bis);
        //update = xcb_generate_id(present_priv->xcb_connection_bis);
        update = xcb_generate_id_dyn(present_priv->xcb_connection_bis);
        //xcb_xfixes_create_region(present_priv->xcb_connection_bis, valid, 1, &rect_update);
        xcb_xfixes_create_region_dyn(present_priv->xcb_connection_bis, valid, 1, &rect_update);
        if (pDirtyRegion && pDirtyRegion->rdh.nCount) {
            rect_updates = (xcb_rectangle_t *) calloc(pDirtyRegion->rdh.nCount, sizeof(xcb_rectangle_t));
            for (i = 0; i < pDirtyRegion->rdh.nCount; i++)
            {
                RECT rc;
                memcpy(&rc, pDirtyRegion->Buffer + i * sizeof(RECT), sizeof(RECT));
                rect_update.x = rc.left;
                rect_update.y = rc.top;
                rect_update.width = rc.right - rc.left;
                rect_update.height = rc.bottom - rc.top;
                memcpy(rect_updates + i * sizeof(xcb_rectangle_t), &rect_update, sizeof(xcb_rectangle_t));
            }
            //xcb_xfixes_create_region(present_priv->xcb_connection_bis, update, pDirtyRegion->rdh.nCount, rect_updates);
            xcb_xfixes_create_region_dyn(present_priv->xcb_connection_bis, update, pDirtyRegion->rdh.nCount, rect_updates);
            free(rect_updates);
        } else
            //xcb_xfixes_create_region(present_priv->xcb_connection_bis, update, 1, &rect_update);
            xcb_xfixes_create_region_dyn(present_priv->xcb_connection_bis, update, 1, &rect_update);
    }
    if (pPresentationParameters->SwapEffect == D3DSWAPEFFECT_COPY)
        options |= XCB_PRESENT_OPTION_COPY;
    //cookie = xcb_present_pixmap_checked(present_priv->xcb_connection_bis,
    cookie = xcb_present_pixmap_checked_dyn(present_priv->xcb_connection_bis,
                                        window,
                                        present_pixmap_priv->pixmap,
                                        present_pixmap_priv->serial,
                                        valid, update, x_off, y_off,
                                        None, None, None, options,
                                        target_msc, 0, 0, 0, NULL);
    //error = xcb_request_check(present_priv->xcb_connection_bis, cookie); /* performs a flush */
    error = xcb_request_check_dyn(present_priv->xcb_connection_bis, cookie); /* performs a flush */

    if (update)
        //xcb_xfixes_destroy_region(present_priv->xcb_connection_bis, update);
        xcb_xfixes_destroy_region_dyn(present_priv->xcb_connection_bis, update);
    if (valid)
        //xcb_xfixes_destroy_region(present_priv->xcb_connection_bis, valid);
        xcb_xfixes_destroy_region_dyn(present_priv->xcb_connection_bis, valid);

    if (error) {
        xcb_get_geometry_cookie_t cookie_geom;
        xcb_get_geometry_reply_t *reply;

        //cookie_geom = xcb_get_geometry(present_priv->xcb_connection_bis, window);
        cookie_geom = xcb_get_geometry_dyn(present_priv->xcb_connection_bis, window);
        //reply = xcb_get_geometry_reply(present_priv->xcb_connection_bis, cookie_geom, NULL);
        reply = xcb_get_geometry_reply_dyn(present_priv->xcb_connection_bis, cookie_geom, NULL);

        ERR("Error using PRESENT. Here some debug info\n");
        if (!reply) {
            ERR("Error querying window info. Perhaps it doesn't exist anymore\n");
            pthread_mutex_unlock(&present_priv->mutex_present);
            return FALSE;
        }
        ERR("Pixmap: width=%d, height=%d, depth=%d\n",
            present_pixmap_priv->width, present_pixmap_priv->height,
            present_pixmap_priv->depth);
        ERR("Window: width=%d, height=%d, depth=%d, x=%d, y=%d\n",
            (int) reply->width, (int) reply->height,
            (int) reply->depth, (int) reply->x, (int) reply->y);
        ERR("Present parameter: PresentationInterval=%d, BackBufferCount=%d, Pending presentations=%d\n",
            pPresentationParameters->PresentationInterval,
            pPresentationParameters->BackBufferCount,
            present_priv->pixmap_present_pending
           );
        if (present_pixmap_priv->depth != reply->depth)
            ERR("Depths are different. PRESENT needs the pixmap and the window have same depth\n");
        free(reply);
        pthread_mutex_unlock(&present_priv->mutex_present);
        return FALSE;
    }
    present_priv->last_target = target_msc;
    present_priv->pixmap_present_pending++;
    present_pixmap_priv->present_complete_pending = TRUE;
    present_pixmap_priv->released = FALSE;
    pthread_mutex_unlock(&present_priv->mutex_present);
    return TRUE;
}

BOOL
PRESENTWaitPixmapReleased(PRESENTPixmapPriv *present_pixmap_priv)
{
    PRESENTpriv *present_priv = present_pixmap_priv->present_priv;

    pthread_mutex_lock(&present_priv->mutex_present);

    PRESENTflush_events(present_priv, FALSE);

    while (!present_pixmap_priv->released || present_pixmap_priv->present_complete_pending) {
        /* Note: following if should not happen because we'll never
         * use two PRESENTWaitPixmapReleased in parallels on same window.
         * However it would make it work in that case */
        if (present_priv->xcb_wait) { /* we allow only one thread to dispatch events */
            pthread_mutex_lock(&present_priv->mutex_xcb_wait);
            /* here the other thread got an event but hasn't treated it yet */
            pthread_mutex_unlock(&present_priv->mutex_xcb_wait);
            pthread_mutex_unlock(&present_priv->mutex_present);
            struct timespec duration = { 0, 10 * 1000*1000 };
            nanosleep(&duration,NULL); /* Let it treat the event */
            pthread_mutex_lock(&present_priv->mutex_present);
        } else if (!PRESENTwait_events(present_priv, TRUE)) {
            pthread_mutex_unlock(&present_priv->mutex_present);
            return FALSE;
        }
    }
    pthread_mutex_unlock(&present_priv->mutex_present);
    return TRUE;
}
