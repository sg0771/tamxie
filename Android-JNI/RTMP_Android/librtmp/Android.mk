LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE :=librtmp
LOCAL_ARM_MODE := arm
LOCAL_C_INCLUDES +=	$(LOCAL_PATH)

LOCAL_CFLAGS :=-DPIC=1 -fPIC   -D_FILE_OFFSET_BITS=64  
LOCAL_CPP_FEATURES += exceptions

LOCAL_SRC_FILES:=amf.c  \
hashswf.c  \
log.c  \
parseurl.c  \
rtmp.c  \
library/aes.c  \
library/arc4.c  \
library/asn1parse.c  \
library/asn1write.c  \
library/base64.c  \
library/bignum.c  \
library/blowfish.c  \
library/camellia.c  \
library/certs.c  \
library/cipher.c  \
library/cipher_wrap.c  \
library/ctr_drbg.c  \
library/debug.c  \
library/des.c  \
library/dhm.c  \
library/entropy.c  \
library/entropy_poll.c  \
library/error.c  \
library/gcm.c  \
library/havege.c  \
library/md.c  \
library/md2.c  \
library/md4.c  \
library/md5.c  \
library/md_wrap.c  \
library/net.c  \
library/padlock.c  \
library/pbkdf2.c  \
library/pem.c  \
library/pkcs11.c  \
library/rsa.c  \
library/sha1.c  \
library/sha2.c  \
library/sha4.c  \
library/ssl_cache.c  \
library/ssl_cli.c  \
library/ssl_srv.c  \
library/ssl_tls.c  \
library/timing.c  \
library/version.c  \
library/x509parse.c  \
library/x509write.c  \
library/xtea.c  \


#LOCAL_LDLIBS +=  -lz 

include $(BUILD_STATIC_LIBRARY)
