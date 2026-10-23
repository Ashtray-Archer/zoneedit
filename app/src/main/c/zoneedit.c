/* Ordinary C at the Android NDK/JNI interface; no generated Java code. */
#include <jni.h>
#include <android/log.h>
#include <stdarg.h>
#include <string.h>
#include "navigation.h"
#include "mobile-css.h"

#define WEB_ID 0x00e00101
#define ACTIVITY "android/app/Activity"
#define VIEW "android/webkit/WebView"
#define BUNDLE "(Landroid/os/Bundle;)V"

static jmethodID method(JNIEnv *e, jobject object, const char *name, const char *signature) {
    if (!object || (*e)->ExceptionCheck(e)) return NULL;
    jclass type = (*e)->GetObjectClass(e, object);
    if (!type) return NULL;
    jmethodID id = (*e)->GetMethodID(e, type, name, signature);
    (*e)->DeleteLocalRef(e, type);
    return id;
}

static void call(JNIEnv *e, jobject object, const char *name, const char *signature, ...) {
    jmethodID id = method(e, object, name, signature);
    if (!id) return;
    va_list args; va_start(args, signature);
    (*e)->CallVoidMethodV(e, object, id, args);
    va_end(args);
}

static jobject object_call(JNIEnv *e, jobject object, const char *name, const char *signature, ...) {
    jmethodID id = method(e, object, name, signature);
    if (!id) return NULL;
    va_list args; va_start(args, signature);
    jobject result = (*e)->CallObjectMethodV(e, object, id, args);
    va_end(args);
    return result;
}

static jboolean bool_call(JNIEnv *e, jobject object, const char *name) {
    jmethodID id = method(e, object, name, "()Z");
    return id ? (*e)->CallBooleanMethod(e, object, id) : JNI_FALSE;
}

static void super_call(JNIEnv *e, jobject activity, const char *name, jobject state) {
    if ((*e)->ExceptionCheck(e)) return;
    jclass base = (*e)->FindClass(e, ACTIVITY);
    if (!base) return;
    const char *signature = state || !strcmp(name, "onCreate") || !strcmp(name, "onSaveInstanceState")
        ? BUNDLE : "()V";
    jmethodID id = (*e)->GetMethodID(e, base, name, signature);
    if (id) (*e)->CallNonvirtualVoidMethod(e, activity, base, id, state);
    (*e)->DeleteLocalRef(e, base);
}

static jobject web_view(JNIEnv *e, jobject activity) {
    return object_call(e, activity, "findViewById", "(I)Landroid/view/View;", WEB_ID);
}

static jobject cookies(JNIEnv *e) {
    if ((*e)->ExceptionCheck(e)) return NULL;
    jclass type = (*e)->FindClass(e, "android/webkit/CookieManager");
    if (!type) return NULL;
    jmethodID get = (*e)->GetStaticMethodID(e, type, "getInstance", "()Landroid/webkit/CookieManager;");
    jobject result = get ? (*e)->CallStaticObjectMethod(e, type, get) : NULL;
    (*e)->DeleteLocalRef(e, type);
    return result;
}

static void load_start(JNIEnv *e, jobject view) {
    jstring start = (*e)->NewStringUTF(e, "https://cp.zoneedit.com/login.php");
    call(e, view, "loadUrl", "(Ljava/lang/String;)V", start);
    (*e)->DeleteLocalRef(e, start);
}

static enum ze_navigation navigation(JNIEnv *e, jstring url, int main_frame) {
    if (!url || (*e)->ExceptionCheck(e)) return ZE_BLOCK;
    const char *text = (*e)->GetStringUTFChars(e, url, NULL);
    if (!text) return ZE_BLOCK;
    enum ze_navigation action = ze_navigation_for(text, main_frame);
    (*e)->ReleaseStringUTFChars(e, url, text);
    return action;
}

static int safe_history(JNIEnv *e, jobject history) {
    jmethodID size_method = method(e, history, "getSize", "()I");
    if (!size_method) return 0;
    jint count = (*e)->CallIntMethod(e, history, size_method);
    if ((*e)->ExceptionCheck(e) || count <= 0) return 0;
    for (jint index = 0; index < count; ++index) {
        jobject item = object_call(e, history, "getItemAtIndex", "(I)Landroid/webkit/WebHistoryItem;", index);
        jstring url = (jstring)object_call(e, item, "getUrl", "()Ljava/lang/String;");
        int allowed = navigation(e, url, 1) == ZE_INTERNAL;
        if (url) (*e)->DeleteLocalRef(e, url);
        if (item) (*e)->DeleteLocalRef(e, item);
        if (!allowed) return 0;
    }
    return 1;
}

