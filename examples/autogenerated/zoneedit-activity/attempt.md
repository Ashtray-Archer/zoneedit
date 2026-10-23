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

Result: checked by compiler `0e8cfdd1106ebdeffd01b6c519880ea3afc7dc35`;
the retained ANF, compiled host generator and direct DEX are produced by
`CMakeLists.txt`. SDK dexdump independently verifies the DEX checksum;
pinned baksmali decodes both classes and their overloads. NDK compilation
produces ARM32, ARM64 and emulator x86-64 libraries. URL cases pass and reject
a deliberately weakened hostname boundary. Finished APK signer verification
uses the independent AI-CI registry.

The necessary language work is the reusable framework-class adapter in
`fuego-ironworks/idric-arm-thumb` at
`76a5c5086b01abd57d2e4bb763bcd7a5ddc5b524`. No Java/Smali fallback is used.
The generic arithmetic ANF backend still does not own arbitrary Android object
operations; this consumer deliberately uses the authorized C/JNI boundary.

Hosted runtime acceptance is defined in `_/runtime-check.cmake`. Physical
A1/C67 and real authenticated DNS editing remain separate requirements.
