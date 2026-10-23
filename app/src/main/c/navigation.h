#ifndef ZONEEDIT_NAVIGATION_H
#define ZONEEDIT_NAVIGATION_H

/* This is the explicit ordinary-C NDK/JNI boundary. */
enum ze_navigation { ZE_BLOCK, ZE_INTERNAL, ZE_BROWSER };
enum ze_navigation ze_navigation_for(const char *url, int main_frame);
int ze_https_url(const char *url);

#endif
