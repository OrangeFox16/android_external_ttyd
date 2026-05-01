#include <libwebsockets.h>

#ifndef LWS_WITH_LIBUV
#error "ttyd requires libwebsockets built with LWS_WITH_LIBUV=1"
#endif

#ifndef LWS_WITH_SERVER
#error "ttyd requires libwebsockets server support"
#endif

#ifndef LWS_ROLE_WS
#error "ttyd requires websocket role support in libwebsockets"
#endif
