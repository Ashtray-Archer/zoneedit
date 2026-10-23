# ZoneEdit Android boundary

Purpose: replace the inherited Java Activity with checked Idriç class
descriptions, direct DEX encoding, and Android framework operations through JNI.

Types: framework subclass declarations and native virtual callback signatures
are typed inputs to the backend. DEX bytes are a checked encoder result;
writing those bytes is IO. Activity state and WebView objects stay under
Android ownership. Native callback bodies use the NDK JNI interface.

The Activity constructor delegates to Activity. Its initializer loads the
zoneedit library. Lifecycle callbacks and WebViewClient overrides dispatch to
that library. Credentials are never exported from CookieManager or WebView.

Source → checked Idriç generator → typed class plan → direct DEX is the class
adapter path. This does not claim arbitrary Android object operations in the
generic arithmetic ANF lowerer. No Java/Kotlin/Smali application intermediary
is permitted. The host generator uses the compiler's maintained Chez backend;
the Android application uses direct DEX plus NDK-compiled C.

Invariants: HTTPS and exact ZoneEdit hostname boundaries; no debugging,
JavaScript bridge, file/content access or mixed content. Restore history using
WebView's Bundle contract and persist cookies using CookieManager.flush.

Result: pending build and runtime verification. Physical A1/C67 and real
authenticated DNS editing remain separate acceptance requirements.
