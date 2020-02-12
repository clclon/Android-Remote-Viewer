
#include "libfake.h"
#include <jni.h>

/// see README-libart.md for details..

# define FUNWEAK __attribute__ ((weak))
# include "libsigchain.h"

extern "C" JNIEXPORT jint JNI_GetDefaultJavaVMInitArgs(void *v)
{
	LOGPF();
	v = nullptr;
	return 0;
}
extern "C" JNIEXPORT jint JNI_CreateJavaVM(JavaVM **jv, JNIEnv **ev, void *v)
{
	LOGPF();
	jv = nullptr;
	ev = nullptr;
	jint *ji = static_cast<jint*>(v);
	*ji = 0;
	return 0;
}
extern "C" JNIEXPORT jint JNI_GetCreatedJavaVMs(JavaVM **jv, jsize jsz, jsize *pjsz)
{
	LOGPF();
	(void) jsz;
	jv = nullptr;
	*pjsz = 0;
	return 0;
}
