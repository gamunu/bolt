/***
* ==++==
*
* Copyright (c) Microsoft Corporation. All rights reserved.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* ==--==
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
*
* x509_cert_utilities.cpp
*
* Contains utility functions for helping to verify server certificates in OS X/iOS.
*
* For the latest on this and related APIs, please see http://casablanca.codeplex.com.
*
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
****/

#include "stdafx.h"
#include "cpprest/x509_cert_utilities.h"

#include <vector>
#include <string>

#if defined(__APPLE__)
#include <CoreFoundation/CFData.h>
#include <Security/SecBase.h>
#include <Security/SecCertificate.h>
#include <Security/SecPolicy.h>
#include <Security/SecTrust.h>
#endif

#if defined(ANDROID)
#include <jni.h>
#endif

using namespace crossplat;

namespace web { namespace http { namespace client { namespace details {

#if defined(__APPLE__) || defined(ANDROID)
bool verify_cert_chain_platform_specific(boost::asio::ssl::verify_context &verifyCtx, const std::string &hostName)
{
    X509_STORE_CTX *storeContext = verifyCtx.native_handle();
    int currentDepth = X509_STORE_CTX_get_error_depth(storeContext);
    if (currentDepth != 0)
    {
        return true;
    }

    STACK_OF(X509) *certStack = X509_STORE_CTX_get_chain(storeContext);
    const int numCerts = sk_X509_num(certStack);
    if (numCerts < 0)
    {
        return false;
    }

    std::vector<std::string> certChain;
    certChain.reserve(numCerts);
    for (int i = 0; i < numCerts; ++i)
    {
        X509 *cert = sk_X509_value(certStack, i);

        // Encode into DER format into raw memory.
        int len = i2d_X509(cert, nullptr);
        if (len < 0)
        {
            return false;
        }

        std::string certData;
        certData.resize(len);
        unsigned char * buffer = reinterpret_cast<unsigned char *>(&certData[0]);
        len = i2d_X509(cert, &buffer);
        if (len < 0)
        {
            return false;
        }

        certChain.push_back(std::move(certData));
    }

    return verify_X509_cert_chain(certChain, hostName);
}
#endif

#if defined(__APPLE__)

// Simple RAII pattern wrapper to perform CFRelease on objects.
template <typename T>
class cf_ref
{
public:
    cf_ref(T v) : value(v)
    {
        static_assert(sizeof(cf_ref<T>) == sizeof(T), "Code assumes just a wrapper, see usage in CFArrayCreate below.");
    }
    cf_ref() : value(nullptr) {}
    cf_ref(cf_ref &&other) : value(other.value) { other.value = nullptr; }

    ~cf_ref()
    {
        if(value != nullptr)
        {
            CFRelease(value);
        }
    }

