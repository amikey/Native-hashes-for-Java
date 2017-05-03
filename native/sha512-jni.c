/* 
 * Native hash functions for Java
 * 
 * Copyright (c) Project Nayuki
 * https://www.nayuki.io/page/native-hash-functions-for-java
 */

#include <stdint.h>
#include <jni.h>


#define STATE_LEN 8
extern void sha512_compress_block(const jbyte *block, uint64_t state[STATE_LEN]);


/* 
 * Class:     nayuki_nativehash_Sha512
 * Method:    compress
 * Signature: ([J[BII)Z
 */
JNIEXPORT jboolean JNICALL Java_nayuki_nativehash_Sha512_compress(JNIEnv *env, jclass thisClass, jlongArray stateArray, jbyteArray msg, jint off, jint len) {
	if (len < 0 || (len & 127) != 0)  // Block size is 128 bytes
		return JNI_FALSE;
	JNIEnv theEnv = *env;
	(void)thisClass;
	
	// Get state array and convert to uint64_t
	jlong *stateJava = theEnv->GetLongArrayElements(env, stateArray, NULL);
	if (stateJava == NULL)
		return JNI_FALSE;
	uint64_t state[STATE_LEN];
	for (int i = 0; i < STATE_LEN; i++)
		state[i] = (uint64_t)stateJava[i];
	
	// Iterate over each block in msg
	jbyte *block = theEnv->GetPrimitiveArrayCritical(env, msg, NULL);
	if (block == NULL)
		return JNI_FALSE;
	for (jint end = off + len; off < end; off += 128)
		sha512_compress_block(&block[off], state);
	theEnv->ReleasePrimitiveArrayCritical(env, msg, block, JNI_ABORT);
	
	// Convert state array to jlong and clean up
	for (int i = 0; i < STATE_LEN; i++)
		stateJava[i] = (jlong)state[i];
	theEnv->ReleaseLongArrayElements(env, stateArray, stateJava, 0);
	return JNI_TRUE;
}
