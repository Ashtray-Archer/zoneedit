# Public development signing

`test-signer.p12` is the existing public test signer from Wegert commit
`ab10a9e163b4be1250cc369d98901c9cc48a6501`, path
`_/build/app/wegert-debug.keystore`. Alias and both passwords: `wegert-debug`.
This is not a production signing key. The central AI-CI registry owns the
permitted package/certificate pair and checks the finished APK. Builds never
generate a key.

Package: `es.isomorphism.zoneedit`; version code: 2. Inherited Gradle builds
may have a different ephemeral debug signer. Check upgrade compatibility on
the actual device. Never uninstall automatically to bypass a conflict.
