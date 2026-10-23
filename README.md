# ZoneEdit Mobile

An Android WebView wrapper around the real ZoneEdit control panel. ZoneEdit
owns authentication and DNS editing. The app does not store a password,
scrape forms, or invent a DNS-management API.

The native build checks Idriç class descriptions and runs their host generator
to directly encode Activity/WebViewClient DEX classes. C callbacks invoke
Android through JNI. Application compilation requires no Java, Kotlin,
generated Java, d8, Gradle or assembled Smali.

- Opens `https://cp.zoneedit.com/login.php`.
- Keeps HTTPS ZoneEdit hosts with exact subdomain boundaries inside the app.
- Hands external HTTPS links to the browser and blocks unsafe/malformed URLs.
- Blocks external top-level POSTs instead of replaying credential data.
- Uses standard cookies/autofill, flushes cookies, saves/restores navigation
  across Activity recreation, and supports Android Back.
- Injects fixed mobile CSS for readable controls, responsive images and
  horizontally scrollable tables.
- Disables WebView debugging, file/content access, unsafe file-origin access
  and mixed content. No JavaScript bridge is installed.

## Build on Linux x86-64

Use SDK 35, build-tools 35.0.0, NDK r27c, CMake 3.22+, a bootstrapped maintained
Idriç compiler, and the backend/AI-CI commits pinned in
[the workflow](.github/workflows/android.yml). See [build boundaries](_/toolchain.md).

Configure `CMakeLists.txt` with absolute paths for `ANDROID_SDK_ROOT`,
`IDRIC_EXECUTABLE`, `IDRIC_SUPPORT_DIR` (the compiler's `_` directory),
`IDRIC_SCHEME`, `DEX_BACKEND_DIR` and `AICI_DIR`. Build target `zoneedit`;
`navigation-test` executes URL policy cases. No phone-side build is needed.

With `-B build/native`, the APK is `build/native/zoneedit-mobile.apk`. It
contains ARM32 for MIRO A1, ARM64 for MIRO C67 and a separately tested x86-64
emulator library. Package: `es.isomorphism.zoneedit`; version code: 2.
[Signing](_/signing/README.md) uses the stable public test identity and checks
the finished APK against the independent central registry. Updates preserve
app data; never silently uninstall to bypass a signature conflict.

## Acceptance

The workflow builds on `main` and pull requests and retains the APK, checked
source evidence, signer verdict, Android 14 logs and screenshots. Runtime
acceptance requires a live login form, native callbacks, saved-state
restoration, reopening and replacement installation. It does not log in or
change DNS records.

Host, emulator, physical A1, physical C67 and authenticated DNS editing are
separate evidence. On each actual device, test login, domain selection,
record tables, editing a disposable test record, Back, rotation, reopening
and an update without uninstalling. Identify the APK SHA-256 and device.
Never use production records merely to exercise the app. A build or mock page
cannot establish physical/authenticated acceptance.