    T & get()
    {
        return value;
    }
private:
    cf_ref(const cf_ref &);
    cf_ref & operator=(const cf_ref &);
    T value;
};

bool verify_X509_cert_chain(const std::vector<std::string> &certChain, const std::string &hostName)
{
    // Build up CFArrayRef with all the certificates.
    // All this code is basically just to get into the correct structures for the Apple APIs.
    // Copies are avoided whenever possible.
    std::vector<cf_ref<SecCertificateRef>> certs;
    for(const auto & certBuf : certChain)
    {
        cf_ref<CFDataRef> certDataRef = CFDataCreateWithBytesNoCopy(kCFAllocatorDefault,
                                                                   reinterpret_cast<const unsigned char*>(certBuf.c_str()),
                                                                   certBuf.size(),
                                                                   kCFAllocatorNull);
        if(certDataRef.get() == nullptr)
        {
            return false;
        }

        cf_ref<SecCertificateRef> certObj = SecCertificateCreateWithData(nullptr, certDataRef.get());
        if(certObj.get() == nullptr)
        {
            return false;
        }
        certs.push_back(std::move(certObj));
    }
    cf_ref<CFArrayRef> certsArray = CFArrayCreate(kCFAllocatorDefault, const_cast<const void **>(reinterpret_cast<void **>(&certs[0])), certs.size(), nullptr);
    if(certsArray.get() == nullptr)
    {
        return false;
    }

    // Create trust management object with certificates and SSL policy.
    // Note: SecTrustCreateWithCertificates expects the certificate to be
    // verified is the first element.
    cf_ref<CFStringRef> cfHostName = CFStringCreateWithCStringNoCopy(kCFAllocatorDefault,
                                                                    hostName.c_str(),
                                                                    kCFStringEncodingASCII,
                                                                    kCFAllocatorNull);
    if(cfHostName.get() == nullptr)
    {
        return false;
    }
    cf_ref<SecPolicyRef> policy = SecPolicyCreateSSL(true /* client side */, cfHostName.get());
    cf_ref<SecTrustRef> trust;
    OSStatus status = SecTrustCreateWithCertificates(certsArray.get(), policy.get(), &trust.get());
    if(status == noErr)
    {
        // Perform actual certificate verification.
        SecTrustResultType trustResult;
        status = SecTrustEvaluate(trust.get(), &trustResult);
        if(status == noErr && (trustResult == kSecTrustResultUnspecified || trustResult == kSecTrustResultProceed))
        {
            return true;
        }
    }

    return false;
}
#endif

#if defined(ANDROID)

/// <summary>
/// Helper function to check return value and see if any exceptions
/// occurred when calling a JNI function.
/// <summary>
/// <returns><c>true</c> if JNI call failed, <c>false</c> othewise.</returns>
bool jni_failed(JNIEnv *env)
{
    if(env->ExceptionOccurred())
    {
        // Clear exception otherwise no other JNI functions can be called.
        // In the future if we improve error reporting the exception message
        // can be retrieved from here.
        env->ExceptionClear();
        return true;
    }
    return false;
}
template <typename T>
bool jni_failed(JNIEnv *env, const java_local_ref<T> &result)
{
    if(jni_failed(env) || !result)
    {
        return true;
    }
    return false;
}
bool jni_failed(JNIEnv *env, const jmethodID &result)
{
    if(jni_failed(env) || result == nullptr)
    {
        return true;
    }
    return false;
}
#define CHECK_JREF(env, obj) if(jni_failed<decltype(obj)::element_type>(env, obj)) return false;
#define CHECK_JMID(env, mid) if(jni_failed(env, mid)) return false;
#define CHECK_JNI(env) if(jni_failed(env)) return false;

bool verify_X509_cert_chain(const std::vector<std::string> &certChain, const std::string &hostName)
{
    JNIEnv* env = get_jvm_env();

    // Possible performance improvement:
    // In the future we could gain performance by turning all the jclass local
    // references into global references. Then we could lazy initialize and
    // save them globally. If this is done I'm not exactly sure where the release
    // should be.

    // ByteArrayInputStream
    java_local_ref<jclass> byteArrayInputStreamClass(env->FindClass("java/io/ByteArrayInputStream"));
    CHECK_JREF(env, byteArrayInputStreamClass);
    jmethodID byteArrayInputStreamConstructorMethod = env->GetMethodID(
        byteArrayInputStreamClass.get(),
        "<init>",
        "([B)V");
    CHECK_JMID(env, byteArrayInputStreamConstructorMethod);

    // CertificateFactory
    java_local_ref<jclass> certificateFactoryClass(env->FindClass("java/security/cert/CertificateFactory"));
    CHECK_JREF(env, certificateFactoryClass);
    jmethodID certificateFactoryGetInstanceMethod = env->GetStaticMethodID(
        certificateFactoryClass.get(),
        "getInstance",
        "(Ljava/lang/String;)Ljava/security/cert/CertificateFactory;");
    CHECK_JMID(env, certificateFactoryGetInstanceMethod);
    jmethodID generateCertificateMethod = env->GetMethodID(
        certificateFactoryClass.get(),
        "generateCertificate",
        "(Ljava/io/InputStream;)Ljava/security/cert/Certificate;");
    CHECK_JMID(env, generateCertificateMethod);

    // X509Certificate
    java_local_ref<jclass> X509CertificateClass(env->FindClass("java/security/cert/X509Certificate"));
    CHECK_JREF(env, X509CertificateClass);

    // TrustManagerFactory
    java_local_ref<jclass> trustManagerFactoryClass(env->FindClass("javax/net/ssl/TrustManagerFactory"));
    CHECK_JREF(env, trustManagerFactoryClass);
    jmethodID trustManagerFactoryGetInstanceMethod = env->GetStaticMethodID(
        trustManagerFactoryClass.get(),
        "getInstance",
        "(Ljava/lang/String;)Ljavax/net/ssl/TrustManagerFactory;");
    CHECK_JMID(env, trustManagerFactoryGetInstanceMethod);
    jmethodID trustManagerFactoryInitMethod = env->GetMethodID(
        trustManagerFactoryClass.get(),
        "init",
        "(Ljava/security/KeyStore;)V");
    CHECK_JMID(env, trustManagerFactoryInitMethod);
    jmethodID trustManagerFactoryGetTrustManagersMethod = env->GetMethodID(
        trustManagerFactoryClass.get(),
        "getTrustManagers",
        "()[Ljavax/net/ssl/TrustManager;");
    CHECK_JMID(env, trustManagerFactoryGetTrustManagersMethod);

    // X509TrustManager
    java_local_ref<jclass> X509TrustManagerClass(env->FindClass("javax/net/ssl/X509TrustManager"));
    CHECK_JREF(env, X509TrustManagerClass);
    jmethodID X509TrustManagerCheckServerTrustedMethod = env->GetMethodID(
        X509TrustManagerClass.get(),
        "checkServerTrusted",
        "([Ljava/security/cert/X509Certificate;Ljava/lang/String;)V");
    CHECK_JMID(env, X509TrustManagerCheckServerTrustedMethod);

    // StrictHostnameVerifier
    java_local_ref<jclass> strictHostnameVerifierClass(env->FindClass("org/apache/http/conn/ssl/StrictHostnameVerifier"));
    CHECK_JREF(env, strictHostnameVerifierClass);
    jmethodID strictHostnameVerifierConstructorMethod = env->GetMethodID(strictHostnameVerifierClass.get(), "<init>", "()V");
    CHECK_JMID(env, strictHostnameVerifierConstructorMethod);
    jmethodID strictHostnameVerifierVerifyMethod = env->GetMethodID(
        strictHostnameVerifierClass.get(),
        "verify",
        "(Ljava/lang/String;Ljava/security/cert/X509Certificate;)V");
    CHECK_JMID(env, strictHostnameVerifierVerifyMethod);

    // Create CertificateFactory
    java_local_ref<jstring> XDot509String(env->NewStringUTF("X.509"));
    CHECK_JREF(env, XDot509String);
    java_local_ref<jobject> certificateFactory(env->CallStaticObjectMethod(
         certificateFactoryClass.get(),
         certificateFactoryGetInstanceMethod,
         XDot509String.get()));
    CHECK_JREF(env, certificateFactory);

    // Create Java array to store all the certs in.
    java_local_ref<jobjectArray> certsArray(env->NewObjectArray(certChain.size(), X509CertificateClass.get(), nullptr));
    CHECK_JREF(env, certsArray);

    // For each certificate perform the following steps:
    //   1. Create ByteArrayInputStream backed by DER certificate bytes
    //   2. Create Certificate using CertificateFactory.generateCertificate
    //   3. Add Certificate to array
    int i = 0;
    for(const auto &certData : certChain)
    {
        java_local_ref<jbyteArray> byteArray(env->NewByteArray(certData.size()));
        CHECK_JREF(env, byteArray);
        env->SetByteArrayRegion(byteArray.get(), 0, certData.size(), reinterpret_cast<const jbyte *>(certData.c_str()));
        CHECK_JNI(env);
        java_local_ref<jobject> byteArrayInputStream(env->NewObject(
            byteArrayInputStreamClass.get(),
            byteArrayInputStreamConstructorMethod,
            byteArray.get()));
        CHECK_JREF(env, byteArrayInputStream);

        java_local_ref<jobject> cert(env->CallObjectMethod(
            certificateFactory.get(),
            generateCertificateMethod,
            byteArrayInputStream.get()));
        CHECK_JREF(env, cert);

        env->SetObjectArrayElement(certsArray.get(), i, cert.get());
        CHECK_JNI(env);
        ++i;
    }

    // Create TrustManagerFactory, init with Android system certs
    java_local_ref<jstring> X509String(env->NewStringUTF("X509"));
    CHECK_JREF(env, X509String);
    java_local_ref<jobject> trustFactoryManager(env->CallStaticObjectMethod(
        trustManagerFactoryClass.get(),
        trustManagerFactoryGetInstanceMethod,
        X509String.get()));
    CHECK_JREF(env, trustFactoryManager);
    env->CallVoidMethod(trustFactoryManager.get(), trustManagerFactoryInitMethod, nullptr);
    CHECK_JNI(env);

    // Get TrustManager
    java_local_ref<jobjectArray> trustManagerArray(static_cast<jobjectArray>(
        env->CallObjectMethod(trustFactoryManager.get(), trustManagerFactoryGetTrustManagersMethod)));
    CHECK_JREF(env, trustManagerArray);
    java_local_ref<jobject> trustManager(env->GetObjectArrayElement(trustManagerArray.get(), 0));
    CHECK_JREF(env, trustManager);

    // Validate certificate chain.
    java_local_ref<jstring> RSAString(env->NewStringUTF("RSA"));
    CHECK_JREF(env, RSAString);
    env->CallVoidMethod(
        trustManager.get(),
        X509TrustManagerCheckServerTrustedMethod,
        certsArray.get(),
        RSAString.get());
    CHECK_JNI(env);

    // Verify hostname on certificate according to RFC 2818.
    java_local_ref<jobject> hostnameVerifier(env->NewObject(
        strictHostnameVerifierClass.get(), strictHostnameVerifierConstructorMethod));
    CHECK_JREF(env, hostnameVerifier);
    java_local_ref<jstring> hostNameString(env->NewStringUTF(hostName.c_str()));
    CHECK_JREF(env, hostNameString);
    java_local_ref<jobject> cert(env->GetObjectArrayElement(certsArray.get(), 0));
    CHECK_JREF(env, cert);
    env->CallVoidMethod(
        hostnameVerifier.get(),
        strictHostnameVerifierVerifyMethod,
        hostNameString.get(),
        cert.get());
    CHECK_JNI(env);

    return true;
}
#endif

}}}}
