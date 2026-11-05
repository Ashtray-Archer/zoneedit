# Charcoal ZoneEdit shell

The application window now explicitly selects the common charcoal theme from
android-NDK e8b67dfc67776c2b8da919785d15f38d68c91db0. Its header/resources are
verified byte-for-byte, and CMake compiles/links the actual resource table.
The WebView's initial clear is #212121 before external content paints. Fixed
window/dialog/system-bar colors do not depend on Android system night mode.

ZoneEdit's external website, authentication, forms, documents and existing
mobile CSS retain their own colors. No inversion, color filter or forced-dark
WebView content transformation is added. Navigation policy, cookie behavior,
state restoration, direct checked Idriç class-description DEX, explicit NDK
boundary, three ABIs, package/version and existing public test signer are
preserved. No Java/Kotlin, Gradle, d8 or new framework is introduced.

Current source build, URL-policy tests and API34 live unauthenticated runtime
checks must run on this source. API29 and both-mode own-window/partial-error
visual acceptance and physical C67 checks remain NOT_RUN. This change alone
does not qualify all WebView/framework offline error pages.
No credentials are submitted or DNS records changed. No merge, public release
or installed-app replacement is performed.
