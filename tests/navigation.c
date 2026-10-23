#include "../app/src/main/c/navigation.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    const struct { const char *url; enum ze_navigation expected; } cases[] = {
        {"https://cp.zoneedit.com/login.php", ZE_INTERNAL},
        {"HTTPS://ZONEEDIT.COM/", ZE_INTERNAL},
        {"https://a.b.zoneedit.com:443/edit?x=1#record", ZE_INTERNAL},
        {"https://zoneedit.com.evil.test/", ZE_BROWSER},
        {"https://notzoneedit.com/", ZE_BROWSER},
        {"https://example.com:8443/help", ZE_BROWSER},
        {"http://zoneedit.com/", ZE_BLOCK},
        {"javascript:alert(1)", ZE_BLOCK},
        {"file:///data/private", ZE_BLOCK},
        {"content://zoneedit.com/", ZE_BLOCK},
        {"intent://zoneedit.com/", ZE_BLOCK},
        {"https:///zoneedit.com", ZE_BLOCK},
        {"https://zoneedit.com@evil.test/", ZE_BLOCK},
        {"https://evil.test@zoneedit.com/", ZE_BLOCK},
        {"https://zoneedit.com%2eevil.test/", ZE_BLOCK},
        {"https://zoneedit.com\\@evil.test/", ZE_BLOCK},
        {"https://zoneedit.com:8443/", ZE_BLOCK},
        {"https://zoneedit.com:999999999999/", ZE_BLOCK},
        {"https://zoneedit.com:/", ZE_BLOCK},
        {"https://zoneedit.com:0/", ZE_BLOCK},
        {"https://zoneedit.com./", ZE_BLOCK},
        {"https://.zoneedit.com/", ZE_BLOCK},
        {"https://a..zoneedit.com/", ZE_BLOCK},
        {"https://-a.zoneedit.com/", ZE_BLOCK},
        {"https://a-.zoneedit.com/", ZE_BLOCK},
        {"https://zoneedit.com/\nhttps://evil.test/", ZE_BLOCK},
        {"", ZE_BLOCK}, {NULL, ZE_BLOCK}
    };
    for (size_t i = 0; i < sizeof(cases)/sizeof(cases[0]); ++i) {
        if (ze_navigation_for(cases[i].url, 1) != cases[i].expected) {
            fprintf(stderr, "FAIL policy case %zu\n", i); return 1;
        }
    }
    if (ze_navigation_for("https://example.com/frame", 0) != ZE_BLOCK ||
        ze_navigation_for("https://cp.zoneedit.com/frame", 0) != ZE_INTERNAL) return 1;
    puts("PASS: 28 URL cases and internal/external frame boundaries");
    return 0;
}
