#include "navigation.h"
#include <stddef.h>
#include <string.h>

static unsigned char lower(unsigned char c) {
    return c >= 'A' && c <= 'Z' ? (unsigned char)(c + ('a' - 'A')) : c;
}

/* Parse the authority before comparing hosts; reject parser-confusion inputs.
 * URLs supplied by WebView are normally canonical, but restored history and
 * callback arguments still cross an untrusted text boundary. */
static int https_host(const char *url, char host[254], int *port) {
    static const char prefix[] = "https://";
    if (!url) return 0;
    for (size_t i = 0; i < sizeof(prefix) - 1; ++i)
        if (!url[i] || lower((unsigned char)url[i]) != prefix[i]) return 0;
    for (const unsigned char *p = (const unsigned char *)url; *p; ++p)
        if (*p <= 32 || *p == 127 || *p == '\\') return 0;
    const char *start = url + sizeof(prefix) - 1;
    const char *end = start;
    while (*end && *end != '/' && *end != '?' && *end != '#') ++end;
    const char *colon = NULL;
    for (const char *p = start; p < end; ++p) {
        if (*p == ':') { if (colon) return 0; colon = p; }
        else if (*p == '@' || *p == '%' || *p == '[' || *p == ']') return 0;
    }
    const char *host_end = colon ? colon : end;
    size_t size = (size_t)(host_end - start);
    if (!size || size > 253) return 0;
    size_t label = 0;
    for (size_t i = 0; i < size; ++i) {
        unsigned char c = lower((unsigned char)start[i]);
        if (c == '.') {
            if (!label || start[i - 1] == '-') return 0;
            label = 0;
        } else {
            if (!((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-')) return 0;
            if ((!label && c == '-') || ++label > 63) return 0;
        }
        host[i] = (char)c;
    }
    if (!label || start[size - 1] == '-') return 0;
    host[size] = 0;
    *port = 443;
    if (colon) {
        if (colon + 1 == end) return 0;
        unsigned value = 0;
        for (const char *p = colon + 1; p < end; ++p) {
            if (*p < '0' || *p > '9') return 0;
            value = value * 10 + (unsigned)(*p - '0');
            if (value > 65535) return 0;
        }
        if (!value) return 0;
        *port = (int)value;
    }
    return 1;
}

int ze_https_url(const char *url) {
    char host[254]; int port;
    return https_host(url, host, &port);
}

enum ze_navigation ze_navigation_for(const char *url, int main_frame) {
    char host[254]; int port;
    if (!https_host(url, host, &port)) return ZE_BLOCK;
    size_t length = strlen(host);
    static const char suffix[] = ".zoneedit.com";
    int internal = !strcmp(host, "zoneedit.com") ||
        (length > sizeof(suffix) - 1 &&
         !strcmp(host + length - (sizeof(suffix) - 1), suffix));
    if (internal) return port == 443 ? ZE_INTERNAL : ZE_BLOCK;
    return main_frame ? ZE_BROWSER : ZE_BLOCK;
}