static jstring request_url(JNIEnv *e, jobject request) {
    jobject uri = object_call(e, request, "getUrl", "()Landroid/net/Uri;");
    jstring text = (jstring)object_call(e, uri, "toString", "()Ljava/lang/String;");
    if (uri) (*e)->DeleteLocalRef(e, uri);
    return text;
}

static void open_browser(JNIEnv *e, jobject view, jstring url) {
    jobject context = object_call(e, view, "getContext", "()Landroid/content/Context;");
    jclass uri_type = (*e)->FindClass(e, "android/net/Uri");
    if (!uri_type || (*e)->ExceptionCheck(e)) return;
    jmethodID parse = (*e)->GetStaticMethodID(e, uri_type, "parse", "(Ljava/lang/String;)Landroid/net/Uri;");
    jobject uri = parse ? (*e)->CallStaticObjectMethod(e, uri_type, parse, url) : NULL;
    jclass type = (*e)->FindClass(e, "android/content/Intent");
    if (!type || !uri || (*e)->ExceptionCheck(e)) return;
    jmethodID init = (*e)->GetMethodID(e, type, "<init>", "(Ljava/lang/String;Landroid/net/Uri;)V");
    jstring action = (*e)->NewStringUTF(e, "android.intent.action.VIEW");
    jobject intent = init ? (*e)->NewObject(e, type, init, action, uri) : NULL;
    call(e, context, "startActivity", "(Landroid/content/Intent;)V", intent);
    /* A device without a browser must still keep the external URL out of WebView. */
    if ((*e)->ExceptionCheck(e)) {
        jthrowable failure = (*e)->ExceptionOccurred(e);
        (*e)->ExceptionClear(e);
        jclass unavailable = (*e)->FindClass(e, "android/content/ActivityNotFoundException");
        if (!unavailable || !(*e)->IsInstanceOf(e, failure, unavailable))
            (*e)->Throw(e, failure);
        (*e)->DeleteLocalRef(e, failure);
    }
}

static void create(JNIEnv *e, jobject activity, jobject state) {
    super_call(e, activity, "onCreate", state);
    if ((*e)->ExceptionCheck(e)) return;
    jclass type = (*e)->FindClass(e, VIEW);
    if (!type) return;
    jmethodID debug = (*e)->GetStaticMethodID(e, type, "setWebContentsDebuggingEnabled", "(Z)V");
    if (!debug) return;
    (*e)->CallStaticVoidMethod(e, type, debug, JNI_FALSE);
    jmethodID init = (*e)->GetMethodID(e, type, "<init>", "(Landroid/content/Context;)V");
    jobject view = init ? (*e)->NewObject(e, type, init, activity) : NULL;
    if (!view || (*e)->ExceptionCheck(e)) return;
    call(e, view, "setId", "(I)V", WEB_ID);
    call(e, view, "setImportantForAutofill", "(I)V", 1);
    call(e, activity, "setContentView", "(Landroid/view/View;)V", view);
    jobject manager = cookies(e);
    call(e, manager, "setAcceptCookie", "(Z)V", JNI_TRUE);
    call(e, manager, "setAcceptThirdPartyCookies", "(Landroid/webkit/WebView;Z)V", view, JNI_FALSE);
    jobject settings = object_call(e, view, "getSettings", "()Landroid/webkit/WebSettings;");
    call(e, settings, "setJavaScriptEnabled", "(Z)V", JNI_TRUE);
    call(e, settings, "setDomStorageEnabled", "(Z)V", JNI_TRUE);
    call(e, settings, "setUseWideViewPort", "(Z)V", JNI_TRUE);
    call(e, settings, "setLoadWithOverviewMode", "(Z)V", JNI_TRUE);
    call(e, settings, "setAllowFileAccess", "(Z)V", JNI_FALSE);
    call(e, settings, "setAllowContentAccess", "(Z)V", JNI_FALSE);
    call(e, settings, "setAllowFileAccessFromFileURLs", "(Z)V", JNI_FALSE);
    call(e, settings, "setAllowUniversalAccessFromFileURLs", "(Z)V", JNI_FALSE);
    call(e, settings, "setMixedContentMode", "(I)V", 1); /* NEVER_ALLOW */
    call(e, settings, "setSafeBrowsingEnabled", "(Z)V", JNI_TRUE);
    call(e, settings, "setBuiltInZoomControls", "(Z)V", JNI_TRUE);
    call(e, settings, "setDisplayZoomControls", "(Z)V", JNI_FALSE);
    call(e, settings, "setTextZoom", "(I)V", 110);
    call(e, settings, "setSupportMultipleWindows", "(Z)V", JNI_FALSE);
    jclass client_type = (*e)->FindClass(e, "es/isomorphism/zoneedit/ZoneEditClient");
    if (!client_type || (*e)->ExceptionCheck(e)) return;
    jmethodID client_init = (*e)->GetMethodID(e, client_type, "<init>", "()V");
    jobject client = client_init ? (*e)->NewObject(e, client_type, client_init) : NULL;
    call(e, view, "setWebViewClient", "(Landroid/webkit/WebViewClient;)V", client);
    /* Preserve ordinary JavaScript alert/confirm dialogs used by web forms. */
    jclass chrome_type = (*e)->FindClass(e, "android/webkit/WebChromeClient");
    if (!chrome_type || (*e)->ExceptionCheck(e)) return;
    jmethodID chrome_init = (*e)->GetMethodID(e, chrome_type, "<init>", "()V");
    jobject chrome = chrome_init ? (*e)->NewObject(e, chrome_type, chrome_init) : NULL;
    call(e, view, "setWebChromeClient", "(Landroid/webkit/WebChromeClient;)V", chrome);
    if ((*e)->ExceptionCheck(e)) return;
    jobject history = state ? object_call(e, view, "restoreState", "(Landroid/os/Bundle;)Landroid/webkit/WebBackForwardList;", state) : NULL;
    int restored = safe_history(e, history);
    __android_log_print(ANDROID_LOG_INFO, "ZoneEdit", "activity_create restored=%d", restored);
    if (!restored) {
        if (history) call(e, view, "clearHistory", "()V");
        load_start(e, view);
    }
}

