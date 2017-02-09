#include "br_com_mzsw_PesoLibWrapper.h"
#include "PesoLib.h"

#ifdef _WIN64
typedef jlong lib_ptr_t;
#else
typedef int lib_ptr_t;
#endif

JNIEXPORT jlong JNICALL Java_br_com_mzsw_PesoLibWrapper_criar(JNIEnv* env, jobject obj, jstring config)
{
	const char *str = (*env)->GetStringUTFChars(env, config, 0);

	PesoLib * lib = PesoLib_cria(str);
	(*env)->ReleaseStringUTFChars(env, config, str);
	return (lib_ptr_t)lib;
}

JNIEXPORT jboolean JNICALL Java_br_com_mzsw_PesoLibWrapper_isConectado(JNIEnv* env, jobject obj, jlong instance)
{
	PesoLib * lib = (PesoLib*)(lib_ptr_t)instance;
	return PesoLib_isConectado(lib) != 0;
}

JNIEXPORT void JNICALL Java_br_com_mzsw_PesoLibWrapper_setConfiguracao(JNIEnv* env, jobject obj, jlong instance, jstring config)
{
	PesoLib * lib = (PesoLib*)(lib_ptr_t)instance;
	const char *str = (*env)->GetStringUTFChars(env, config, 0);
	
	PesoLib_setConfiguracao(lib, str);
	(*env)->ReleaseStringUTFChars(env, config, str);
}

JNIEXPORT jstring JNICALL Java_br_com_mzsw_PesoLibWrapper_getConfiguracao(JNIEnv* env, jobject obj, jlong instance)
{
	PesoLib * lib = (PesoLib*)(lib_ptr_t)instance;
	
	const char *buf = PesoLib_getConfiguracao(lib);
	jstring ret = (*env)->NewStringUTF(env, buf);
	return ret;
}

JNIEXPORT jstring JNICALL Java_br_com_mzsw_PesoLibWrapper_getMarcas(JNIEnv* env, jobject obj, jlong instance)
{
	PesoLib * lib = (PesoLib*)(lib_ptr_t)instance;
	
	const char *buf = PesoLib_getMarcas(lib);
	jstring ret = (*env)->NewStringUTF(env, buf);
	return ret;
}

JNIEXPORT jstring JNICALL Java_br_com_mzsw_PesoLibWrapper_getModelos(JNIEnv* env, jobject obj, jlong instance, jstring marca)
{
	PesoLib * lib = (PesoLib*)(lib_ptr_t)instance;
	const char *str = (*env)->GetStringUTFChars(env, marca, 0);
	const char *buf = PesoLib_getModelos(lib, str);
	
	jstring ret = (*env)->NewStringUTF(env, buf);
	return ret;
}

JNIEXPORT jint JNICALL Java_br_com_mzsw_PesoLibWrapper_aguardaEvento(JNIEnv* env, jobject obj, jlong instance)
{
	PesoLib * lib = (PesoLib*)(lib_ptr_t)instance;
	
	switch(PesoLib_aguardaEvento(lib))
	{
	case Evento_Cancelado:
		return br_com_mzsw_PesoLibWrapper_EVENTO_CANCELADO;
	case Evento_Conectado:
		return br_com_mzsw_PesoLibWrapper_EVENTO_CONECTADO;
	case Evento_Desconectado:
		return br_com_mzsw_PesoLibWrapper_EVENTO_DESCONECTADO;
	default:
		return br_com_mzsw_PesoLibWrapper_EVENTO_PESORECEBIDO;
	}
}

JNIEXPORT jint JNICALL Java_br_com_mzsw_PesoLibWrapper_getUltimoPeso(JNIEnv* env, jobject obj, jlong instance)
{
	PesoLib * lib = (PesoLib*)(lib_ptr_t)instance;

	return PesoLib_getUltimoPeso(lib);
}

JNIEXPORT jboolean JNICALL Java_br_com_mzsw_PesoLibWrapper_solicitaPeso(JNIEnv* env, jobject obj, jlong instance, jfloat preco)
{
	PesoLib * lib = (PesoLib*)(lib_ptr_t)instance;

	return PesoLib_solicitaPeso(lib, preco) != 0;
}

JNIEXPORT void JNICALL Java_br_com_mzsw_PesoLibWrapper_cancela(JNIEnv* env, jobject obj, jlong instance)
{
	PesoLib * lib = (PesoLib*)(lib_ptr_t)instance;
	PesoLib_cancela(lib);
}

JNIEXPORT void JNICALL Java_br_com_mzsw_PesoLibWrapper_libera(JNIEnv* env, jobject obj, jlong instance)
{
	PesoLib * lib = (PesoLib*)(lib_ptr_t)instance;
	PesoLib_libera(lib);
}

JNIEXPORT jstring JNICALL Java_br_com_mzsw_PesoLibWrapper_getVersao(JNIEnv * env, jobject obj, jlong instance)
{
	PesoLib * lib = (PesoLib*)(lib_ptr_t)instance;
	
	const char *buf = PesoLib_getVersao(lib);
	jstring ret = (*env)->NewStringUTF(env, buf);
	return ret;
}
