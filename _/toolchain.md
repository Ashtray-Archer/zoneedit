# Build boundaries

The selected C toolchain is Android NDK r27c (`27.2.12479018`). The explicit
ordinary-C JNI boundary invokes Android framework objects. It does not claim
Icky-C source-syntax qualification.

ICK was resolved at `8a7aac0e42bc8af4550577bbc87e2a5a15eb9db7`. This app has
no qualification for that compiler's JNI compilation/platform linking on the
packaged ABIs. That is an absent consumer qualification, not a claim that ICK
cannot implement JNI. The user explicitly allowed NDK. AI-CI reports this gap.

The pinned host Idriç compiler checks a class-description generator and runs
it using its maintained Chez backend. The generator directly encodes Android
DEX. Retained ANF proves source elaboration; the typed class adapter, rather
than the generic arithmetic ANF lowerer, owns class metadata. No generated C,
RefC, Java/Kotlin compilation, d8 or assembled Smali enters this path.

SDK 35, build-tools 35.0.0, NDK r27c, and the compiler/backend/AI-CI commits in
the workflow are material inputs. SDK packaging/signing uses a host JVM.