static void save(JNIEnv *e, jobject activity, jobject state) {
    jobject view = web_view(e, activity);
    if (view) object_call(e, view, "saveState", "(Landroid/os/Bundle;)Landroid/webkit/WebBackForwardList;", state);
    __android_log_write(ANDROID_LOG_INFO, "ZoneEdit", "activity_save");
    call(e, cookies(e), "flush", "()V");
    super_call(e, activity, "onSaveInstanceState", state);
}

static void back(JNIEnv *e, jobject activity) {
    jobject view = web_view(e, activity);
    jobject history = view ? object_call(e, view, "copyBackForwardList", "()Landroid/webkit/WebBackForwardList;") : NULL;
    int can_back = view && bool_call(e, view, "canGoBack") && safe_history(e, history);
    __android_log_print(ANDROID_LOG_INFO, "ZoneEdit", "activity_back history=%d", can_back);
    if (can_back) call(e, view, "goBack", "()V");
    else call(e, activity, "finish", "()V");
}

static void pause_activity(JNIEnv *e, jobject activity) {
    jobject view = web_view(e, activity);
    if (view) call(e, view, "onPause", "()V");
    call(e, cookies(e), "flush", "()V");
    super_call(e, activity, "onPause", NULL);
}

static void resume_activity(JNIEnv *e, jobject activity) {
    super_call(e, activity, "onResume", NULL);
    jobject view = web_view(e, activity);
    if (view) call(e, view, "onResume", "()V");
}

static void destroy_activity(JNIEnv *e, jobject activity) {
    jobject view = web_view(e, activity);
    if (view) {
        call(e, view, "stopLoading", "()V");
        jobject parent = object_call(e, view, "getParent", "()Landroid/view/ViewParent;");
        if (parent) call(e, parent, "removeView", "(Landroid/view/View;)V", view);
        call(e, view, "destroy", "()V");
    }
    super_call(e, activity, "onDestroy", NULL);
}

static jboolean navigate_text(JNIEnv *e, jobject client, jobject view, jstring url) {
    (void)client;
    enum ze_navigation action = navigation(e, url, 1);
    if (action == ZE_BROWSER) open_browser(e, view, url);
    return action == ZE_INTERNAL ? JNI_FALSE : JNI_TRUE;
}

static jboolean navigate_request(JNIEnv *e, jobject client, jobject view, jobject request) {
    (void)client;
    jstring url = request_url(e, request);
    int main_frame = bool_call(e, request, "isForMainFrame");
    enum ze_navigation action = navigation(e, url, main_frame);
    if (main_frame) __android_log_print(ANDROID_LOG_INFO, "ZoneEdit",
        "navigation_decision action=%d redirect=%d", action,
        bool_call(e, request, "isRedirect"));
    if (action == ZE_BROWSER) open_browser(e, view, url);
    return action == ZE_INTERNAL ? JNI_FALSE : JNI_TRUE;
}

static jobject blocked_response(JNIEnv *e) {
    if ((*e)->ExceptionCheck(e)) return NULL;
    jclass type = (*e)->FindClass(e, "android/webkit/WebResourceResponse");
    if (!type) return NULL;
    jmethodID init = (*e)->GetMethodID(e, type, "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/io/InputStream;)V");
    jstring mime = (*e)->NewStringUTF(e, "text/plain");
    jstring encoding = (*e)->NewStringUTF(e, "UTF-8");
    jobject response = init ? (*e)->NewObject(e, type, init, mime, encoding, NULL) : NULL;
    jstring reason = (*e)->NewStringUTF(e, "Blocked navigation");
    call(e, response, "setStatusCodeAndReasonPhrase", "(ILjava/lang/String;)V", 403, reason);
    return response;
}

/* WebView omits shouldOverrideUrlLoading for POST. Fail closed for external
 * top-level requests here too, without replaying a credential-bearing POST. */
static jobject intercept(JNIEnv *e, jobject client, jobject view, jobject request) {
    (void)client; (void)view;
    jstring url = request_url(e, request);
    int main_frame = bool_call(e, request, "isForMainFrame");
    if (main_frame) return navigation(e, url, 1) == ZE_INTERNAL ? NULL : blocked_response(e);
    if (!url || (*e)->ExceptionCheck(e)) return blocked_response(e);
    const char *text = (*e)->GetStringUTFChars(e, url, NULL);
    int allowed = text && ze_https_url(text);
    if (text) (*e)->ReleaseStringUTFChars(e, url, text);
    return allowed ? NULL : blocked_response(e);
}

static void finished(JNIEnv *e, jobject client, jobject view, jstring url) {
    (void)client;
    if (navigation(e, url, 1) != ZE_INTERNAL) return;
    jstring script = (*e)->NewStringUTF(e, mobile_script);
    call(e, view, "evaluateJavascript", "(Ljava/lang/String;Landroid/webkit/ValueCallback;)V", script, NULL);
    if (!(*e)->ExceptionCheck(e)) __android_log_write(ANDROID_LOG_INFO, "ZoneEdit", "mobile_style_queued");
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    (void)reserved;
    JNIEnv *e = NULL;
    if ((*vm)->GetEnv(vm, (void **)&e, JNI_VERSION_1_6) != JNI_OK) return JNI_ERR;
    const JNINativeMethod activity_methods[] = {
        {"onCreate", BUNDLE, (void *)create},
        {"onSaveInstanceState", BUNDLE, (void *)save},
        {"onBackPressed", "()V", (void *)back},
        {"onPause", "()V", (void *)pause_activity},
        {"onResume", "()V", (void *)resume_activity},
        {"onDestroy", "()V", (void *)destroy_activity}
    };
    const JNINativeMethod client_methods[] = {
        {"shouldOverrideUrlLoading", "(Landroid/webkit/WebView;Landroid/webkit/WebResourceRequest;)Z", (void *)navigate_request},
        {"shouldOverrideUrlLoading", "(Landroid/webkit/WebView;Ljava/lang/String;)Z", (void *)navigate_text},
        {"shouldInterceptRequest", "(Landroid/webkit/WebView;Landroid/webkit/WebResourceRequest;)Landroid/webkit/WebResourceResponse;", (void *)intercept},
        {"onPageFinished", "(Landroid/webkit/WebView;Ljava/lang/String;)V", (void *)finished}
    };
    jclass activity = (*e)->FindClass(e, "es/isomorphism/zoneedit/MainActivity");
    if (!activity || (*e)->RegisterNatives(e, activity, activity_methods, 6) < 0) return JNI_ERR;
    jclass client = (*e)->FindClass(e, "es/isomorphism/zoneedit/ZoneEditClient");
    if (!client || (*e)->RegisterNatives(e, client, client_methods, 4) < 0) return JNI_ERR;
    return JNI_VERSION_1_6;
}
